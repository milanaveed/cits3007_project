#include <p_and_p.h>

#include <ctype.h>
#include <fcntl.h>
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
#define ERR_FORKING 0
#define ERR_CLOSE_FD 0

//* compile with gcc -I to look for header files
//* write a fairly brief documentation block: when documenting, we can assume the reader is familiar with the file format and the struct types
//* will get low mark if too many comments. Comments should be used to explain what is not in the code
//* only submit this .c file
//* Inline comments should not say what the code is doing – anyone who understands the programming language should be able to see that – but rather why it is doing it.
//? Arran said in the lecture that, one of the best practises is making sure that we zero out all of the memory there before we write it to a file. What does that mean?
// We have to zero out buffers when we write them to disc
// If we read something from disc, we assume that it's in a nice sanitised format and everything has been zeroed out. We want to be careful when we read stuff in. We validate it's in the format we expect.
// Put in some static asserts and say, i'm about to assume that a size_t is 64 bits.
// TODO: write secureLoad()
// TODO: write a check test for loadCharacters()
// TODO: enable all sanitizer flags
// TODO: try fuzzing
// TODO: sanitise things in and out
// todo: be careful with memory leak
// TODO: Don't close the file pointer
// TODO: use fflush() instead of fclose(), check the help forum
// TODO: use calloc instead of malloc

/**
 * @brief  Serializes an array of ItemDetails structs.
 *
 * @param  arr: An array of ItemDetails structs.
 * @param  nmemb: Number of items.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
//* take some structs as input, and produce a file that matches the specification given
//*There's also a tip in this week's lab – when working with a FILE pointer obtained from a file descriptor, you might need to call fflush (see man fflush) to "write out" those buffers. The buffers are automatically written out when a FILE pointer is closed with fclose; but if you have a FILE pointer you created yourself (e.g. with fdopen) that just goes out of scope, then you may need to flush the buffers before your function exits.
//* use fread and fwrite
//* If reading or writing from a FILE*, it’s a good idea to call fflush before finishing the current function – especially if the FILE* was obtained using fdopen, since it may contain buffered input or output that hasn’t yet been fully read or written."
//* done, checked, passed moodle, not improved
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
        struct ItemDetails currentItem = arr[i];

        if (!isValidItemDetails(&currentItem)) {
            return ERR_INVALID_TYPE;
        }

        size_t elsWritten = fwrite(&currentItem, sizeof(struct ItemDetails), 1, fp);

        if (elsWritten != 1) {
            return ERR_FILE_CORRUPTION;
        }
    }

    if (fflush(fp) != 0) { //* enough and no need to close the fp
        return ERR_FFLUSH;
    }

    return SUCCESS;
}

//* not required to implement that, but will be helpful to do so when doing my own testing
/**
 * @brief
 * @note
 * @param  arr:
 * @param  nmemb:
 * @param  filename:
 * @retval
 */
int saveItemDetailsToPath(const struct ItemDetails *arr, size_t nmemb, const char *filename) {
    int fd = open(filename, O_WRONLY);
    if (fd == -1) {
        return ERR_OPEN_FILE;
    }

    if (saveItemDetails(arr, nmemb, fd) == 1) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Deserializes a file, including allocating enough memory to store the number of records contained in the file, and write the address of that memory to ptr. The memory is to be freed by the caller. Write all records contained in the file into the allocated memory. Write the number of records to nmemb.
 *
 * @param  ptr: The address of a pointer-to-ItemDetails struct.
 * @param  nmemb: The address of a size_t.
 * @param  fd: A file descriptor for the file being deserialized.
 * @retval Returns 1 if an error occurs, and no net memory should be allocated (any allocated memory should be freed). Otherwise, it returns 0.
 */
//* done, checked, passed moodle, not improved
int loadItemDetails(struct ItemDetails **ptr, size_t *nmemb, int fd) {
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

    *ptr = (struct ItemDetails *)malloc(num * sizeof(struct ItemDetails));
    if (*ptr == NULL) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_INSUFFICIENT_MEMORY;
    }

    for (size_t i = 0; i < num; i++) {
        struct ItemDetails currentItem;

        size_t elsRead = fread(&currentItem, sizeof(struct ItemDetails), 1, fp);

        if (elsRead != 1) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            memset(*ptr, 0, num * sizeof(struct ItemDetails));
            free(*ptr);
            *ptr = NULL; // Avoid dangling pointer
            return ERR_FILE_CORRUPTION;
        }

        if (!isValidItemDetails(&currentItem)) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            memset(*ptr, 0, num * sizeof(struct ItemDetails));
            free(*ptr);
            *ptr = NULL;
            return ERR_INVALID_TYPE;
        }

        (*ptr)[i] = currentItem;
    }

    if (nmemb == NULL) {
        return ERR_NULL_POINTER;
    }
    *nmemb = num;

    if (fflush(fp) != 0) {
        return ERR_FFLUSH;
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

//* done, checked, passed moodle, not improved
/**
 * @brief  Checks whether a string constitutes a valid name field, which requires the characters contained to have a graphical representation (as defined by the C function isgraph). No other characters are permitted. This means that names cannot contain (for instance) whitespace or control characters.
 * @note   A name field is always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: A string (a pointer to a character).
 * @retval Returns 1 if it is a valid name field, and 0 if not.
 */
int isValidName(const char *str) {
    size_t length = strlen(str);
    if (length >= DEFAULT_BUFFER_SIZE) {
        return 0;
    }

    for (size_t i = 0; i < length; i++) {
        if (!isgraph(str[i])) {
            return 0;
        }
    }

    return 1;
}

//* done, checked, passed moodle, not improved
/**
 * @brief  Checks whether a string constitutes a valid multi-word field, which may contain all the characters in a name field, and may also contain space characters (but the first and the last characters must not be spaces).
 * @note   A multi-word field is always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: A string (a pointer to a character).
 * @retval Returns 1 if it is a valid multi-word field, and 0 if not.
 */
int isValidMultiword(const char *str) {
    size_t length = strlen(str);
    if (length >= DEFAULT_BUFFER_SIZE) {
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
//* done, passed moodle, not improved
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
    if (fd < 0) {
        return ERR_INVALID_FD;
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

    if (arr == NULL) {
        return ERR_NULL_POINTER;
    }

    for (size_t i = 0; i < nmemb; i++) {
        struct Character currentCharacter = arr[i];

        if (!isValidCharacter(&currentCharacter)) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            return ERR_INVALID_TYPE;
        }

        size_t elsWritten = fwrite(&currentCharacter, sizeof(struct Character), 1, fp);

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
//* done, not checked yet, no moodle test available, not improved
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

    *ptr = (struct Character *)malloc(num * sizeof(struct Character));
    if (*ptr == NULL) {
        if (fclose(fp) != 0) {
            return ERR_CLOSE_FILE;
        }
        return ERR_INSUFFICIENT_MEMORY;
    }

    for (size_t i = 0; i < num; ++i) {
        struct Character currentCharacter = {0};

        size_t elsRead = fread(&currentCharacter, sizeof(struct Character), 1, fp);

        if (elsRead != 1) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            memset(*ptr, 0, sizeof(num * sizeof(struct Character)));
            free(*ptr);
            return ERR_FILE_CORRUPTION;
        }

        if (!isValidCharacter(&currentCharacter)) {
            if (fclose(fp) != 0) {
                return ERR_CLOSE_FILE;
            }
            memset(*ptr, 0, sizeof(num * sizeof(struct Character)));
            free(*ptr);
            return ERR_INVALID_TYPE;
        }

        (*ptr)[i] = currentCharacter;
    }

    if (nmemb == NULL) {
        return ERR_NULL_POINTER;
    }
    *nmemb = num;

    if (fflush(fp) != 0) { 
        return ERR_FFLUSH;
    }

    if (fclose(fp) != 0) {
        return ERR_CLOSE_FILE;
    }

    return SUCCESS;
}

/**
 * @brief  Acquires appropriate permissions, loads the ItemDetails database, and then permanently drops permissions.
 * @note   It should attempt to acquire appropriate permissions for opening the ItemDetails database (that is: the effective userID should be set to the userID of pitchpoltadmin), should load the database from the specified file, and then (after permanently dropping privileges), call the function `playGame()` to which it should pass the loaded data.
 * @param  *filepath:
 * @retval Returns 1 if an error occurs during the deserialization process. It should check the running process’s permissions to ensure that the executable it was launched from was indeed a setUID executable owned by user pitchpoltadmin. If that is not the case, or if an error occurs in acquiring or dropping permissions, the function should return 2. In all other cases, it should return 0.
 */
//*check labs and forum (the setuid lab)
// "We could start it as one process, and then one of them splits off, and it's the privileged bit, and the other one splits off, and it's the unprivileged bit"
//"In the project for CITS3007, it will be up to you to ensure you follow good secure coding practices – dropping privileges when appropriate, calling fstat() instead of stat(), and checking the return values of functions that can fail – and following these practices will comprise a significant proportion of your mark."
//"Traditionally on Unix-like systems, running processes can be divided into two categories:
// - privileged processes, which have an effective user ID of 0(that is, root)
// - unprivileged processes – all others."
// https://cits3007.github.io/labs/lab04-solutions.html
// https://cits3007.github.io/labs/lab07-solutions.html
int secureLoad(const char *filepath) {
    uid_t originalEuid = geteuid();

    struct ItemDetails *ptr = NULL;
    size_t numItems = 0;

    // spawning a child process
    pid_t pid = fork();
    if (pid < 0) {
        return ERR_FORKING;
    } else if (pid == 0) { // child process (privileged)
        struct passwd *userInfo = getpwnam("pitchpoltadmin");
        if (userInfo == NULL) {
            return ERR_NO_USER_INFO;
        }

        // check permissions, acquire permissions to open the database
        if (originalEuid != userInfo->pw_uid) {
            if (seteuid(userInfo->pw_uid) != 0) {
                return ERR_ACQUIRE_PERMISSION;
            }
        }

        int fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            return ERR_OPEN_FILE;
        }

        // drop privileges
        //? seteuid(originalEuid) or seteuid(getuid())
        if (setuid(getuid()) != 0) {
            if (close(fd) != 0) {
                return ERR_CLOSE_FD;
            }
            return ERR_DROP_PERMISSION;
        }

        // check the fd UID and mode by fstat(), check the running process’s permissions to ensure that the executable it was launched from was indeed a setUID executable owned by user pitchpoltadmin.
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

        if (loadItemDetails(&ptr, &numItems, fd) != 0) {
            if (close(fd) != 0) {
                return ERR_CLOSE_FD;
            }
            return ERR_DESERIALIZATION;
        }

        if (close(fd) != 0) {
            return ERR_CLOSE_FD;
        }
    } else {
        // parent process (unprivileged)
        // wait for child process
        wait(NULL);
        playGame(ptr, numItems);
    }

    return 0;
}

/**
 *  "the spec says that your secureLoad() function must call the playGame() function; but no playGame() is defined in your p_and_p.c file.

For testing purposes only, you'll therefore probably want to define a playGame() function somewhere (so that you can successfully link and run tests on your code); if the function isn't defined, you'll get failures at link time.

But you should not include a definition of playGame() in your submitted code (since if you do, the tests we run on your submitted code won't compile and link correctly)"
*/

void playGame(struct ItemDetails *ptr, size_t nmemb);

//! delete the playGame() definition, but not the declaration before submitting this .c file
void playGame(struct ItemDetails *ptr, size_t nmemb) {
    // TODO: check if the privileges are dropped
}
