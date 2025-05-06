// clang-format Language: C
#include "mib_header.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef IO_HEADER_H
#define IO_HEADER_H

typedef struct {
  char *header_id;
  unsigned int max_length;
  unsigned int *sequence_number;
  unsigned int *header_bytes;
  unsigned int *num_chips;
  unsigned int *det_x;
  unsigned int *det_y;
  char *pixel_depth;
  char *sensor_layout;
  char *chip_select;
  char *timestamp;
  double *exposure_time_s;
  unsigned int *counter;
  unsigned int *colour_mode;
  unsigned int *gain_mode;
  float *threshold;
  char *header_extension_id;
  char *extended_timestamp;
  unsigned int *exposure_time_ns;
  unsigned int *bit_depth;
} MQ1_fields;

MQ1_fields allocate_MQ1_fields(unsigned int nheaders);

void deallocate_MQ1_fields(MQ1_fields mq1_fields);

unsigned int mq1_single_from_file(FILE *mib_ptr,
                                  unsigned int nheaders,
                                  unsigned int detector_frame_bytes,
                                  mq1s *mq1s_h,
                                  MQ1_fields *mq1_fields);

unsigned int mq1_quad_from_file(FILE *mib_ptr,
                                unsigned int nheaders,
                                unsigned int detector_frame_bytes,
                                mq1q *mq1q_h,
                                MQ1_fields *mq1_fields);

void fill_MQ1_single_fields(MQ1_fields *mq1_field,
                            unsigned int index,
                            mq1s mq1_h);

void fill_MQ1_quad_fields(MQ1_fields *mq1_field,
                          unsigned int index,
                          mq1q mq1_h);

typedef struct {
  const char *name;
  void *data;
} info;

info *mq1_fields_info(MQ1_fields *fields);
info *dac_info(dac_rx *dac);

#endif
