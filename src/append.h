// clang-format Language: C
#ifndef APPEND_H
#define APPEND_H

#include "framebuffer.h"
#include "io_header.h"
#include "parser.h"
#include "utils.h"
#include <hdf5.h>
#include <stdint.h>

void append_frame_to_dataset(hid_t dset, framebuffer *fb, int cbytes);

void append_meta_to_dataset(hid_t *meta_handle, framebuffer *fb);

void append_dac_to_dataset(unsigned int num_chips,
                           hid_t *dac_handle,
                           framebuffer *fb);

#endif
