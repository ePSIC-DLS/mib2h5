// clang-format Language: C
#ifndef HDF5_INIT_H
#define HDF5_INIT_H

#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_plist(char *path,
                      hid_t *fapl_id,
                      hid_t *fcpl_id,
                      hid_t *lcpl_id);

void initialize_file(char *filename, hid_t *file_id, hid_t fapl, hid_t fcpl);

void create_merlin_dataset(hid_t *merlin_dataset_id,
                           hid_t file,
                           char *merlin_dataset_name,
                           int dtype,
                           hid_t memspace,
                           hid_t dcpl,
                           hid_t lcpl,
                           hsize_t *frame_dim);

#endif
