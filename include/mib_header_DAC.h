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
