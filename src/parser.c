#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Returns 1 if `line` (up to `len` bytes) is a comment line, else 0.
 * A comment line is one whose first non-whitespace character is '#'.
 * Fully blank lines are NOT treated as comments here.
 */
static int is_comment_line(const char *line, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = line[i];
        if (c == ' ' || c == '\t') {
            continue;
        }
        return c == '#';
    }
    return 0;
}

int abstract_load(const char *path, Abstract *out) {
    if (path == NULL || out == NULL) {
        return 1;
    }

    out->text = NULL;
    out->length = 0;

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "error: could not open '%s'\n", path);
        return 1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "error: could not seek '%s'\n", path);
        fclose(fp);
        return 1;
    }

    long size = ftell(fp);
    if (size < 0) {
        fprintf(stderr, "error: could not tell size of '%s'\n", path);
        fclose(fp);
        return 1;
    }
    rewind(fp);

    char *raw = (char *)malloc((size_t)size + 1);
    if (raw == NULL) {
        fprintf(stderr, "error: out of memory loading '%s'\n", path);
        fclose(fp);
        return 1;
    }

    size_t read = fread(raw, 1, (size_t)size, fp);
    fclose(fp);

    if (read != (size_t)size) {
        fprintf(stderr, "error: short read on '%s' (%zu of %ld bytes)\n",
                path, read, size);
        free(raw);
        return 1;
    }
    raw[read] = '\0';

    /*
     * Allocate the output buffer. It can be at most as large as the raw
     * input (we only ever remove bytes), so one allocation is enough.
     */
    char *clean = (char *)malloc(read + 1);
    if (clean == NULL) {
        fprintf(stderr, "error: out of memory loading '%s'\n", path);
        free(raw);
        return 1;
    }

    size_t out_len = 0;
    size_t i = 0;
    while (i < read) {
        /* Find end of current line (exclusive of the newline). */
        size_t j = i;
        while (j < read && raw[j] != '\n' && raw[j] != '\r') {
            j++;
        }
        size_t line_len = j - i;

        if (!is_comment_line(&raw[i], line_len)) {
            memcpy(&clean[out_len], &raw[i], line_len);
            out_len += line_len;
            clean[out_len++] = '\n';
        }

        /* Skip past the line terminator(s): \n, \r, or \r\n. */
        if (j < read && raw[j] == '\r') j++;
        if (j < read && raw[j] == '\n') j++;
        i = j;
    }

    free(raw);

    /* Trim trailing newlines/carriage-returns for tidy output. */
    while (out_len > 0 && (clean[out_len - 1] == '\n' || clean[out_len - 1] == '\r')) {
        out_len--;
    }
    clean[out_len] = '\0';

    out->text = clean;
    out->length = out_len;
    return 0;
}

void abstract_free(Abstract *a) {
    if (a == NULL) {
        return;
    }
    free(a->text);
    a->text = NULL;
    a->length = 0;
}
