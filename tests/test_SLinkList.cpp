#include "catch2/catch.hpp"
#include "linkedlist/SLinkList.hpp"

TEST_CASE("SLinkList: Basic Operations", "[slink]") {
  LinkList L;
  REQUIRE(InitList(L) == true);
  REQUIRE(L != nullptr);

  SECTION("Insertion and Length") {
    REQUIRE(ListInsert(L, 1, 10) == true);  // [10]
    REQUIRE(ListInsert(L, 2, 20) == true);  // [10, 20]
    REQUIRE(ListInsert(L, 2, 15) == true);  // [10, 15, 20]
    REQUIRE(LengthWithoutHeadNode(L) == 3);
  }

  SECTION("Search") {
    ListInsert(L, 1, 10);
    ListInsert(L, 2, 20);

    LNode* target = LocateElem(L, 20);
    REQUIRE(target != nullptr);
    CHECK(target->data == 20);

    LNode* byIndex = GetElem(L, 1);
    REQUIRE(byIndex != nullptr);
    CHECK(byIndex->data == 10);
  }

  SECTION("Deletion") {
    ListInsert(L, 1, 10);
    ListInsert(L, 2, 20);
    REQUIRE(ListDelete(L, 1) == true);  // 删除 10
    CHECK(LengthWithoutHeadNode(L) == 1);
    CHECK(GetElem(L, 1)->data == 20);
  }

  // 内存释放逻辑虽在hpp中未提供Destroy，但此处为示例
}
