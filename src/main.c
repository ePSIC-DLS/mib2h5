#include "append.h"
#include "framebuffer.h"
#include "hdf5_init.h"
#include "hdf5_init_meta.h"
#include "mib_header_DAC.h"
#include "mib_header_MQ1.h"
#include "mib_macros.h"
#include "mib_props_supp.h"
#include "mib_utils.h"
#include "mq1_quad.h"
#include "mq1_single.h"
#include "read.h"
#include "read_mq1_headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Declaration of Marco here, temporary

#define MERLIN_DSET_NAME "MerlinData"
#define COMPRESSION_LEVEL 9
#define DIM 3
#define NUM_META_FIELD 19
#define NUM_DAC_FIELD 28

int main(int argc, char *argv[])
{
  // === start get and check arguments ===
  clock_t begin_total = clock();

  /* Now this can only handle one .mib file
   * TO-DO(near future): add checks to see if more files have been provided as
   * argument. TO-DO(further): handle multiple files
   */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.mib>\n", argv[0]);
    return 1;
  }

  /* Cut out the filename from $(filename).mib for the filename of hdf5
   * The output hdf5 file should have the same filename as the .mib
   * i.e. example.mib -> example.hdf5
   */
  const char *filename = argv[1];
  const char *base     = strrchr(filename, '/');
  base                 = base ? base + 1 : filename;

  char hdf5_filename[512];
  strncpy(hdf5_filename, base, sizeof(hdf5_filename) - 1);
  hdf5_filename[sizeof(hdf5_filename) - 1] = '\0';

  char *dot = strrchr(hdf5_filename, '.');
  if (dot)
    *dot = '\0';

  size_t len = strlen(hdf5_filename);
  if (len + strlen(".hdf5") < sizeof(hdf5_filename)) {
    strcat(hdf5_filename, ".hdf5");
  } else {
    fprintf(stderr, "Filename buffer too small to append .hdf5\n");
  }

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
  initialize_file_and_plist(hdf5_filename, &file_id, &fapl_id, &fcpl_id);
  initialize_lcpl(&lcpl_id);

  hid_t frame_dset_id;
  hsize_t dim[DIM]       = {0, *det_y, *det_x};
  hsize_t max_dim[DIM]   = {H5S_UNLIMITED, *det_y, *det_x};
  hsize_t frame_dim[DIM] = {1, *det_y, *det_x};

  hid_t memspace = H5Screate_simple(DIM, dim, max_dim);
  if (memspace == H5I_INVALID_HID) {
    fprintf(stderr, "Error in creating memspace in main\n");
    return 1;
  }
  // printf("=== create frame dataset ===\n");
  create_merlin_dataset(&frame_dset_id, &file_id, MERLIN_DSET_NAME, bufsize,
                        memspace, &lcpl_id, DIM, frame_dim, COMPRESSION_LEVEL);
  // printf("=== done create frame dataset ===\n");

  hid_t meta_handle[NUM_META_FIELD];
  hid_t dac_handle[NUM_DAC_FIELD * 4];

  create_meta_mq1_fields_dataset(&file_id, &lcpl_id, meta_handle);
  // printf("== done create meta_mq1===\n");
  create_dac_dataset(*num_chips, &file_id, &lcpl_id, dac_handle);

  // === end initialize hdf5 ===

  // printf("+++ init hdf5 done +++\n");
  end                         = clock();
  double time_initialize_hdf5 = (double) (end - begin) / CLOCKS_PER_SEC;
  printf("+++ Time to init hdf5: %02f +++\n", time_initialize_hdf5);

  // === start appending ===
  begin = clock();
  /* Check if it is EOF each time
   * Please check the functions in read.c and append.c
   */

  int c    = 0;
  int loop = 0;
  size_t num_meta, num_dac;
  while ((c = fgetc(mib_ptr)) != EOF) {
    ungetc(c, mib_ptr);
    ptr_pos = ftell(mib_ptr);

    // printf("loop %d: read .mib\n", loop);

    read_header(mib_ptr, ptr_pos, frame_ptr);
    read_frame(mib_ptr, ptr_pos, frame_ptr);

    // printf("\n\nChip0 GND: %u\n\n", frame_ptr->dac0->GND);
    // printf("\n\nChip0 FBK: %u\n\n", frame_ptr->dac0->FBK);

    // printf("loop %d: read .mib end\n", loop);

    // printf("loop %d: append meta\n", loop);

    append_meta_to_dataset(&file_id, &num_meta, meta_handle, frame_ptr);

    // printf("loop %d: append meta end\n", loop);

    // printf("loop %d: append dac\n", loop);

    append_dac_to_dataset(&file_id, &num_dac, *num_chips, dac_handle,
                          frame_ptr);

    // printf("loop %d: append dac end\n", loop);

    // printf("loop %d: append frame\n", loop);

    append_frame_to_dataset(&file_id, &frame_dset_id, frame_ptr);

    // printf("loop %d: append frame end\n", loop);

    deallocate_MQ1_fields(*(frame.mq1_header));
    // printf("deallocate_MQ1_fields\n");
    free(frame.mq1_header);
    // printf("free frame.mq1_header\n");
    frame.mq1_header = NULL;
    // printf("NULL frame.mq1_header\n");
    free_frame(frame_ptr);
    // printf("free_frame\n");

    // printf("pointer position after loop %d: %ld\n", loop, ftell(mib_ptr));
    loop++;
  }

  // === end appending ===
  end                    = clock();
  double time_total_loop = (double) (end - begin) / CLOCKS_PER_SEC;
  double time_per_loop   = time_total_loop / loop;
  printf("+++ Time per loop: %02f +++\n", time_per_loop);

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
