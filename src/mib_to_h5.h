// clang-format Language: C
#ifndef MIB_TO_H5_H
#define MIB_TO_H5_H

#include <stdbool.h>

// function for single file conversion
int mib_to_h5_single_file(const char *filename,
                          const char *output_directory,
                          const char *dataset_key,
                          bool include_metadata,
                          const char *compressor,
                          unsigned int shuffle,
                          unsigned int compression_level);

#endif
