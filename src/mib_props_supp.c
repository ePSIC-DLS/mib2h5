#include "mib_props_supp.h"
#include "mib_header_DAC.h"
#include "mib_header_MQ1.h"
#include "mib_macros.h"
#include "mib_utils.h"
#include "mq1_quad.h"
#include "mq1_single.h"
#include "read_mq1_headers.h"
#include <stdio.h>
#include <stdlib.h>

/* Mainly this is just for easy access to mq1_fields and dac
 */

info *mq1_fields_iter(MQ1_fields *fields_struct, size_t *out_count)
{
  if (!fields_struct || !out_count)
    return NULL;

  const size_t NUM_FIELDS = 19;

  info *fields = malloc(NUM_FIELDS * sizeof(info));
  if (!fields)
    return NULL;

  size_t i = 0;

  fields[i++] = (info) {"max_length", &fields_struct->max_length};
  fields[i++] = (info) {"sequence_number", fields_struct->sequence_number};
  fields[i++] = (info) {"header_bytes", fields_struct->header_bytes};
  fields[i++] = (info) {"num_chips", fields_struct->num_chips};
  fields[i++] = (info) {"det_x", fields_struct->det_x};
  fields[i++] = (info) {"det_y", fields_struct->det_y};
  fields[i++] = (info) {"pixel_depth", fields_struct->pixel_depth};
  fields[i++] = (info) {"sensor_layout", fields_struct->sensor_layout};
  fields[i++] = (info) {"chip_select", fields_struct->chip_select};
  fields[i++] = (info) {"timestamp", fields_struct->timestamp};
  fields[i++] = (info) {"exposure_time_s", fields_struct->exposure_time_s};
  fields[i++] = (info) {"counter", fields_struct->counter};
  fields[i++] = (info) {"colour_mode", fields_struct->colour_mode};
  fields[i++] = (info) {"gain_mode", fields_struct->gain_mode};
  fields[i++] = (info) {"threshold", fields_struct->threshold};
  fields[i++] =
    (info) {"header_extension_id", fields_struct->header_extension_id};
  fields[i++] =
    (info) {"extended_timestamp", fields_struct->extended_timestamp};
  fields[i++] = (info) {"exposure_time_ns", fields_struct->exposure_time_ns};
  fields[i++] = (info) {"bit_depth", fields_struct->bit_depth};

  *out_count = NUM_FIELDS;
  return fields;
}

info *dac_iter(dac_rx *dac, size_t *out_count)
{
  if (!dac || !out_count)
    return NULL;

  const size_t NUM_FIELDS = 28;

  info *dinfo = malloc(NUM_FIELDS * sizeof(info));
  if (!dinfo)
    return NULL;

  size_t i = 0;

  dinfo[i++] = (info) {"dac_format", &dac->dac_format};
  dinfo[i++] = (info) {"threshold0", &dac->threshold0};
  dinfo[i++] = (info) {"threshold1", &dac->threshold1};
  dinfo[i++] = (info) {"threshold2", &dac->threshold2};
  dinfo[i++] = (info) {"threshold3", &dac->threshold3};
  dinfo[i++] = (info) {"threshold4", &dac->threshold4};
  dinfo[i++] = (info) {"threshold5", &dac->threshold5};
  dinfo[i++] = (info) {"threshold6", &dac->threshold6};
  dinfo[i++] = (info) {"threshold7", &dac->threshold7};
  dinfo[i++] = (info) {"preamp", &dac->preamp};
  dinfo[i++] = (info) {"ikrum", &dac->ikrum};
  dinfo[i++] = (info) {"shaper", &dac->shaper};
  dinfo[i++] = (info) {"disc", &dac->disc};
  dinfo[i++] = (info) {"disc_LS", &dac->disc_LS};
  dinfo[i++] = (info) {"shaper_test", &dac->shaper_test};
  dinfo[i++] = (info) {"dac_disc_L", &dac->dac_disc_L};
  dinfo[i++] = (info) {"dac_test", &dac->dac_test};
  dinfo[i++] = (info) {"dac_disc_H", &dac->dac_disc_H};
  dinfo[i++] = (info) {"delay", &dac->delay};
  dinfo[i++] = (info) {"TP_buff_in", &dac->TP_buff_in};
  dinfo[i++] = (info) {"TP_buff_out", &dac->TP_buff_out};
  dinfo[i++] = (info) {"RPZ", &dac->RPZ};
  dinfo[i++] = (info) {"GND", &dac->GND};
  dinfo[i++] = (info) {"TP_ref", &dac->TP_ref};
  dinfo[i++] = (info) {"FBK", &dac->FBK};
  dinfo[i++] = (info) {"Cas", &dac->Cas};
  dinfo[i++] = (info) {"TP_ref_A", &dac->TP_ref_A};
  dinfo[i++] = (info) {"TP_ref_B", &dac->TP_ref_B};

  *out_count = NUM_FIELDS;
  return dinfo;
}
