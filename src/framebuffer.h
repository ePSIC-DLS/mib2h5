// clang-format Language: C
#include "io_header.h"
#include "mib_header.h"

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct {
  /* This struct is to hold the data inside a frame */
  MQ1_fields *mq1_header; // metadata for the frame
  dac_rx *dac0;           // dac0 for both single and quad headers
  dac_rx *dac1; // dac1 for quad(if the header is single then this will be NULL
  dac_rx *dac2; // dac2 for quad(same as dac1)
  dac_rx *dac3; // dac3 for quad(same as dac1)
  void **rows; // just a holder for easy access of actual data, i.e. can be used
               // like fb->rows[i][j]
  void *data;  // row-major array for storing converted binary data in the frame
} framebuffer;

// allocate memory for header in framebuffer
void allocate_frame_header(framebuffer *fb);

// allocate memory for data in framebuffer
void allocate_frame_data(framebuffer *fb);

// compress data inside framebuffer
int compress_frame(framebuffer *fb,
                   unsigned int compression_level,
                   unsigned int shuffle,
                   char *compressor,
                   size_t blocksize,
                   int numinternalthreads);

// this is responsible for freeing the whole struct
void deallocate_frame(framebuffer *fb);

#endif
