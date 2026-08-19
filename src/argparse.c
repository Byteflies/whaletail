/**
 * Copyright (c) 2026 Byteflies
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "argparse.h"

int parse_double(const char* text, double* out)
{
    char* endptr = NULL;
    double value;

    if (text[0] == '\0') {
        return 0;
    }

    errno = 0;
    value = strtod(text, &endptr);
    if (endptr == text || *endptr != '\0') {
        return 0;
    }

    /* strtod() reports overflow and underflow through ERANGE, and happily
       accepts "inf" and "nan", none of which are a usable signal parameter. */
    if (errno == ERANGE || !isfinite(value)) {
        return 0;
    }

    *out = value;
    return 1;
}

int parse_unsigned(const char* text, unsigned* out)
{
    const char* first = text;
    char* endptr = NULL;
    unsigned long value;

    if (text[0] == '\0') {
        return 0;
    }

    /* strtoul() accepts a leading minus sign and wraps the result, so "-1"
       would otherwise parse as UINT_MAX rather than being rejected. */
    while (isspace((unsigned char)*first)) {
        first++;
    }
    if (*first == '-') {
        return 0;
    }

    errno = 0;
    value = strtoul(text, &endptr, 10);
    if (endptr == text || *endptr != '\0') {
        return 0;
    }

    /* ERANGE catches overflow of unsigned long; the comparison catches the
       narrowing to unsigned, which is half as wide on LP64. */
    if (errno == ERANGE || value > UINT_MAX) {
        return 0;
    }

    *out = (unsigned)value;
    return 1;
}

int parse_enum(const enum_name* table, const char* name, int* out)
{
    const enum_name* entry;

    for (entry = table; entry->name != NULL; entry++) {
        if (strcasecmp(name, entry->name) == 0) {
            *out = entry->value;
            return 1;
        }
    }

    return 0;
}

int parse_enum_set(const enum_name* table, const char* text,
                   int* flags, int flags_size,
                   char* bad_token, size_t bad_token_size)
{
    char buffer[256];
    char* saveptr = NULL;
    char* token;
    int count = 0;
    int i;

    if (strlen(text) >= sizeof(buffer)) {
        snprintf(bad_token, bad_token_size, "%s", text);
        return -1;
    }
    snprintf(buffer, sizeof(buffer), "%s", text);

    for (i = 0; i < flags_size; i++) {
        flags[i] = 0;
    }

    for (token = strtok_r(buffer, ",", &saveptr); token != NULL;
         token = strtok_r(NULL, ",", &saveptr)) {
        int value;

        if (!parse_enum(table, token, &value) || value < 0 || value >= flags_size) {
            snprintf(bad_token, bad_token_size, "%s", token);
            return -1;
        }

        flags[value] = 1;
        count++;
    }

    if (count == 0) {
        snprintf(bad_token, bad_token_size, "%s", text);
        return -1;
    }

    return count;
}

void print_enum_names(FILE* stream, const enum_name* table)
{
    const enum_name* entry;

    for (entry = table; entry->name != NULL; entry++) {
        fprintf(stream, "%s%s", entry == table ? "" : ", ", entry->name);
    }
}
