#include "read.h"
#include "framebuffer.h"
#include "io_header.h"
#include "macros.h"
#include "parser.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_LOC_IN_BUF 16
#define HEADERSIZE 6

void read_header(FILE *mib_ptr, long offset, framebuffer *fb)
{
  if (mib_ptr == NULL || fb == NULL) {
    fprintf(stderr, "Missing input mib_ptr or fb in read_header\n");
    goto cleanup;
  }
  if (offset < 0) {
    fprintf(stderr, "offset is negative, please check input\n");
    goto cleanup;
  }
  if (fseek(mib_ptr, offset, SEEK_SET) != 0) {
    fprintf(stderr, "fseek error in read_header\n");
    goto cleanup;
  }

  char buf[HEADER_LOC_IN_BUF]     = {0};
  char headersize_str[HEADERSIZE] = {0};
  size_t status = fread(buf, sizeof(char), HEADER_LOC_IN_BUF, mib_ptr);
  if (status != HEADER_LOC_IN_BUF) {
    fprintf(stderr, "fread error in read_header\n");
    goto cleanup;
  }
  memcpy(headersize_str, buf + HEADER_LOC_IN_BUF - HEADERSIZE + 1,
         HEADERSIZE - 1);
  headersize_str[HEADERSIZE - 1] = '\0';
  char *end_ptr;
  long unsigned int headersize = strtol(headersize_str, &end_ptr, 10);
  if (end_ptr == headersize_str) {
    fprintf(stderr, "headersize strtol error in read_header, no digit found\n");
    goto cleanup;
  } else if (*end_ptr != '\0') {
    fprintf(stderr,
            "headersize strtol error in read_header, invalid character: %c\n",
            *end_ptr);
    goto cleanup;
  }
  char *header = malloc(sizeof(char) * headersize);
  if (!header) {
    fprintf(stderr, "malloc fail for header in read_header\n");
    goto cleanup;
  }
  MQ1_fields *mq1_header = malloc(sizeof(MQ1_fields));
  if (!mq1_header) {
    fprintf(stderr, "malloc fail for mq1_header in read_header\n");
    goto cleanup;
  }
  *mq1_header = allocate_MQ1_fields(1);

  if (fseek(mib_ptr, offset, SEEK_SET) != 0) {
    fprintf(stderr, "fseek error in read_header\n");
    goto cleanup;
  }

  status = fread(header, sizeof(char), headersize, mib_ptr);
  if (status != headersize) {
    fprintf(stderr, "fread error in read_header\n");
    goto cleanup;
  }

  switch (headersize) {
    case MQ1_SINGLE_HEADER_BYTES: {
      mq1s mq1_single;
      parse_mq1_single(header, &mq1_single);
      if (fb->dac0 == NULL) {
        fprintf(stderr, "NULL dac pointer in read_header\n");
        goto cleanup;
      }
      memcpy(fb->dac0, &mq1_single.dac0, sizeof(dac_rx));
      fb->dac1 = NULL;
      fb->dac2 = NULL;
      fb->dac3 = NULL;
      fill_MQ1_single_fields(mq1_header, 0, mq1_single);
      fb->mq1_header = mq1_header;
      break;
    }
    case MQ1_QUAD_HEADER_BYTES: {
      mq1q mq1_quad;
      parse_mq1_quad(header, &mq1_quad);
      if (fb->dac0 == NULL || fb->dac1 == NULL || fb->dac2 == NULL ||
          fb->dac3 == NULL) {
        fprintf(stderr, "NULL dac pointer in read_header\n");
        goto cleanup;
      }
      memcpy(fb->dac0, &mq1_quad.dac0, sizeof(dac_rx));
      memcpy(fb->dac1, &mq1_quad.dac1, sizeof(dac_rx));
      memcpy(fb->dac2, &mq1_quad.dac2, sizeof(dac_rx));
      memcpy(fb->dac3, &mq1_quad.dac3, sizeof(dac_rx));
      fill_MQ1_quad_fields(mq1_header, 0, mq1_quad);
      fb->mq1_header = mq1_header;
      break;
    }
    default: {
      fprintf(stderr, "headersize not 384 or 768\n");
      goto cleanup;
    }
  }

cleanup:
  if (header) {
    free(header);
    header = NULL;
  }
  if (mq1_header) {
    free(mq1_header);
    mq1_header = NULL;
    deallocate_MQ1_fields(*mq1_header);
  }
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

  if (fb->mq1_header == NULL) {
    fprintf(stderr, "NULL pointer fb->mq1_header in read_frame\n");
    return;
  } else {
    if (fb->mq1_header->header_bytes == NULL || fb->mq1_header->det_x == NULL ||
        fb->mq1_header->det_y == NULL) {
      fprintf(stderr, "NULL pointer inside fb->mq1_header in read_frame\n");
      return;
    }
  }

  int headersize = *(fb->mq1_header->header_bytes);

  int bufsize = (fb->mq1_header->pixel_depth[1] - '0') * 10 +
                (fb->mq1_header->pixel_depth[2] - '0');
  bufsize = bufsize / 8;
  if (bufsize != 1 && bufsize != 2 && bufsize != 4 && bufsize != 8) {
    fprintf(stderr, "not supported bufsize in read_frame\n");
    return;
  }

  int detx = (int) *(fb->mq1_header->det_x);
  int dety = (int) *(fb->mq1_header->det_y);

  if (fseek(mib_ptr, offset + headersize, SEEK_SET) != 0) {
    fprintf(stderr, "fseek error in read_frame\n");
    return;
  }

  uint8_t *raw_data = malloc(bufsize * detx * dety);
  if (!raw_data) {
    fprintf(stderr, "malloc failed for raw_data in read_frame\n");
    return;
  }

  size_t status = fread(raw_data, sizeof(char), bufsize * detx * dety, mib_ptr);
  if (status != bufsize * detx * dety) {
    fprintf(stderr, "fread error in read_frame\n");
    return;
  }

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
        default: {
          fprintf(stderr, "not supported bufsize\n");
          return;
        }
      }
    }
  }
  free(raw_data);
  raw_data = NULL;
}
