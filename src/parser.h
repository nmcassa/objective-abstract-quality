#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Represents an abstract loaded from disk.
 * `text`   - null-terminated buffer containing the abstract contents
 * `length` - number of bytes in `text` (excluding the trailing null)
 */
typedef struct {
    char  *text;
    size_t length;
} Abstract;

/*
 * Load an abstract from `path` into `out`.
 * Returns 0 on success, non-zero on failure.
 * On success, caller must free with abstract_free().
 */
int abstract_load(const char *path, Abstract *out);

/*
 * Release memory owned by an Abstract.
 * Safe to call on a zero-initialized Abstract.
 */
void abstract_free(Abstract *a);

#endif /* PARSER_H */
