#ifndef INCLUDE_STACKLISTACK_HPP
#define INCLUDE_STACKLISTACK_HPP
#include <cstdlib>

#define ElemType int

typedef struct LinkNode {
  ElemType data;
  LinkNode* next;
} LinkNode, *LiStack;

inline bool InitStack(LiStack* s);
inline bool DestroyStack(LiStack* s);
inline bool isStackEmpty(LiStack s);
inline bool Push(LiStack s, int val);
inline bool Pop(LiStack s, int* val);
inline bool GetTop(LiStack s, int* val);
#endif  // !INCLUDE_STACKLISTACK_HPP
