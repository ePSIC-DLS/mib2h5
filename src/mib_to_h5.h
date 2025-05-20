// clang-format Language: C
#ifndef MIB_TO_H5_H
#define MIB_TO_H5_H

int mib_to_h5(char *filename,
              char *output_directory,
              char *merlin_dset_name,
              char *compressor,
              unsigned int shuffle,
              unsigned int compression_level);

#endif
