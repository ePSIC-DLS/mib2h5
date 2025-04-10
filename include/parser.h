// clang-format Language: C
#include "mib_header.h"

#ifndef PARSER_H
#define PARSER_H
void parse_mq1_single(const char *header, mq1s *mq1_single);

void print_single_mib_header(mq1s header);

void parse_mq1_quad(const char *header, mq1q *mq1_quad);

void print_quad_mib_header(mq1q header);
#endif
