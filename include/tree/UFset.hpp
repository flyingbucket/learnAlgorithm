#ifndef INCLUDE_TREEUFSET_HPP
#define INCLUDE_TREEUFSET_HPP
#include <cstddef>
#include <cstdlib>
typedef int* UFset;

inline bool Initial(UFset* s, uint len) {
  UFset p = (UFset)malloc(sizeof(int) * len);
  if (p == NULL) return false;
  for (uint i = 0; i < len; i++) {
    p[i] = -1;
  }
  *s = p;
  return true;
}

inline void Union(UFset s, int root1, int root2) {
  if (root1 == root2) return;
  // merge to bigger tree
  if (s[root1] < s[root2]) {
    s[root1] += s[root2];
    s[root2] = root1;
  } else {
    s[root2] += s[root1];
    s[root1] = root2;
  }
}

inline int Find(UFset s, int node) {
  int root = node;
  while (s[root] >= 0) root = s[root];
  // flatten
  while (node != root) {
    int tmp = s[node];
    s[node] = root;
    node = tmp;
  }
  return root;
}
#endif  // !INCLUDE_TREEUFSET_HPP
