// clang-format Language: C
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

const char *only_file_name(const char *absolute_file_path);

unsigned int num_of_headers(FILE *mib_ptr, const unsigned int stride);

void header_meta_from_first(FILE *mib_ptr,
                            char *header_id,
                            unsigned int *header_bytes,
                            unsigned int *num_chips,
                            unsigned int *det_x,
                            unsigned int *det_y,
                            char *pixel_depth);
#endif
