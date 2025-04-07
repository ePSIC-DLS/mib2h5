#include <mib_header_MQ1.h>
#include <stdio.h>

#ifndef MQ1_FIELDS_H
#define MQ1_FIELDS_H

typedef struct {
  unsigned int max_length;
  unsigned int* sequence_number;
  unsigned int* header_bytes;
  unsigned int* num_chips;
  unsigned int* det_x;
  unsigned int* det_y;
  char* pixel_depth;
  char* sensor_layout;
  char* chip_select;
  char* timestamp;
  double* exposure_time_s;
  unsigned int* counter;
  unsigned int* colour_mode;
  unsigned int* gain_mode;
  float* threshold;
  char* header_extension_id;
  char* extended_timestamp;
  unsigned int* exposure_time_ns;
  unsigned int* bit_depth;
} MQ1_fields;

#endif

#ifndef ALLOCATE_MQ1_FIELDS_H
#define ALLOCATE_MQ1_FIELDS_H

MQ1_fields allocate_MQ1_fields(unsigned int nheaders);

#endif

#ifndef DEALLOCATE_MQ1_FIELDS_H
#define DEALLOCATE_MQ1_FIELDS_H

void deallocate_MQ1_fields(MQ1_fields mq1_fields);

#endif

#ifndef MQ1_SINGLE_FROM_FILE_H
#define MQ1_SINGLE_FROM_FILE_H
unsigned int mq1_single_from_file(
    FILE* mib_ptr,
    unsigned int nheaders,
    unsigned int detector_frame_bytes,
    mq1s* mq1s_h,
    MQ1_fields* mq1_fields
    );
#endif

#ifndef MQ1_QUAD_FROM_FILE_H
#define MQ1_QUAD_FROM_FILE_H
unsigned int mq1_quad_from_file(
    FILE* mib_ptr,
    unsigned int nheaders,
    unsigned int detector_frame_bytes,
    mq1q* mq1q_h,
    MQ1_fields* mq1_fields
    );
#endif

#ifndef FILL_MQ1_SINGLE_FIELDS_H
#define FILL_MQ1_SINGLE_FIELDS_H
void fill_MQ1_single_fields(
    MQ1_fields* mq1_field,
    unsigned int index,
    mq1s mq1_h
    );
#endif

#ifndef FILL_MQ1_QUAD_FIELDS_H
#define FILL_MQ1_QUAD_FIELDS_H
void fill_MQ1_quad_fields(
    MQ1_fields* mq1_field,
    unsigned int index,
    mq1q mq1_h
    );
#endif
