#include <stdio.h>
#include <ctype.h>

int main() {
    for (int i = 0; i < 128; i++) {
        if (isgraph(i)) {
            printf("%c ", i);
        }
    }
    printf("\n");
    return 0;
}