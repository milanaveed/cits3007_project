#define _POSIX_C_SOURCE 200809L
#include <p_and_p.h>

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SUCCESS 0
#define ERR_OPEN_FILE 1
#define ERR_INSUFFICIENT_MEMORY 1
#define ERR_FILE_CORRUPTION 1
#define ERR_SIZE_0 1
#define ERR_INVALID_SIZE 1
#define ERR_WRAPPING 1
#define ERR_CLOSE_FILE 1
#define ERR_FFLUSH 1
#define ERR_NULL_POINTER 1
#define ERR_INVALID_TYPE 1
#define ERR_INVALID_FD 1
#define ERR_NO_USER_INFO 0
#define ERR_ACQUIRE_PERMISSION 2
#define ERR_DROP_PERMISSION 2
#define ERR_RETRIEVE_FD_INFO 0
#define ERR_FD_USERID 2
#define ERR_FD_EXECUTE_PERMISSION 2
#define ERR_CLEAR_ENV 0
#define ERR_DESERIALIZATION 1
#define ERR_CLOSE_FD 0

//  ----------------------------------------------------------------------------

//* Inline comments should not say what the code is doing – anyone who understands the programming language should be able to see that – but rather why it is doing it.
//? Arran said in the lecture that, one of the best practises is making sure that we zero out all of the memory there before we write it to a file. What does that mean?
// We have to zero out buffers when we write them to disc
// If we read something from disc, we assume that it's in a nice sanitised format and everything has been zeroed out. We want to be careful when we read stuff in. We validate it's in the format we expect.
// TODO: "Although the inventory field of each Character struct always contains MAX_ITEMS elements, only the used portion of the inventory (that is, inventorySize many elements) is written to (or read from) a character file." : improve load and save Characters functions
// TODO: enable all sanitizer flags
// todo: be careful with memory leak
// TODO: use fflush() instead of fclose(), don't close the file pointer check the help forum

/**
 * @brief  Serializes an array of ItemDetails structs.
 *
 * @param  arr: An array of ItemDetails structs.
 * @param  nmemb: Number of items.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int saveItemDetails(const struct ItemDetails *arr, size_t nmemb, int fd) {
    if (fd < 0) {
        return ERR_INVALID_FD;
    }

    if (arr == NULL) {
        return ERR_NULL_POINTER;
    }

    FILE *fp = fdopen(fd, "wb");
    if (fp == NULL) {
        return ERR_OPEN_FILE;
    }

    if (fwrite(&nmemb, sizeof(uint64_t), 1, fp) != 1) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_FILE_CORRUPTION;
    }

    for (size_t i = 0; i < nmemb; i++) {
        const struct ItemDetails *currentItem = &arr[i];

        if (!isValidItemDetails(currentItem)) {
            return ERR_INVALID_TYPE;
        }

        size_t elsWritten = fwrite(currentItem, sizeof(struct ItemDetails), 1, fp);

        if (elsWritten != 1) {
            return ERR_FILE_CORRUPTION;
        }
    }

    if (fflush(fp) != 0) {
        return ERR_FFLUSH;
    }

    if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
    }

    return SUCCESS;
}

// /**
//  * @brief  Save ItemDetails database to disk.
//  * @note
//  * @param  arr:
//  * @param  nmemb:
//  * @param  filename:
//  * @retval
//  */
// int saveItemDetailsToPath(const struct ItemDetails *arr, size_t nmemb, const char *filename) {
//     int fd = open(filename, O_WRONLY);
//     if (fd == -1) {
//         return ERR_OPEN_FILE;
//     }

//     if (saveItemDetails(arr, nmemb, fd) == 1) {
//         return 1;
//     }

//     return 0;
// }

/**
 * @brief  Deserializes a file, including allocating enough memory to store the number of records contained in the file, and write the address of that memory to ptr. The memory is to be freed by the caller. Write all records contained in the file into the allocated memory. Write the number of records to nmemb.
 *
 * @param  ptr: The address of a pointer-to-ItemDetails struct.
 * @param  nmemb: The address of a size_t.
 * @param  fd: A file descriptor for the file being deserialized.
 * @retval Returns 1 if an error occurs, and no net memory should be allocated (any allocated memory should be freed). Otherwise, it returns 0.
 */
int loadItemDetails(struct ItemDetails **ptr, size_t *nmemb, int fd) {
    //?! Are ptr and nmemb NULL pointers?
    //? If not, should we reallocate the memory?
    if (fd < 0) {
        return ERR_INVALID_FD;
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
        return ERR_FILE_CORRUPTION;
    }

    if (num == 0) { // Ensure that 0 is never passed as a size argument to calloc(). Related to CWE-687
        return ERR_SIZE_0;
    }

    if (num > SIZE_MAX / sizeof(struct ItemDetails)) { // Prevent wrapping when calculating the size in calloc(). Related to CWE-190, CWE-128
        return ERR_WRAPPING;
    }

    *ptr = (struct ItemDetails *)calloc(num, sizeof(struct ItemDetails));
    if (*ptr == NULL) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_INSUFFICIENT_MEMORY;
    }

    for (size_t i = 0; i < num; i++) {
        struct ItemDetails *currentItem = &(*ptr)[i];

        size_t elsRead = fread(currentItem, sizeof(struct ItemDetails), 1, fp);

        if (elsRead != 1) {
            memset(*ptr, 0, num * sizeof(struct ItemDetails)); // Sanitize memory to prevent information leakage
            free(*ptr);
            *ptr = NULL; // Avoid dangling pointer
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_FILE_CORRUPTION;
        }

        if (!isValidItemDetails(currentItem)) {
            memset(*ptr, 0, num * sizeof(struct ItemDetails));
            free(*ptr);
            *ptr = NULL;
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_INVALID_TYPE;
        }
    }

    //! Do we assume the pointers passed in are NULL pointers?
    if (nmemb == NULL) {
        nmemb = (size_t *)calloc(1, sizeof(size_t));
    }
    *nmemb = num;

    if (fflush(fp) != 0) {
        memset(*ptr, 0, num * sizeof(struct ItemDetails));
        free(*ptr);
        *ptr = NULL;
        return ERR_FFLUSH;
    }

    if (fclose(fp) != 0) {
        memset(*ptr, 0, num * sizeof(struct ItemDetails));
        free(*ptr);
        *ptr = NULL;
        return ERR_CLOSE_FILE;
    }

    return SUCCESS;
}

/*
 Let's break down the expression `struct ItemDetails *currentItem = &(*ptr)[i];` step by step:

1. **`(*ptr)`**: `ptr` is a pointer to a pointer to `struct ItemDetails`. When you dereference `ptr` with `*ptr`, you get a pointer to `struct ItemDetails`.

2. **`(*ptr)[i]`**: Now that we have a pointer to `struct ItemDetails`, we can treat it as an array of `struct ItemDetails`. We use the indexing operator `[]` to access the `i`-th element of this array. So, `(*ptr)[i]` gives us the `i`-th `struct ItemDetails` object in the array pointed to by `*ptr`.

3. **`&(*ptr)[i]`**: Next, we take the address of the `i`-th `struct ItemDetails` object using the address-of operator `&`. This gives us a pointer to the `i`-th `struct ItemDetails` object.

4. **`struct ItemDetails *currentItem = &(*ptr)[i];`**: Finally, we assign the address of the `i`-th `struct ItemDetails` object to the pointer `currentItem` of type `struct ItemDetails *`.

So, after this line of code, `currentItem` is a pointer to the `i`-th `struct ItemDetails` object in the array pointed to by `*ptr`.*/

/**
 * @brief  Checks whether a string constitutes a valid name field.
 * @note
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
 * @note
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

//* done, passed moodle, not improved
/**
 * @brief  Checks whether an ItemDetails struct is valid.
 * @note   A valid ItemDetails format means that its name and desc fields are valid name and multi-word fields, respectively.
 * @param  *id: A pointer to an ItemDetails struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidItemDetails(const struct ItemDetails *id) {
    if (!isValidName(id->name) || !isValidMultiword(id->desc)) {
        return 0;
    }

    return 1;
}

//* done, passed moodle, not improved
/**
 * @brief  Checks whether a Character struct is valid.
 * @note   A Character struct is valid iff: the profession field is a valid name field, the name field is a valid multi-word field, the total number of items carried does not exceed MAX_ITEMS, and inventorySize is less than or equal to MAX_ITEMS.
 * `inventorySize` is a 64-bit unsigned integer value
 * @param  *c: A pointer to a Character struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidCharacter(const struct Character *c) {
    if (!isValidName(c->profession) || !isValidMultiword(c->name) || c->inventorySize > MAX_ITEMS) {
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

//* "The file does not necessarily store the full, MAX_ITEMS long inventory array. If the inventorySize is 2, then only 2 records from that array are stored in the file."
/**
 * @brief  Saves characters in the Character file format, and validates records using the isValidCharacter function, but otherwise behave in the same way as saveItemDetails.
 * @note
 * @param  *arr: An array of characters.
 * @param  nmemb: The number of Character structs.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
    if (fd < 0) {
        return ERR_INVALID_FD;
    }

    if (nmemb <= 0) {
        return ERR_INVALID_SIZE;
    }

    if (arr == NULL) {
        return ERR_NULL_POINTER;
    }

    FILE *fp = fdopen(fd, "wb");
    if (fp == NULL) {
        return ERR_OPEN_FILE;
    }

    if (fwrite(&nmemb, sizeof(uint64_t), 1, fp) != 1) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_FILE_CORRUPTION;
    }

    const size_t bytesOfFixedFields = sizeof(uint64_t) + sizeof(enum CharacterSocialClass) + DEFAULT_BUFFER_SIZE * 2 + sizeof(size_t);

    printf("in saveCharacters(): struct fixed_fields = %ld bytes\n", bytesOfFixedFields);
    printf("size of struct Character = %ld bytes\n", sizeof(struct Character));
    printf("struct ItemCarried.itemID = %ld bytes\n", sizeof(uint64_t));
    printf("struct ItemCarried.quantity = %ld bytes\n", sizeof(size_t));

    for (size_t i = 0; i < nmemb; i++) {
        const struct Character *currentCharacter = &arr[i];

        if (!isValidCharacter(currentCharacter)) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_INVALID_TYPE;
        }

        const size_t bytesOfUsedInventory = sizeof(struct ItemCarried) * currentCharacter->inventorySize;
        const size_t bytesToWrite = bytesOfFixedFields + bytesOfUsedInventory;

        printf("writing i= %ld Character\n", i);

        printf("for the i = %ld Character, need to write %ld bytes\n", i, bytesToWrite);

        size_t elsWritten = fwrite(currentCharacter, bytesToWrite, 1, fp);

        if (elsWritten != 1) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_FILE_CORRUPTION;
        }
    }

    if (fflush(fp) != 0) {
        return ERR_FFLUSH;
    }
    if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
    }

    return SUCCESS;
}

int saveCharactersToPath(struct Character *arr, size_t nmemb, const char *filename) {
    int fd = open(filename, O_WRONLY);
    if (fd == -1) {
        return ERR_OPEN_FILE;
    }

    if (saveCharacters(arr, nmemb, fd) == 1) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Loads characters in the Character file format, and validates records using the isValidCharacter function, but otherwise behave in the same way as loadItemDetails.
 * @note
 * @param  **ptr: A pointer to a pointer to characters.
 * @param  *nmemb: The address of a size_t.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int loadCharacters(struct Character **ptr, size_t *nmemb, int fd) {
    if (fd < 0) {
        return ERR_INVALID_FD;
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
        return ERR_FILE_CORRUPTION;
    }

    if (num == 0) { // Ensure that 0 is never passed as a size argument to calloc(). (CWE-687)
        return ERR_SIZE_0;
    }

    if (num > SIZE_MAX / sizeof(struct Character)) { // Prevent wrapping when calculating the size in calloc(). (CWE-190, CWE-128)
        return ERR_WRAPPING;
    }

    *ptr = (struct Character *)calloc(num, sizeof(struct Character));
    if (*ptr == NULL) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_INSUFFICIENT_MEMORY;
    }

    const size_t bytesOfFixedFields = sizeof(uint64_t) + sizeof(enum CharacterSocialClass) + DEFAULT_BUFFER_SIZE * 2 + sizeof(size_t);

    for (size_t i = 0; i < num; ++i) {
        struct Character *currentCharacter = &(*ptr)[i];
        printf("reading the i = %ld Character\n", i);

        // Will read correctly if read the used Character as a whole
        // if (i == 0) {
        //     if (fread(currentCharacter, 1076, 1, fp) != 1) {
        //         printf("failed at reading fixed sized bytes");
        //         free(*ptr);
        //         *ptr = NULL; // Avoid dangling pointer
        //         if (fclose(fp) != 0) {
        //             return ERR_CLOSE_FILE;
        //         }
        //         return ERR_FILE_CORRUPTION;
        //     }
        // } else {
        //     if (fread(currentCharacter, 1060, 1, fp) != 1) {
        //         printf("failed at reading fixed sized bytes");
        //         free(*ptr);
        //         *ptr = NULL; // Avoid dangling pointer
        //         if (fclose(fp) != 0) {
        //             return ERR_CLOSE_FILE;
        //         }
        //         return ERR_FILE_CORRUPTION;
        //     }
        // }

        //! Read incorrectly if i divide the reading into two parts
        if (fread(currentCharacter, bytesOfFixedFields, 1, fp) != 1) {
            free(*ptr);
            *ptr = NULL; // Avoid dangling pointer
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_FILE_CORRUPTION;
        }

        const size_t bytesOfUsedInventory = sizeof(struct ItemCarried) * currentCharacter->inventorySize;

        // printf("bytesOfFixedFields = %ld\n", bytesOfFixedFields);
        // printf("inventorySize = %ld\n", currentCharacter->inventorySize);
        // // printf("bytesOfUsedInventory = %ld\n", bytesOfUsedInventory);

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
        
        size_t totalBytesToRead = bytesOfFixedFields + bytesOfUsedInventory;
        printf("total bytes to read for the current Character = %ld\n", totalBytesToRead);

        if (fread(currentCharacter->inventory, bytesOfUsedInventory, 1, fp) != 1) {
            free(*ptr);
            *ptr = NULL; // Avoid dangling pointer
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_FILE_CORRUPTION;
        }


        // Only read inventorySize many inventory elements
        // Starts to go wrong here
        // if (fread(currentCharacter, bytesOfFixedFields, 1, fp) != 1 || fread(currentCharacter->inventory, sizeof(struct ItemCarried) * currentCharacter->inventorySize, 1, fp) != 1) {
        //     free(*ptr);
        //     *ptr = NULL; // Avoid dangling pointer
        //     if (fclose(fp) != 0) {
        //         return ERR_CLOSE_FILE;
        //     }
        //     return ERR_FILE_CORRUPTION;
        // }

        printf(".characterID = %ld\n", currentCharacter->characterID);
        printf(".socialClass = %d\n", currentCharacter->socialClass);
        printf(".profession = %s\n", currentCharacter->profession);
        printf(".name = %s\n", currentCharacter->name);
        printf(".inventorySize = %ld\n", currentCharacter->inventorySize);
        for (size_t j = 0; j < currentCharacter->inventorySize; ++j) {
            printf("for the index=%ld inventory:\n", j);
            printf(".itemID = %ld\n", currentCharacter->inventory[j].itemID);
            printf(".quantity = %ld\n", currentCharacter->inventory[j].quantity);
        }

        if (!isValidCharacter(currentCharacter)) {
            printf("failed at validating Character\n");
            free(*ptr);
            *ptr = NULL;
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_INVALID_TYPE;
        }
    }
    //?
    // if (nmemb == NULL) {
    //     nmemb = (size_t*) calloc(1, sizeof(size_t));
    // }
    *nmemb = num;

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

    return SUCCESS;
}

/**
 * @brief  Acquires appropriate permissions to load the ItemDetails database securely.
 * @note
 * @param  *filepath: Filepath for ItemDetails database.
 * @retval Returns 1 if an error occurs during the deserialization process. Returns 2 if the executable it was launced from was not a setUID executable owned by user pitchpoltadmin, or if an error occurs in acquiring or dropping permissions. Otherwise, it returns 0.
 */
int secureLoad(const char *filepath) {
    // char *resolvedFilepath = realpath(filepath, NULL);
    // if (!resolvedFilepath) {
    //     return 0;
    // }

    const uid_t originalEuid = geteuid();

    struct ItemDetails *p = NULL;
    size_t numItems = 0;

    struct passwd *userInfo = getpwnam("pitchpoltadmin");
    if (userInfo == NULL) {
        // free(resolvedFilepath);
        // resolvedFilepath = NULL;
        return ERR_NO_USER_INFO;
    }

    if (originalEuid != userInfo->pw_uid) {
        if (seteuid(userInfo->pw_uid) != 0) {
            // free(resolvedFilepath);
            // resolvedFilepath = NULL;
            return ERR_ACQUIRE_PERMISSION;
        }
    }
    /*
    p_and_p.c|493 col 14 warning| [flawfinder] (misc) open: Check when opening files|~
         - can an attacker redirect it (via symlinks), force the opening of special file|~
         type (e.g., device files), move things around to create a race condition, contr|~
        ol its ancestors, or change its contents? (CWE-362).   [Warning]
    */
    // int fd = open(resolvedFilepath, O_RDONLY);
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        // free(resolvedFilepath);
        // resolvedFilepath = NULL;
        return ERR_OPEN_FILE;
    }

    // free(resolvedFilepath);
    // resolvedFilepath = NULL;

    if (seteuid(originalEuid) != 0) {
        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
        return ERR_DROP_PERMISSION;
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) != 0) {
        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
        return ERR_RETRIEVE_FD_INFO;
    }
    if (fileStat.st_uid != userInfo->pw_uid) {
        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
        return ERR_FD_USERID;
    }
    if (!(fileStat.st_mode & S_IXUSR)) {
        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
        return ERR_FD_EXECUTE_PERMISSION;
    }

    if (loadItemDetails(&p, &numItems, fd) != 0) {
        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
        return ERR_DESERIALIZATION;
    }

    if (close(fd) != 0) {
        free(p);
        p = NULL;
        return ERR_CLOSE_FD;
    }

    playGame(p, numItems);

    free(p);
    p = NULL;

    return 0;
}

void playGame(struct ItemDetails *ptr, size_t nmemb);

//! delete the playGame() definition, but not the declaration before submitting this .c file
void playGame(struct ItemDetails *ptr, size_t nmemb) {
    struct ItemDetails item = ptr[0];
    uint64_t id = item.itemID;
    printf("id is: %ld\n", id);
    printf("id is: %ld\n", nmemb);
    printf("euid is: %d\n", geteuid());
    printf("uid is: %d\n", getuid());
}

// int main() {
//     struct Character chaArr[] = {{.characterID = 1,
//                                   .socialClass = MERCHANT,
//                                   .profession = "inn-keeper",
//                                   .name = "Edgar Crawford",
//                                   .inventorySize = 2,
//                                   .inventory = {{.itemID = 200648657395984580, .quantity = 1}, {.itemID = 200648657395984581, .quantity = 2}}},
//                                  {.characterID = 1, .socialClass = MERCHANT, .profession = "inn-keeper", .name = "Edgar Crawford", .inventorySize = 1, .inventory = {{.itemID = 200648657395984580, .quantity = 1}}},
//                                  {.characterID = 1, .socialClass = GENTRY, .profession = "inn-keeper", .name = "Edgar Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984582, .quantity = 1}}},
//                                  {.characterID = 1, .socialClass = GENTRY, .profession = "dreamer", .name = "Edga Craw", .inventorySize = 1, .inventory = {{.itemID = 200648657395984583, .quantity = 1}}}};

//     if(saveCharactersToPath(chaArr, 4, "characters06.dat")==0){
//         printf("saved successfully");
//     }

//     int fd = open("characters06.dat", O_RDONLY);

//     struct Character* result = NULL;
//     size_t nmemb = 0;
//     loadCharacters(&result, &nmemb, fd);

//     return 0;
// }