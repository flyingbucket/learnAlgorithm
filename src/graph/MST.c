#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "queue/SqQueue.h"

MSTResult* Prim(const void* graph_context, const BaseGraph* graph_methods) {
  if (!graph_context || !graph_methods) return NULL;
  const void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int nv = bg->iops.vertex_count(G);
  MSTResult* result = (MSTResult*)malloc(sizeof(MSTResult));
  result->edges = (Edge*)malloc(sizeof(Edge) * (nv - 1));  // MST 最多 nv-1 条边
  result->component_count = 0;
  result->edge_count = 0;
  result->component_offset = NULL;
  SqQueue* offset;
  InitQueue(&offset, sizeof(VertexId), nv - 1);

  bool* in_tree = (bool*)calloc(nv, sizeof(bool));
  Weight* dist_to_tree = (Weight*)malloc(sizeof(Weight) * nv);
  VertexId* parent = (VertexId*)malloc(sizeof(VertexId) * nv);
  for (int i = 0; i < nv; i++) {
    dist_to_tree[i] = INFINITY;
    parent[i] = -1;
  }
  int edge_count = 0;

  // traverse all components
  for (int start_v = 0; start_v < nv; start_v++) {
    if (in_tree[start_v]) continue;

    dist_to_tree[start_v] = 0;
    // build MST for one component
    for (int _ = 0; _ < nv; _++) {
      VertexId target = -1;
      Weight min_w = INFINITY;
      for (VertexId i = 0; i < nv; i++) {
        if (!in_tree[i] && dist_to_tree[i] < min_w) {
          min_w = dist_to_tree[i];
          target = i;
        }
      }

      if (target == -1)
        break;  // no targeted vex found outside tree, which means the MST of
                // this component is complete

      in_tree[target] = true;
      if (parent[target] !=
          -1) {  // not the  first vex of this component, add edge
        result->edges[edge_count++] =
            (Edge){.t = parent[target], .h = target, .w = min_w};
      }

      // update recentlly added target's neighbors' dist_to_tree
      Edge neighbor = bg->qops.first_neighbor(G, target);
      while (neighbor.h >= 0) {
        Weight old = dist_to_tree[neighbor.h];
        if (old > neighbor.w) {
          dist_to_tree[neighbor.h] = neighbor.w;
          parent[neighbor.h] = target;
        }
        neighbor = bg->qops.next_neighbor(G, target, neighbor.h);
      }
    }

    result->component_count++;
    EnQueue(offset, &edge_count);
  }

  result->edge_count = edge_count;
  result->component_offset =
      (int*)malloc(result->component_count * sizeof(int));

  for (int i = 0; i < result->component_count; i++) {
    DeQueue(offset, &result->component_offset[i]);
  }
  assert(isSqQueueEmpty(offset));
  assert(result->edge_count == nv - result->component_count);
  DestorySqQueue(&offset);
  free(in_tree);
  free(dist_to_tree);
  free(parent);
  return result;
}

MSTResult* Kruskal(void* graph_context, const BaseGraph* graph_methods);
