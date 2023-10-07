#include <stdio.h>
#include <ctype.h>
#include <p_and_p.h>


 struct Character arr[] = { {
  .characterID = 1,
  .socialClass = MERCHANT,
  .profession = "inn-keeper",
  .name = "Edgar Crawford",
  .inventorySize = 1,
  .inventory = {
    { .itemID = 200648657395984580,
      .quantity = 1
    }
  }
} };
const int expected_result = 1;
int actual_result = saveCharacters(arr, 1, 99);
ck_assert_int_eq(actual_result, expected_result);













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