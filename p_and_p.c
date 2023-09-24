#include <p_and_p.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Finish first, then improve

/**
 * Requirments: Code should handle errors gracefully when reading or writing files – such errors include file open failures, insufficient memory, and file corruption.
Additionally, since the code will be part of a library – rather than being an executable – it should:
• never print to standard out or standard error (unless the specification states otherwise); and
• never exit or abort, but instead return with an error value, unless the specification states otherwise.
*/

/**
 * @brief  Serializes an array of ItemDetails structs. It should store the array using the ItemDetails file format.
 *
 * @param  arr: An array of ItemDetails structs.
 * @param  numEls: Number of items.
 * @param  fd: A file descriptor.
 * @retval Returns 1 if an error occurs in the serialization process. Otherwise, it returns 0.
 */
int saveItemDetails(const struct ItemDetails *arr, size_t numEls, int fd) {
    return 0;
}

//! no information about this function in the project specification
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
    return 0;
}

/**
 * @brief  Checks whether a string constitutes a valid name field, which requires the characters contained to have a graphical representation (as defined by the C function isgraph). No other characters are permitted. This means that names cannot contain (for instance) whitespace or control characters.
 * @note   A name field is always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: A string (a pointer to a character).
 * @retval Returns 1 if it is a valid name field, and 0 if not.
 */
int isValidName(const char *str) {
    return 0;
}

/**
 * @brief  Checks whether a string constitutes a valid multi-word field, which may contain all the characters in a name field, and may also contain space characters (but the first and the last characters must not be spaces).
 * @note   A multi-word field is always a DEFAULT_BUFFER_SIZE block of bytes. The block contains a NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. It is undefined what characters are in the block after the first NUL byte.
 * @param  *str: A string (a pointer to a character).
 * @retval Returns 1 if it is a valid multi-word field, and 0 if not.
 */
int isValidMultiword(const char *str) {
    return 0;
}

/**
 * @brief  Checks whether an ItemDetails struct is valid.
 * @note   A valid ItemDetails format means that its name and desc fields are valid name and multi-word fields, respectively.
 * @param  *id: A pointer to an ItemDetails struct.
 * @retval Returns 1 if the struct is valid, and 0 if not.
 */
int isValidItemDetails(const struct ItemDetails *id) {
    return 0;
}

//* done, not improved
/**
 * @brief  Checks whether a Character struct is valid.
 * @note   A Character struct is valid iff: the profession field is a valid name field, the name field is a valid multi-word field, the total number of items carried does not exceed MAX_ITEMS, and inventorySize is less than or equal to MAX_ITEMS.
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
    int n_items = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        n_items += c->inventory[i].quantity;
    }
    if (n_items > MAX_ITEMS) {
        return 0;
    }

    return 1;
}

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
int secureLoad(const char *filepath) {
    return 0;
}

void playGame(struct ItemDetails *ptr, size_t numEls);
