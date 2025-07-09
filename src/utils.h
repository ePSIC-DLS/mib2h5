// clang-format Language: C
#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <hdf5.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static inline void *xmalloc_debug(
  size_t size, const char *var, const char *func, const char *file, int line)
{
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "[ERROR] malloc error for %s: %s (at %s:%d in %s)\n", var,
            strerror(errno), file, line, func);
    exit(1);
  }
  return ptr;
}

#define XMALLOC(size, var)                                                     \
  xmalloc_debug((size), (var), __func__, __FILE__, __LINE__)

const char *only_file_name(const char *absolute_file_path);

char *create_output_filename(const char *input_path, const char *output_dir);

unsigned int num_of_headers(FILE *mib_ptr, const unsigned int stride);

void header_meta_from_first(FILE *mib_ptr,
                            char *header_id,
                            unsigned int *header_bytes,
                            unsigned int *num_chips,
                            unsigned int *det_x,
                            unsigned int *det_y,
                            char *pixel_depth);

hid_t bufsize_to_datatype(int dtype);

unsigned long get_filesystem_block_size(const char *path);

#endif
