// clang-format Language: C
#include "mib_header.h"

#ifndef PARSE_MQ1_SINGLE_H
#define PARSE_MQ1_SINGLE_H
void parse_mq1_single(const char *header, mq1s *mq1_single);
#endif

#ifndef PRINT_SINGLE_MIB_HEADER_H
#define PRINT_SINGLE_MIB_HEADER_H
void print_single_mib_header(mq1s header);
#endif

#ifndef PARSE_MQ1_QUAD_H
#define PARSE_MQ1_QUAD_H
void parse_mq1_quad(const char *header, mq1q *mq1_quad);
#endif

#ifndef PRINT_QUAD_MIB_HEADER_H
#define PRINT_QUAD_MIB_HEADER_H
void print_quad_mib_header(mq1q header);
#endif
