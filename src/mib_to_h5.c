#include "append.h"
#include "compress.h"
#include "framebuffer.h"
#include "hdf5_init.h"
#include "hdf5_init_meta.h"
#include "io_header.h"
#include "macros.h"
#include "mib_header.h"
#include "read.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* I put these here for now, will move to macros.h later
 *
 * I think setting blocksize 0 is to let the system guess the best blocksize
 * I am not sure should we calculate the optimal blocksize, let blosc calculate
 * or let user specify
 *
 * As for numinternalthreads, I am not sure if we should detect how many threads
 * are available and use that or just stick to one, or again let user specify
 *
 * But these are only for blosc
 */

#define BLOCKSIZE 0
#define NUMINTERNALTHREADS 1

/* Options for blosc compression
 * BLOSC_BLOSCLZ blosclz
 * BLOSC_LZ4 lz4
 * BLOSC_LZ4HC lz4hc
 * BLOSC_SNAPPY snappy
 * BLOSC_ZLIB zlib
 * BLOSC_ZSTD zstd
 */

#define DIM 3

int mib_to_h5(char *filename,
              char *output_directory,
              char *merlin_dset_name,
              char *compressor,
              unsigned int shuffle,
              unsigned int compression_level)
{
  // === start get and check arguments ===
  clock_t begin_total = clock();

  const char *base = strrchr(filename, '/');
  base             = base ? base + 1 : filename;

  char hdf5_filename[256];
  strncpy(hdf5_filename, base, sizeof(hdf5_filename) - 1);
  hdf5_filename[sizeof(hdf5_filename) - 1] = '\0';

  char *dot = strrchr(hdf5_filename, '.');
  if (dot)
    *dot = '\0';

  size_t len = strlen(hdf5_filename);
  if (len + strlen(".h5") < sizeof(hdf5_filename)) {
    strcat(hdf5_filename, ".h5");
  } else {
    fprintf(stderr, "Filename buffer too small to append .h5\n");
  }

  char full_path[512];
  snprintf(full_path, sizeof(full_path), "%s/%s", output_directory,
           hdf5_filename);

  FILE *mib_ptr = fopen(filename, "rb");
  if (!mib_ptr) {
    perror("Failed to open .mib file");
    return 1;
  }

  // === end ===

  printf("+++ get and check argument done +++\n\n");

  // === start declaring variables and get metadata for the .mib file ===

  clock_t begin = clock();

  /* Please check the struct framebuffer from the header files in /include
   */

  framebuffer frame;
  framebuffer *frame_ptr = &frame;
  long ptr_pos           = ftell(mib_ptr);
  // printf("pointer starting position: %ld\n", ptr_pos);

  char *header_id            = malloc(4 * sizeof(char));
  unsigned int *header_bytes = malloc(5 * sizeof(unsigned int));
  unsigned int *num_chips    = malloc(sizeof(unsigned int));
  unsigned int *det_x        = malloc(4 * sizeof(unsigned int));
  unsigned int *det_y        = malloc(4 * sizeof(unsigned int));
  char *pixel_depth          = malloc(4 * sizeof(char));

  header_meta_from_first(mib_ptr, header_id, header_bytes, num_chips, det_x,
                         det_y, pixel_depth);
  int bufsize = (pixel_depth[1] - '0') * 10 + (pixel_depth[2] - '0');
  bufsize     = bufsize / 8;

  // === end ===

  printf("+++ declaring variables done +++\n\n");
  clock_t end = clock();
  double time_get_header_meta_from_first =
    (double) (end - begin) / CLOCKS_PER_SEC;
  printf("+++ Time to get header meta data: %02f +++\n",
         time_get_header_meta_from_first);

  // === start initialize hdf5 ===

  begin = clock();

  /* Declare variables related to creating hdf5 files
   * Please check the functions in hdf5_init.c and hdf5_meta_init.c
   * Handles are arrays that stored dataset ids for different metadata
   */

  hid_t file_id, fapl_id, fcpl_id, lcpl_id;

  initialize_plist(full_path, &fapl_id, &fcpl_id, &lcpl_id);
  initialize_file(full_path, &file_id, fapl_id, fcpl_id);

  hid_t frame_dset_id;
  hsize_t dim[DIM]       = {0, *det_y, *det_x};
  hsize_t max_dim[DIM]   = {H5S_UNLIMITED, *det_y, *det_x};
  hsize_t frame_dim[DIM] = {1, *det_y, *det_x};

  hid_t memspace = H5Screate_simple(DIM, dim, max_dim);
  if (memspace == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating memspace in main\n");
    return 1;
  }

  hid_t dcpl_id =
    dcpl_compress(DIM, frame_dim, compression_level, shuffle, compressor);
  create_merlin_dataset(&frame_dset_id, file_id, merlin_dset_name, bufsize,
                        memspace, dcpl_id, lcpl_id, frame_dim);

  hid_t meta_handle[MQ1_FIELDS_NUM_FIELDS];
  hid_t dac_handle[DAC_NUM_FIELDS * 4];

  create_meta_mq1_fields_dataset(file_id, lcpl_id, meta_handle);
  create_dac_dataset(*num_chips, file_id, lcpl_id, dac_handle);
  //  === end initialize hdf5 ===

  end                         = clock();
  double time_initialize_hdf5 = (double) (end - begin) / CLOCKS_PER_SEC;
  printf("+++ Time to init hdf5: %02f +++\n", time_initialize_hdf5);

  // === start appending ===
  begin = clock();
  /* Check if it is EOF each time
   * Please check the functions in read.c and append.c
   */

  int c      = 0;
  int loop   = 0;
  int cbytes = 0;
  while ((c = fgetc(mib_ptr)) != EOF) {
    ungetc(c, mib_ptr);
    ptr_pos = ftell(mib_ptr);

    allocate_frame_header(frame_ptr);

    read_header(mib_ptr, ptr_pos, frame_ptr);

    allocate_frame_data(frame_ptr);

    read_frame(mib_ptr, ptr_pos, frame_ptr);

    cbytes = compress_frame(frame_ptr, compression_level, shuffle, compressor,
                            BLOCKSIZE, NUMINTERNALTHREADS);

    append_meta_to_dataset(meta_handle, frame_ptr);

    append_dac_to_dataset(*num_chips, dac_handle, frame_ptr);

    append_frame_to_dataset(frame_dset_id, frame_ptr, cbytes);

    deallocate_frame(frame_ptr);

    printf("pointer position after loop %d: %ld,  cbytes: %d\r", loop,
           ftell(mib_ptr), cbytes);
    fflush(stdout);
    loop++;
  }
  printf("\n");

  // === end appending ===
  end                    = clock();
  double time_total_loop = (double) (end - begin) / CLOCKS_PER_SEC;
  double time_per_loop   = time_total_loop / loop;
  printf("\n+++ Time per loop: %02f +++\n", time_per_loop);

  close_dataset_handle(meta_handle, MQ1_FIELDS_NUM_FIELDS);
  close_dataset_handle(dac_handle, DAC_NUM_FIELDS * 4);

  if (frame_dset_id >= 0)
    H5Dclose(frame_dset_id);

  H5Pclose(fapl_id);
  H5Pclose(fcpl_id);
  H5Pclose(lcpl_id);
  H5Fflush(file_id, H5F_SCOPE_GLOBAL);
  H5Fclose(file_id);
  fclose(mib_ptr);

  clock_t end_total = clock();
  double time_total = (double) (end_total - begin_total) / CLOCKS_PER_SEC;
  printf("+++ total time usage: %02f +++\n", time_total);
  return 0;
}
