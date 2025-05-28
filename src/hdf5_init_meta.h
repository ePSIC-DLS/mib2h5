// clang-format Language: C
#ifndef HDF5_INIT_META_H
#define HDF5_INIT_META_H

#include "macros.h"
#include <hdf5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void create_meta_mq1_fields_dataset(hid_t *file_id,
                                    hid_t *lcpl_id,
                                    hid_t *meta_handle);

void create_dac_dataset(unsigned int num_chips,
                        hid_t *file_id,
                        hid_t *lcpl_id,
                        hid_t *dac_handle);

void close_dataset_handle(hid_t *handle, size_t count);

#endif
