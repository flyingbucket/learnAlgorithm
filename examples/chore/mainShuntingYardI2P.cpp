#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack/ShuntingYard.hpp"

int main() {
  char infix[1024];

  printf("Enter infix format: ");
  if (fgets(infix, sizeof(infix), stdin) == NULL) {
    return 1;
  }

  infix[strcspn(infix, "\n")] = '\0';

  size_t required_size = infix_to_postfix(infix, NULL);
  if (required_size == 0) {
    fprintf(stderr, "Error: Invalid expression\n");
    return 1;
  }

  char* res_buff = (char*)malloc(sizeof(char) * required_size);
  if (res_buff == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return 1;
  }

  infix_to_postfix(infix, res_buff);

  printf("The postfix format is: %s\n", res_buff);

  free(res_buff);

  return 0;
}
