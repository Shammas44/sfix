#ifndef INTEGRITY_SFIX_H
#define INTEGRITY_SFIX_H
#include "Client.SFIX.h"
#include <stddef.h>

typedef struct {
  bool (*fn)(const char[SFIX_MAX_VALUE_LEN]);
} SFIX_Check;

bool SFIX_is_valid_double(const char str[SFIX_MAX_VALUE_LEN]);
bool SFIX_is_non_empty_printable(const char str[SFIX_MAX_VALUE_LEN]);
bool SFIX_is_valid_integer(const char str[SFIX_MAX_VALUE_LEN]);
bool SFIX_is_valid_formatted_date(const char str[SFIX_MAX_VALUE_LEN]);
bool SFIX_is_valid_formatted_time(const char str[SFIX_MAX_VALUE_LEN]);
bool SFIX_is_valid_ticker(const char str[SFIX_MAX_VALUE_LEN]);

bool SFIX_integrity_validator(SFIX_KeyValue pairs[SFIX_TAGSNUM],
                              SFIX_Check checks[SFIX_TAGSNUM]);
#endif

#ifdef UNIT_TESTING
  bool __match_string(const char *str, const char *pattern);
#endif
