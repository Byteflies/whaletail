/**
 * Copyright (c) 2026 Byteflies
 *
 * Command line value parsing. Device agnostic: must not include an SDK header.
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdio.h>
#include <stddef.h>

/**
 * Parse a decimal number. The whole string must be consumed, so trailing
 * garbage ("1.0mV") is rejected rather than silently truncated. Values that
 * overflow or underflow a double are rejected, as are "inf" and "nan".
 *
 * @return 1 on success, 0 on failure. *out is untouched on failure.
 */
int parse_double(const char* text, double* out);

/**
 * Parse a decimal unsigned integer. As with parse_double(), the whole string
 * must be consumed. Negative values and values too large for an unsigned are
 * rejected rather than wrapping.
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

/**
 * Parse a comma separated list of names from table into a set of flags, for
 * options that select several of something at once. Values in table are used
 * as indices into flags, so they must all be in [0, flags_size).
 *
 * Repeating a name is harmless. An empty list is an error, since every caller
 * so far needs at least one entry selected.
 *
 * @param[out] flags           flags_size ints, zeroed and then set to 1 for
 *                             each name present in text
 * @param[out] bad_token       on failure, receives the token that did not parse
 * @return the number of names parsed, or -1 on failure
 */
int parse_enum_set(const enum_name* table, const char* text,
                   int* flags, int flags_size,
                   char* bad_token, size_t bad_token_size);

/** Print every name in table as a comma separated list, without a newline. */
void print_enum_names(FILE* stream, const enum_name* table);

#endif /* ARGPARSE_H */
