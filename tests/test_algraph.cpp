#include <catch2/catch.hpp>

#include "graph/ALGraph.h"
#include "graph/GraphOps.h"

TEST_CASE("ALGraph Operations", "[ALGraph]") {
  // 创建一个 ALGraph，预分配 4 个顶点空间
  ALGraph* g = algraph_create(4, 0);
  REQUIRE(g != nullptr);

  SECTION("Vertex Operations: Add and Delete") {
    REQUIRE(g->bg.iops.vertex_count(g) == 0);

    // 添加 3 个顶点
    VertexId v0 = g->bg.mops.add_vert(g);
    VertexId v1 = g->bg.mops.add_vert(g);
    VertexId v2 = g->bg.mops.add_vert(g);

    REQUIRE(v0 == 0);
    REQUIRE(v1 == 1);
    REQUIRE(v2 == 2);
    REQUIRE(g->bg.iops.vertex_count(g) == 3);
    REQUIRE(g->bg.iops.valid_vertex(g, v1) == true);
    REQUIRE(g->bg.iops.valid_vertex(g, 99) == false);  // 无效顶点

    // 删除顶点 v1
    bool deleted = g->bg.mops.delete_vert(g, v1);
    REQUIRE(deleted == true);
    REQUIRE(g->bg.iops.vertex_count(g) == 2);
  }

  SECTION("Edge Operations: Add, Remove and Weight") {
    g->bg.mops.add_vert(g);  // 0
    g->bg.mops.add_vert(g);  // 1
    g->bg.mops.add_vert(g);  // 2

    // 添加边 0 -> 1，权重 2.5
    bool edge_added = g->bg.mops.add_edge(g, 0, 1, 2.5);
    REQUIRE(edge_added == true);

    // 添加边 0 -> 2，权重 3.0
    g->bg.mops.add_edge(g, 0, 2, 3.0);

    // 验证相邻关系
    REQUIRE(g->bg.qops.adjacent(g, 0, 1) == true);
    REQUIRE(g->bg.qops.adjacent(g, 1, 0) == false);  // 有向图
    REQUIRE(g->bg.qops.adjacent(g, 0, 2) == true);

    // 验证权重读取
    Weight w = g->bg.wops.get_edge_weight(g, 0, 1);
    REQUIRE(w == 2.5);

    // 验证权重更新
    Weight old_w = g->bg.wops.update_edge_weight(g, 0, 1, 5.0);
    REQUIRE(old_w == 2.5);
    REQUIRE(g->bg.wops.get_edge_weight(g, 0, 1) == 5.0);

    // 删除边 0 -> 1
    bool edge_removed = g->bg.mops.remove_edge(g, 0, 1);
    REQUIRE(edge_removed == true);
    REQUIRE(g->bg.qops.adjacent(g, 0, 1) == false);
  }

  SECTION("Query Operations: First and Next Neighbor") {
    g->bg.mops.add_vert(g);  // 0
    g->bg.mops.add_vert(g);  // 1
    g->bg.mops.add_vert(g);  // 2

    g->bg.mops.add_edge(g, 0, 1, 1.0);
    g->bg.mops.add_edge(g, 0, 2, 1.0);

    // ALGraph 头插法：后添加的边在链表头部，所以首个邻居是 2
    int first = g->bg.qops.first_neighbor(g, 0);
    REQUIRE(first == 2);

    // 2 之后的下一个邻居是 1
    int next = g->bg.qops.next_neighbor(g, 0, 2);
    REQUIRE(next == 1);

    // 1 之后没有邻居了
    int none = g->bg.qops.next_neighbor(g, 0, 1);
    REQUIRE(none == -1);
  }

  algraph_destroy(g);
}
