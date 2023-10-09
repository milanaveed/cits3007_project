
//////////////////////////
// DO NOT SUBMIT THIS FILE
//////////////////////////


// for memfd_create
#define _GNU_SOURCE

#include <sys/types.h>
#include "test_utils.h"
#include <p_and_p.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

// for fcntl
#include <unistd.h>
#include <fcntl.h>

// for memfd_create
#include <sys/mman.h>

// for libcheck
#include <check.h>

// be verbose
//#define DEBUG

// check whether two ItemDetails structs are equal. returns 1 if they are,
// 0 otherwise.
void assert_itemDetails_are_equal(const struct ItemDetails *id1, const struct ItemDetails *id2) {
  ck_assert_msg(id1->itemID == id2->itemID, "ItemID for id1 and id2 should be equal");
  ck_assert_str_eq(id1->name, id2->name);
  ck_assert_str_eq(id1->desc, id2->desc);
}


void* random_contents(size_t size) {
  const char allowable_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789";

  const size_t arr_len = sizeof(allowable_chars)/sizeof(char);

  // only initialize srand once
  static int has_been_initialized = 0;
  if (!has_been_initialized) {
    srand( (unsigned int) time(NULL));
    has_been_initialized = 1;
  }

  char *conts = malloc(size);

  for(size_t i = 0; i < size; i++) {
    size_t c_pos = (size_t) rand() % arr_len;
    conts[i]  = allowable_chars[c_pos];
  }

  return conts;
}

void assert_characters_are_equal(const struct Character *c1, const struct Character *c2){
    ck_assert_msg(c1->characterID == c2->characterID, "characterID for c1 and c2 should be equal");
    ck_assert_msg(c1->socialClass == c2->socialClass, "socialClass for c1 and c2 should be equal");
    ck_assert_str_eq(c1->profession, c2->profession);
    ck_assert_str_eq(c1->name, c2->name);
    ck_assert_msg(c1->inventorySize == c2->inventorySize, "inventorySize for c1 and c2 should be equal");
    ck_assert_msg(c1->inventory->itemID == c2->inventory->itemID, "inventory itemID for c1 and c2 should be equal");
    ck_assert_msg(c1->inventory->quantity == c2->inventory->quantity, "inventory quantity for c1 and c2 should be equal");
}


size_t count_struct_data_size(const struct Character* charArr, size_t charArr_size){
    size_t count = 0;
    size_t regular = (sizeof(uint64_t) + sizeof(enum CharacterSocialClass) + sizeof(DEFAULT_BUFFER_SIZE) * 2 + sizeof(size_t))*charArr_size;
    for (int i = 0; i < charArr_size; ++i) {
        
    }
}


int mock_file(const char * name, const void *conts, size_t size) {
#ifdef DEBUG
  fprintf(stderr, __FILE__
          ":mock_file: creating mock file '%s' with conts size '%zu'\n",
          name,
          size
  );
#endif

  int fd = memfd_create(name, MFD_ALLOW_SEALING);
  if (fd == -1) {
    die_perror(__FILE__, __LINE__, "memfd_create() failed");
  }
  int res = fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK );
  if (res == -1) {
    die_perror(__FILE__, __LINE__, "fcntl() initial seals failed");
  }
  ssize_t write_res = write(fd, conts, size);
  if (write_res == -1) {
    die_perror(__FILE__, __LINE__, "write() failed");
  }
  if ((size_t) write_res < size) {
    die_perror(__FILE__, __LINE__, "short write()");
  }
  res = fcntl(fd, F_ADD_SEALS, F_SEAL_SEAL );
  if (res == -1) {
    die_perror(__FILE__, __LINE__, "fcntl() final seals failed");
  }
  return fd;
}

// read the contents of `filename` into malloc'd memory.
// the caller should free `*file_conts`.
int slurp_file(
  const char * filename, const char *mode, char **file_conts, size_t *file_size
) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_sizel = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_sizel == -1) {
        perror("Error getting file size");
        fclose(file);
        return 1;
    }

    *file_size = (size_t) file_sizel;

    // memory for contents
    assert(file_conts != NULL);
    *file_conts = malloc( (size_t) file_sizel);

    char * file_conts_ = *file_conts;

    if (file_conts_ == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }

    // Read the file contents into the array
    size_t bytes_read = fread(file_conts_, 1, (size_t) file_sizel, file);

    if (bytes_read != (size_t) file_sizel) {
        perror("Error reading file");
        free(file_conts_);
        fclose(file);
        return 1;
    }

    fclose(file);

    return 0;
}
