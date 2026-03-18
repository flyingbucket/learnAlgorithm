#include <vector>

#include "catch2/catch.hpp"
#include "tree/BinaryTree.hpp"
#include "tree/ImplBiTree.hpp"

// =========================================================
// 测试辅助工具
// =========================================================

// 全局容器，用于收集遍历结果
std::vector<int> g_visit_result;

// 1. 链式二叉树的 Visit 函数
void CaptureLiVisit(void* tree, void* node) {
  if (node) {
    Node* n = (Node*)node;
    // 假设数据是 int (注意：你的 fromPreAndInOrder 是 malloc(elem_size)
    // 拷贝数据的)
    int val = *(int*)n->data;
    g_visit_result.push_back(val);
  }
}

// 2. 数组二叉树的 Visit 函数
void CaptureArrVisit(void* tree, void* node) {
  BiTreeArr* arTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node;

  // 计算数组中的偏移地址
  char* target = (char*)arTree->arr + index * arTree->elem_size;
  int val = *(int*)target;
  g_visit_result.push_back(val);
}

// =========================================================
// 测试用例：链式二叉树 (BiTreeLi)
// =========================================================

TEST_CASE("Linked Binary Tree Operations", "[BiTreeLi]") {
  // 准备测试数据:
  //        1
  //       / \
  //      2   3
  //     / \   \
  //    4   5   6

  int pre[] = {1, 2, 4, 5, 3, 6};
  int in[] = {4, 2, 5, 1, 3, 6};
  size_t len = 6;
  size_t elem_size = sizeof(int);

  // 1. 测试重建与基本结构
  SECTION("Reconstruct from PreOrder & InOrder") {
    BiTreeLi root = fromPreAndInOrder(pre, in, elem_size, len);

    REQUIRE(root != NULL);
    REQUIRE(*(int*)root->data == 1);

    REQUIRE(root->l != NULL);
    REQUIRE(*(int*)root->l->data == 2);

    REQUIRE(root->r != NULL);
    REQUIRE(*(int*)root->r->data == 3);

    // 验证叶子
    REQUIRE(*(int*)root->l->l->data == 4);
    REQUIRE(*(int*)root->l->r->data == 5);
    REQUIRE(*(int*)root->r->r->data == 6);

    DestroyBiTreeLi(root);
  }

  // 2. 测试所有遍历方式
  SECTION("Traversals (Pre, In, Post, Level)") {
    BiTreeLi root = fromPreAndInOrder(pre, in, elem_size, len);
    REQUIRE(root != NULL);

    // --- PreOrder ---
    g_visit_result.clear();
    PreOrder(NULL, root, BiTreeLiOps, CaptureLiVisit);
    std::vector<int> expected_pre = {1, 2, 4, 5, 3, 6};
    REQUIRE(g_visit_result == expected_pre);

    // --- InOrder ---
    g_visit_result.clear();
    InOrder(NULL, root, BiTreeLiOps, CaptureLiVisit);
    std::vector<int> expected_in = {4, 2, 5, 1, 3, 6};
    REQUIRE(g_visit_result == expected_in);

    // --- PostOrder ---
    // 预期: 4, 5, 2, 6, 3, 1
    g_visit_result.clear();
    PostOrder(NULL, root, BiTreeLiOps, CaptureLiVisit);
    std::vector<int> expected_post = {4, 5, 2, 6, 3, 1};
    REQUIRE(g_visit_result == expected_post);

    // --- LevelOrder (核心测试点: SqQueue 是否工作正常) ---
    // 预期: 1, 2, 3, 4, 5, 6
    g_visit_result.clear();

    // 注意：LevelOrder 内部使用的是 EnQueue(que, root_handle)
    // 这里的 root_handle 是 Node* 指针。
    // SqQueue 的 elem_size 在 InitQueue 时被设为 sizeof(void*)
    LevelOrder(NULL, root, BiTreeLiOps, CaptureLiVisit);

    std::vector<int> expected_level = {1, 2, 3, 4, 5, 6};
    REQUIRE(g_visit_result == expected_level);

    DestroyBiTreeLi(root);
  }
}

// =========================================================
// 测试用例：数组二叉树 (BiTreeArr)
// =========================================================

TEST_CASE("Array Binary Tree Operations", "[BiTreeArr]") {
  // 构造一个完全二叉树数组
  // Index: 0  1  2  3  4  5  6
  // Value: 1  2  3  4  5  6  7
  int data[] = {1, 2, 3, 4, 5, 6, 7};

  BiTreeArr treeArr;
  treeArr.arr = data;
  treeArr.elem_size = sizeof(int);
  treeArr.size = 7;
  treeArr.capacity = 10;

  // 根节点的 handle 是 index 0
  void* root_handle = (void*)0;

  SECTION("Level Order on Array Tree") {
    g_visit_result.clear();
    LevelOrder(&treeArr, root_handle, BiTreeArrOps, CaptureArrVisit);

    std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7};
    REQUIRE(g_visit_result == expected);
  }

  SECTION("PreOrder on Array Tree") {
    // 1 -> 2 -> 4, 5 -> 3 -> 6, 7
    g_visit_result.clear();
    PreOrder(&treeArr, root_handle, BiTreeArrOps, CaptureArrVisit);

    std::vector<int> expected = {1, 2, 4, 5, 3, 6, 7};
    REQUIRE(g_visit_result == expected);
  }
}

// =========================================================
// 测试用例：边缘情况与错误处理
// =========================================================

TEST_CASE("Edge Cases and Error Handling", "[Edge]") {
  size_t elem_size = sizeof(int);

  SECTION("Single Node Tree") {
    int pre[] = {99};
    int in[] = {99};
    BiTreeLi root = fromPreAndInOrder(pre, in, elem_size, 1);

    REQUIRE(root != NULL);
    REQUIRE(*(int*)root->data == 99);
    REQUIRE(root->l == NULL);
    REQUIRE(root->r == NULL);

    g_visit_result.clear();
    LevelOrder(NULL, root, BiTreeLiOps, CaptureLiVisit);
    REQUIRE(g_visit_result.size() == 1);
    REQUIRE(g_visit_result[0] == 99);

    DestroyBiTreeLi(root);
  }

  SECTION("Invalid Input - Root Not Found") {
    // Pre: 1, 2
    // In:  3, 4 (没有 1)
    int pre[] = {1, 2};
    int in[] = {3, 4};

    // 应该在 stderr 打印错误并返回 NULL
    BiTreeLi root = fromPreAndInOrder(pre, in, elem_size, 2);

    REQUIRE(root == NULL);
  }

  SECTION("Invalid Input - Subtree Mismatch") {
    // 根匹配，但右子树数据对不上
    // Pre: 1, 2
    // In:  1, 3 (InOrder 说右边是 3，Pre 说右边是 2)
    int pre[] = {1, 2};
    int in[] = {1, 3};

    BiTreeLi root = fromPreAndInOrder(pre, in, elem_size, 2);

    REQUIRE(root == NULL);
  }

  SECTION("Empty Tree") {
    BiTreeLi root = fromPreAndInOrder(NULL, NULL, elem_size, 0);
    REQUIRE(root == NULL);
  }
}
