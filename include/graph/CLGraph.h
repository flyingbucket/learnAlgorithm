#ifndef INCLUDE_GRAPH_CLGRAPH_H
#define INCLUDE_GRAPH_CLGRAPH_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include "GraphOps.h"

struct CLVNode;
struct CLENode;

typedef struct CLVNode {
  void* data;
  struct CLENode* firstin;
  struct CLENode* firstout;
} CLVNode;

typedef struct CLENode {
  Weight w;
  VertexId tailvex;
  VertexId headvex;
  struct CLENode* hlink;
  struct CLENode* tlink;
} CLENode;

typedef struct CLGraph {
  BaseGraph bg;
  CLVNode* xlist;
  int n_verts;
  int n_edges;
  int vert_capacity;
} CLGraph;

CLGraph* clgraph_init(int capacity);
void destroy_clgraph(CLGraph* g);
#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_CLGRAPH_H
