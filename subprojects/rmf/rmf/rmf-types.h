#ifndef RMF_TYPES_H
#define RMF_TYPES_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include <stddef.h>
#include <stdint.h>

typedef uint8_t rmf_byte;
typedef uint32_t rmf_int;
typedef float rmf_float;

typedef struct {
    rmf_byte length;
    char data[256];
} rmf_nstring;

typedef struct {
    rmf_byte r, g, b;
} rmf_color;

typedef struct {
    rmf_float x, y, z;
} rmf_vector;

/* clang-format off */
static_assert(sizeof(rmf_byte) == 1);
static_assert(sizeof(rmf_int) == 4);
static_assert(sizeof(rmf_float) == 4);
static_assert(
    sizeof(rmf_color) == 3
    && offsetof(rmf_color, r) == 0
    && offsetof(rmf_color, g) == 1
    && offsetof(rmf_color, b) == 2);
static_assert(
    sizeof(rmf_vector) == 12
    && offsetof(rmf_vector, x) == 0
    && offsetof(rmf_vector, y) == 4
    && offsetof(rmf_vector, z) == 8);
/* clang-format on */

#endif
