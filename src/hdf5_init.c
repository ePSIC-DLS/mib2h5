#include "hdf5_init.h"
#include "blosc_filter.h"
#include <blosc.h>
#include <hdf5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize_plist(char *filename,
                      hid_t *file_id,
                      hid_t *fapl_id,
                      hid_t *fcpl_id,
                      hid_t *lcpl_id)
{
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
  if ((*lcpl_id = H5Pcreate(H5P_LINK_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating lcpl in initialize_dataset_plist\n");
    return;
  }
}

void initialize_file(char *filename,
                     hid_t *file_id,
                     hid_t fapl,
                     hid_t fcpl)
{
  if (!H5Iis_valid(fapl) || !H5Iis_valid(fcpl)) {
    fprintf(stderr, "fapl or fcpl is invalid in initialize_file\n");
    return;
  }

  if ((*file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating file_id in intialize_file\n");
    return;
  }
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
