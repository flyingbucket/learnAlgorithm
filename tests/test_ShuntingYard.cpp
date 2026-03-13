#include "catch2/catch.hpp"
#include "stack/ShuntingYard.hpp"

TEST_CASE("中缀转后缀转换正确性验证", "[infix_to_postfix]") {
  // 基础运算
  REQUIRE(std::string(infix_to_postfix("a+b")) == "ab+");

  // 优先级处理
  REQUIRE(std::string(infix_to_postfix("a+b*c")) == "abc*+");

  // 嵌套括号处理
  REQUIRE(std::string(infix_to_postfix("(a+b)*c")) == "ab+c*");
  REQUIRE(std::string(infix_to_postfix("a*(b+c*d)")) == "abcd*+*");
}

// TEST_CASE("后缀转中缀转换正确性验证", "[postfix_to_infix]") {
//   char buffer[256];  // 假设你有足够的缓冲区空间
//
//   // 简单验证
//   postfix_to_infix("ab+", buffer);
//   REQUIRE(std::string(buffer) == "(a+b)");
//
//   // 复杂验证
//   postfix_to_infix("abc*+", buffer);
//   REQUIRE(std::string(buffer) == "(a+(b*c))");
//
//   // 深度验证
//   postfix_to_infix("abcd*+*", buffer);
//   REQUIRE(std::string(buffer) == "(a*(b+(c*d)))");
// }
