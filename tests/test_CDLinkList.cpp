#include "catch2/catch.hpp"
#include "linkedlist/CDLinkList.hpp"

TEST_CASE("CDLinkList: Circularity and Indexing", "[cdlink]") {
  CDLinkList L;
  REQUIRE(InitCList(L) == true);

  SECTION("Circular Properties") {
    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);
    // [Head, 10, 20] - 长度为3 (包含头节点数据存储)

    // 循环双链表头节点的 prior 应该指向最后一个节点
    REQUIRE(L->prior->data == 20);
    // 最后一个节点的 next 应该指向头节点
    REQUIRE(GetElem(L, 2)->next == L);
  }

  SECTION("Modulo Indexing (GetElem)") {
    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);  // List: Head(0), 10(1), 20(2)

    // 正常索引
    CHECK(GetElem(L, 1)->data == 10);
    // 循环索引：索引3应回到索引0 (头节点)
    CHECK(GetElem(L, 3) == L);
    // 负数索引：-1 应指向最后一个节点 (20)
    CHECK(GetElem(L, -1)->data == 20);
  }

  SECTION("DeletePrior in Circular List") {
    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);
    InsertPrior(L, 3, 30);  // [10, 20, 30]

    int val;
    // 删除第3个节点(30)的前一个节点(20)
    REQUIRE(DeletePrior(L, 3, val) == true);
    CHECK(val == 20);
    CHECK(GetElem(L, 2)->data == 30);
  }
  SECTION("GetLength and Length Tracking") {
    // 初始状态：只有头节点，长度为 1
    CHECK(GetLength(L) == 1);
    CHECK(L->data == 1);  // 头节点的 data 维护着链表长度

    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);

    // 插入两个节点后，总长度变为 3
    CHECK(GetLength(L) == 3);
    CHECK(L->data == 3);

    // 删除一个节点后，长度应该动态减少
    int val;
    DeletePrior(L, 2, val);
    CHECK(GetLength(L) == 2);
    CHECK(L->data == 2);
  }

  SECTION("LocateElem (Search by Value)") {
    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);
    InsertPrior(L, 3, 30);

    // 正常查找存在的值
    DLNode* target = LocateElem(L, 20);
    REQUIRE(target != nullptr);
    CHECK(target->data == 20);
    // 验证查找到的节点确实在链表正确位置
    CHECK(target->next->data == 30);

    // 查找不存在的值
    CHECK(LocateElem(L, 99) == nullptr);

    // 关键边界测试：确保 LocateElem
    // 不会错误地把头节点里的“长度数据”当成普通数据匹配出来 当前链表长度为
    // 4（含头节点），尝试查找值为 4 的节点
    CHECK(LocateElem(L, 4) == nullptr);
  }

  SECTION("DeletePrior Edge Cases (Protecting Head Node)") {
    InsertPrior(L, 1, 10);
    InsertPrior(L, 2, 20);
    // 当前链表: Head(0), 10(1), 20(2)，长度为 3

    int val;
    // 尝试删除索引 1 的前一个节点（即索引 0 的头节点）。代码里已经做了拦截
    CHECK(DeletePrior(L, 1, val) == false);

    // 结合模运算的恶意越界删除：
    // 当 index = 4 时，4 % 3 = 1，同样是指向头节点，应该被拦截
    CHECK(DeletePrior(L, 4, val) == false);
  }
  DestroyCDList(L);
}
