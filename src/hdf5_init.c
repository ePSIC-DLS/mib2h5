#include "hdf5_init.h"
#include "macros.h"
#include "utils.h"
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_plist(char *output_dir,
                      hid_t *fapl_id,
                      hid_t *fcpl_id,
                      hid_t *lcpl_id)
{
  if ((*fcpl_id = H5Pcreate(H5P_FILE_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fcpl in create_file\n");
    return;
  }
  unsigned long f_blocksize = get_filesystem_block_size(output_dir);
  printf("block size of filesystem: %ld\n", f_blocksize);
  if ((*fapl_id = H5Pcreate(H5P_FILE_ACCESS)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating fapl in create_file\n");
    return;
  } else {
    if (H5Pset_alignment(*fapl_id, ALIGNMENT_THRESHOLD, f_blocksize) < 0) {
      fprintf(stderr, "Error in H5Pset_alignment\n");
      return;
    }
  }
  if ((*lcpl_id = H5Pcreate(H5P_LINK_CREATE)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating lcpl in initialize_dataset_plist\n");
    return;
  }
}

void initialize_file(char *filename, hid_t *file_id, hid_t fapl, hid_t fcpl)
{
  if (filename == NULL) {
    fprintf(stderr, "Empty or other error in filename, please check\n");
    return;
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
                           hid_t dtype,
                           hid_t dcpl,
                           hid_t lcpl,
                           hsize_t *frame_dim)
{
  hid_t dapl = H5I_INVALID_HID;

  if ((dapl = H5Pcreate(H5P_DATASET_ACCESS)) == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating dapl_id\n");
    goto cleanup;
  } else {
    // frame_dim is 2D with det_y and det_x
    unsigned int y = frame_dim[0];
    unsigned int x = frame_dim[1];

    size_t dtype_size = H5Tget_size(dtype);
    if (H5Pset_chunk_cache(dapl, PRIME_FOR_HASH,
                           NUM_CHUNKS_IN_CACHE * dtype_size * y * x, 1.0) < 0) {
      fprintf(stderr, "Error in H5Pset_chunk_cache\n");
      goto cleanup;
    }
  }

  // create 3D dataspace for the dataset (frames x height x width)
  // frames start at 0 for expandable dataset
  hsize_t dims[3]    = {0, frame_dim[0], frame_dim[1]};
  hsize_t maxdims[3] = {H5S_UNLIMITED, frame_dim[0], frame_dim[1]};
  hid_t filespace    = H5Screate_simple(3, dims, maxdims);

  if ((*merlin_dataset_id = H5Dcreate2(file, merlin_dataset_name, dtype,
                                       filespace, lcpl, dcpl, dapl)) ==
      H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating merlin_dataset\n");
    H5Sclose(filespace);
    goto cleanup;
  }
  H5Sclose(filespace);

cleanup:
  if (H5Iis_valid(dapl))
    H5Pclose(dapl);
}
