//////////////////////////
// DO NOT SUBMIT THIS FILE
//////////////////////////

// Command for make test:
//$ make test
// Command for more readable test result:
//$ make all && CK_TAP_LOG_FILE_NAME=- prove --verbose ./check_p_and_p


#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <p_and_p.h>

#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// useful if you want to coredump
// yourself -- see `man raise`
//#include <signal.h>

int open_with_fileno(const char * infile_path) {
  FILE *ifp = fopen(infile_path, "rb");
  if (ifp == NULL)
     die_perror(__FILE__, __LINE__, "couldn't open file")

  int fd = fileno(ifp);

  if (fd == -1)
    die_perror(__FILE__, __LINE__, "couldn't get fd for file");

  return fd;
}


#suite p_and_p_tests


// #tcase arithmetic_testcase
// ^ this isn't a "real" test-case -- it's just used as an illustration
// of how test-cases are written.

// #test arithmetic_works
//     int m = 3;
//     int n = 4;
//     int expected = 7;
//     int actual = m + n;
//     ck_assert_int_eq(actual, expected);

//////////////////////////////////////////////////
// Uncomment the following to see AddressSanitizer
// in action:
//////////////////////////////////////////////////

// #tcase deliberately_bad_testcase

// #test go_out_of_bounds
//    char buf[]            = "supercallifragilistic";
//    const size_t buf_size = sizeof(buf)/sizeof(buf[0]);
//    buf[buf_size-1]       = 'X';
//    // ^ buf is now not nul-terminated
//    size_t len            = strlen(buf);
//    // we expect the strlen-calculated size
//    // to be greater than 0.
//    ck_assert_uint_gt(len, 0);

// #tcase loadItemDetails_items001_works_testcase

//    struct ItemDetails items001_expectedItems[] = {
//      { .itemID = 16602759796824695000UL, .name = "telescope",      .desc = "brass with wooden tripod, 25x30x60 in." },
//      { .itemID = 13744653742375254000UL, .name = "rope",           .desc = "hemp, 50 ft." },
//      { .itemID = 3400701144194139000UL,  .name = "music-box",      .desc = "brass gears with tin-plated case, 6 in., cursed" },
//      { .itemID = 734628920708863200UL,   .name = "billiard-ball",  .desc = "ivory, 2 in., set of 16 in wooden case, of mysterious origin" },
//      { .itemID = 14734865628490349000UL, .name = "sword-cane",     .desc = "steel-bladed, concealed in Malacca-wood walking-cane, 36 in." },
//      { .itemID = 14324391740292973000UL, .name = "dynamite-stick", .desc = "with paper wrapping, 1 in. diameter x 12 in." },
//      { .itemID = 7562791295545618000UL,  .name = "Epsom-salts",    .desc = "6 oz, in glass bottle with cork stopper" },
//      { .itemID = 13658877949582529000UL, .name = "camp-stool",     .desc = "canvas and wood, 12 in. seat height" },
//      { .itemID = 2390949174291477500UL,  .name = "slide-rule",     .desc = "wood and brass, 12 in., cursed" },
//    };

//    size_t items001_expectedSize = sizeof(items001_expectedItems)/sizeof(struct ItemDetails);


// #test loadItemDetails_items001_size_is_correct

//    const char * infile_path = "test-data/items001.dat";
//    int fd = open_with_fileno(infile_path);

//    size_t numItems = 0;
//    struct ItemDetails * itemsArr = NULL;
//    int res = loadItemDetails(&itemsArr, &numItems, fd);

//    if (res != 0)
//      die_perror(__FILE__, __LINE__, "loadItems failed");

//    // check we got the expected number of items
//    ck_assert_msg(numItems == items001_expectedSize, "numItems should equal items001_expectedSize");

//    free(itemsArr);

// static_assert( sizeof(items001_expectedItems)/sizeof(struct ItemDetails) == 9 ,
//               "expect correct number of file 001 items" );

// // test each item is correct, from idx 0 through to 8
// // loop index is available as `_i`

// #test-loop (0,9) loadItemDetails_items001_items_are_equal

//    const char * infile_path = "test-data/items001.dat";
//    int fd = open_with_fileno(infile_path);

//    size_t numItems = 0;
//    struct ItemDetails * itemsArr = NULL;
//    int res = loadItemDetails(&itemsArr, &numItems, fd);

//    if (res != 0)
//      die_perror(__FILE__, __LINE__, "loadItems failed");

//    // pre-requisite: we got the expected number of items
//    assert(numItems == items001_expectedSize);

//    fprintf(stderr, "checking equality of item %d, with expected itemID %zu\n",
//            _i, items001_expectedItems[_i].itemID
//    );
//    assert_itemDetails_are_equal(itemsArr + _i, &(items001_expectedItems[_i]));

//    free(itemsArr);

////////////////////////////////////////////////////////////
////loadCharacter()
////////////////////////////////////////////////////////////

// #tcase loadCharacters_testcase
// struct Character items001_expectedCharacters[] =
//     {{.characterID = 1,
//       .socialClass = MERCHANT,
//       .profession = "inn-keeper",
//       .name = "Edgar Crawford",
//       .inventorySize = 2,
//       .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
//      {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
//      {.characterID = 1, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 1}}},
//      {.characterID = 1, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 1}}}};

// size_t items001_expectedSize = sizeof(items001_expectedCharacters) / sizeof(struct Character);

// #test loadCharacters_items001_size_is_correct

// const char *infile_path = "characters04.dat";
// int fd = open_with_fileno(infile_path);

// size_t numCharacters = 0;
// struct Character *chaArr = NULL;
// int res = loadCharacters(&chaArr, &numCharacters, fd);

// if (res != 0)
//     die_perror(__FILE__, __LINE__, "loadCharacters failed");

// // check we got the expected number of items
// ck_assert_msg(numCharacters == items001_expectedSize, "numCharacters should equal items001_expectedSize");

// free(chaArr);

// static_assert(sizeof(items001_expectedCharacters) / sizeof(struct Character) == 4,
//               "expect correct number of Characters");


// test each item is correct, from idx 0 through to 3
// loop index is available as `_i`

// #test-loop (0,4) loadItemDetails_items001_items_are_equal

// const char * infile_path = "characters04.dat";
// int fd = open_with_fileno(infile_path);

// size_t numCharacters = 0;
// struct Character * chaArr = NULL;
// int res = loadCharacters(&chaArr, &numCharacters, fd);

// if (res != 0)
//     die_perror(__FILE__, __LINE__, "loadCharacters failed");

// // pre-requisite: we got the expected number of items
// assert(numCharacters == items001_expectedSize);

// fprintf(stderr, "checking equality of Character %d, with expected characterID %zu\n",
//         _i, items001_expectedCharacters[_i].characterID
// );
// assert_characters_are_equal(chaArr + _i, &(items001_expectedCharacters[_i]));

// free(chaArr);

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// #tcase loadCharacters_testcase
// struct Character characters001_expectedCharacters[] =
//     {{.characterID = 1,
//                               .socialClass = MERCHANT,
//                               .profession = "inn-keeper",
//                               .name = "Edgar Crawford",
//                               .inventorySize = 2,
//                               .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
//                              {.characterID = 2, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 2}}},
//                              {.characterID = 3, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 3}}},
//                              {.characterID = 4, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984584, .quantity = 4}}}};

// size_t characters001_expectedSize = sizeof(characters001_expectedCharacters) / sizeof(struct Character);
// // printf("expectedSize = %ld bytes\n", characters001_expectedSize);

// #test loadCharacters_characters001_size_is_correct

// const char *infile_path = "characters05.dat";
// int fd = open_with_fileno(infile_path);

// size_t numCharacters = 0;
// struct Character *chaArr = NULL;
// int res = loadCharacters(&chaArr, &numCharacters, fd);

// if (res != 0)
//     die_perror(__FILE__, __LINE__, "loadCharacters failed");

// // check we got the expected number of items
// ck_assert_msg(numCharacters == characters001_expectedSize, "numCharacters should equal characters001_expectedSize");

// free(chaArr);

// static_assert(sizeof(characters001_expectedCharacters) / sizeof(struct Character) == 4,
//               "expect correct number of Characters");

// // test each item is correct, from idx 0 through to 3 loop index is available as `_i`

// #test-loop(0, 4) loadCharacters_characters001_characters_are_equal

// const char *infile_path = "characters05.dat";
// int fd = open_with_fileno(infile_path);

// size_t numCharacters = 0;
// struct Character *chaArr = NULL;
// int res = loadCharacters(&chaArr, &numCharacters, fd);

// if (res != 0)
//     die_perror(__FILE__, __LINE__, "loadCharacters failed");

// // pre-requisite: we got the expected number of items
// assert(numCharacters == characters001_expectedSize);

// fprintf(stderr, "checking equality of Character %d, with expected characterID %zu\n",
//         _i, characters001_expectedCharacters[_i].characterID);
// assert_characters_are_equal(chaArr + _i, &(characters001_expectedCharacters[_i]));

// free(chaArr);



//// ///////////////////////////////////////////////////////
//// // Uncomment the following if you have a isValidName()
//// // implementation to test:
//// ///////////////////////////////////////////////////////
////
// #tcase isValidName_testcase

// // datatypes and data used for this testcase

// struct NameAndValidity {
//  char name[DEFAULT_BUFFER_SIZE];
//  int validity;
// };

// struct NameAndValidity isValidName_test_data[] = {
//  { .name = "a b", .validity = 0 },
//  { .name = "a-b", .validity = 1 },
//  { .name = "a'b", .validity = 1 }
// };

// #test isValidName_simple_case
//    const char *name = "bob";
//    int expected_result = 1;
//    int actual_result = isValidName(name);
//    ck_assert_int_eq(actual_result, expected_result);

// #test-loop (0,3) isValidName_more_cases

//    struct NameAndValidity nv = isValidName_test_data[_i];
//    const char *name = nv.name;
//    int expected_result = nv.validity;
//    int actual_result = isValidName(name);
//    ck_assert_int_eq(actual_result, expected_result);

/////////////////////////////////////////////////////////
//// Uncomment the following if you have a saveItemDetails()
//// implementation to test:
/////////////////////////////////////////////////////////
// //
// #tcase saveItemDetails_testcase

// #test saveItemDetails_works

//    struct ItemDetails itemArr[] = {
//      { .itemID = 16602759796824695000UL, .name = "telescope",      .desc = "brass with wooden tripod, 25x30x60 in." }
//    };
//    size_t itemArr_size = sizeof(itemArr)/sizeof(struct ItemDetails);

//    char* file_conts = NULL;
//    size_t file_size = 0;

//    FILE *ofp = fopen("itemDetails.dat", "wb");
//    assert(ofp != NULL);
//    int fd = fileno(ofp);
//    assert(fd != -1);
//    int res = saveItemDetails(itemArr, itemArr_size, fd);
//    assert(res == 0);
//    fclose(ofp);

//    res = slurp_file("itemDetails.dat", "rb", &file_conts, &file_size);
//    assert(res == 0);

//    const size_t expected_size = sizeof(uint64_t) + sizeof(struct ItemDetails);

//    fprintf(stderr, "%s:%d: actual file_size = %zu\n",
//            __FILE__, __LINE__, file_size);

//    ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

//    // metadata should be `1`
//    size_t actual_read_metadata = 0;
//    memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
//    ck_assert_msg(actual_read_metadata == itemArr_size, "size of written metadata should be as expected");

//    // following the metadata should be our struct
//    struct ItemDetails actual_read_item = { 0 };
//    memcpy(&actual_read_item, file_conts + sizeof(size_t), sizeof(struct ItemDetails));
//    ck_assert_msg(actual_read_metadata == itemArr_size, "size of written metadata should be as expected");

//    assert_itemDetails_are_equal(&actual_read_item, &(itemArr[0]));

//    if (file_conts != NULL)
//      free(file_conts);


////////////////////////////////////////////////////////////
//// saveCharacters()
////////////////////////////////////////////////////////////
// #tcase saveCharacters_testcase

// #test saveCharacters_works
// struct Character chaArr[] = { {
//   .characterID = 1,
//   .socialClass = MERCHANT,
//   .profession = "inn-keeper",
//   .name = "Edgar Crawford",
//   .inventorySize = 1,
//   .inventory = {
//     { .itemID = 200648657395984580,
//       .quantity = 1
//     }
//   }
// } };

// size_t chaArr_size = sizeof(chaArr)/sizeof(struct Character);

// char* file_conts = NULL;
// size_t file_size = 0;

// FILE *ofp = fopen("characters01.dat", "wb");
// assert(ofp!=NULL);

// int fd = fileno(ofp);
// assert(fd!=-1);
// int res = saveCharacters(chaArr, chaArr_size, fd);
// assert(res==0);
// fclose(ofp);

// res = slurp_file("characters01.dat", "rb", &file_conts, &file_size);
// assert(res==0);

// const size_t expected_size = sizeof(uint64_t) + sizeof(struct Character);

// fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

// ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// // metadata should be `1`
// size_t actual_read_metadata = 0;
// memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
// ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");

// // following the metadata should be our struct
// struct Character actual_read_item = { 0 };
// memcpy(&actual_read_item, file_conts + sizeof(size_t), sizeof(struct Character));

// assert_characters_are_equal(&actual_read_item, &(chaArr[0]));

// if (file_conts != NULL)
//     free(file_conts);

/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
// #tcase saveCharacters_testcase

// #test saveCharacters_works
// struct Character chaArr[] = {{.characterID = 1,
//                               .socialClass = MERCHANT,
//                               .profession = "inn-keeper",
//                               .name = "Edgar Crawford",
//                               .inventorySize = 1,
//                               .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
//                              {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}}};

// size_t chaArr_size = sizeof(chaArr) / sizeof(struct Character);

// char *file_conts = NULL;
// size_t file_size = 0;

// FILE *ofp = fopen("characters02.dat", "wb");
// assert(ofp != NULL);

// int fd = fileno(ofp);
// assert(fd != -1);
// int res = saveCharacters(chaArr, chaArr_size, fd);
// assert(res == 0);
// fclose(ofp);

// res = slurp_file("characters02.dat", "rb", &file_conts, &file_size);
// assert(res == 0);

// const size_t expected_size = sizeof(uint64_t) + sizeof(chaArr);

// fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

// ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// // metadata should be `1`
// size_t actual_read_metadata = 0;
// memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
// ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");

// // following the metadata should be our struct
// struct Character actual_read_item1 = {0};
// memcpy(&actual_read_item1, file_conts + sizeof(size_t), sizeof(struct Character));

// assert_characters_are_equal(&actual_read_item1, &(chaArr[0]));

// struct Character actual_read_item2 = {0};
// memcpy(&actual_read_item2, file_conts + sizeof(size_t) + sizeof(struct Character), sizeof(struct Character));

// if (file_conts != NULL)
//     free(file_conts);


/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
// #tcase saveCharacters_testcase

// #test saveCharacters_works
// struct Character chaArr[] = {{.characterID = 1,
//                               .socialClass = MERCHANT,
//                               .profession = "inn-keeper",
//                               .name = "Edgar Crawford",
//                               .inventorySize = 2,
//                               .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
//                              {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}}};

// size_t chaArr_size = sizeof(chaArr) / sizeof(struct Character);

// char *file_conts = NULL;
// size_t file_size = 0;

// FILE *ofp = fopen("characters03.dat", "wb");
// assert(ofp != NULL);

// int fd = fileno(ofp);
// assert(fd != -1);
// int res = saveCharacters(chaArr, chaArr_size, fd);
// assert(res == 0);
// fclose(ofp);

// res = slurp_file("characters03.dat", "rb", &file_conts, &file_size);
// assert(res == 0);

// const size_t expected_size = sizeof(uint64_t) + sizeof(chaArr);

// fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

// ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// // metadata should be `1`
// size_t actual_read_metadata = 0;
// memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
// ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");

// // following the metadata should be our struct
// struct Character actual_read_item1 = {0};
// memcpy(&actual_read_item1, file_conts + sizeof(size_t), sizeof(struct Character));

// assert_characters_are_equal(&actual_read_item1, &(chaArr[0]));

// struct Character actual_read_item2 = {0};
// memcpy(&actual_read_item2, file_conts + sizeof(size_t) + sizeof(struct Character), sizeof(struct Character));

// if (file_conts != NULL)
//     free(file_conts);

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
// #tcase saveCharacters_testcase

// #test saveCharacters_works
// struct Character chaArr[] = {{.characterID = 1,
//                               .socialClass = MERCHANT,
//                               .profession = "inn-keeper",
//                               .name = "Edgar Crawford",
//                               .inventorySize = 2,
//                               .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
//                              {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
//                              {.characterID = 1, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 1}}},
//                              {.characterID = 1, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 1}}}};

// size_t chaArr_size = sizeof(chaArr) / sizeof(struct Character);

// char *file_conts = NULL;
// size_t file_size = 0;

// FILE *ofp = fopen("characters04.dat", "wb");
// assert(ofp != NULL);

// int fd = fileno(ofp);
// assert(fd != -1);
// int res = saveCharacters(chaArr, chaArr_size, fd);
// assert(res == 0);
// fclose(ofp);

// res = slurp_file("characters04.dat", "rb", &file_conts, &file_size);
// assert(res == 0);

// const size_t expected_size = sizeof(uint64_t) + sizeof(chaArr);

// fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

// ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// // metadata should be `1`
// size_t actual_read_metadata = 0;
// memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
// ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");

// // following the metadata should be our struct
// struct Character actual_read_item1 = {0};
// memcpy(&actual_read_item1, file_conts + sizeof(size_t), sizeof(struct Character));

// assert_characters_are_equal(&actual_read_item1, &(chaArr[0]));

// struct Character actual_read_item2 = {0};
// memcpy(&actual_read_item2, file_conts + sizeof(size_t) + sizeof(struct Character), sizeof(struct Character));

// if (file_conts != NULL)
//     free(file_conts);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#tcase saveCharacters_testcase

#test saveCharacters_works
struct Character chaArr[] = {{.characterID = 1,
                              .socialClass = MERCHANT,
                              .profession = "inn-keeper",
                              .name = "Edgar Crawford",
                              .inventorySize = 2,
                              .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
                             {.characterID = 2, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 2}}},
                             {.characterID = 3, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 3}}},
                             {.characterID = 4, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984584, .quantity = 4}}}};

size_t chaArr_size = sizeof(chaArr) / sizeof(struct Character);

char *file_conts = NULL;
size_t file_size = 0;

FILE *ofp = fopen("characters05.dat", "wb");
assert(ofp != NULL);

int fd = fileno(ofp);
assert(fd != -1);
int res = saveCharacters(chaArr, chaArr_size, fd);
assert(res == 0);
fclose(ofp);

res = slurp_file("characters05.dat", "rb", &file_conts, &file_size);
assert(res == 0);

const size_t expected_size = sizeof(uint64_t) + count_struct_data_size(chaArr, chaArr_size);

printf("expected_size = %ld bytes\n", expected_size);

fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

ck_assert_msg(file_size == expected_size, "size of written file should eq expected size");

// metadata should be `1`
size_t actual_read_metadata = 0;
memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
ck_assert_msg(actual_read_metadata == chaArr_size, "size of written metadata should be as expected");
printf("chaArray size = %ld\n", actual_read_metadata);

// following the metadata should be our struct
struct Character actual_read_item1 = {0};
memcpy(&actual_read_item1, file_conts + sizeof(size_t), 1076);

assert_characters_are_equal(&actual_read_item1, &(chaArr[0]));

struct Character actual_read_item2 = {0};
memcpy(&actual_read_item2, file_conts + sizeof(size_t) + 1076, 1060);
assert_characters_are_equal(&actual_read_item2, &(chaArr[1]));

struct Character actual_read_item3 = {0};
memcpy(&actual_read_item3, file_conts + sizeof(size_t) + 1076+1060, 1060);
assert_characters_are_equal(&actual_read_item3, &(chaArr[2]));


struct Character actual_read_item4 = {0};
memcpy(&actual_read_item4, file_conts + sizeof(size_t) + 1076+1060+1060, 1060);
assert_characters_are_equal(&actual_read_item4, &(chaArr[3]));


if (file_conts != NULL)
    free(file_conts);

// vim: syntax=c :
