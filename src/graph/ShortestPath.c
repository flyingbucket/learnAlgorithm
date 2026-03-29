#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"

/*
 * Dijkstra
 */
Weight* Dijkstra(const void* graph_context, const BaseGraph* graph_methods,
                 VertexId source) {
  Edge* all_edges = NULL;
  Weight* dist = NULL;
  Weight* res = NULL;
  bool* visited = NULL;
  if (graph_methods == NULL || graph_context == NULL) goto cleanup;
  const void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  if (!bg->iops.valid_vertex(G, source)) goto cleanup;

  int nv = bg->iops.vertex_count(G);
  int ne = (bg->iops.directed(G)) ? bg->iops.edge_count(G)
                                  : bg->iops.edge_count(G) / 2;

  // chekc wheather all edges' weight are non-negative
  all_edges = bg->qops.all_edges(G, true);
  if (all_edges == NULL) {
    fprintf(stderr, "Failed to get all edges from graph!\n");
    goto cleanup;
  }
  for (int i = 0; i < ne; i++) {
    if (all_edges[i].w < 0) {
      fprintf(stderr,
              "This graph hase NEGATIVE WEIGHT edge:\n"
              "tail:%d\t head:%d\t weight:%.2f\n",
              all_edges[i].t, all_edges[i].h, all_edges[i].w);
      goto cleanup;
    }
  }

  // init visited array
  visited = (bool*)calloc(nv, sizeof(bool));
  if (visited == NULL) goto cleanup;

  // init dist array
  dist = (Weight*)malloc(sizeof(Weight) * nv);
  if (dist == NULL) goto cleanup;
  for (VertexId i = 0; i < nv; i++) {
    dist[i] = INFINITY;
  }
  dist[source] = 0;

  // DP
  for (VertexId count = 0; count < nv; count++) {
    VertexId new_v = -1;
    Weight min_w = INFINITY;
    for (VertexId i = 0; i < nv; i++) {
      if (!visited[i] && dist[i] < min_w) {
        new_v = i;
        min_w = dist[i];
      }
    }
    if (new_v == -1) break;

    visited[new_v] = true;
    Edge new_neighbor = bg->qops.first_neighbor(G, new_v);
    while (new_neighbor.h != -1) {
      Weight new_dist = dist[new_v] + new_neighbor.w;
      if (new_dist < dist[new_v]) {
        fprintf(stderr, "Precision Overflow!\n");
        goto cleanup;
      }
      dist[new_neighbor.h] =
          (dist[new_neighbor.h] > new_dist) ? new_dist : dist[new_neighbor.h];
      new_neighbor = bg->qops.next_neighbor(G, new_v, new_neighbor.h);
    }
  }
  res = dist;
  dist = NULL;

cleanup:
  if (all_edges != NULL) free(all_edges);
  if (visited != NULL) free(visited);
  if (dist != NULL) free(dist);
  return res;
}

/*
 * Floyd
 */
Weight* Floyd(const void* graph_context, const BaseGraph* graph_methods) {
  Weight* res = NULL;
  Weight* mat = NULL;
  if (graph_methods == NULL || graph_context == NULL) goto cleanup;
  const void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int nv = bg->iops.vertex_count(G);
  mat = bg->qops.get_materix(G);
  if (mat == NULL) goto cleanup;
  for (int i = 0; i < nv; i++) {
    mat[i * nv + i] = 0;
  }
  for (int k = 0; k < nv; k++) {
    for (int i = 0; i < nv; i++) {
      if (mat[i * nv + k] == INFINITY) continue;
      for (int j = 0; j < nv; j++) {
        Weight dist = mat[i * nv + k] + mat[k * nv + j];
        if (dist < mat[i * nv + j]) {
          mat[i * nv + j] = dist;
        }
      }
    }
  }
  res = mat;
  mat = NULL;
cleanup:
  if (mat) free(mat);
  return res;
}
