#include "hdf5_init.h"
#include "blosc_filter.h"

#include <blosc.h>
#include <hdf5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
// functions for debugging
void check_space_and_storage(hid_t dset) {
        H5D_space_status_t space_status;
        hsize_t storage_size;

        status = H5Dget_space_status(dset, &space_status);
        storage_size = H5Dget_storage_size(dset);
        printf("Space for dataset has%sbeen allocated. \n", space_status ==
H5D_SPACE_STATUS_ALLOCATED ? " " : " NOT "); printf("Storage size for dataset is
: %ld bytes.\n", (long)storage_size);
}
*/

// REAL functions
void initialize_file_and_plist(char *filename,
                               hid_t *file_id,
                               hid_t *fapl_id,
                               hid_t *fcpl_id)
{
  unsigned mode = H5F_ACC_TRUNC;
  char *version, *date;

  if ((*fcpl_id = H5Pcreate(H5P_FILE_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fcpl in create_file\n");
    return;
  }
  if ((*fapl_id = H5Pcreate(H5P_FILE_ACCESS)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fapl in create_file\n");
    return;
  } else {
    if (H5Pset_alignment(*fapl_id, 1024, 4096) < 0) {
      fprintf(stderr, "Error in H5Pset_alignment\n");
      return;
    }
  }
  if ((*file_id = H5Fcreate(filename, mode, *fcpl_id, *fapl_id)) ==
      H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating file_id in create_file, please check if "
                    "file already existed\n");
    return;
  }
  if (register_blosc(&version, &date) < 0) {
    fprintf(stderr, "Error in register_blosc\n");
    return;
  } else {
    printf("Blosc version info: %s (%s)\n", version, date);
    free(version);
    free(date);
  }
}

void initialize_lcpl(hid_t *lcpl_id)
{
  if ((*lcpl_id = H5Pcreate(H5P_LINK_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating lcpl in initialize_dataset_plist\n");
    return;
  }
  /*
  else {
    if (H5Pset_create_intermediate_group(*lcpl_id, 1) < 0) {
      fprintf(stderr, "Error in H5Pset_create_intermediate_group\n");
      return;
    }
    if (H5Pset_char_encoding(*lcpl_id, H5T_CSET_UTF8) < 0) {
      fprintf(stderr, "Error in H5Pset_char_encoding\n");
      return;
    }
  }
  */
}

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
                           unsigned int compressor)
{
  hid_t file = *file_id;
  hid_t lcpl = *lcpl_id;
  hid_t dcpl;
  hid_t dapl;
  // int fill_value = -1;
  unsigned int cd_values[7] = {0};

  if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating dcpl\n");
    return;
  } else {
    if (H5Pset_chunk(dcpl, dim, frame_dim) < 0) {
      fprintf(stderr, "Error in H5Pset_chunk\n");
      return;
    }
    if (H5Pset_fill_time(dcpl, H5D_FILL_TIME_NEVER) < 0) {
      fprintf(stderr, "Error in H5Pset_fill_time\n");
      return;
    }
    /*
    if (H5Pset_fill_value(dcpl, H5T_NATIVE_INT, &fill_value) < 0) {
      fprintf(stderr, "Error in H5Pset_fill_value\n");
      return;
    }
   */
    cd_values[0] = 0;
    cd_values[1] = compression_level;
    cd_values[2] = shuffle;
    cd_values[3] = 0; // blocksize
    cd_values[4] = 0; // unused
    cd_values[5] = 0; // unused
    cd_values[6] = compressor;
    if (H5Pset_filter(dcpl, FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values) <
        0) {
      fprintf(stderr, "Error in H5Pset_filter\n");
      return;
    }
  }

  if ((dapl = H5Pcreate(H5P_DATASET_ACCESS)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating dapl_id\n");
    return;
  } else {
    unsigned int y = frame_dim[1];
    unsigned int x = frame_dim[2];

    if (H5Pset_chunk_cache(dapl, 521, 2 * dtype * y * x, 1.0) < 0) {
      fprintf(stderr, "Error in H5Pset)chunk_cache\n");
      return;
    }
  }

  hid_t datatype;
  switch (dtype) {
    case 1: {
      datatype = H5T_STD_U8LE;
      break;
    }
    case 2: {
      datatype = H5T_STD_U16LE;
      break;
    }
    case 4: {
      datatype = H5T_STD_U32LE;
      break;
    }
    case 8: {
      datatype = H5T_STD_U64LE;
      break;
    }
    default: {
      fprintf(stderr, "Error in datatype, please check input dtype\n");
      return;
    }
  }

  if ((*merlin_dataset_id = H5Dcreate2(file, merlin_dataset_name, datatype,
                                       memspace, lcpl, dcpl, dapl)) ==
      H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating merlin_dataset\n");
    return;
  }
}
