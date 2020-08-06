#include <stdio.h>

extern void assFunc(int x, int y);

extern char c_checkValidity(int x, int y) {
if (x>=y)
  return '1';
return '0';
}

int main(int argc, char** argv) {
  int x, y;

  scanf("%d", &x);
  scanf("%d", &y);
  assFunc(x,y);

  return 0;
}
