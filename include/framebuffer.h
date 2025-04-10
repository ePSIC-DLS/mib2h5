// clang-format Language: C
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct {
  MQ1_fields *mq1_header;
  dac_rx *dac0;
  dac_rx *dac1;
  dac_rx *dac2;
  dac_rx *dac3;
  void **rows;
  void *data;
} framebuffer;

void free_frame(framebuffer *fb);

#endif
