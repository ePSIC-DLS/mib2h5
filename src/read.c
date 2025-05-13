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

void read_header(FILE *mib_ptr, unsigned long offset, framebuffer *fb)
{
  char *header;
  MQ1_fields *mq1_header;
  if (mib_ptr == NULL || fb == NULL) {
    fprintf(stderr, "Missing input mib_ptr or fb in read_header\n");
    return;
  }
  if (fseek(mib_ptr, offset, SEEK_SET) != 0) {
    fprintf(stderr, "fseek error in read_header\n");
    return;
  }
  char buf[MIB_HEADER_METADATA_BUF_SIZE]            = {0};
  char headersize_str[MIB_HEADER_SIZE_FIELD_LENGTH] = {0};
  size_t status =
    fread(buf, sizeof(char), MIB_HEADER_METADATA_BUF_SIZE, mib_ptr);
  if (status != MIB_HEADER_METADATA_BUF_SIZE) {
    fprintf(stderr, "fread error in read_header\n");
    return;
  }
  memcpy(headersize_str, buf + MIB_HEADER_SIZE_FIELD_OFFSET + 1,
         MIB_HEADER_SIZE_FIELD_LENGTH - 1);
  headersize_str[MIB_HEADER_SIZE_FIELD_LENGTH - 1] = '\0';
  char *end_ptr;
  long unsigned int headersize = strtoul(headersize_str, &end_ptr, 10);
  if (end_ptr == headersize_str) {
    fprintf(stderr, "headersize strtol error in read_header, no digit found\n");
    return;
  } else if (*end_ptr != '\0') {
    fprintf(stderr,
            "headersize strtol error in read_header, invalid character: %c\n",
            *end_ptr);
    return;
  }

  if (headersize != MQ1_SINGLE_HEADER_BYTES ||
      headersize != MQ1_QUAD_HEADER_BYTES) {
    fprintf(
      stderr,
      "headersize not equal to either single or quad header byte size.\n");
    return;
  }

  header = (char *) malloc(sizeof(char) * headersize);
  if (!header) {
    fprintf(stderr, "malloc fail for header in read_header\n");
    return;
  }
  mq1_header = (MQ1_fields *) malloc(sizeof(MQ1_fields));
  if (!mq1_header) {
    fprintf(stderr, "malloc fail for mq1_header in read_header\n");
    if (header) {
      free(header);
      header = NULL;
    }
    return;
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
      if (mq1_header) {
        deallocate_MQ1_fields(*mq1_header);
        free(mq1_header);
        mq1_header = NULL;
      }
      goto cleanup;
    }
  }

cleanup:
  if (header) {
    free(header);
    header = NULL;
  }
}

void read_frame(FILE *mib_ptr, unsigned long offset, framebuffer *fb)
{
  if (!mib_ptr || !fb || !fb->rows) {
    fprintf(stderr, "Missing input in read_frame\n");
    if (!mib_ptr)
      fprintf(stderr, "NO MIB_PTR\n");
    if (!fb)
      fprintf(stderr, "NO fb\n");
    if (!fb->rows)
      fprintf(stderr, "NO fb->rows\n");
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

  int status = fread(raw_data, sizeof(char), bufsize * detx * dety, mib_ptr);
  if (status != bufsize * detx * dety) {
    fprintf(stderr, "fread error in read_frame\n");
    goto cleanup;
  }

  for (int i = 0; i < dety; i++) {
    for (int j = 0; j < detx; j++) {
      size_t index       = (i * detx + j) * bufsize;
      uint8_t *raw_bytes = &raw_data[index];

      switch (bufsize) {
        case 1: {
          ((uint8_t **) fb->rows)[i][j] = raw_bytes[0];
          break;
        }
        case 2: {
          ((uint16_t **) fb->rows)[i][j] = convert_uint16_be(raw_bytes);
          break;
        }
        case 4: {
          ((uint32_t **) fb->rows)[i][j] = convert_uint32_be(raw_bytes);
          break;
        }
        case 8: {
          ((uint64_t **) fb->rows)[i][j] = convert_uint64_be(raw_bytes);
          break;
        }
        default: {
          fprintf(stderr, "not supported bufsize\n");
          goto cleanup;
        }
      }
    }
  }
cleanup:
  free(raw_data);
  raw_data = NULL;
}
