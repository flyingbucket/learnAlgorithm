#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"

MSTResult* Prim(void* graph_context, const BaseGraph* graph_methods) {
  if (!graph_context || !graph_methods) return NULL;
  void* g = graph_context;
  const BaseGraph* bg = graph_methods;
  MSTEdge* edges = (MSTEdge*)malloc(sizeof(MSTEdge) * (bg->iops.edge_count(g)));
}
