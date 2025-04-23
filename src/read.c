#include "read.h"
#include "framebuffer.h"
#include "io_header.h"
#include "macros.h"
#include "parser.h"
#include "utils.h"

#include <blosc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_header(FILE *mib_ptr, long offset, framebuffer *fb)
{
  fseek(mib_ptr, offset, SEEK_SET);
  char buf[16]           = {0};
  char headersize_str[6] = {0};
  fread(buf, sizeof(char), 16, mib_ptr);
  memcpy(headersize_str, buf + 11, 5);
  headersize_str[5]      = '\0';
  int headersize         = atoi(headersize_str);
  char *header           = malloc(sizeof(char) * headersize);
  MQ1_fields *mq1_header = malloc(sizeof(MQ1_fields));
  if (!mq1_header) {
    fprintf(stderr, "malloc fail for mq1_header in read_header\n");
    free(header);
    header = NULL;
    return;
  }
  *mq1_header = allocate_MQ1_fields(1);
  fb->dac0    = (dac_rx *) malloc(sizeof(dac_rx));
  fb->dac1    = (dac_rx *) malloc(sizeof(dac_rx));
  fb->dac2    = (dac_rx *) malloc(sizeof(dac_rx));
  fb->dac3    = (dac_rx *) malloc(sizeof(dac_rx));

  fseek(mib_ptr, offset, SEEK_SET);
  fread(header, sizeof(char), headersize, mib_ptr);

  switch (headersize) {
    case 384: {
      mq1s mq1_single;
      parse_mq1_single(header, &mq1_single);
      memcpy(fb->dac0, &mq1_single.dac0, sizeof(dac_rx));
      fb->dac1 = NULL;
      fb->dac2 = NULL;
      fb->dac3 = NULL;
      fill_MQ1_single_fields(mq1_header, 0, mq1_single);
      fb->mq1_header = mq1_header;
      break;
    }
    case 768: {
      mq1q mq1_quad;
      parse_mq1_quad(header, &mq1_quad);
      memcpy(fb->dac0, &mq1_quad.dac0, sizeof(dac_rx));
      memcpy(fb->dac1, &mq1_quad.dac1, sizeof(dac_rx));
      memcpy(fb->dac2, &mq1_quad.dac2, sizeof(dac_rx));
      memcpy(fb->dac3, &mq1_quad.dac3, sizeof(dac_rx));
      fill_MQ1_quad_fields(mq1_header, 0, mq1_quad);
      fb->mq1_header = mq1_header;
      break;
    }
    default: {
      deallocate_MQ1_fields(*mq1_header);
      free(header);
      header = NULL;
      free(mq1_header);
      mq1_header = NULL;
      fprintf(stderr, "headersize not 384 or 768\n");
      return;
    }
  }
  free(header);
  header = NULL;
}

void read_frame(FILE *mib_ptr, long offset, framebuffer *fb)
{
  if (!mib_ptr || !fb) {
    fprintf(stderr, "Missing input in read_frame\n");
    if (!mib_ptr)
      fprintf(stderr, "NO MIB_PTR\n");
    if (!fb)
      fprintf(stderr, "NO fb\n");
    return;
  }

  int headersize = *(fb->mq1_header->header_bytes);

  int bufsize = (fb->mq1_header->pixel_depth[1] - '0') * 10 +
                (fb->mq1_header->pixel_depth[2] - '0');
  bufsize = bufsize / 8;

  int detx = (int) *(fb->mq1_header->det_x);
  int dety = (int) *(fb->mq1_header->det_y);

  // move mib_ptr to the correct place
  fseek(mib_ptr, offset + headersize, SEEK_SET);

  // write data into buffer
  uint8_t *raw_data = malloc(bufsize * detx * dety);
  if (!raw_data) {
    fprintf(stderr, "malloc failed for raw_data in read_frame\n");
    return;
  }

  // TO-DO: add checks for corruption
  fread(raw_data, sizeof(char), bufsize * detx * dety, mib_ptr);

  for (int i = 0; i < dety; i++) {
    for (int j = 0; j < detx; j++) {
      size_t index       = (i * detx + j) * bufsize;
      uint8_t *raw_bytes = &raw_data[index];
      uint64_t value     = 0;

      switch (bufsize) {
        case 1: {
          value                         = raw_bytes[0];
          ((uint8_t **) fb->rows)[i][j] = (uint8_t) value;
          break;
        }
        case 2: {
          value                          = (raw_bytes[0] << 8) | raw_bytes[1];
          ((uint16_t **) fb->rows)[i][j] = (uint16_t) value;
          break;
        }
        case 4: {
          value = (raw_bytes[0] << 24) | (raw_bytes[1] << 16) |
                  (raw_bytes[2] << 8) | raw_bytes[3];
          ((uint32_t **) fb->rows)[i][j] = (uint32_t) value;
          break;
        }
        case 8: {
          value =
            ((uint64_t) raw_bytes[0] << 56) | ((uint64_t) raw_bytes[1] << 48) |
            ((uint64_t) raw_bytes[2] << 40) | ((uint64_t) raw_bytes[3] << 32) |
            ((uint64_t) raw_bytes[4] << 24) | ((uint64_t) raw_bytes[5] << 16) |
            ((uint64_t) raw_bytes[6] << 8) | ((uint64_t) raw_bytes[7]);
          ((uint64_t **) fb->rows)[i][j] = value;
          break;
        }
      }
    }
  }
  free(raw_data);
  raw_data = NULL;
}
