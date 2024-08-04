#include "Integrity.SFIX.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int SFIX_is_valid_double(const char *str) {
  /*#region*/
  char *endptr;

  // Check if the input is NULL
  if (str == NULL) {
    return 0;
  }

  // Skip leading white space
  while (isspace(*str)) {
    str++;
  }

  // Handle empty string and whitespace only string
  if (*str == '\0') {
    return 0;
  }

  // Reset errno to 0 before calling strtod
  errno = 0;

  // Convert string to double
  double val = strtod(str, &endptr);

  // Check for conversion errors
  if (errno == ERANGE && (val == HUGE_VAL || val == -HUGE_VAL)) {
    return 0;
  }

  // Check for conversion errors
  if (errno != 0 && val == 0) {
    return 0;
  }

  // Check if no digits were found
  if (endptr == str) {
    return 0;
  }

  // Check for characters left after conversion
  while (isspace(*endptr)) {
    endptr++;
  }
  if (*endptr != '\0') {
    return 0;
  }

  return 1;
  /*#endregion*/
}

int SFIX_is_valid_integer(const char *str) {
  /*#region*/
  char *endptr;

  // Check if the input is NULL
  if (str == NULL) {
    return 0;
  }

  // Skip leading white space
  while (isspace(*str)) {
    str++;
  }

  // Handle empty string and whitespace only string
  if (*str == '\0') {
    return 0;
  }

  // Reset errno to 0 before calling strtol
  errno = 0;

  // Convert string to long
  long val = strtol(str, &endptr, 10);

  // Check for conversion errors
  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
      (errno != 0 && val == 0)) {
    return 0;
  }

  // Check if no digits were found
  if (endptr == str) {
    return 0;
  }

  // Check for characters left after conversion
  while (isspace(*endptr)) {
    endptr++;
  }
  if (*endptr != '\0') {
    return 0;
  }

  return 1;
  /*#endregion*/
}
