#include <stdio.h>

int foo(int bar) {
    printf("%d\n", bar);
    return 0;
}

int main(int argc, char** argv) {
    int a;
    scanf("%d", &a);
    a += 1;
    printf("%d\n", a);
    foo(2);
}
