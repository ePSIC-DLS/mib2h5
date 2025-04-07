#include "mq1_quad.h"
#include "mib_header_MQ1.h"
#include "mib_macros.h"
#include "mib_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this assumes the provided header is valid
 * the header struct will be overwritten
 */
void parse_mq1_quad(const char *header, mq1q *mq1_quad)
{

  int n = sscanf(
    header,
    "%[^,],%u,%u,%u,%u,%u,%[^,],%[^,],%[^,],%[^,],"
    "%lf,%u,%u,%u,%f,%f,%f,%f,%f,%f,%f,%f,"
    "%[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
    "u,%u,%u,%u,%u,"
    "%[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
    "u,%u,%u,%u,%u,"
    "%[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
    "u,%u,%u,%u,%u,"
    "%[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
    "u,%u,%u,%u,%u,"
    "%[^,],%[^,],%uns,%u",
    mq1_quad->header_id, &mq1_quad->sequence_number, &mq1_quad->header_bytes,
    &mq1_quad->num_chips, &mq1_quad->det_x, &mq1_quad->det_y,
    mq1_quad->pixel_depth, mq1_quad->sensor_layout, mq1_quad->chip_select,
    mq1_quad->timestamp, &mq1_quad->exposure_time_s, &mq1_quad->counter,
    &mq1_quad->colour_mode, &mq1_quad->gain_mode, &mq1_quad->threshold[0],
    &mq1_quad->threshold[1], &mq1_quad->threshold[2], &mq1_quad->threshold[3],
    &mq1_quad->threshold[4], &mq1_quad->threshold[5], &mq1_quad->threshold[6],
    &mq1_quad->threshold[7], mq1_quad->dac0.dac_format,
    &mq1_quad->dac0.threshold0, &mq1_quad->dac0.threshold1,
    &mq1_quad->dac0.threshold2, &mq1_quad->dac0.threshold3,
    &mq1_quad->dac0.threshold4, &mq1_quad->dac0.threshold5,
    &mq1_quad->dac0.threshold6, &mq1_quad->dac0.threshold7,
    &mq1_quad->dac0.preamp, &mq1_quad->dac0.ikrum, &mq1_quad->dac0.shaper,
    &mq1_quad->dac0.disc, &mq1_quad->dac0.disc_LS, &mq1_quad->dac0.shaper_test,
    &mq1_quad->dac0.dac_disc_L, &mq1_quad->dac0.dac_test,
    &mq1_quad->dac0.dac_disc_H, &mq1_quad->dac0.delay,
    &mq1_quad->dac0.TP_buff_in, &mq1_quad->dac0.TP_buff_out,
    &mq1_quad->dac0.RPZ, &mq1_quad->dac0.GND, &mq1_quad->dac0.TP_ref,
    &mq1_quad->dac0.FBK, &mq1_quad->dac0.Cas, &mq1_quad->dac0.TP_ref_A,
    &mq1_quad->dac0.TP_ref_B, mq1_quad->dac1.dac_format,
    &mq1_quad->dac1.threshold0, &mq1_quad->dac1.threshold1,
    &mq1_quad->dac1.threshold2, &mq1_quad->dac1.threshold3,
    &mq1_quad->dac1.threshold4, &mq1_quad->dac1.threshold5,
    &mq1_quad->dac1.threshold6, &mq1_quad->dac1.threshold7,
    &mq1_quad->dac1.preamp, &mq1_quad->dac1.ikrum, &mq1_quad->dac1.shaper,
    &mq1_quad->dac1.disc, &mq1_quad->dac1.disc_LS, &mq1_quad->dac1.shaper_test,
    &mq1_quad->dac1.dac_disc_L, &mq1_quad->dac1.dac_test,
    &mq1_quad->dac1.dac_disc_H, &mq1_quad->dac1.delay,
    &mq1_quad->dac1.TP_buff_in, &mq1_quad->dac1.TP_buff_out,
    &mq1_quad->dac1.RPZ, &mq1_quad->dac1.GND, &mq1_quad->dac1.TP_ref,
    &mq1_quad->dac1.FBK, &mq1_quad->dac1.Cas, &mq1_quad->dac1.TP_ref_A,
    &mq1_quad->dac1.TP_ref_B, mq1_quad->dac2.dac_format,
    &mq1_quad->dac2.threshold0, &mq1_quad->dac2.threshold1,
    &mq1_quad->dac2.threshold2, &mq1_quad->dac2.threshold3,
    &mq1_quad->dac2.threshold4, &mq1_quad->dac2.threshold5,
    &mq1_quad->dac2.threshold6, &mq1_quad->dac2.threshold7,
    &mq1_quad->dac2.preamp, &mq1_quad->dac2.ikrum, &mq1_quad->dac2.shaper,
    &mq1_quad->dac2.disc, &mq1_quad->dac2.disc_LS, &mq1_quad->dac2.shaper_test,
    &mq1_quad->dac2.dac_disc_L, &mq1_quad->dac2.dac_test,
    &mq1_quad->dac2.dac_disc_H, &mq1_quad->dac2.delay,
    &mq1_quad->dac2.TP_buff_in, &mq1_quad->dac2.TP_buff_out,
    &mq1_quad->dac2.RPZ, &mq1_quad->dac2.GND, &mq1_quad->dac2.TP_ref,
    &mq1_quad->dac2.FBK, &mq1_quad->dac2.Cas, &mq1_quad->dac2.TP_ref_A,
    &mq1_quad->dac2.TP_ref_B, mq1_quad->dac3.dac_format,
    &mq1_quad->dac3.threshold0, &mq1_quad->dac3.threshold1,
    &mq1_quad->dac3.threshold2, &mq1_quad->dac3.threshold3,
    &mq1_quad->dac3.threshold4, &mq1_quad->dac3.threshold5,
    &mq1_quad->dac3.threshold6, &mq1_quad->dac3.threshold7,
    &mq1_quad->dac3.preamp, &mq1_quad->dac3.ikrum, &mq1_quad->dac3.shaper,
    &mq1_quad->dac3.disc, &mq1_quad->dac3.disc_LS, &mq1_quad->dac3.shaper_test,
    &mq1_quad->dac3.dac_disc_L, &mq1_quad->dac3.dac_test,
    &mq1_quad->dac3.dac_disc_H, &mq1_quad->dac3.delay,
    &mq1_quad->dac3.TP_buff_in, &mq1_quad->dac3.TP_buff_out,
    &mq1_quad->dac3.RPZ, &mq1_quad->dac3.GND, &mq1_quad->dac3.TP_ref,
    &mq1_quad->dac3.FBK, &mq1_quad->dac3.Cas, &mq1_quad->dac3.TP_ref_A,
    &mq1_quad->dac3.TP_ref_B, mq1_quad->header_extension_id,
    mq1_quad->extended_timestamp, &mq1_quad->exposure_time_ns,
    &mq1_quad->bit_depth);

  if (n == EOF) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: the header appears to be empty\n",
            current_file, __LINE__);
    exit(1);
  } else if (n != MQ1_QUAD_HEADER_NUM_FIELDS) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: failed to match the MQ1 Quad header\n",
            current_file, __LINE__);
    exit(1);
  }
}

void print_quad_mib_header(mq1q header)
{
  /* Assume the MQ1 Quad header (mq1q) has been initialised,
   * otherwise you should treat the printed values as garbages.
   */
  printf("header id: %3s\n", header.header_id);
  printf("sequence number: %u\n", header.sequence_number);
  printf("header bytes: %u\n", header.header_bytes);
  printf("number of chip(s): %u\n", header.num_chips);
  printf("detector size in x: %u\n", header.det_x);
  printf("detector size in y: %u\n", header.det_y);
  printf("pixel data type: %3s\n", header.pixel_depth);
  printf("sensor layout: %6s\n", header.sensor_layout);
  printf("chip selected: %2s\n", header.chip_select);
  printf("time stamp: %26s\n", header.timestamp);
  printf("exposure time (s): %lf\n", header.exposure_time_s);
  printf("counter: %u\n", header.counter);
  printf("colour mode: %u\n", header.colour_mode);
  printf("gain mode: %u\n", header.gain_mode);
  printf("threshold 0: %f keV\n", header.threshold[0]);
  printf("threshold 1: %f keV\n", header.threshold[1]);
  printf("threshold 2: %f keV\n", header.threshold[2]);
  printf("threshold 3: %f keV\n", header.threshold[3]);
  printf("threshold 4: %f keV\n", header.threshold[4]);
  printf("threshold 5: %f keV\n", header.threshold[5]);
  printf("threshold 6: %f keV\n", header.threshold[6]);
  printf("threshold 7: %f keV\n", header.threshold[7]);
  puts("DAC 0:");
  printf("\t DAC format: %3s\n", header.dac0.dac_format);
  printf("\t threshold0: %u\n", header.dac0.threshold0);
  printf("\t threshold1: %u\n", header.dac0.threshold1);
  printf("\t threshold2: %u\n", header.dac0.threshold2);
  printf("\t threshold3: %u\n", header.dac0.threshold3);
  printf("\t threshold4: %u\n", header.dac0.threshold4);
  printf("\t threshold5: %u\n", header.dac0.threshold5);
  printf("\t threshold6: %u\n", header.dac0.threshold6);
  printf("\t threshold7: %u\n", header.dac0.threshold7);
  printf("\t preamp: %u\n", header.dac0.preamp);
  printf("\t ikrum: %u\n", header.dac0.ikrum);
  printf("\t shaper: %u\n", header.dac0.shaper);
  printf("\t disc: %u\n", header.dac0.disc);
  printf("\t disc_LS: %u\n", header.dac0.disc_LS);
  printf("\t shaper_test: %u\n", header.dac0.shaper_test);
  printf("\t dac_disc_L: %u\n", header.dac0.dac_disc_L);
  printf("\t dac_test: %u\n", header.dac0.dac_test);
  printf("\t dac_disc_H: %u\n", header.dac0.dac_disc_H);
  printf("\t delay: %u\n", header.dac0.delay);
  printf("\t TP_buff_in: %u\n", header.dac0.TP_buff_in);
  printf("\t TP_buff_out: %u\n", header.dac0.TP_buff_out);
  printf("\t RPZ: %u\n", header.dac0.RPZ);
  printf("\t GND: %u\n", header.dac0.GND);
  printf("\t TP_ref: %u\n", header.dac0.TP_ref);
  printf("\t FBK: %u\n", header.dac0.FBK);
  printf("\t Cas: %u\n", header.dac0.Cas);
  printf("\t TP_ref_A: %u\n", header.dac0.TP_ref_A);
  printf("\t TP_ref_B: %u\n", header.dac0.TP_ref_B);
  puts("DAC 1:");
  printf("\t DAC format: %3s\n", header.dac1.dac_format);
  printf("\t threshold0: %u\n", header.dac1.threshold0);
  printf("\t threshold1: %u\n", header.dac1.threshold1);
  printf("\t threshold2: %u\n", header.dac1.threshold2);
  printf("\t threshold3: %u\n", header.dac1.threshold3);
  printf("\t threshold4: %u\n", header.dac1.threshold4);
  printf("\t threshold5: %u\n", header.dac1.threshold5);
  printf("\t threshold6: %u\n", header.dac1.threshold6);
  printf("\t threshold7: %u\n", header.dac1.threshold7);
  printf("\t preamp: %u\n", header.dac1.preamp);
  printf("\t ikrum: %u\n", header.dac1.ikrum);
  printf("\t shaper: %u\n", header.dac1.shaper);
  printf("\t disc: %u\n", header.dac1.disc);
  printf("\t disc_LS: %u\n", header.dac1.disc_LS);
  printf("\t shaper_test: %u\n", header.dac1.shaper_test);
  printf("\t dac_disc_L: %u\n", header.dac1.dac_disc_L);
  printf("\t dac_test: %u\n", header.dac1.dac_test);
  printf("\t dac_disc_H: %u\n", header.dac1.dac_disc_H);
  printf("\t delay: %u\n", header.dac1.delay);
  printf("\t TP_buff_in: %u\n", header.dac1.TP_buff_in);
  printf("\t TP_buff_out: %u\n", header.dac1.TP_buff_out);
  printf("\t RPZ: %u\n", header.dac1.RPZ);
  printf("\t GND: %u\n", header.dac1.GND);
  printf("\t TP_ref: %u\n", header.dac1.TP_ref);
  printf("\t FBK: %u\n", header.dac1.FBK);
  printf("\t Cas: %u\n", header.dac1.Cas);
  printf("\t TP_ref_A: %u\n", header.dac1.TP_ref_A);
  printf("\t TP_ref_B: %u\n", header.dac1.TP_ref_B);
  puts("DAC 2:");
  printf("\t DAC format: %3s\n", header.dac2.dac_format);
  printf("\t threshold0: %u\n", header.dac2.threshold0);
  printf("\t threshold1: %u\n", header.dac2.threshold1);
  printf("\t threshold2: %u\n", header.dac2.threshold2);
  printf("\t threshold3: %u\n", header.dac2.threshold3);
  printf("\t threshold4: %u\n", header.dac2.threshold4);
  printf("\t threshold5: %u\n", header.dac2.threshold5);
  printf("\t threshold6: %u\n", header.dac2.threshold6);
  printf("\t threshold7: %u\n", header.dac2.threshold7);
  printf("\t preamp: %u\n", header.dac2.preamp);
  printf("\t ikrum: %u\n", header.dac2.ikrum);
  printf("\t shaper: %u\n", header.dac2.shaper);
  printf("\t disc: %u\n", header.dac2.disc);
  printf("\t disc_LS: %u\n", header.dac2.disc_LS);
  printf("\t shaper_test: %u\n", header.dac2.shaper_test);
  printf("\t dac_disc_L: %u\n", header.dac2.dac_disc_L);
  printf("\t dac_test: %u\n", header.dac2.dac_test);
  printf("\t dac_disc_H: %u\n", header.dac2.dac_disc_H);
  printf("\t delay: %u\n", header.dac2.delay);
  printf("\t TP_buff_in: %u\n", header.dac2.TP_buff_in);
  printf("\t TP_buff_out: %u\n", header.dac2.TP_buff_out);
  printf("\t RPZ: %u\n", header.dac2.RPZ);
  printf("\t GND: %u\n", header.dac2.GND);
  printf("\t TP_ref: %u\n", header.dac2.TP_ref);
  printf("\t FBK: %u\n", header.dac2.FBK);
  printf("\t Cas: %u\n", header.dac2.Cas);
  printf("\t TP_ref_A: %u\n", header.dac2.TP_ref_A);
  printf("\t TP_ref_B: %u\n", header.dac2.TP_ref_B);
  puts("DAC 3:");
  printf("\t DAC format: %3s\n", header.dac3.dac_format);
  printf("\t threshold0: %u\n", header.dac3.threshold0);
  printf("\t threshold1: %u\n", header.dac3.threshold1);
  printf("\t threshold2: %u\n", header.dac3.threshold2);
  printf("\t threshold3: %u\n", header.dac3.threshold3);
  printf("\t threshold4: %u\n", header.dac3.threshold4);
  printf("\t threshold5: %u\n", header.dac3.threshold5);
  printf("\t threshold6: %u\n", header.dac3.threshold6);
  printf("\t threshold7: %u\n", header.dac3.threshold7);
  printf("\t preamp: %u\n", header.dac3.preamp);
  printf("\t ikrum: %u\n", header.dac3.ikrum);
  printf("\t shaper: %u\n", header.dac3.shaper);
  printf("\t disc: %u\n", header.dac3.disc);
  printf("\t disc_LS: %u\n", header.dac3.disc_LS);
  printf("\t shaper_test: %u\n", header.dac3.shaper_test);
  printf("\t dac_disc_L: %u\n", header.dac3.dac_disc_L);
  printf("\t dac_test: %u\n", header.dac3.dac_test);
  printf("\t dac_disc_H: %u\n", header.dac3.dac_disc_H);
  printf("\t delay: %u\n", header.dac3.delay);
  printf("\t TP_buff_in: %u\n", header.dac3.TP_buff_in);
  printf("\t TP_buff_out: %u\n", header.dac3.TP_buff_out);
  printf("\t RPZ: %u\n", header.dac3.RPZ);
  printf("\t GND: %u\n", header.dac3.GND);
  printf("\t TP_ref: %u\n", header.dac3.TP_ref);
  printf("\t FBK: %u\n", header.dac3.FBK);
  printf("\t Cas: %u\n", header.dac3.Cas);
  printf("\t TP_ref_A: %u\n", header.dac3.TP_ref_A);
  printf("\t TP_ref_B: %u\n", header.dac3.TP_ref_B);
  printf("header extension id: %4s\n", header.header_extension_id);
  printf("extended time stamp: %28s\n", header.extended_timestamp);
  printf("exposure time (ns): %u\n", header.exposure_time_ns);
  printf("pixel bit depth: %u\n", header.bit_depth);
}
