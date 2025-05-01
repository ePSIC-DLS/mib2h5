#include "hdf5_init.h"
#include "blosc_filter.h"
#include "macros.h"
#include "utils.h"
#include <blosc.h>
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize_plist(char *path,
                      hid_t *fapl_id,
                      hid_t *fcpl_id,
                      hid_t *lcpl_id)
{
  char *version = (char *) malloc(sizeof(char) * 512);
  if (version == NULL) {
    fprintf(stderr, "malloc for version failed\n");
    return;
  }
  char *date = (char *) malloc(sizeof(char) * 512);
  if (date == NULL) {
    fprintf(stderr, "malloc for date failed\n");
    free(version);
    free(date);
    return;
  }
  if ((*fcpl_id = H5Pcreate(H5P_FILE_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fcpl in create_file\n");
    free(version);
    free(date);
    return;
  }
  unsigned long f_blocksize = get_filesystem_block_size(path);
  printf("f_blocksize: %ld\n", f_blocksize);
  if ((*fapl_id = H5Pcreate(H5P_FILE_ACCESS)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fapl in create_file\n");
    free(version);
    free(date);
    return;
  } else {
    if (H5Pset_alignment(*fapl_id, ALIGNMENT_THRESHOLD, f_blocksize) < 0) {
      fprintf(stderr, "Error in H5Pset_alignment\n");
      free(version);
      free(date);
      return;
    }
  }
  if ((*lcpl_id = H5Pcreate(H5P_LINK_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating lcpl in initialize_dataset_plist\n");
    free(version);
    free(date);
    return;
  }
  if (register_blosc(&version, &date) < 0) {
    fprintf(stderr, "Error in register_blosc\n");
    free(version);
    free(date);
    return;
  } else {
    printf("Blosc version info: %s (%s)\n", version, date);
  }
  free(version);
  free(date);
}

void initialize_file(char *filename, hid_t *file_id, hid_t fapl, hid_t fcpl)
{
  if (!filename) {
    fprintf(stderr, "Empty or other error in filename, please check\n");
  }

  if (!H5Iis_valid(fapl) || !H5Iis_valid(fcpl)) {
    fprintf(stderr, "fapl or fcpl is invalid in initialize_file\n");
    return;
  }

  if ((*file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl)) ==
      H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating file_id in intialize_file\n");
    return;
  }
}

void create_merlin_dataset(hid_t *merlin_dataset_id,
                           hid_t file,
                           char *merlin_dataset_name,
                           int dtype,
                           hid_t memspace,
                           hid_t lcpl,
                           size_t dim,
                           hsize_t *frame_dim,
                           unsigned int compression_level,
                           unsigned int shuffle,
                           unsigned int compressor)
{
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

    if (H5Pset_chunk_cache(dapl, PRIME_FOR_HASH,
                           NUM_CHUNKS_IN_CACHE * dtype * y * x, 1.0) < 0) {
      fprintf(stderr, "Error in H5Pset)chunk_cache\n");
      return;
    }
  }

  hid_t datatype = bufsize_to_datatype(dtype);

  if ((*merlin_dataset_id = H5Dcreate2(file, merlin_dataset_name, datatype,
                                       memspace, lcpl, dcpl, dapl)) ==
      H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating merlin_dataset\n");
    return;
  }
}
