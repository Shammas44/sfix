#include "Integrity.SFIX.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

bool __match_string(const char *str, const char *pattern);

bool SFIX_is_valid_double(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  return __match_string(str, "^[-+]?[0-9]+\\.?([0-9]+)?$");
  /*#endregion*/
}

bool SFIX_is_valid_integer(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  return __match_string(str, "^[-+]?[0-9]+$");
  /*#endregion*/
}

bool SFIX_is_valid_formatted_date(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  return __match_string(str, "^[0-9]{4}-[0-9]{2}-[0-9]{2}$");
  /*#endregion*/
}

bool SFIX_is_valid_formatted_time(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  return __match_string(str, "^[0-9]{2}:[0-9]{2}:[0-9]{2}$");
  /*#endregion*/
}

bool SFIX_is_valid_ticker(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  return __match_string(str, "^[A-Z]{1,5}$");
  /*#endregion*/
}

bool SFIX_is_non_empty_printable(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  if (str == NULL || str[0] == '\0') {
    return false; // Empty or null string
  }

  for (const char *p = str; *p != '\0'; p++) {
    if (isprint((unsigned char)*p)) {
      return true; // At least one printable character
    }
  }

  return false; // Only unprintable characters
  /*#endregion*/
}

bool SFIX_integrity_validator(SFIX_KeyValue pairs[SFIX_TAGSNUM],
                              SFIX_Check checks[SFIX_TAGSNUM]) {
  /*#region*/
  for (size_t i = 0; i < SFIX_TAGSNUM; i++) {
    const char *tag = pairs[i].tag;
    const char *value = pairs[i].value;

    // Skip if tag is empty
    if (tag[0] == '\0') {
      continue;
    }

    int tag_id = atoi(tag);

    // Defensive: ensure tag index is in bounds
    if (tag_id < 0 || (size_t)tag_id >= SFIX_TAGSNUM) {
      continue;
    }

    bool (*validate_fn)(const char[SFIX_MAX_VALUE_LEN]) = checks[tag_id].fn;

    if (validate_fn && !validate_fn(value)) {
      // printf("Invalid value for tag %s: %s\n", tag, value);
      return false;
    }
  }

  return true;
  /*#endregion*/
}

bool __match_string(const char *str, const char *pattern) {
  /*#region*/
  regex_t regex;
  int reti;
  char msgbuf[100];

  if (str == NULL || pattern == NULL) {
    return 0;
  }

  // Compile the regular expression with extended syntax
  reti = regcomp(&regex, pattern, REG_EXTENDED);
  if (reti) {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Could not compile regex '%s': %s\n", pattern, msgbuf);
    exit(EXIT_FAILURE);
  }

  // Execute the regex
  // 0, NULL, 0 means we don't care about capturing groups or flags
  reti = regexec(&regex, str, 0, NULL, 0);

  regfree(&regex); // Free compiled regex resources

  if (!reti) {
    return true; // Match found
  } else if (reti == REG_NOMATCH) {
    return false; // No match
  } else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex execution failed for '%s' with pattern '%s': %s\n",
            str, pattern, msgbuf);
    exit(EXIT_FAILURE);
  }
  /*#endregion*/
}
