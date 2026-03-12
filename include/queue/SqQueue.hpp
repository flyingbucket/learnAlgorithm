#ifndef INCLUDE_SQQUEUE_HPP
#define INCLUDE_SQQUEUE_HPP
#include <cstddef>
#include <cstdlib>

#define ElemType int
// #define capacity 50

/**
 * @brief 循环队列,队尾后空出一个节点用于解决空队、满队判别
 */
typedef struct {
  ElemType* data;
  int capacity;
  int front, rear;
} SqQueue;

/**
 * @brief 初始化队列
 */
inline bool InitQueue(SqQueue** q, int capacity = 50) {
  ElemType* data = (ElemType*)malloc(sizeof(ElemType) * capacity);
  *q = (SqQueue*)malloc(sizeof(SqQueue));
  if (data == NULL || *q == NULL) {
    return false;
  }
  (*q)->capacity = capacity;
  (*q)->data = data;
  (*q)->front = 0;
  (*q)->rear = 0;
  return true;
}

/**
 * @brief 循环队列 空队判别
 */
inline bool isSqQueueEmpty(SqQueue* q) {
  if (q->front == q->rear) {
    return true;
  } else {
    return false;
  }
}
/**
 * @brief 循环队列 满队判别
 */
inline bool isSqQueueFull(SqQueue* q) {
  int capacity = q->capacity;
  int rear_plus_one = (q->rear + 1) % capacity;
  if (q->front == rear_plus_one) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief 循环队列 入队
 */
inline bool EnQueue(SqQueue* q, ElemType val) {
  if (isSqQueueFull(q)) {
    return false;
  }
  int capacity = q->capacity;
  q->data[q->rear] = val;
  q->rear = (q->rear + 1 + capacity) % capacity;
  return true;
}

/**
 * @brief 循环队列 出队
 */
inline bool DeQueue(SqQueue* q, ElemType* val) {
  if (isSqQueueEmpty(q)) {
    return false;
  }
  int capacity = q->capacity;
  *val = q->data[q->front];
  q->front = (q->front + 1 + capacity) % capacity;
  return true;
}

inline int SqQueueLength(SqQueue* q) {
  int capacity = q->capacity;
  return (q->rear - q->front + capacity) % capacity;
}

inline bool DestorySqQueue(SqQueue** q) {
  if (q == NULL || *q == NULL || (*q)->data == NULL) {
    return false;
  }
  free((*q)->data);
  free(*q);
  *q = NULL;
  return true;
}
#endif  // !INCLUDE_SQQUEUE_HPP
