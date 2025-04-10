// clang-format Language: C
#include "macros.h"
#ifndef MIB_HEADER_DAC_RX_H
#define MIB_HEADER_DAC_RX_H

typedef struct {
  char dac_format[4];
  unsigned int threshold0;
  unsigned int threshold1;
  unsigned int threshold2;
  unsigned int threshold3;
  unsigned int threshold4;
  unsigned int threshold5;
  unsigned int threshold6;
  unsigned int threshold7;
  unsigned int preamp;
  unsigned int ikrum;
  unsigned int shaper;
  unsigned int disc;
  unsigned int disc_LS;
  unsigned int shaper_test;
  unsigned int dac_disc_L;
  unsigned int dac_test;
  unsigned int dac_disc_H;
  unsigned int delay;
  unsigned int TP_buff_in;
  unsigned int TP_buff_out;
  unsigned int RPZ;
  unsigned int GND;
  unsigned int TP_ref;
  unsigned int FBK;
  unsigned int Cas;
  unsigned int TP_ref_A;
  unsigned int TP_ref_B;
} dac_rx;

#endif
/*
field 1: header ID (MQ1)
field 2: sequence number of a frame
field 3: the number of bytes of the header
field 4: the number of chips
field 5: the number of columns of the detector (x)
field 6: the number of rows of the detector (y)
field 7: the pixel data type (U01, U08, U16, U32, U64)
field 8: sensor layout (2x2, Nx1, 2x2G, Nx1G, with leading spaces)
field 9: active chip(s) during the capture of the frame, U8 hexadecimal repr
field 10: timestamp
field 11: shutter opening time (seconds)
field 12: counter 0 or 1
field 13: colour mode, 0=monochrome image, 1=colour image
field 14: gain modem 0 = SLGM, 1 = LGM, 2 = HGM, 3 = SHGM
field 15-22: threshold values (keV)

DAC of each chips contains 28 entries.

Header extension
       Single / Quad
field  51     / 135  : MQ1A
field  52     / 136  : UTC timestamp (time in ns)
field  53     / 137  : shutter opening time (with suffix "ns")
field  54     / 138  : the pixel data bit depth

The remainings are null bytes for padding.
*/

#ifndef MIB_HEADER_MQ1_SINGLE_H
#define MIB_HEADER_MQ1_SINGLE_H

typedef struct {
  char header_id[MQ1_CHAR_LEN_HEADER_ID];
  unsigned int sequence_number;
  unsigned int header_bytes;
  unsigned int num_chips;
  unsigned int det_x;
  unsigned int det_y;
  char pixel_depth[MQ1_CHAR_LEN_PIXEL_DEPTH];
  char sensor_layout[MQ1_CHAR_LEN_SENSOR_LAYOUT];
  char chip_select[MQ1_CHAR_LEN_CHIP_SELECT];
  char timestamp[MQ1_CHAR_LEN_TIMESTAMP];
  double exposure_time_s;
  unsigned int counter;
  unsigned int colour_mode;
  unsigned int gain_mode;
  float threshold[MQ1_FLOAT_LEN_THRESHOLD];
  dac_rx dac0;
  char header_extension_id[MQ1_CHAR_LEN_HEADER_EXTENSION_ID];
  char extended_timestamp[MQ1_CHAR_LEN_EXTENDED_TIMESTAMP];
  unsigned int exposure_time_ns;
  unsigned int bit_depth;
} mq1s;

#endif

#ifndef MIB_HEADER_MQ1_QUAD_H
#define MIB_HEADER_MQ1_QUAD_H

typedef struct {
  char header_id[MQ1_CHAR_LEN_HEADER_ID];
  unsigned int sequence_number;
  unsigned int header_bytes;
  unsigned int num_chips;
  unsigned int det_x;
  unsigned int det_y;
  char pixel_depth[MQ1_CHAR_LEN_PIXEL_DEPTH];
  char sensor_layout[MQ1_CHAR_LEN_SENSOR_LAYOUT];
  char chip_select[MQ1_CHAR_LEN_CHIP_SELECT];
  char timestamp[MQ1_CHAR_LEN_TIMESTAMP];
  double exposure_time_s;
  unsigned int counter;
  unsigned int colour_mode;
  unsigned int gain_mode;
  float threshold[MQ1_FLOAT_LEN_THRESHOLD];
  dac_rx dac0;
  dac_rx dac1;
  dac_rx dac2;
  dac_rx dac3;
  char header_extension_id[MQ1_CHAR_LEN_HEADER_EXTENSION_ID];
  char extended_timestamp[MQ1_CHAR_LEN_EXTENDED_TIMESTAMP];
  unsigned int exposure_time_ns;
  unsigned int bit_depth;
} mq1q;

#endif
