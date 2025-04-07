#include <stdio.h>

#ifndef ONLY_FILE_NAME_H
#define ONLY_FILE_NAME_H
const char* only_file_name(const char* absolute_file_path);
#endif /*ONLY_FILE_NAME_H*/

#ifndef NUM_OF_HEADERS_H
#define NUM_OF_HEADERS_H
unsigned int num_of_headers(
    FILE* mib_ptr,
    const unsigned int stride
    );
#endif

#ifndef HEADER_META_FROM_FIRST_H
#define HEADER_META_FROM_FIRST_H
void header_meta_from_first(
    FILE* mib_ptr,
    char* header_id,
    unsigned int* header_bytes,
    unsigned int* num_chips,
    unsigned int* det_x,
    unsigned int* det_y,
    char* pixel_depth
    );
#endif
