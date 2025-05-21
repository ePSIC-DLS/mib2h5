#include "io_header.h"
#include "macros.h"
#include "mib_header.h"
#include "parser.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int mq1_single_from_file(FILE *mib_ptr,
                                  unsigned int nheaders,
                                  unsigned int detector_frame_bytes,
                                  mq1s *mq1s_h,
                                  MQ1_fields *mq1_fields)
{
  unsigned int num_parsed                         = 0;
  char header_buffer[MQ1_SINGLE_HEADER_BYTES + 1] = {0};

  for (unsigned int i = 0; i < nheaders; ++i) {
    /*at the current file position, read the size of a header and save in a
     * buffer*/
    if (fgets(header_buffer, sizeof(header_buffer), mib_ptr) == NULL) {
      const char *current_file = only_file_name(__FILE__);
      fprintf(stderr,
              "%s:%d: error: failed to read the header (%u) of the "
              "MIB file into a buffer\n",
              current_file, __LINE__, i);
      return 0;
    }

    /*parse the buffer and assign to the fields of a header struct*/
    parse_mq1_single(header_buffer, &mq1s_h[i]);

    /*upon completion, increased the count of parsed header*/
    num_parsed += 1;

    /*update the corresponding field array*/
    fill_MQ1_single_fields(mq1_fields, i, mq1s_h[i]);

    /*move the file by the size of the detector*/
    /*i.e. point to the next header*/
    fseek(mib_ptr, (long int) detector_frame_bytes, SEEK_CUR);
  }

  /*go to the beginning after parsing*/
  if (fseek(mib_ptr, 0, SEEK_SET) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr,
            "%s:%d: error: failed to seek to the beginning of the "
            "MIB file\n",
            current_file, __LINE__);
    num_parsed = 0;
  }

  return num_parsed;
}

unsigned int mq1_quad_from_file(FILE *mib_ptr,
                                unsigned int nheaders,
                                unsigned int detector_frame_bytes,
                                mq1q *mq1q_h,
                                MQ1_fields *mq1_fields)
{
  unsigned int num_parsed                       = 0;
  char header_buffer[MQ1_QUAD_HEADER_BYTES + 1] = {0};

  for (unsigned int i = 0; i < nheaders; ++i) {
    /*at the current file position, read the size of a header and save in a
     * buffer*/
    if (fgets(header_buffer, sizeof(header_buffer), mib_ptr) == NULL) {
      const char *current_file = only_file_name(__FILE__);
      fprintf(stderr,
              "%s:%d: error: failed to read the header (%u) of the "
              "MIB file into a buffer\n",
              current_file, __LINE__, i);
      return 0;
    }

    /*parse the buffer and assign to the fields of a header struct*/
    parse_mq1_quad(header_buffer, &mq1q_h[i]);

    /*upon completion, increased the count of parsed header*/
    num_parsed += 1;

    /*update the corresponding field array*/
    fill_MQ1_quad_fields(mq1_fields, i, mq1q_h[i]);

    /*move the file by the size of the detector*/
    /*i.e. point to the next header*/
    fseek(mib_ptr, (long int) detector_frame_bytes, SEEK_CUR);
  }

  /*go to the beginning after parsing*/
  if (fseek(mib_ptr, 0, SEEK_SET) != 0) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr,
            "%s:%d: error: failed to seek to the beginning of the "
            "MIB file\n",
            current_file, __LINE__);
    num_parsed = 0;
  }

  return num_parsed;
}

/* ensure consistent memory allocation for all fields
 * use deallocate_MQ1_fields to free all the allocated memory here
 * */
MQ1_fields allocate_MQ1_fields(unsigned int nheaders)
{
  MQ1_fields mq1_fields;

  mq1_fields.max_length = nheaders;

  mq1_fields.header_id = XMALLOC(
    sizeof(char) * MQ1_CHAR_LEN_HEADER_ID * mq1_fields.max_length, "header_id");
  mq1_fields.sequence_number =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "sequence_number");
  mq1_fields.header_bytes =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "header_bytes");
  mq1_fields.num_chips =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "num_chips");
  mq1_fields.det_x =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "det_x");
  mq1_fields.det_y =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "det_y");
  mq1_fields.pixel_depth =
    XMALLOC(sizeof(char) * MQ1_CHAR_LEN_PIXEL_DEPTH * mq1_fields.max_length,
            "pixel_depth");
  mq1_fields.sensor_layout =
    XMALLOC(sizeof(char) * MQ1_CHAR_LEN_SENSOR_LAYOUT * mq1_fields.max_length,
            "sensor_layout");
  mq1_fields.chip_select =
    XMALLOC(sizeof(char) * MQ1_CHAR_LEN_CHIP_SELECT * mq1_fields.max_length,
            "chip_select");
  mq1_fields.timestamp = XMALLOC(
    sizeof(char) * MQ1_CHAR_LEN_TIMESTAMP * mq1_fields.max_length, "timestamp");
  mq1_fields.exposure_time_s =
    XMALLOC(sizeof(double) * mq1_fields.max_length, "exposure_time_s");
  mq1_fields.counter =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "counter");
  mq1_fields.colour_mode =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "colour_mode");
  mq1_fields.gain_mode =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "gain_mode");
  mq1_fields.threshold0 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold0");
  mq1_fields.threshold1 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold1");
  mq1_fields.threshold2 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold2");
  mq1_fields.threshold3 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold3");
  mq1_fields.threshold4 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold4");
  mq1_fields.threshold5 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold5");
  mq1_fields.threshold6 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold6");
  mq1_fields.threshold7 =
    XMALLOC(sizeof(float) * mq1_fields.max_length, "threshold7");
  mq1_fields.header_extension_id = XMALLOC(
    sizeof(char) * MQ1_CHAR_LEN_HEADER_EXTENSION_ID * mq1_fields.max_length,
    "header_extension_id");
  mq1_fields.extended_timestamp = XMALLOC(
    sizeof(char) * MQ1_CHAR_LEN_EXTENDED_TIMESTAMP * mq1_fields.max_length,
    "extended_timestamp");
  mq1_fields.exposure_time_ns =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "exposure_time_ns");
  mq1_fields.bit_depth =
    XMALLOC(sizeof(unsigned int) * mq1_fields.max_length, "bit_depth");

  return mq1_fields;
}

/* ensure all allocated memory is free */
void deallocate_MQ1_fields(MQ1_fields mq1_fields)
{
  free(mq1_fields.header_id);
  mq1_fields.header_id = NULL;
  free(mq1_fields.sequence_number);
  mq1_fields.sequence_number = NULL;
  free(mq1_fields.header_bytes);
  mq1_fields.header_bytes = NULL;
  free(mq1_fields.num_chips);
  mq1_fields.num_chips = NULL;
  free(mq1_fields.det_x);
  mq1_fields.det_x = NULL;
  free(mq1_fields.det_y);
  mq1_fields.det_y = NULL;
  free(mq1_fields.pixel_depth);
  mq1_fields.pixel_depth = NULL;
  free(mq1_fields.sensor_layout);
  mq1_fields.sensor_layout = NULL;
  free(mq1_fields.chip_select);
  mq1_fields.chip_select = NULL;
  free(mq1_fields.timestamp);
  mq1_fields.timestamp = NULL;
  free(mq1_fields.exposure_time_s);
  mq1_fields.exposure_time_s = NULL;
  free(mq1_fields.counter);
  mq1_fields.counter = NULL;
  free(mq1_fields.colour_mode);
  mq1_fields.colour_mode = NULL;
  free(mq1_fields.gain_mode);
  mq1_fields.gain_mode = NULL;
  free(mq1_fields.threshold0);
  mq1_fields.threshold0 = NULL;
  free(mq1_fields.threshold1);
  mq1_fields.threshold1 = NULL;
  free(mq1_fields.threshold2);
  mq1_fields.threshold2 = NULL;
  free(mq1_fields.threshold3);
  mq1_fields.threshold3 = NULL;
  free(mq1_fields.threshold4);
  mq1_fields.threshold4 = NULL;
  free(mq1_fields.threshold5);
  mq1_fields.threshold5 = NULL;
  free(mq1_fields.threshold6);
  mq1_fields.threshold6 = NULL;
  free(mq1_fields.threshold7);
  mq1_fields.threshold7 = NULL;
  free(mq1_fields.header_extension_id);
  mq1_fields.header_extension_id = NULL;
  free(mq1_fields.extended_timestamp);
  mq1_fields.extended_timestamp = NULL;
  free(mq1_fields.exposure_time_ns);
  mq1_fields.exposure_time_ns = NULL;
  free(mq1_fields.bit_depth);
  mq1_fields.bit_depth = NULL;
}

/*for MQ1 single, basically a copy of MQ1 quad*/
void fill_MQ1_single_fields(MQ1_fields *mq1_field,
                            unsigned int index,
                            mq1s mq1_h)
{
  if (index >= mq1_field->max_length) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr,
            "%s:%d: error: the index of the field is larger than "
            "the maximum number of the fields\n",
            current_file, __LINE__);
    exit(1);
  }

  mq1_field->sequence_number[index] = mq1_h.sequence_number;
  mq1_field->header_bytes[index]    = mq1_h.header_bytes;
  mq1_field->num_chips[index]       = mq1_h.num_chips;
  mq1_field->det_x[index]           = mq1_h.det_x;
  mq1_field->det_y[index]           = mq1_h.det_y;
  /*header_id is char[4]*/
  snprintf(mq1_field->header_id + index * MQ1_CHAR_LEN_HEADER_ID,
           MQ1_CHAR_LEN_HEADER_ID, "%s", mq1_h.header_id);
  /*pixel_depth is char[4]*/
  snprintf(mq1_field->pixel_depth + index * MQ1_CHAR_LEN_PIXEL_DEPTH,
           MQ1_CHAR_LEN_PIXEL_DEPTH, "%s", mq1_h.pixel_depth);
  /*sensor_layout is char[7]*/
  snprintf(mq1_field->sensor_layout + index * MQ1_CHAR_LEN_SENSOR_LAYOUT,
           MQ1_CHAR_LEN_SENSOR_LAYOUT, "%s", mq1_h.sensor_layout);
  /*chip_select is char[3]*/
  snprintf(mq1_field->chip_select + index * MQ1_CHAR_LEN_CHIP_SELECT,
           MQ1_CHAR_LEN_CHIP_SELECT, "%s", mq1_h.chip_select);
  /*timestamp is char[27]*/
  snprintf(mq1_field->timestamp + index * MQ1_CHAR_LEN_TIMESTAMP,
           MQ1_CHAR_LEN_TIMESTAMP, "%s", mq1_h.timestamp);
  mq1_field->exposure_time_s[index] = mq1_h.exposure_time_s;
  mq1_field->counter[index]         = mq1_h.counter;
  mq1_field->colour_mode[index]     = mq1_h.colour_mode;
  mq1_field->gain_mode[index]       = mq1_h.gain_mode;
  /*threshold is float[8]*/
  unsigned int i               = -1;
  mq1_field->threshold0[index] = mq1_h.threshold[++i];
  mq1_field->threshold1[index] = mq1_h.threshold[++i];
  mq1_field->threshold2[index] = mq1_h.threshold[++i];
  mq1_field->threshold3[index] = mq1_h.threshold[++i];
  mq1_field->threshold4[index] = mq1_h.threshold[++i];
  mq1_field->threshold5[index] = mq1_h.threshold[++i];
  mq1_field->threshold6[index] = mq1_h.threshold[++i];
  mq1_field->threshold7[index] = mq1_h.threshold[++i];
  /*header_extension_id is char[5]*/
  snprintf(mq1_field->header_extension_id +
             index * MQ1_CHAR_LEN_HEADER_EXTENSION_ID,
           MQ1_CHAR_LEN_HEADER_EXTENSION_ID, "%s", mq1_h.header_extension_id);
  /*extended_timestamp is char[31]*/
  snprintf(mq1_field->extended_timestamp +
             index * MQ1_CHAR_LEN_EXTENDED_TIMESTAMP,
           MQ1_CHAR_LEN_EXTENDED_TIMESTAMP, "%s", mq1_h.extended_timestamp);
  mq1_field->exposure_time_ns[index] = mq1_h.exposure_time_ns;
  mq1_field->bit_depth[index]        = mq1_h.bit_depth;
}

/*for MQ1 quad, basically a copy of MQ1 single*/
void fill_MQ1_quad_fields(MQ1_fields *mq1_field, unsigned int index, mq1q mq1_h)
{
  if (index >= mq1_field->max_length) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr,
            "%s:%d: error: the index of the field is larger than "
            "the maximum number of the fields\n",
            current_file, __LINE__);
    exit(1);
  }

  mq1_field->sequence_number[index] = mq1_h.sequence_number;
  mq1_field->header_bytes[index]    = mq1_h.header_bytes;
  mq1_field->num_chips[index]       = mq1_h.num_chips;
  mq1_field->det_x[index]           = mq1_h.det_x;
  mq1_field->det_y[index]           = mq1_h.det_y;
  /*header_id is char[4]*/
  snprintf(mq1_field->header_id + index * MQ1_CHAR_LEN_HEADER_ID,
           MQ1_CHAR_LEN_HEADER_ID, "%s", mq1_h.header_id);
  /*pixel_depth is char[4]*/
  snprintf(mq1_field->pixel_depth + index * MQ1_CHAR_LEN_PIXEL_DEPTH,
           MQ1_CHAR_LEN_PIXEL_DEPTH, "%s", mq1_h.pixel_depth);
  /*sensor_layout is char[7]*/
  snprintf(mq1_field->sensor_layout + index * MQ1_CHAR_LEN_SENSOR_LAYOUT,
           MQ1_CHAR_LEN_SENSOR_LAYOUT, "%s", mq1_h.sensor_layout);
  /*chip_select is char[3]*/
  snprintf(mq1_field->chip_select + index * MQ1_CHAR_LEN_CHIP_SELECT,
           MQ1_CHAR_LEN_CHIP_SELECT, "%s", mq1_h.chip_select);
  /*timestamp is char[27]*/
  snprintf(mq1_field->timestamp + index * MQ1_CHAR_LEN_TIMESTAMP,
           MQ1_CHAR_LEN_TIMESTAMP, "%s", mq1_h.timestamp);
  mq1_field->exposure_time_s[index] = mq1_h.exposure_time_s;
  mq1_field->counter[index]         = mq1_h.counter;
  mq1_field->colour_mode[index]     = mq1_h.colour_mode;
  mq1_field->gain_mode[index]       = mq1_h.gain_mode;
  /*threshold is float[8]*/
  unsigned int i               = -1;
  mq1_field->threshold0[index] = mq1_h.threshold[++i];
  mq1_field->threshold1[index] = mq1_h.threshold[++i];
  mq1_field->threshold2[index] = mq1_h.threshold[++i];
  mq1_field->threshold3[index] = mq1_h.threshold[++i];
  mq1_field->threshold4[index] = mq1_h.threshold[++i];
  mq1_field->threshold5[index] = mq1_h.threshold[++i];
  mq1_field->threshold6[index] = mq1_h.threshold[++i];
  mq1_field->threshold7[index] = mq1_h.threshold[++i];
  /*header_extension_id is char[5]*/
  snprintf(mq1_field->header_extension_id +
             index * MQ1_CHAR_LEN_HEADER_EXTENSION_ID,
           MQ1_CHAR_LEN_HEADER_EXTENSION_ID, "%s", mq1_h.header_extension_id);
  /*extended_timestamp is char[31]*/
  snprintf(mq1_field->extended_timestamp +
             index * MQ1_CHAR_LEN_EXTENDED_TIMESTAMP,
           MQ1_CHAR_LEN_EXTENDED_TIMESTAMP, "%s", mq1_h.extended_timestamp);
  mq1_field->exposure_time_ns[index] = mq1_h.exposure_time_ns;
  mq1_field->bit_depth[index]        = mq1_h.bit_depth;
}

/* Mainly this is just for easy access to mq1_fields and dac
 * Please free the returning array after using it
 */

info *mq1_fields_info(MQ1_fields *fields_struct)
{
  if (!fields_struct)
    return NULL;

  info *fields = malloc(MQ1_FIELDS_NUM_FIELDS * sizeof(info));
  if (!fields)
    return NULL;

  fields[0]  = (info) {"header_id", fields_struct->header_id};
  fields[1]  = (info) {"max_length", &fields_struct->max_length};
  fields[2]  = (info) {"sequence_number", fields_struct->sequence_number};
  fields[3]  = (info) {"header_bytes", fields_struct->header_bytes};
  fields[4]  = (info) {"num_chips", fields_struct->num_chips};
  fields[5]  = (info) {"det_x", fields_struct->det_x};
  fields[6]  = (info) {"det_y", fields_struct->det_y};
  fields[7]  = (info) {"pixel_depth", fields_struct->pixel_depth};
  fields[8]  = (info) {"sensor_layout", fields_struct->sensor_layout};
  fields[9]  = (info) {"chip_select", fields_struct->chip_select};
  fields[10] = (info) {"timestamp", fields_struct->timestamp};
  fields[11] = (info) {"exposure_time_s", fields_struct->exposure_time_s};
  fields[12] = (info) {"counter", fields_struct->counter};
  fields[13] = (info) {"colour_mode", fields_struct->colour_mode};
  fields[14] = (info) {"gain_mode", fields_struct->gain_mode};
  fields[15] = (info) {"threshold0", fields_struct->threshold0};
  fields[16] = (info) {"threshold1", fields_struct->threshold1};
  fields[17] = (info) {"threshold2", fields_struct->threshold2};
  fields[18] = (info) {"threshold3", fields_struct->threshold3};
  fields[19] = (info) {"threshold4", fields_struct->threshold4};
  fields[20] = (info) {"threshold5", fields_struct->threshold5};
  fields[21] = (info) {"threshold6", fields_struct->threshold6};
  fields[22] = (info) {"threshold7", fields_struct->threshold7};
  fields[23] =
    (info) {"header_extension_id", fields_struct->header_extension_id};
  fields[24] = (info) {"extended_timestamp", fields_struct->extended_timestamp};
  fields[25] = (info) {"exposure_time_ns", fields_struct->exposure_time_ns};
  fields[26] = (info) {"bit_depth", fields_struct->bit_depth};

  return fields;
}

info *dac_info(dac_rx *dac)
{
  if (!dac)
    return NULL;

  info *dinfo = malloc(DAC_NUM_FIELDS * sizeof(info));
  if (!dinfo)
    return NULL;

  dinfo[0]  = (info) {"dac_format", &dac->dac_format};
  dinfo[1]  = (info) {"threshold0", &dac->threshold0};
  dinfo[2]  = (info) {"threshold1", &dac->threshold1};
  dinfo[3]  = (info) {"threshold2", &dac->threshold2};
  dinfo[4]  = (info) {"threshold3", &dac->threshold3};
  dinfo[5]  = (info) {"threshold4", &dac->threshold4};
  dinfo[6]  = (info) {"threshold5", &dac->threshold5};
  dinfo[7]  = (info) {"threshold6", &dac->threshold6};
  dinfo[8]  = (info) {"threshold7", &dac->threshold7};
  dinfo[9]  = (info) {"preamp", &dac->preamp};
  dinfo[10] = (info) {"ikrum", &dac->ikrum};
  dinfo[11] = (info) {"shaper", &dac->shaper};
  dinfo[12] = (info) {"disc", &dac->disc};
  dinfo[13] = (info) {"disc_LS", &dac->disc_LS};
  dinfo[14] = (info) {"shaper_test", &dac->shaper_test};
  dinfo[15] = (info) {"dac_disc_L", &dac->dac_disc_L};
  dinfo[16] = (info) {"dac_test", &dac->dac_test};
  dinfo[17] = (info) {"dac_disc_H", &dac->dac_disc_H};
  dinfo[18] = (info) {"delay", &dac->delay};
  dinfo[19] = (info) {"TP_buff_in", &dac->TP_buff_in};
  dinfo[20] = (info) {"TP_buff_out", &dac->TP_buff_out};
  dinfo[21] = (info) {"RPZ", &dac->RPZ};
  dinfo[22] = (info) {"GND", &dac->GND};
  dinfo[23] = (info) {"TP_ref", &dac->TP_ref};
  dinfo[24] = (info) {"FBK", &dac->FBK};
  dinfo[25] = (info) {"Cas", &dac->Cas};
  dinfo[26] = (info) {"TP_ref_A", &dac->TP_ref_A};
  dinfo[27] = (info) {"TP_ref_B", &dac->TP_ref_B};

  return dinfo;
}
