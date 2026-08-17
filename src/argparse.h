/**
 * Copyright (c) 2026 Byteflies
 *
 * Command line value parsing. Device agnostic: must not include an SDK header.
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdio.h>

/**
 * Parse a decimal number. The whole string must be consumed, so trailing
 * garbage ("1.0mV") is rejected rather than silently truncated.
 *
 * @return 1 on success, 0 on failure. *out is untouched on failure.
 */
int parse_double(const char* text, double* out);

/**
 * Parse a decimal unsigned integer. As with parse_double(), the whole string
 * must be consumed.
 *
 * @return 1 on success, 0 on failure. *out is untouched on failure.
 */
int parse_unsigned(const char* text, unsigned* out);

/**
 * A name accepted on the command line for one SDK enum value. Enum types
 * differ per device, so values are carried as int and cast back by the caller.
 */
typedef struct {
    const char* name;
    int value;
} enum_name;

/** Terminates an enum_name table. */
#define ENUM_NAME_END { NULL, 0 }

/**
 * Look up a name in table, case insensitively. Several names may map to the
 * same value.
 *
 * @return 1 on success, 0 if the name is not in the table.
 */
int parse_enum(const enum_name* table, const char* name, int* out);

/** Print every name in table as a comma separated list, without a newline. */
void print_enum_names(FILE* stream, const enum_name* table);

#endif /* ARGPARSE_H */
