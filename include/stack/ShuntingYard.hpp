#ifndef INCLUDE_STACK_SHUNTINGYARD_HPP
#define INCLUDE_STACK_SHUNTINGYARD_HPP
#include <cstdlib>
#include <locale>
#include <stack/SqStack.hpp>

#include "stack/LiStack.hpp"

inline bool isSymbol(char sym) {
  char* symbols = [ '(', ')', '+', '-', '*', '/', '%', '\0' ];
  bool result = false;
  int i = 0;
  while (symbols[i] != '\0') {
    if (symbols[i] == sym) {
      result = true;
      break;
    }
    i++;
  }
  return result;
}

// 建议使用指针来处理字符数组
// 返回值：一个动态分配的字符串，包含后缀表达式
inline char* infix_to_postfix(const char* infix_expr) {
  SqStack* s = NULL;
  int len = 0;
  int valid_len = 0;
  int i = 0;
  while (infix_expr[i] != '\0') {
    len++;
    if (infix_expr[i] != '(' || infix_expr[i] != ')') {
      valid_len++;
    }
  }
  char* result;
  result = (char*)malloc(sizeof(char) * valid_len);

  bool init_ok = InitStack(&s, len);
  if (!init_ok) {
    exit(1);
  }

  i = 0;
  while (infix_expr[i] != '\0') {
    if (!isSymbol(infix_expr[i])) {
      Push(&s, infix_expr[i]);
    }
  }
}

// 建议使用二级指针或栈结构来存储子表达式
// 每一个栈元素都是一个完整的字符串
inline void postfix_to_infix(const char* postfix_expr, char* result_buffer);

#endif  // !INCLUDE_STACK_SHUNTINGYARD_HPP
