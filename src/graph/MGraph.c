#include "graph/MGraph.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static int m_vertex_count(const void* G) {
  MGraph* g = (MGraph*)G;
  return g->n_verts;
}
static int m_edge_count(const void* G) {
  if (G == NULL) return -1;
  MGraph* g = (MGraph*)G;
  return g->n_edges;
}
static bool m_valid_vertex(const void* G, VertexId v) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v >= 0 && v < n)
    return true;
  else
    return false;
}
static bool m_direct(const void* G) {
  assert(G != NULL && "Graph pointer cannot be NULL");
  MGraph* g = (MGraph*)G;

  return g->directed;
}
static const GraphInfoOps MGRAPH_IOPS = {
    .vertex_count = m_vertex_count,
    .edge_count = m_edge_count,
    .valid_vertex = m_valid_vertex,
    .directed = m_direct,
};

static bool m_adjacent(const void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  Weight(*adj)[n] = (Weight(*)[n])g->adj;
  return adj[v1][v2] != 0;
}
static Edge m_first_neighbor(const void* G, VertexId v) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v > n * n) return invalid_edge;
  for (int h = 0; h < n; ++h) {
    if (g->adj[v * n + h] != 0) {
      Edge res = {
          .t = v,
          .h = h,
          .w = g->adj[v * n + h],
      };
      return res;
    };
  }
  return invalid_edge;
}
static Edge m_next_neighbor(const void* G, VertexId v, VertexId w) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  for (int x = w + 1; x < n; ++x) {
    if (g->adj[v * n + x] != 0) {
      Edge res = {
          .t = v,
          .h = x,
          .w = g->adj[v * n + x],
      };
      return res;
    }
  }
  return invalid_edge;
}
static Edge* m_all_edges(const void* G, bool filter) {
  if (!G) return NULL;
  MGraph* g = (MGraph*)G;

  // 确定返回的边数量
  // 如果是无向图且开启过滤，则边数减半（因为矩阵中存了两份）
  int n_edges = (g->directed || !filter) ? g->n_edges : g->n_edges / 2;

  Edge* edges = (Edge*)malloc(sizeof(Edge) * n_edges);
  if (!edges) return NULL;

  int e_count = 0;
  for (int i = 0; i < g->n_verts; i++) {
    // 如果是无向图且需要过滤，j 从 i + 1 开始，只看上三角
    int start_j = (!g->directed && filter) ? (i + 1) : 0;

    for (int j = start_j; j < g->n_verts; j++) {
      Weight w = g->adj[i * g->n_verts + j];

      // 假设权重不为0表示存在边（请根据你的业务逻辑修改判断条件，如 w < INF）
      if (w != 0) {
        // 进一步检查：如果是无向图但未开启 filter，i < j
        // 的逻辑已由循环范围或下述判断覆盖 这里保持与你 ALGraph
        // 类似的健壮性判断
        if (g->directed || !filter || i < j) {
          if (e_count < n_edges) {
            edges[e_count].t = (VertexId)i;
            edges[e_count].h = (VertexId)j;
            edges[e_count].w = w;
            e_count++;
          }
        }
      }
    }
  }
  return edges;
}
static Weight* m_get_materix(const void* G) {
  if (G == NULL) return NULL;
  MGraph* g = (MGraph*)G;
  size_t size = sizeof(Weight) * g->n_verts * g->n_verts;
  Weight* mat = (Weight*)malloc(size);
  if (mat == NULL) return NULL;
  memcpy(mat, g->adj, size);
  return mat;
}
static const GraphQueryOps MGRAPH_QOPS = {
    .adjacent = m_adjacent,
    .first_neighbor = m_first_neighbor,
    .next_neighbor = m_next_neighbor,
    .all_edges = m_all_edges,
    .get_materix = m_get_materix,
};

static VertexId m_add_vert(void* G) {
#ifndef NDEBUG
  printf(
      "Warning: MGraph does not support dynamic scaling. This function does "
      "nothing.\nIf you are calling this function in the init process, it's "
      "all fine because all information is stored in the adjcancy matrix and "
      "added to the graph through function `add_edge`.\n");
#endif  // !NDEBUG
  (void)G;
  return -1;
}
static bool m_delete_vert(void* G, VertexId v) {
  printf(
      "Warning: MGraph does not support dynamic expansion. This function does "
      "nothing\n");
  (void)G;
  (void)v;
  return false;
}
static bool m_add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops.valid_vertex(G, v1) && g->bg.iops.valid_vertex(G, v2) &&
      v1 != v2) {
    g->adj[v1 * n + v2] = w;
    return true;
  }
  return false;
}
static bool m_remove_edge(void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops.valid_vertex(G, v1) && g->bg.iops.valid_vertex(G, v2) &&
      v1 != v2) {
    g->adj[v1 * n + v2] = 0;
    return true;
  }
  return false;
}
static const GraphMutateOps MGRAPH_MOPS = {
    .add_vert = m_add_vert,
    .delete_vert = m_delete_vert,
    .add_edge = m_add_edge,
    .remove_edge = m_remove_edge,
};

static Weight m_update_edge_weight(void* G, VertexId v1, VertexId v2,
                                   Weight w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  Weight old = g->bg.wops.get_edge_weight(G, v1, v2);
  g->adj[v1 * n + v2] = w;
  return old;
}
static Weight m_get_edge_weight(const void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (!g->bg.iops.valid_vertex(G, v1) || !g->bg.iops.valid_vertex(G, v1) ||
      v1 == v2)
    return -1;
  Weight w = g->adj[v1 * n + v2];
  return w;
}
static const WeightedGraphOps MGRAPH_WOPS = {
    .get_edge_weight = m_get_edge_weight,
    .update_edge_weight = m_update_edge_weight,
};

MGraph* mgraph_init(int n_verts) {
  MGraph* g = (MGraph*)malloc(sizeof(MGraph));
  if (!g) return NULL;

  g->n_verts = n_verts;
  g->n_edges = 0;
  g->verts = NULL;
  g->adj = (Weight*)calloc((size_t)n_verts * (size_t)n_verts, sizeof(Weight));
  if (!g->adj) {
    free(g);
    return NULL;
  }

  BaseGraph initial_bg = {.iops = MGRAPH_IOPS,
                          .qops = MGRAPH_QOPS,
                          .mops = MGRAPH_MOPS,
                          .wops = MGRAPH_WOPS};

  g->bg = initial_bg;
  return g;
}

void mgraph_destroy(MGraph* g) {
  if (!g) return;
  free(g->adj);
  free(g);
}
