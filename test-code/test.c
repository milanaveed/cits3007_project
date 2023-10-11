#include <ctype.h>
#include <p_and_p.h>
#include <stdio.h>

// // Reading the fixed part
// if (fread(currentCharacter, bytesOfFixedFields, 1, fp) != 1) {
//     free(*ptr);
//     *ptr = NULL; // Avoid dangling pointer
//     if (fclose(fp) != 0) {
//         return ERR_CLOSE_FILE;
//     }
//     return ERR_FILE_CORRUPTION;
// }

// // Reading the flexible part
// for (size_t j = 0; j < currentCharacter->inventorySize; ++j) {
//     if (fread(&currentCharacter->inventory[j], sizeof(struct ItemCarried), 1, fp) != 1) {
//         free(*ptr);
//         *ptr = NULL; // Avoid dangling pointer
//         if (fclose(fp) != 0) {
//             return ERR_CLOSE_FILE;
//         }
//         return ERR_FILE_CORRUPTION;
//     }
// }

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// int main() {
//     size_t num;
//     printf("%ld", num);
//     // for (int i = 0; i < 128; i++) {
//     //     if (isgraph(i)) {
//     //         printf("%c ", i);
//     //     }
//     // }
//     printf("\n");
//     return 0;
// }