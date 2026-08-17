/**
 * Copyright (c) 2026 Byteflies
 */

#include <stdlib.h>
#include <strings.h>

#include "argparse.h"

int parse_double(const char* text, double* out)
{
    char* endptr = NULL;
    double value;

    if (text[0] == '\0') {
        return 0;
    }

    value = strtod(text, &endptr);
    if (endptr == text || *endptr != '\0') {
        return 0;
    }

    *out = value;
    return 1;
}

int parse_unsigned(const char* text, unsigned* out)
{
    char* endptr = NULL;
    unsigned long value;

    if (text[0] == '\0') {
        return 0;
    }

    value = strtoul(text, &endptr, 10);
    if (endptr == text || *endptr != '\0') {
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

void print_enum_names(FILE* stream, const enum_name* table)
{
    const enum_name* entry;

    for (entry = table; entry->name != NULL; entry++) {
        fprintf(stream, "%s%s", entry == table ? "" : ", ", entry->name);
    }
}
