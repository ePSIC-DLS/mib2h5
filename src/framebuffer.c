#include "framebuffer.h"
#include "mib_header_DAC.h"
#include "mib_header_MQ1.h"
#include "mib_macros.h"
#include "mib_props_supp.h"
#include "mib_utils.h"
#include "mq1_quad.h"
#include "mq1_single.h"
#include "read_mq1_headers.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void allocate_frame(framebuffer *fb)
{
  return;
}

void free_frame(framebuffer *fb)
{
  if (fb->rows)
    free(fb->rows);
  if (fb->data)
    free(fb->data);
  if (fb->dac0)
    free(fb->dac0);
  if (fb->dac1)
    free(fb->dac1);
  if (fb->dac2)
    free(fb->dac2);
  if (fb->dac3)
    free(fb->dac3);
  fb->rows = NULL;
  fb->data = NULL;
  fb->dac0 = NULL;
  fb->dac1 = NULL;
  fb->dac2 = NULL;
  fb->dac3 = NULL;
}
