#ifndef INCLUDE_GRAPH_MGRAPH_H
#define INCLUDE_GRAPH_MGRAPH_H

#include "GraphOps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MGraph {
  BaseGraph bg;
  int n_verts;
  void* verts; /* 以后你可以把它变成 void** 或别的 */
  int* adj;    /* n_verts * n_verts 的邻接矩阵，按行展开的一维数组 */
} MGraph;

/* 构造 / 析构函数 */
MGraph* mgraph_create(int n_verts);
void mgraph_destroy(MGraph* g);

/* 这些是 MGraph 对 GraphQueryOps / GraphMutateOps 的具体实现函数 */
bool m_adjacent(void* G, VertexId v1, VertexId v2);
int m_first_neighbor(void* G, VertexId v);
int m_next_neighbor(void* G, VertexId v, VertexId w);

VertexId m_add_vert(void* G);
bool m_delete_vert(void* G, VertexId v);
bool m_add_edge(void* G, VertexId v1, VertexId v2);
bool m_remove_edge(void* G, VertexId v1, VertexId v2);

#ifdef __cplusplus
}
#endif

#endif  // !INCLUDE_GRAPH_MGRAPH_H
