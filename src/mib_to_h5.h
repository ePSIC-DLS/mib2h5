// clang-format Language: C
#ifndef MIB_TO_H5_H
#define MIB_TO_H5_H

int mib_to_h5(const char *filename,
              const char *output_directory,
              const char *merlin_dset_name,
              const char *compressor,
              unsigned int shuffle,
              unsigned int compression_level);

#endif
