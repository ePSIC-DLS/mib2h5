// clang-format Language: C
#include <hdf5.h>
#include <stdint.h>
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

static inline uint16_t convert_uint16_be(const uint8_t *bytes)
{
  return ((uint16_t) bytes[0] << 8) | bytes[1];
}

static inline uint32_t convert_uint32_be(const uint8_t *bytes)
{
  return ((uint32_t) bytes[0] << 24) | ((uint32_t) bytes[1] << 16) |
         ((uint32_t) bytes[2] << 8) | bytes[3];
}

static inline uint64_t convert_uint64_be(const uint8_t *bytes)
{
  return ((uint64_t) bytes[0] << 56) | ((uint64_t) bytes[1] << 48) |
         ((uint64_t) bytes[2] << 40) | ((uint64_t) bytes[3] << 32) |
         ((uint64_t) bytes[4] << 24) | ((uint64_t) bytes[5] << 16) |
         ((uint64_t) bytes[6] << 8) | bytes[7];
}

const char *only_file_name(const char *absolute_file_path);

unsigned int num_of_headers(FILE *mib_ptr, const unsigned int stride);

void header_meta_from_first(FILE *mib_ptr,
                            char *header_id,
                            unsigned int *header_bytes,
                            unsigned int *num_chips,
                            unsigned int *det_x,
                            unsigned int *det_y,
                            char *pixel_depth);

hid_t bufsize_to_datatype(int dtype);
#endif
