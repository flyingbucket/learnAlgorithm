#ifndef LINKLIST_H
#define LINKLIST_H

#include <cstdio>
#include <cstdlib>

#define ElemType int

typedef struct LNode {
  ElemType data;
  struct LNode *next;
} LNode, *LinkList;

/**
 * @brief C风格初始化链表 (通过返回值)
 * * 分配一块新的内存作为头节点，并返回其指针。
 * * @return LinkList 分配成功的头节点指针，若失败则返回 NULL
 */
inline LinkList DirectInitList(void) {
  LinkList head = (LNode *)std::malloc(sizeof(LNode));
  if (head != NULL) {
    head->next = NULL;
  }
  return head;
}

/**
 * @brief C风格初始化链表 (通过二级指针)
 * * @param L 指向头指针的二级指针
 * @return true 初始化成功
 * @return false 内存分配失败
 */
inline bool PointerInitList(LinkList *L) {
  *L = (LinkList)std::malloc(sizeof(LNode));
  if (*L == NULL)
    return false;
  return true;
}

/**
 * @brief C++风格初始化链表 (通过引用)
 * * @param L 头指针的引用
 * @return true 初始化成功
 * @return false 内存分配失败
 */
inline bool InitList(LinkList &L) {
  L = (LNode *)std::malloc(sizeof(LNode));
  if (L == NULL)
    return false;
  L->next = NULL;
  return true;
}

/**
 * @brief C++风格求链表长度(包含头节点)
 * * @return int
 */
inline int LengthWithHeadNode(LinkList L) {
  LNode *p = L;
  int len = 0;
  while (p != NULL) {
    len++;
    p = p->next;
  }
  return len;
}

/**
 * @brief C++风格求链表长度(只计算数据节点长度，不包含头节点)
 * * @return int
 *    - 返回-1表示传入的头指针是NULL,即该链表不存在
 */
inline int LengthWithoutHeadNode(LinkList L) {
  LNode *p = L;
  int len = 0;
  while (p != NULL) {
    len++;
    p = p->next;
  }
  return len - 1;
}

/**
 * @brief C++风格按索引查找节点
 * - 传入的链表应包含头节点，规定头节点索引为0，数据节点的索引从1开始
 * * @return LNode*
 *    - 返回携带该值的链表节点的指针
 */
inline LNode *GetElem(LinkList L, uint index) {
  LNode *p = L;
  uint j = 0;
  while (p != NULL && j < index) {
    p = p->next;
    j++;
  }
  return p;
}

/**
 * @brief C++风格按值查找节点
 * @return LNode*
 *  - 返回携带该值的链表节点的指针
 */
inline LNode *LocateElem(LinkList L, ElemType val) {
  if (L == NULL)
    return NULL;
  LNode *p =
      L->next; // headnote存储的数据语义和DataNode的语义完全不同，须手动跳过
  while (p != NULL && p->data != val) {
    p = p->next;
  }
  return p;
}

/**
 * @brief C++风格插入节点
 * @param
 *  - L: 指向被插入链表的头指针
 *  - index: 插入位置，即令新节点成为链表L的第`index` 个数据节点,
 * 传参应满足index >= 1
 *  - val: 新节点的值
 * @return bool
 * - 是否插入成功
 */
inline bool ListInsert(LinkList &L, uint index, ElemType val) {
  if (index == 0)
    return false;

  LNode *p = L;
  uint j = 0;
  while (p != NULL && j < index - 1) {
    j++;
    p = p->next;
  }

  if (j < index - 1)
    return false;

  LNode *new_node = (LNode *)std::malloc(sizeof(LNode));
  new_node->data = val;
  new_node->next = p->next;
  p->next = new_node;
  return true;
}

/**
 * @brief C++风格删除节点
 * @param
 *  - L: 指向被插入链表的头指针
 *  - index: 删除位置，即删除链表L的第`index` 个数据节点, 传参应满足index >= 1
 * @return bool
 * - 是否删除成功
 */
inline bool ListDelete(LinkList &L, uint index) {
  if (index == 0)
    return false;

  LNode *p = L;
  uint j = 0;
  while (p != NULL && j < index - 1) {
    p = p->next;
    j++;
  }

  if (p == NULL || p->next == NULL) {
    return false;
  }

  LNode *q = p->next;
  p->next = q->next;
  free(q);
  return true;
}

inline void PrintList(LinkList L) {
  if (L == NULL) {
    printf("List is NULL\n");
    return;
  }
  LNode *p = L->next; // 跳过头节点
  printf("Head -> ");
  while (p != NULL) {
    printf("[%d] -> ", p->data);
    p = p->next;
  }
  printf("NULL\n");
}
#endif
