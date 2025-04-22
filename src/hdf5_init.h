// clang-format Language: C
#ifndef HDF5_INIT_H
#define HDF5_INIT_H

#include <hdf5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void check_space_and_storage(hid_t dset);

void initialize_file_and_plist(char *filename,
                               hid_t *file_id,
                               hid_t *fapl_id,
                               hid_t *fcpl_id);

void initialize_lcpl(hid_t *lcpl_id);

void create_merlin_dataset(hid_t *merlin_dataset_id,
                           hid_t *file_id,
                           char *merlin_dataset_name,
                           int dtype,
                           hid_t memspace,
                           hid_t *lcpl_id,
                           size_t dim,
                           hsize_t *frame_dim,
                           unsigned int compression_level,
                           unsigned int shuffle,
                           unsigned int compressor);

#endif
