#ifndef PAIRCONVERTOR_SFIX_H
#define PAIRCONVERTOR_SFIX_H
#include "Client.SFIX.h"
#include "Tag.SFIX.h"
#include <time.h>

int SFIX_double_to_string(double value, SFIX_Tag tag, SFIX_Pair *pair);

int SFIX_string_to_string(const char *value, SFIX_Tag tag, SFIX_Pair *pair);

int SFIX_int_to_string(int value, SFIX_Tag tag, SFIX_Pair *pair);

int SFIX_time_to_string(time_t value, SFIX_Tag tag, SFIX_Pair *pair);

int SFIX_time_to_timestamp(time_t value, SFIX_Tag tag, SFIX_Pair *pair);

#endif
