#include <ctype.h>
#include <p_and_p.h>
#include <stdio.h>

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
int main() {
    struct Character c = {
        .characterID = 0,
        .socialClass = 5,
        .profession = "inn-keeper",
        .name = "Miiila",
        .inventorySize = 1,
        .inventory = {{0, 2}}};

    printf("socialClass = %d \n", c.socialClass);

    // size_t num;
    // printf("%ld", num);
    // // for (int i = 0; i < 128; i++) {
    // //     if (isgraph(i)) {
    // //         printf("%c ", i);
    // //     }
    // // }
    // printf("\n");

    return 0;
}