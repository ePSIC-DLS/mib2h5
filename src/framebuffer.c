/* These are the memory allocation functions of he struct framebuffer declared
 * in framebuffer.h
 *
 * allocate_frame_header only allocates mq1_header and dacs in framebuffer
 * allocate_frame_data only allocates rows and data in framebuffer
 *
 * Please check if both of them have been used before any usage on the struct
 *
 * Please also call deallocate_frame for freeing the memory. deallocate_frame
 * frees both header and data
 */

#include "framebuffer.h"
#include "macros.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void allocate_frame_header(framebuffer *fb)
{
  fb->mq1_header = malloc(sizeof(MQ1_fields));
  if (!mq1_header) {
    fprintf(stderr,
            "Error in malloc for mq1_header in allocate_frame_header\n");
    return;
  }
  fb->mq1_header = allocate_MQ1_fields(1);
  fb->dac0       = (dac_rx *) malloc(sizeof(dac_rx));
  if (!fb->dac0) {
    fprintf(stderr, "Error in malloc for dac0 in allocate_frame_header\n");
    return;
  }
  fb->mq1_header = allocate_MQ1_fields(1);
  fb->dac1       = (dac_rx *) malloc(sizeof(dac_rx));
  if (!fb->dac1) {
    fprintf(stderr, "Error in malloc for dac1 in allocate_frame_header\n");
    return;
  }
  fb->dac2 = (dac_rx *) malloc(sizeof(dac_rx));
  if (!fb->dac2) {
    fprintf(stderr, "Error in malloc for dac2 in allocate_frame_header\n");
    return;
  }
  fb->dac3 = (dac_rx *) malloc(sizeof(dac_rx));
  if (!fb->dac3) {
    fprintf(stderr, "Error in malloc for dac3 in allocate_frame_header\n");
    return;
  }
}

void allocate_frame_data(framebuffer *fb)
{
  int bufsize = (fb->mq1_header->pixel_depth[1] - '0') * 10 +
                (fb->mq1_header->pixel_depth[2] - '0');
  bufsize = bufsize / 8;

  int detx = (int) *(fb->mq1_header->det_x);
  int dety = (int) *(fb->mq1_header->det_y);

  void **buffer = malloc(sizeof(void *) * dety);
  if (!buffer) {
    fprintf(stderr, "Error in malloc for fb->rows in allocate_frame_data\n");
    return;
  }
  void *data - NULL;
  fb->rows = buffer;
  switch (bufsize) {
    case 1: {
      data = malloc(sizeof(uint8_t) * detx * dety);
      if (!data) {
        fprintf(stderr, "malloc failed for data in read_frame");
        return;
      }
      fb->data = data;
      for (int i = 0; i < (int) dety; i++) {
        buffer[i] = (uint8_t *) data + i * detx;
      }
      break;
    }
    case 2: {
      data = malloc(sizeof(uint16_t) * detx * dety);
      if (!data) {
        fprintf(stderr, "malloc failed for data in read_frame");
        return;
      }
      fb->data = data;
      for (int i = 0; i < (int) dety; i++) {
        buffer[i] = (uint16_t *) data + i * detx;
      }
      break;
    }
    case 4: {
      data = malloc(sizeof(uint32_t) * detx * dety);
      if (!data) {
        fprintf(stderr, "malloc failed for data in read_frame");
        return;
      }
      fb->data = data;
      for (int i = 0; i < (int) dety; i++) {
        buffer[i] = (uint32_t *) data + i * detx;
      }
      break;
    }
    case 8: {
      data = malloc(sizeof(uint64_t) * detx * dety);
      if (!data) {
        fprintf(stderr, "malloc failed for data in read_frame");
        return;
      }
      fb->data = data
      for (int i = 0; i < (int) dety; i++) {
        buffer[i] = (uint64_t *) data + i * detx;
      }
      break;
    }
    default:
      printf("Unsupported pixel depth, single bit will be implemented later\n");
      if (data)
        free(data);
      free(buffer);
      return;
  }
}

void deallocate_frame(framebuffer *fb)
{
  if (fb == NULL) {
    fprintf(stderr, "NULL framebuffer\n");
    return;
  }
  if (fb->rows)
    free(fb->rows);
  if (fb->data)
    free(fb->data);
  if (fb->dac0)
    free(fb->dac0);
  if (fb->dac1)
    free(fb->dac1);
  if (fb->dac2)
    free(fb->dac2);
  if (fb->dac3)
    free(fb->dac3);
  if (fb->mq1_header) {
    deallocate_MQ1_fields(fb->mq1_header);
    free(fb->mq1_header);
    fb->mq1_header = NULL;
  }
  fb->rows = NULL;
  fb->data = NULL;
  fb->dac0 = NULL;
  fb->dac1 = NULL;
  fb->dac2 = NULL;
  fb->dac3 = NULL;
}
