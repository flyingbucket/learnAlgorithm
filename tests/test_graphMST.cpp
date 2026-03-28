#include <algorithm>
#include <catch2/catch.hpp>
#include <cstdio>
#include <vector>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"

/**
 * @brief 构造测试图的辅助模板
 * * 逻辑修正：针对无向图（MST的主要场景），必须成对添加边 (u,v) 和 (v,u)，
 * 这样 ALGraph.c 中的 all_edges (n_edges / 2) 逻辑和邻接表遍历才能正确工作。
 */
template <typename T>
T* BuildTestGraph(int nv, const std::vector<Edge>& edges, bool is_directed,
                  T* (*init)(int, bool), void (*destroy)(T*)) {
  // 1. 调用更新后的 init
  T* g = init(nv, is_directed);
  if (!g) return nullptr;

  BaseGraph* bg = (BaseGraph*)g;

  // 2. 添加顶点
  for (int i = 0; i < nv; ++i) {
    if (bg->mops.add_vert) bg->mops.add_vert(g);
  }

  // 3. 添加边
  for (const auto& e : edges) {
    if (bg->mops.add_edge) {
      // 添加主向边
      bg->mops.add_edge(g, e.t, e.h, e.w);
      // 如果是无向图，添加反向边以维持对称性
      if (!is_directed) {
        bg->mops.add_edge(g, e.h, e.t, e.w);
      }
    }
  }
  return g;
}

/**
 * @brief 计算生成森林的总权重
 */
double GetTotalWeight(const MSTResult* res) {
  if (!res) return 0.0;
  double sum = 0;
  for (int i = 0; i < res->edge_count; ++i) {
    sum += res->edges[i].w;
  }
  return sum;
}

TEST_CASE("MST Algorithms: Adjacency List (ALGraph)", "[ALGraph][MST]") {
  // 对于 MST，通常使用无向图 (directed = false)
  const bool is_directed = false;

  SECTION("Connected Graph: Classic Triangle") {
    // 0-1 (1.0), 1-2 (2.0), 0-2 (0.5)
    // 预期 MST: 0-1 (1.0) 和 0-2 (0.5), 总权重 1.5
    std::vector<Edge> edges = {{0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 0.5}};
    ALGraph* g =
        BuildTestGraph(3, edges, is_directed, algraph_init, algraph_destroy);

    REQUIRE(g != nullptr);

    MSTResult* res_k = Kruskal(g, &g->bg);
    MSTResult* res_p = Prim(g, &g->bg);

    REQUIRE(res_k != nullptr);
    REQUIRE(res_p != nullptr);

    // 验证连通性
    CHECK(res_k->component_count == 1);
    CHECK(res_p->component_count == 1);

    // 验证权重
    CHECK(GetTotalWeight(res_k) == Approx(1.5));
    CHECK(GetTotalWeight(res_p) == Approx(1.5));

    // 验证边数：3个顶点连通图 MST 应有 2 条边
    CHECK(res_k->edge_count == 2);

    DestroyMSTResult(&res_k);
    DestroyMSTResult(&res_p);
    algraph_destroy(g);
  }

  SECTION("Disconnected Forest: Two Separate Components") {
    // C1: 0-1 (10.0)
    // C2: 2-3 (5.0), 3-4 (2.0)
    // 预期: 2个分量, 3条边, 总重 17.0
    std::vector<Edge> edges = {{0, 1, 10.0}, {2, 3, 5.0}, {3, 4, 2.0}};
    ALGraph* g =
        BuildTestGraph(5, edges, is_directed, algraph_init, algraph_destroy);

    MSTResult* res_k = Kruskal(g, &g->bg);
    MSTResult* res_p = Prim(g, &g->bg);

    REQUIRE(res_k != nullptr);
    REQUIRE(res_p != nullptr);

    CHECK(res_k->component_count == 2);
    CHECK(res_p->component_count == 2);
    CHECK(res_k->edge_count == 3);
    CHECK(GetTotalWeight(res_k) == Approx(17.0));

    // 验证分量偏移量
    // 第一个分量 (0,1) -> 1条边；第二个分量 (2,3,4) -> 2条边
    // Kruskal 可能会根据边权排序，所以偏移量顺序可能不同，但总数应一致
    int c1_edges = res_k->component_offset[1] - res_k->component_offset[0];
    int c2_edges = res_k->component_offset[2] - res_k->component_offset[1];
    CHECK(
        ((c1_edges == 1 && c2_edges == 2) || (c1_edges == 2 && c2_edges == 1)));

    DestroyMSTResult(&res_k);
    DestroyMSTResult(&res_p);
    algraph_destroy(g);
  }
}

TEST_CASE("MST Cross-Model Consistency", "[mst]") {
  std::vector<Edge> edges = {{0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 0.5}};

  SECTION("ALGraph Model Consistency") {
    auto* g =
        BuildTestGraph<ALGraph>(3, edges, false, algraph_init, algraph_destroy);
    REQUIRE(g != nullptr);

    MSTResult* res = Kruskal(g, &g->bg);
    REQUIRE(res != nullptr);
    CHECK(res->edge_count == 2);
    CHECK(GetTotalWeight(res) == Approx(1.5));

    algraph_destroy(g);
    DestroyMSTResult(&res);
  }
}
