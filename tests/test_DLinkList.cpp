#include "catch2/catch.hpp"
#include "linkedlist/DLinkList.hpp"

TEST_CASE("DLinkList: Prior Operations", "[dlink]") {
  DLinkList L;
  REQUIRE(InitDList(L) == true);

  SECTION("InsertPrior Logic") {
    InsertPrior(L, 1, 10);  // [10]
    InsertPrior(L, 2, 20);  // [10, 20]
    InsertPrior(L, 2, 15);  // [10, 15, 20]
    REQUIRE(LengthWithoutHeadNode(L) == 3);
    CHECK(GetElem(L, 1)->data == 10);
    CHECK(GetElem(L, 2)->data == 15);
  }

  SECTION("DeletePrior Logic") {
    InsertPrior(L, 1, 10);  // index 1: 10
    InsertPrior(L, 2, 20);  // index 2: 20
    InsertPrior(L, 2, 15);  // index 1: 10, index 2: 15, index 3: 20

    int deleted;
    // 删除第2个节点(15)的前一个节点，即第1个节点(10)
    REQUIRE(DeletePrior(L, 2, deleted) == true);
    CHECK(deleted == 10);
    CHECK(GetElem(L, 1)->data == 15);
  }

  SECTION("Boundary Conditions") {
    int val;
    CHECK(DeletePrior(L, 1, val) == false);  // 不能删除头节点
    CHECK(InsertPrior(L, 0, val) == false);  // 非法索引
  }
}
