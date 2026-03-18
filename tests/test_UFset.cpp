#include <catch2/catch.hpp>

#include "tree/UFset.hpp"

TEST_CASE("TreeUFset: Basic Initialization and Find", "[UFset]") {
  UFset s = nullptr;
  uint len = 5;

  SECTION("Initial should allocate and set all elements to -1") {
    REQUIRE(Initial(&s, len) == true);
    REQUIRE(s != nullptr);

    for (uint i = 0; i < len; i++) {
      REQUIRE(s[i] == -1);
    }
    free(s);  // 测试结束释放内存
  }

  SECTION("Find on initial disjoint sets should return themselves") {
    REQUIRE(Initial(&s, len) == true);

    for (uint i = 0; i < len; i++) {
      REQUIRE(Find(s, i) == static_cast<int>(i));
    }
    free(s);
  }
}

TEST_CASE("TreeUFset: Union Operations", "[UFset]") {
  UFset s = nullptr;
  uint len = 5;
  REQUIRE(Initial(&s, len) == true);

  SECTION("Union of two disjoint elements") {
    // 初始状态: s[0]=-1, s[1]=-1
    // 根据当前代码的 else 分支: s[1] += s[0], s[0] = 1
    Union(s, 0, 1);

    // 0 和 1 应该拥有相同的根节点 (1)
    REQUIRE(Find(s, 0) == 1);
    REQUIRE(Find(s, 1) == 1);

    // 验证集合大小 (负数表示大小，-1 + -1 = -2)
    REQUIRE(s[1] == -2);
    REQUIRE(s[0] == 1);  // 0 的父节点是 1
  }

  SECTION("Union to bigger tree (Union by Size)") {
    // 构造根为 1 的集合: {0, 1, 2}, 大小为 3 (即 s[1] = -3)
    Union(s, 0, 1);  // 0挂到1下，root: 1, size: -2
    Union(s, 1, 2);  // 2挂到1下，root: 1, size: -3

    // 构造根为 4 的集合: {3, 4}, 大小为 2 (即 s[4] = -2)
    Union(s, 3, 4);  // 3挂到4下，root: 4, size: -2

    // 合并两个集合
    // 期望：因为 s[1] < s[4] (-3 < -2)，较小的树(root 4)应该挂到较大的树(root
    // 1)下
    int rootA = Find(s, 2);  // 返回 1
    int rootB = Find(s, 4);  // 返回 4
    Union(s, rootA, rootB);

    // 最终大家的老大都是 1
    REQUIRE(Find(s, 4) == 1);
    REQUIRE(Find(s, 3) == 1);
    REQUIRE(s[1] == -5);  // 总节点数 3 + 2 = 5
  }
  free(s);
}

TEST_CASE("TreeUFset: Path Compression", "[UFset]") {
  UFset s = nullptr;
  uint len = 10;
  REQUIRE(Initial(&s, len) == true);

  SECTION("Find should flatten the tree structure") {
    // 手动构造一条极端的链表状树: 0 -> 1 -> 2 -> 3 (其中3是根，大小为-4)
    s[0] = 1;
    s[1] = 2;
    s[2] = 3;
    s[3] = -4;

    // 对最底层的 0 执行 Find，应该触发路径压缩
    int root = Find(s, 0);
    REQUIRE(root == 3);

    // 验证路径压缩的结果：0, 1, 2 都应该直接指向根节点 3
    REQUIRE(s[0] == 3);
    REQUIRE(s[1] == 3);
    REQUIRE(s[2] == 3);
    REQUIRE(s[3] == -4);  // 根节点大小不变
  }

  free(s);
}
