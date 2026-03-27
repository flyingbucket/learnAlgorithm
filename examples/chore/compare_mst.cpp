#include <random>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"

template <typename GType>
void SetupRandomGraph(GType* g, int nv, int ne, BaseGraph* bg) {
  for (int i = 0; i < nv; ++i) bg->mops.add_vert(g);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, nv - 1);
  std::uniform_real_distribution<double> weight_dist(1.0, 10.0);

  for (int i = 0; i < ne; ++i) {
    bg->mops.add_edge(g, dist(rng), dist(rng), weight_dist(rng));
  }
}

int main() {
  int nv = 2000, ne = 1000;

  // 1. 初始化
  ALGraph* g = algraph_init(nv);

  // 2. 关键！必须明确图的类型，否则 all_edges 会分配错误的大小
  g->directed = true;

  printf("Initializing Graph: V=%d, E=%d...\n", nv, ne);
  SetupRandomGraph(g, nv, ne, &g->bg);

  printf("Running Kruskal...\n");
  MSTResult* res = Kruskal(g, &g->bg);

  if (res) {
    printf("Kruskal Finished. MST edge count: %.d\n", res->edge_count);
    DestroyMSTResult(&res);
  } else {
    printf("Kruskal failed to find an MST (Graph might be disconnected).\n");
  }

  algraph_destroy(g);
  printf("Done.\n");
  return 0;
}
