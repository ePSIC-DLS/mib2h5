#include "mq1_single.h"
#include "mib_header_MQ1.h"
#include "mib_macros.h"
#include "mib_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this assumes the provided header is valid
 * the header struct will be overwritten
 */
void parse_mq1_single(const char *header, mq1s *mq1_single)
{

  int n = sscanf(
    header,
    "%[^,],%u,%u,%u,%u,%u,%[^,],%[^,],%[^,],%[^,],"
    "%lf,%u,%u,%u,%f,%f,%f,%f,%f,%f,%f,%f,"
    "%[^,],%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%"
    "u,%u,%u,%u,%u,"
    "%[^,],%[^,],%uns,%u",
    mq1_single->header_id, &mq1_single->sequence_number,
    &mq1_single->header_bytes, &mq1_single->num_chips, &mq1_single->det_x,
    &mq1_single->det_y, mq1_single->pixel_depth, mq1_single->sensor_layout,
    mq1_single->chip_select, mq1_single->timestamp,
    &mq1_single->exposure_time_s, &mq1_single->counter,
    &mq1_single->colour_mode, &mq1_single->gain_mode, &mq1_single->threshold[0],
    &mq1_single->threshold[1], &mq1_single->threshold[2],
    &mq1_single->threshold[3], &mq1_single->threshold[4],
    &mq1_single->threshold[5], &mq1_single->threshold[6],
    &mq1_single->threshold[7], mq1_single->dac0.dac_format,
    &mq1_single->dac0.threshold0, &mq1_single->dac0.threshold1,
    &mq1_single->dac0.threshold2, &mq1_single->dac0.threshold3,
    &mq1_single->dac0.threshold4, &mq1_single->dac0.threshold5,
    &mq1_single->dac0.threshold6, &mq1_single->dac0.threshold7,
    &mq1_single->dac0.preamp, &mq1_single->dac0.ikrum, &mq1_single->dac0.shaper,
    &mq1_single->dac0.disc, &mq1_single->dac0.disc_LS,
    &mq1_single->dac0.shaper_test, &mq1_single->dac0.dac_disc_L,
    &mq1_single->dac0.dac_test, &mq1_single->dac0.dac_disc_H,
    &mq1_single->dac0.delay, &mq1_single->dac0.TP_buff_in,
    &mq1_single->dac0.TP_buff_out, &mq1_single->dac0.RPZ, &mq1_single->dac0.GND,
    &mq1_single->dac0.TP_ref, &mq1_single->dac0.FBK, &mq1_single->dac0.Cas,
    &mq1_single->dac0.TP_ref_A, &mq1_single->dac0.TP_ref_B,
    mq1_single->header_extension_id, mq1_single->extended_timestamp,
    &mq1_single->exposure_time_ns, &mq1_single->bit_depth);

  if (n == EOF) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: the header appears to be empty\n",
            current_file, __LINE__);
    exit(1);
  } else if (n != MQ1_SINGLE_HEADER_NUM_FIELDS) {
    const char *current_file = only_file_name(__FILE__);
    fprintf(stderr, "%s:%d: error: failed to match the MQ1 Quad header\n",
            current_file, __LINE__);
    exit(1);
  }
}

void print_single_mib_header(mq1s header)
{
  /* Assume the MQ1 Single header (mq1s) has been initialised,
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
  printf("header extension id: %4s\n", header.header_extension_id);
  printf("extended time stamp: %28s\n", header.extended_timestamp);
  printf("exposure time (ns): %u\n", header.exposure_time_ns);
  printf("pixel bit depth: %u\n", header.bit_depth);
}
