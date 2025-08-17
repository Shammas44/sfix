#include "PairConvertor.SFIX.h"
#include <stdio.h>
#include <string.h>

int SFIX_double_to_string(double value, SFIX_Tag tag, SFIX_Pair *pair) {
  /*#region*/
  char buffer[20] = {0};
  sprintf(buffer, "%.10f", value);
  pair->key = tag;
  strcpy(pair->value, buffer);
  return 0;
  /*#endregion*/
}

int SFIX_string_to_string(const char *value, SFIX_Tag tag, SFIX_Pair *pair) {
  /*#region*/
  pair->key = tag;
  strcpy(pair->value, value);
  return 0;
  /*#endregion*/
}

int SFIX_int_to_string(int value, SFIX_Tag tag, SFIX_Pair *pair) {
  /*#region*/
  char buffer[20] = {0};
  sprintf(buffer, "%d", value);
  pair->key = tag;
  strcpy(pair->value, buffer);
  return 0;
  /*#endregion*/
}

int SFIX_time_to_string(time_t value, SFIX_Tag tag, SFIX_Pair *pair) {
  /*#region*/
  char buffer[20] = {0};
  struct tm *time_info = localtime(&value);
  strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", time_info);
  pair->key = tag;
  strcpy(pair->value, buffer);
  return 0;
  /*#endregion*/
}

int SFIX_time_to_timestamp(time_t value, SFIX_Tag tag, SFIX_Pair *pair) {
  /*#region*/
  char buffer[20] = {0};
  sprintf(buffer, "%ld", value);
  pair->key = tag;
  strcpy(pair->value, buffer);
  return 0;
  /*#endregion*/
}
