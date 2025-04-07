#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include "mib_header_MQ1.h"
#include "mib_header_DAC.h"
#include "mib_macros.h"
#include "mib_utils.h"
#include "mq1_quad.h"
#include "mq1_single.h"
#include "read_mq1_headers.h"

typedef struct {
	MQ1_fields *mq1_header;
	dac_rx *dac0;
	dac_rx *dac1;
	dac_rx *dac2;
	dac_rx *dac3;
	void **rows;
	void *data;
} framebuffer;

void allocate_frame(framebuffer* fb);
void free_frame(framebuffer* fb);

#endif
