#include "mib_to_h5.h"
#include "append.h"
#include "compress.h"
#include "framebuffer.h"
#include "hdf5_init.h"
#include "hdf5_init_meta.h"
#include "macros.h"
#include "parser.h"
#include "read.h"
#include "utils.h"
#include <hdf5.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mib_to_h5(const char *filename,
              const char *output_directory,
              const char *merlin_dset_name,
              const char *compressor,
              unsigned int shuffle,
              unsigned int compression_level)
{
  FILE *mib_ptr                            = NULL;
  char *output_file                        = NULL;
  hid_t fapl_id                            = H5P_DEFAULT;
  hid_t fcpl_id                            = H5P_DEFAULT;
  hid_t lcpl_id                            = H5P_DEFAULT;
  hid_t file_id                            = H5I_INVALID_HID;
  hid_t dcpl                               = H5P_DEFAULT;
  hid_t memspace                           = H5I_INVALID_HID;
  hid_t dtype                              = H5I_INVALID_HID;
  hid_t merlin_dataset_id                  = H5I_INVALID_HID;
  hid_t meta_handle[MQ1_FIELDS_NUM_FIELDS] = {0};
  hid_t *dac_handle                        = NULL;
  framebuffer fb                           = {0};
  int ret                                  = -1;

  // validate inputs
  if (!filename || !output_directory || !merlin_dset_name) {
    fprintf(stderr, "Error: Missing required parameters\n");
    return -1;
  }

  // open MIB file
  mib_ptr = fopen(filename, "rb");
  if (!mib_ptr) {
    fprintf(stderr, "Error: Cannot open input file %s\n", filename);
    return -1;
  }

  // get header metadata from first frame
  char header_id[MQ1_CHAR_LEN_HEADER_ID] = {0};
  unsigned int header_bytes = 0, num_chips = 0, det_x = 0, det_y = 0;
  char pixel_depth[MQ1_CHAR_LEN_PIXEL_DEPTH] = {0};

  header_meta_from_first(mib_ptr, header_id, &header_bytes, &num_chips, &det_x,
                         &det_y, pixel_depth);

  // validate pixel_depth string length and format
  if (strlen(pixel_depth) != 3 || pixel_depth[0] != 'U' ||
      pixel_depth[1] < '0' || pixel_depth[1] > '9' || pixel_depth[2] < '0' ||
      pixel_depth[2] > '9') {
    fprintf(stderr, "Error: Invalid pixel depth format: %s\n", pixel_depth);
    goto cleanup;
  }

  // calculate size of pixel data type
  int bufsize = (pixel_depth[1] - '0') * 10 + (pixel_depth[2] - '0');
  bufsize     = bufsize / 8;

  // sanity check for detector dimensions
  // (arbitrarily limit to max 4096x4096 currently)
  if (det_x > 4096 || det_y > 4096) {
    fprintf(stderr,
            "Error: Detector dimensions exceed maximum (4096x4096): %ux%u\n",
            det_x, det_y);
    goto cleanup;
  }

  // calculate frame size in bytes
  size_t frame_size = det_x * det_y * bufsize;

  // calculate stride (header + frame data)
  size_t stride_calc = header_bytes + frame_size;

  // check stride fits in unsigned int (just for sure for API compatibility)
  if (stride_calc > UINT_MAX) {
    fprintf(stderr, "Error: Stride too large for unsigned int\n");
    goto cleanup;
  }
  unsigned int stride = (unsigned int) stride_calc;

  unsigned int num_frames = num_of_headers(mib_ptr, stride);

  printf("Converting %s:\n", filename);
  printf("  Header: %s, %u bytes\n", header_id, header_bytes);
  printf("  Detector: %ux%u, %s depth, %u chip(s)\n", det_x, det_y, pixel_depth,
         num_chips);
  printf("  Frames: %u, stride: %u bytes\n", num_frames, stride);

  // create output filename
  output_file = create_output_filename(filename, output_directory);
  if (!output_file) {
    fprintf(stderr, "Error: Cannot create output filename\n");
    goto cleanup;
  }

  // initialise HDF5 property lists
  initialize_plist(output_file, &fapl_id, &fcpl_id, &lcpl_id);

  // create HDF5 file
  initialize_file(output_file, &file_id, fapl_id, fcpl_id);
  if (file_id < 0) {
    fprintf(stderr, "Error: Cannot create HDF5 file\n");
    goto cleanup;
  }

  // create dataset creation property list with chunking
  dcpl = H5Pcreate(H5P_DATASET_CREATE);
  if (dcpl < 0) {
    fprintf(stderr, "Error: Cannot create dataset property list\n");
    goto cleanup;
  }

  hsize_t chunk_dims[3] = {1, det_y, det_x};
  if (H5Pset_chunk(dcpl, 3, chunk_dims) < 0) {
    fprintf(stderr, "Error: Cannot set chunk dimensions\n");
    goto cleanup;
  }

  // TODO: Compression functionality to be implemented in a future PR
  // The compression parameters (compressor, shuffle, compression_level) are
  // currently accepted but not used. The cbytes parameter is set to 0 to
  // indicate no compression.
  int cbytes = 0;

  // create data memspace
  hsize_t frame_dim[2] = {det_y, det_x};
  memspace             = H5Screate_simple(2, frame_dim, NULL);
  if (memspace < 0) {
    fprintf(stderr, "Error: Cannot create memory dataspace\n");
    goto cleanup;
  }

  // determine datatype
  dtype = bufsize_to_datatype(bufsize);
  if (dtype < 0) {
    fprintf(stderr, "Error: Invalid buffer size for datatype\n");
    goto cleanup;
  }

  // create main dataset
  create_merlin_dataset(&merlin_dataset_id, file_id, merlin_dset_name, dtype,
                        dcpl, lcpl_id, frame_dim);
  if (merlin_dataset_id < 0) {
    fprintf(stderr, "Error: Cannot create main dataset\n");
    goto cleanup;
  }

  // create metadata datasets
  create_meta_mq1_fields_dataset(file_id, lcpl_id, meta_handle);

  // create DAC datasets
  size_t dac_size = sizeof(hid_t) * DAC_NUM_FIELDS * num_chips;
  dac_handle      = malloc(dac_size);
  if (!dac_handle) {
    fprintf(stderr, "Error: Cannot allocate memory for DAC handles\n");
    goto cleanup;
  }
  create_dac_dataset(num_chips, file_id, lcpl_id, dac_handle);

  // allocate framebuffer
  allocate_frame_header(&fb);

  // read first header to get dimensions for frame data allocation
  read_header(mib_ptr, 0, &fb);
  allocate_frame_data(&fb);

  // check if allocation succeeded
  if (!fb.data || !fb.rows) {
    fprintf(stderr, "Error: Failed to allocate framebuffer\n");
    goto cleanup;
  }

  // process all frames
  for (unsigned int i = 0; i < num_frames; ++i) {
    unsigned long offset = (unsigned long) i * stride;

    // read frame header and data
    read_header(mib_ptr, offset, &fb);
    read_frame(mib_ptr, offset, &fb);

    // append frame data to dataset
    append_frame_to_dataset(merlin_dataset_id, &fb, cbytes);

    // append metadata
    append_meta_to_dataset(meta_handle, &fb);

    // append DAC data
    append_dac_to_dataset(num_chips, dac_handle, &fb);

    // progress indicator
    if ((i + 1) % 100 == 0 || i == num_frames - 1) {
      printf("\rProcessed %u/%u frames", i + 1, num_frames);
      fflush(stdout);
    }
  }
  printf("\n");

  printf("Conversion complete: %s\n", output_file);
  ret = 0; // success

cleanup:
  // cleanup framebuffer
  if (fb.data || fb.rows) {
    deallocate_frame(&fb);
  }

  // close HDF5 handles
  if (memspace >= 0)
    H5Sclose(memspace);
  if (merlin_dataset_id >= 0)
    H5Dclose(merlin_dataset_id);

  for (int i = 0; i < MQ1_FIELDS_NUM_FIELDS; ++i) {
    if (meta_handle[i] > 0 && H5Iis_valid(meta_handle[i])) {
      H5Dclose(meta_handle[i]);
    }
  }

  if (dac_handle) {
    for (unsigned int i = 0; i < DAC_NUM_FIELDS * num_chips; ++i) {
      if (dac_handle[i] > 0 && H5Iis_valid(dac_handle[i])) {
        H5Dclose(dac_handle[i]);
      }
    }
    free(dac_handle);
  }

  if (dcpl != H5P_DEFAULT && dcpl >= 0)
    H5Pclose(dcpl);
  if (fapl_id != H5P_DEFAULT && fapl_id >= 0)
    H5Pclose(fapl_id);
  if (fcpl_id != H5P_DEFAULT && fcpl_id >= 0)
    H5Pclose(fcpl_id);
  if (lcpl_id != H5P_DEFAULT && lcpl_id >= 0)
    H5Pclose(lcpl_id);
  if (file_id >= 0)
    H5Fclose(file_id);

  if (mib_ptr)
    fclose(mib_ptr);
  if (output_file)
    free(output_file);

  return ret;
}
