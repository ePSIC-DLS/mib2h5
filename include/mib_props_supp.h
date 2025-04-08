// clang-format Language: C
#ifndef MQ1_FIELD_ITER_H
#define MQ1_FIELD_ITER_H

#include "mib_header_DAC.h"
#include "mib_header_MQ1.h"
#include "read_mq1_headers.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char *name;
  void *data;
} info;

info *mq1_fields_iter(MQ1_fields *fields, size_t *out_count);
info *dac_iter(dac_rx *dac, size_t *out_count);

#endif
