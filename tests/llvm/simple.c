#include <stdio.h>
#include <string.h>

int foo(int bar) {
    printf("%d\n", bar);
    return 0;
}

int main(int argc, char** argv) {
    int a;
    char c[10];
    scanf("%d%s", &a, c);
    a += 1;
    size_t b = strlen(c);
    a += b;
    printf("%d\n", a);
    foo(2);

}
