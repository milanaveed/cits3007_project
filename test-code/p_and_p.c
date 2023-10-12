#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <p_and_p.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SUCCESS 0
#define ERR_OPEN_FILE 1
#define ERR_MEMORY_ALLOCATION 1
#define ERR_READ_FILE 1
#define ERR_WRITE_FILE 1
#define ERR_SANITIZATION 1
#define ERR_SIZE_0 1
#define ERR_WRAPPING 1
#define ERR_CLOSE_FILE 1
#define ERR_FFLUSH 1
#define ERR_NULL_POINTER 1
#define ERR_INVALID_TYPE 1

// static_assert(sizeof(size_t) == 8, "We assume the size of size_t is 64bit.");

/**
 * @brief  Serializes an array of ItemDetails structs, and stores the array using the ItemDetails
 * file format.
 * @param  arr: An array of ItemDetails structs.
 * @param  nmemb: The number of struct ItemDetails.
 * @param  fd: A file descriptor for the file being written.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int saveItemDetails(const struct ItemDetails *arr, size_t nmemb, int fd) {
  if (arr == NULL) {
    return ERR_NULL_POINTER;
  }

  if (nmemb == 0) {
    return ERR_SIZE_0;
  }

  FILE *fp = fdopen(fd, "wb");
  if (fp == NULL) {
    return ERR_OPEN_FILE;
  }

  if (fwrite(&nmemb, sizeof(uint64_t), 1, fp) != 1) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_WRITE_FILE;
  }

  for (size_t i = 0; i < nmemb; i++) {
    const struct ItemDetails *currentItem = &arr[i];

    if (!isValidItemDetails(currentItem)) {
      if (ftruncate(fd, 0) || fclose(fp) != 0) {
        return ERR_CLOSE_FILE;  // use of ftruncate(): Ensure the serialization is atomic
      }
      return ERR_INVALID_TYPE;
    }

    size_t elsWritten = fwrite(currentItem, sizeof(struct ItemDetails), 1, fp);

    if (elsWritten != 1) {
      if (ftruncate(fd, 0) || fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_WRITE_FILE;
    }
  }

  if (fflush(fp) != 0) {
    if (ftruncate(fd, 0) != 0) {
      return ERR_SANITIZATION;
    }
    return ERR_FFLUSH;
  }

  if (fclose(fp) != 0) {
    if (ftruncate(fd, 0) != 0) {
      return ERR_SANITIZATION;
    }
    return ERR_CLOSE_FILE;
  }

  return SUCCESS;
}

int saveItemDetailsToPath(const struct ItemDetails *arr, size_t nmemb, const char *filename) {
  return 0;
}

/**
 * @brief  Deserializes a file, including allocating enough memory to store the number of records
 * contained in the file, and writing the address of that memory to ptr. The memory is to be freed
 * by the caller. Write all records contained in the file into the allocated memory. Write the
 * number of records to nmemb.
 * @param  ptr: The address of a pointer-to-ItemDetails struct.
 * @param  nmemb: The address of a size_t.
 * @param  fd: A file descriptor for the file being deserialized.
 * @retval Returns 1 if an error occurs, and no net memory should be allocated (any allocated memory
 * should be freed). Otherwise, it returns 0.
 */
int loadItemDetails(struct ItemDetails **ptr, size_t *nmemb, int fd) {
  if (ptr == NULL || nmemb == NULL) {
    return ERR_NULL_POINTER;  // Avoid dereferencing NULL pointer later
  }

  FILE *fp = fdopen(fd, "rb");
  if (fp == NULL) {
    return ERR_OPEN_FILE;
  }

  size_t num = 0;
  if (fread(&num, sizeof(uint64_t), 1, fp) != 1) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_READ_FILE;
  }

  if (num == 0) {  // Ensure that 0 is never passed as a size argument to calloc(). (CWE-687)
    return ERR_SIZE_0;
  }

  if (num > SIZE_MAX / sizeof(struct ItemDetails)) {  // Prevent wrapping when calculating the size
                                                      // in calloc(). (CWE-190, CWE-128)
                                                      // https://wiki.sei.cmu.edu/confluence/display/c/
    return ERR_WRAPPING;
  }

  *ptr = (struct ItemDetails *)calloc(num, sizeof(struct ItemDetails));
  if (*ptr == NULL) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_MEMORY_ALLOCATION;
  }

  for (size_t i = 0; i < num; i++) {
    struct ItemDetails *currentItem = &(*ptr)[i];

    size_t elsRead = fread(currentItem, sizeof(struct ItemDetails), 1, fp);

    if (elsRead != 1) {
      free(*ptr);
      *ptr = NULL;  // Avoid dangling pointer
      if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_READ_FILE;
    }

    if (!isValidItemDetails(currentItem)) {
      free(*ptr);
      *ptr = NULL;
      if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_INVALID_TYPE;
    }
  }

  if (fflush(fp) != 0) {
    free(*ptr);
    *ptr = NULL;
    return ERR_FFLUSH;
  }

  if (fclose(fp) != 0) {
    free(*ptr);
    *ptr = NULL;
    return ERR_CLOSE_FILE;
  }

  *nmemb = num;

  return SUCCESS;
}

/**
 * @brief  Checks whether a string constitutes a valid name field.
 * @note   The characters contained in a name field must have a graphical representation (as defined
 * by the C function isgraph). No other characters are permitted. This means that names cannot
 * contain (for instance) whitespace or control characters. A name field is always a
 * DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most
 * DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: String to be validated.
 * @retval Returns 1 if it is a valid name field, and 0 if not.
 */
int isValidName(const char *str) {
  size_t length = strnlen(str, DEFAULT_BUFFER_SIZE);
  if (length == DEFAULT_BUFFER_SIZE) {
    return 0;
  }

  for (size_t i = 0; i < length; i++) {
    if (!isgraph(str[i])) {
      return 0;
    }
  }

  return 1;
}

/**
 * @brief  Checks whether a string constitutes a valid multi-word field.
 * @note   A multi-word field may contain all the characters in a name field, and may also contain
 * space characters (but the first and last characters must not be spaces). A multi-word field is
 * always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length
 * at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first
 * NUL byte.
 * @param  *str: String to be validated.
 * @retval Returns 1 if it is a valid multi-word field, and 0 if not.
 */
int isValidMultiword(const char *str) {
  size_t length = strnlen(str, DEFAULT_BUFFER_SIZE);
  if (length == DEFAULT_BUFFER_SIZE) {
    return 0;
  }

  if (str[0] == ' ' || str[length - 1] == ' ') {
    return 0;
  }

  for (size_t i = 0; i < length; i++) {
    if (!isgraph(str[i]) && str[i] != ' ') {
      return 0;
    }
  }

  return 1;
}

/**
 * @brief  Checks whether an ItemDetails struct is valid.
 * @note   A valid ItemDetails format means that its name and desc fields are valid name and
 * multi-word fields, respectively.
 * @param  *id: A pointer to the ItemDetails struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidItemDetails(const struct ItemDetails *id) {
  if (!isValidName(id->name) || !isValidMultiword(id->desc)) {
    return 0;
  }

  return 1;
}

/**
 * @brief  Checks whether a Character struct is valid.
 * @note   A Character struct is valid iff: the profession field is a valid name field, the name
 * field is a valid multi-word field, the total number of items carried does not exceed MAX_ITEMS,
 * and inventorySize is less than or equal to MAX_ITEMS. `inventorySize` is a 64-bit unsigned
 * integer value.
 * @param  *c: A pointer to the Character struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidCharacter(const struct Character *c) {
  if (c->socialClass > 4 || !isValidName(c->profession) || !isValidMultiword(c->name) ||
      c->inventorySize > MAX_ITEMS) {
    return 0;
  }

  size_t n_items = 0;
  for (size_t i = 0; i < c->inventorySize; i++) {
    n_items += c->inventory[i].quantity;
  }
  if (n_items > MAX_ITEMS) {
    return 0;
  }

  return 1;
}

/**
 * @brief  Saves characters in the Character file format, and validates records using the
 * isValidCharacter function, but otherwise behave in the same way as saveItemDetails.
 * @param  *arr: An array of characters.
 * @param  nmemb: The number of Character structs.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
  if (arr == NULL) {
    return ERR_NULL_POINTER;
  }

  if (nmemb == 0) {
    return ERR_SIZE_0;
  }

  FILE *fp = fdopen(fd, "wb");
  if (fp == NULL) {
    return ERR_OPEN_FILE;
  }

  if (fwrite(&nmemb, sizeof(uint64_t), 1, fp) != 1) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_WRITE_FILE;
  }

  const size_t bytesOfFixedFields = sizeof(uint64_t) + sizeof(enum CharacterSocialClass) +
                                    DEFAULT_BUFFER_SIZE * 2 +
                                    sizeof(size_t);  // No wrapping aroud risk, value is small

  for (size_t i = 0; i < nmemb; i++) {
    const struct Character *currentCharacter = &arr[i];

    if (!isValidCharacter(currentCharacter)) {
      if (ftruncate(fd, 0) || fclose(fp) != 0) {
        return ERR_CLOSE_FILE;  // use of ftruncate(): Ensure the serialization is atomic
      }
      return ERR_INVALID_TYPE;
    }

    const size_t bytesOfUsedInventory =
        sizeof(struct ItemCarried) * currentCharacter->inventorySize;

    const size_t bytesToWrite =
        bytesOfFixedFields + bytesOfUsedInventory;  // No wrapping around risk, value is small

    size_t elsWritten = fwrite(currentCharacter, bytesToWrite, 1, fp);

    if (elsWritten != 1) {
      if (ftruncate(fd, 0) || fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_WRITE_FILE;
    }
  }

  if (fflush(fp) != 0) {
    if (ftruncate(fd, 0) != 0) {
      return ERR_SANITIZATION;
    }
    return ERR_FFLUSH;
  }
  if (fclose(fp) != 0) {
    if (ftruncate(fd, 0) != 0) {
      return ERR_SANITIZATION;
    }
    return ERR_CLOSE_FILE;
  }

  return SUCCESS;
}

/**
 * @brief  Loads characters in the Character file format, and validates records using the
 * isValidCharacter function, but otherwise behave in the same way as loadItemDetails.
 * @param  **ptr: A pointer to a pointer to characters.
 * @param  *nmemb: The address of a size_t.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int loadCharacters(struct Character **ptr, size_t *nmemb, int fd) {
  if (ptr == NULL || nmemb == NULL) {
    return ERR_NULL_POINTER;  // Avoid dereferencing NULL pointer later
  }

  FILE *fp = fdopen(fd, "rb");
  if (fp == NULL) {
    return ERR_OPEN_FILE;
  }

  size_t num = 0;
  if (fread(&num, sizeof(uint64_t), 1, fp) != 1) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_READ_FILE;
  }

  if (num == 0) {  // Ensure that 0 is never passed as a size argument to calloc(). (CWE-687)
    return ERR_SIZE_0;
  }

  if (num > SIZE_MAX / sizeof(struct Character)) {  // Prevent wrapping when calculating the size in
                                                    // calloc(). (CWE-190, CWE-128)
    return ERR_WRAPPING;
  }

  *ptr = (struct Character *)calloc(num, sizeof(struct Character));
  if (*ptr == NULL) {
    if (fclose(fp) != 0) {
      return ERR_CLOSE_FILE;
    }
    return ERR_MEMORY_ALLOCATION;
  }

  const size_t bytesOfFixedFields = sizeof(uint64_t) + sizeof(enum CharacterSocialClass) +
                                    DEFAULT_BUFFER_SIZE * 2 +
                                    sizeof(size_t);  // No wrapping around risk, value is small

  for (size_t i = 0; i < num; ++i) {
    struct Character *currentCharacter = &(*ptr)[i];

    if (fread(currentCharacter, bytesOfFixedFields, 1, fp) != 1) {
      free(*ptr);
      *ptr = NULL;  // Avoid dangling pointer
      if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_READ_FILE;
    }

    const size_t bytesOfUsedInventory =
        sizeof(struct ItemCarried) * currentCharacter->inventorySize;

    if (fread((char *)currentCharacter + bytesOfFixedFields, bytesOfUsedInventory, 1, fp) != 1) {
      free(*ptr);
      *ptr = NULL;
      if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_READ_FILE;
    }

    if (!isValidCharacter(currentCharacter)) {
      free(*ptr);
      *ptr = NULL;
      if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
      }
      return ERR_INVALID_TYPE;
    }
  }

  if (fflush(fp) != 0) {
    free(*ptr);
    *ptr = NULL;
    return ERR_FFLUSH;
  }

  if (fclose(fp) != 0) {
    free(*ptr);
    *ptr = NULL;
    return ERR_CLOSE_FILE;
  }

  *nmemb = num;

  return SUCCESS;
}

/**
 * @brief  Acquires appropriate permissions to load the ItemDetails database securely.
 * @note   It should attempt to acquire appropriate permissions for opening the ItemDetails database
 * (that is: the effective userID should be set to the userID of pitchpoltadmin), should load the
 * database from the specified file, and then (after permanently dropping privileges), call the
 * function playGame to which it should pass the loaded data and the number of items in the loaded
 * data. It should check the running process's permissions to ensure that the executable it was
 * launched from was indeed a setUID executable owned by user pitchpoltadmin.
 * @param  *filepath: Filepath for ItemDetails database.
 * @retval Returns 1 if an error occurs during the deserialization process. Returns 2 if the
 * executable it was launced from was not a setUID executable owned by user pitchpoltadmin, or if an
 * error occurs in acquiring or dropping permissions. Otherwise, it returns 0.
 */
int secureLoad(const char *filepath) {
  const uid_t originalEuid = geteuid();

  struct ItemDetails *p = NULL;
  size_t numItems = 0;

  struct passwd *userInfo = getpwnam("pitchpoltadmin");
  if (userInfo == NULL) {
    return 0;
  }

  if (originalEuid != userInfo->pw_uid) {
    if (seteuid(userInfo->pw_uid) != 0) {
      return 2;  // Error occurs in acquiring permissions
    }
  }

  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    return 0;
  }

  if (seteuid(originalEuid) != 0) {
    if (close(fd) != 0) {
      return 2;
    }
    return 2;  // Error occurs in dropping permissions
  }

  struct stat fileStat;
  if (fstat(fd, &fileStat) != 0) {
    if (close(fd) != 0) {
      return 0;
    }
    return 0;
  }

  if (fileStat.st_uid != userInfo->pw_uid) {
    if (close(fd) != 0) {
      return 2;
    }
    return 2;  // The executable it was launced from was not owned by user pitchpoltadmin
  }

  if (!(fileStat.st_mode & S_IXUSR)) {
    if (close(fd) != 0) {
      return 2;
    }
    return 2;  // The executable it was launced from was not a setUID executable
  }

  if (loadItemDetails(&p, &numItems, fd) != 0) {
    if (close(fd) != 0) {
      return 1;
    }
    return 1;  // Error occurs in deserialization
  }

  if (close(fd) != 0) {
    free(p);
    p = NULL;
    return 0;
  }

  playGame(p, numItems);

  free(p);
  p = NULL;

  return 0;
}

void playGame(struct ItemDetails *ptr, size_t nmemb);

//! Only submit the above code
void playGame(struct ItemDetails *ptr, size_t nmemb) {
  //   struct ItemDetails item = ptr[0];
  //   uint64_t id = item.itemID;
  //   printf("id is: %ld\n", id);
  //   printf("id is: %ld\n", nmemb);
  //   printf("euid is: %d\n", geteuid());
  //   printf("uid is: %d\n", getuid());
}
