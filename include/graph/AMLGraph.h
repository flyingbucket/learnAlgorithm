#ifndef INCLUDE_GRAPH_ALMGRAPH_H
#define INCLUDE_GRAPH_ALMGRAPH_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include "GraphOps.h"
struct AMLVNode;
struct AMLENode;
typedef struct AMLVNode {
  void* data;
  struct AMLENode* firstedge;
} AMLVNode;
typedef struct AMLENode {
  VertexId ivex, jvex;
  struct AMLENode* ilink;
  struct AMLENode* jlink;
  Weight w;
} AMLENode;

typedef struct {
  AMLVNode* verts;
  BaseGraph bg;
  int n_verts;
  int n_edges;
  int vert_capacity;
} AMLGraph;

AMLGraph* amlgraph_init(int capacity);
void amlgraph_destroy(AMLGraph* g);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_ALMGRAPH_H
