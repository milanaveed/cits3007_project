#include <p_and_p.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SUCCESS 0
#define ERR_OPEN_FILE -1
#define ERR_INSUFFICIENT_MEMORY -2
//#define ERR_


//? Do we need this?
static_assert(sizeof(size_t) ==8, "we assume the size of size_t is 64bit.");

// TODO: Finish first, then improve
//* pay attention to type specifier: size_t or uint64_t
//* compile with gcc -I.
//* write a fairly brief documentation block: when documenting, we can assume the reader is familiar with the file format and the struct types
//* [FILE* and file descriptor read/write performance](https://stackoverflow.com/questions/17524512/file-and-file-descriptor-read-write-performance/17524609#17524609)
//* will get low mark if too many comments. Comments should be used to explain what is not in the code
//! sanitise things in and out
//? when should we zero things out? By using memset?
//* only submit this .c file
//* Inline comments should not say what the code is doing – anyone who understands the programming language should be able to see that – but rather why it is doing it.

/**
 * Requirments: Code should handle errors gracefully when reading or writing files – such errors include file open failures, insufficient memory, and file corruption.
Additionally, since the code will be part of a library – rather than being an executable – it should:
• never print to standard out or standard error (unless the specification states otherwise); and
• never exit or abort, but instead return with an error value, unless the specification states otherwise.
*/

/**
 //* Once your validation functions are complete, you should incorporate them into loadItemDetails and saveItemDetails where applicable, and those functions should return an error if they encounter an invalid struct or file record.
 */
/**
 * @brief  Serializes an array of ItemDetails structs. It should store the array using the ItemDetails file format.
 *
 * @param  arr: An array of ItemDetails structs.
 * @param  numEls: Number of items.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
//* take some structs as input, and produce a file that matches the specification given
//*There's also a tip in this week's lab – when working with a FILE pointer obtained from a file descriptor, you might need to call fflush (see man fflush) to "write out" those buffers. The buffers are automatically written out when a FILE pointer is closed with fclose; but if you have a FILE pointer you created yourself (e.g. with fdopen) that just goes out of scope, then you may need to flush the buffers before your function exits.
//* use fread and fwrite
int saveItemDetails(const struct ItemDetails *arr, size_t numEls, int fd) {
    return 0;
}

//* not required to implement that, but will be helpful to do so when doing my own testing
/**
 * @brief
 * @note
 * @param  arr:
 * @param  numEls:
 * @param  filename:
 * @retval
 */
int saveItemDetailsToPath(const struct ItemDetails *arr, size_t numEls, const char *filename) {
    return 0;
}

/**
 * @brief  Deserializes a file, including allocating enough memory to store the number of records contained in the file, and write the address of that memory to ptr. The memory is to be freed by the caller. Write all records contained in the file into the allocated memory. Write the number of records to numEls.
 *
 * @param  ptr: The address of a pointer-to-ItemDetails struct.
 * @param  numEls: The address of a size_t.
 * @param  fd: A file descriptor for the file being deserialized.
 * @retval Returns 1 if an error occurs, and no net memory should be allocated (any allocated memory should be freed). Otherwise, it returns 0.
 */
int loadItemDetails(struct ItemDetails **ptr, size_t *numEls, int fd) {
    //Read the number of records from the file header

    
    
    return 0;
}

//* done, not improved
/**
 * @brief  Checks whether a string constitutes a valid name field, which requires the characters contained to have a graphical representation (as defined by the C function isgraph). No other characters are permitted. This means that names cannot contain (for instance) whitespace or control characters.
 * @note   A name field is always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: A string (a pointer to a character).
 * @retval Returns 1 if it is a valid name field, and 0 if not.
 */
int isValidName(const char *str) {
    // check the length (at most DEFAULT_BUFFER_SIZE-1) and characters
    //? Should we use the strlen() to get the length of the string, or should we count it manually? Which is a better practice, way 1 or way 2?
    //* way 1:
    size_t length = strlen(str);
    if (length >= DEFAULT_BUFFER_SIZE) {
        return 0;
    }
    for (size_t i = 0; i < length; i++) {
        if (!isgraph(str[i])) {
            return 0;
        }
    }

    //* way 2:
    // char *p = str;
    // size_t length = 0;
    // while (*p) {
    //     if (!isgraph(*p)) {
    //         return 0;
    //     }
    //     length++;
    //     if (length >= DEFAULT_BUFFER_SIZE) {
    //         return 0;
    //     }
    //     p++;
    // }

    return 1;
}

//* done, not improved
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

//* done, not improved
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

//* done, not improved
/**
 * @brief  Checks whether a Character struct is valid.
 * @note   A Character struct is valid iff: the profession field is a valid name field, the name field is a valid multi-word field, the total number of items carried does not exceed MAX_ITEMS, and inventorySize is less than or equal to MAX_ITEMS.
 * `inventorySize` is a 64-bit unsigned integer value
 * @param  *c: A pointer to a Character struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidCharacter(const struct Character *c) {
    // Check if the profession field is a valid name field
    // Check if the name field is a valid multi-word field
    // Check if inventorySize is less than or equal to MAX_ITEMS
    if (!isValidName(c->profession) || !isValidMultiword(c->name) || c->inventorySize > MAX_ITEMS) {
        return 0;
    }

    // Check if the total number of items carried does not exceed MAX_ITEMS
    //?! size_t or uint64_t
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
 * @param  *arr:
 * @param  numEls:
 * @param  fd: A file descriptor.
 * @retval
 */
int saveCharacters(struct Character *arr, size_t numEls, int fd) {
    return 0;
}

/**
 * @brief  Loads characters in the Character file format, and validates records using the isValidCharacter function, but otherwise behave in the same way as loadItemDetails.
 * @note
 * @param  **ptr:
 * @param  *numEls:
 * @param  fd:
 * @retval
 */
int loadCharacters(struct Character **ptr, size_t *numEls, int fd) {
    return 0;
}

//! parameters are inconsistent with the project specification
/**
 * @brief  Acquires appropriate permissions, loads the ItemDetails database, and then permanently drops permissions.
 * @note   It should attempt to acquire appropriate permissions for opening the ItemDetails database (that is: the effective userID should be set to the userID of pitchpoltadmin), should load the database from the specified file, and then (after permanently dropping privileges), call the function `playGame()` to which it should pass the loaded data.
 * @param  *filepath:
 * @retval Returns 1 if an error occurs during the deserialization process. It should check the running process’s permissions to ensure that the executable it was launched from was indeed a setUID executable owned by user pitchpoltadmin. If that is not the case, or if an error occurs in acquiring or dropping permissions, the function should return 2. In all other cases, it should return 0.
 */
//*check labs and forum (the setuid lab)
// "We could start it as one process, and then one of them splits off, and it's the privileged bit, and the other one splits off, and it's the unprivileged bit"
int secureLoad(const char *filepath) {
    return 0;
}

/**
 *  "the spec says that your secureLoad() function must call the playGame() function; but no playGame() is defined in your p_and_p.c file.

For testing purposes only, you'll therefore probably want to define a playGame() function somewhere (so that you can successfully link and run tests on your code); if the function isn't defined, you'll get failures at link time.

But you should not include a definition of playGame() in your submitted code (since if you do, the tests we run on your submitted code won't compile and link correctly)"
*/
//! delete the playGame() definition, but not the declaration
void playGame(struct ItemDetails *ptr, size_t numEls);
