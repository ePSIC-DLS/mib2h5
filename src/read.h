// clang-format Language: C
#ifndef READ_H
#define READ_H

#include "framebuffer.h"

void read_header(FILE *mib_ptr, long offset, framebuffer *fb);

void read_frame(FILE *mib_ptr, long offset, framebuffer *fb);

#endif
