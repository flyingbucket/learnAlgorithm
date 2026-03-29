#include <catch2/catch.hpp>
#include <cmath>

extern "C" {
#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "graph/MGraph.h"
}

TEST_CASE("Dijkstra Shortest Path", "[dijkstra][algraph]") {
  SECTION("Normal Graph from Source") {
    ALGraph* g = algraph_init(4, true);
    for (int i = 0; i < 4; i++) g->bg.mops.add_vert(g);

    /* 构造图:
       0 --(4)--> 1 --(1)--> 3
        \--(1)--> 2 --(2)-/ (这里 2->1 权重 2, 2->3 权重 5)
    */
    g->bg.mops.add_edge(g, 0, 1, 4.0);
    g->bg.mops.add_edge(g, 0, 2, 1.0);
    g->bg.mops.add_edge(g, 2, 1, 2.0);
    g->bg.mops.add_edge(g, 2, 3, 5.0);
    g->bg.mops.add_edge(g, 1, 3, 1.0);

    Weight* dist = Dijkstra(g, &(g->bg), 0);
    REQUIRE(dist != nullptr);

    CHECK(dist[0] == Catch::Detail::Approx(0.0));
    CHECK(dist[1] == Catch::Detail::Approx(3.0));  // 0->2->1 = 3
    CHECK(dist[2] == Catch::Detail::Approx(1.0));  // 0->2 = 1
    CHECK(dist[3] == Catch::Detail::Approx(4.0));  // 0->2->1->3 = 4

    free(dist);
    algraph_destroy(g);
  }

  SECTION("Graph with Unreachable Vertex") {
    ALGraph* g = algraph_init(3, true);
    for (int i = 0; i < 3; i++) g->bg.mops.add_vert(g);

    g->bg.mops.add_edge(g, 0, 1, 2.0);
    // 节点 2 不可达

    Weight* dist = Dijkstra(g, &(g->bg), 0);
    REQUIRE(dist != nullptr);

    CHECK(dist[0] == Catch::Detail::Approx(0.0));
    CHECK(dist[1] == Catch::Detail::Approx(2.0));
    CHECK(std::isinf(dist[2]));  // Catch2 断言是否为无穷大

    free(dist);
    algraph_destroy(g);
  }
}

TEST_CASE("Floyd All-Pairs Shortest Path", "[floyd][mgraph]") {
  SECTION("Compute shortest paths between all pairs") {
    int nv = 4;
    MGraph* g = mgraph_init(nv);
    g->directed = true;

    // 手动打补丁：Floyd 依赖 INFINITY 表示无边，但 MGraph 默认是 0
    // 在实际生产代码中，你应该修改 mgraph_init 来处理这个问题
    for (int i = 0; i < nv * nv; i++) {
      if (i % (nv + 1) != 0) {
        g->adj[i] = INFINITY;
      }
    }

    g->bg.mops.add_edge(g, 0, 1, 2.0);
    g->bg.mops.add_edge(g, 0, 2, 6.0);
    g->bg.mops.add_edge(g, 1, 2, 3.0);
    g->bg.mops.add_edge(g, 2, 3, 1.0);

    Weight* mat = Floyd(g, &(g->bg));
    REQUIRE(mat != nullptr);

    // 0 -> 2 的捷径是 0 -> 1 -> 2 (权重为 2+3=5)
    CHECK(mat[0 * nv + 2] == Catch::Detail::Approx(5.0));
    // 0 -> 3 的最短路是 0 -> 1 -> 2 -> 3 (权重为 2+3+1=6)
    CHECK(mat[0 * nv + 3] == Catch::Detail::Approx(6.0));
    // 节点到自身的距离为 0
    CHECK(mat[1 * nv + 1] == Catch::Detail::Approx(0.0));
    // 3 -> 0 是不可达的
    CHECK(std::isinf(mat[3 * nv + 0]));

    free(mat);
    mgraph_destroy(g);
  }
}
