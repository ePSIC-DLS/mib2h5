// clang-format Language: C
#ifndef READ_H
#define READ_H

#include "framebuffer.h"
#include "io_header.h"
#include "macros.h"
#include "parser.h"
#include "utils.h"
#include <stdint.h>

void read_header(FILE *mib_ptr, long offset, framebuffer *fb);

void read_frame(FILE *mib_ptr, long offset, framebuffer *fb);

#endif
