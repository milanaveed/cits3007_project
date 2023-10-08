#include <ctype.h>
#include <p_and_p.h>
#include <stdio.h>
// #tcase saveCharacters_testcase

// #test saveCharacters_works
struct Character chaArr[] = {{.characterID = 1,
                              .socialClass = MERCHANT,
                              .profession = "inn-keeper",
                              .name = "Edgar Crawford",
                              .inventorySize = 2,
                              .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
                             {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
                             {.characterID = 1, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 1}}},
                             {.characterID = 1, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 1}}}};

size_t chaArr_size = sizeof(chaArr) / sizeof(struct Character);

char *file_conts = NULL;
size_t file_size = 0;

FILE *ofp = fopen("characters04.dat", "wb");
assert(ofp != NULL);

int fd = fileno(ofp);
assert(fd != -1);
int res = saveCharacters(chaArr, chaArr_size, fd);
assert(res == 0);
fclose(ofp);

res = slurp_file("characters04.dat", "rb", &file_conts, &file_size);
assert(res == 0);

const size_t expected_size = sizeof(uint64_t) + sizeof(chaArr);

fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// metadata should be `1`
size_t actual_read_metadata = 0;
memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");

// following the metadata should be our struct
struct Character actual_read_item1 = {0};
memcpy(&actual_read_item1, file_conts + sizeof(size_t), sizeof(struct Character));

assert_characters_are_equal(&actual_read_item1, &(chaArr[0]));

struct Character actual_read_item2 = {0};
memcpy(&actual_read_item2, file_conts + sizeof(size_t) + sizeof(struct Character), sizeof(struct Character));

if (file_conts != NULL)
    free(file_conts);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#tcase loadCharacters_testcase
struct Character items001_expectedCharacters[] =
    {{.characterID = 1,
      .socialClass = MERCHANT,
      .profession = "inn-keeper",
      .name = "Edgar Crawford",
      .inventorySize = 2,
      .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
     {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
     {.characterID = 1, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 1}}},
     {.characterID = 1, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 1}}}};

size_t items001_expectedSize = sizeof(items001_expectedCharacters) / sizeof(struct Character);

#test loadCharacters_items001_size_is_correct

const char *infile_path = "characters04.dat";
int fd = open_with_fileno(infile_path);

size_t numCharacters = 0;
struct Character *chaArr = NULL;
int res = loadCharacters(&chaArr, &numCharacters, fd);

if (res != 0)
    die_perror(__FILE__, __LINE__, "loadCharacters failed");

// check we got the expected number of items
ck_assert_msg(numCharacters == items001_expectedSize, "numCharacters should equal items001_expectedSize");

free(chaArr);

static_assert(sizeof(items001_expectedCharacters) / sizeof(struct Character) == 4,
              "expect correct number of Characters");

// test each item is correct, from idx 0 through to 3
// loop index is available as `_i`

#test-loop (0,4) loadItemDetails_items001_items_are_equal

const char * infile_path = "characters04.dat";
int fd = open_with_fileno(infile_path);

size_t numCharacters = 0;
struct Character * chaArr = NULL;
int res = loadCharacters(&chaArr, &numCharacters, fd);

if (res != 0)
    die_perror(__FILE__, __LINE__, "loadCharacters failed");

// pre-requisite: we got the expected number of items
assert(numCharacters == items001_expectedSize);

fprintf(stderr, "checking equality of Character %d, with expected characterID %zu\n",
        _i, items001_expectedCharacters[_i].characterID
);
assert_itemDetails_are_equal(chaArr + _i, &(items001_expectedCharacters[_i]));

free(chaArr);

///////////////////////////////////////////////////////////
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