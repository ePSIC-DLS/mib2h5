// clang-format Language: C
#ifndef READ_H
#define READ_H

#include "framebuffer.h"
#include <stdint.h>

void read_header(FILE *mib_ptr, unsigned long offset, framebuffer *fb);

void read_frame(FILE *mib_ptr, unsigned long offset, framebuffer *fb);

#endif
