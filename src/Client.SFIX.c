#include "Client.SFIX.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOH "\001" // Regular tag delimiter
#define STX "\002" // Inner message delimiter
#define PROTOCOL "SFIX.1"

static char *colors[] = {
    "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m",
    "\x1b[35m", "\x1b[36m", "\x1b[37m", "\x1b[0m",
};

typedef enum {
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  Reset,
} Keys;

#define KEY(index)                                                             \
  ({                                                                           \
    char *_key = colors[index];                                                \
    _key;                                                                      \
  })

static int _$append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]);
static int _$body_length(char *fix_message);
static int _$count_digits(int num);
static int _$parse(const char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   const char *sep);
static void _$remplace(int value, char *message, int tag);

static int _$append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]) {
  /*#region*/
  char temp[SFIX_TAGSNUM];
  int writtenChars =
      snprintf(temp, sizeof(temp), "%d=%s%s", tag, value, delimiter);
  if (writtenChars > SFIX_MAX_VALUE_LEN) {
    perror("sprintf");
    exit(1);
  }
  strcat(buffer, temp);
  return writtenChars;
  /*#endregion*/
}

static int _$body_length(char *fix_message) {
  /*#region*/
  char *body_start = strstr(fix_message, "35=") + 5; // Start of Body
  int body_length = strlen(body_start) - 1;          // -1 for STX
  return body_length;
  /*#endregion*/
}

static int _$count_digits(int num) {
  /*#region*/
  int count = 0;
  // Handle negative numbers
  if (num == 0) {
    return 1;
  } else if (num < 0) {
    num = -num;
  }

  while (num != 0) {
    num /= 10;
    count++;
  }

  return count;
  /*#endregion*/
}

static int _$parse(const char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   const char *sep) {
  /*#region*/
  char msg_copy[SFIX_MSG_SIZE(256)];
  strncpy(msg_copy, message, sizeof(msg_copy) - 1);
  msg_copy[sizeof(msg_copy) - 1] = '\0'; // Ensure null termination

  char *token = strtok(msg_copy, sep);
  while (token) {
    char *separator = strchr(token, '=');
    if (separator) {
      *separator = '\0'; // Split tag and value

      int num = atoi(token);
      if (num >= 0 && num < SFIX_TAGSNUM) {
        // Copy the tag and value to the output structure
        strncpy(out[num].tag, token, SFIX_MAX_TAG_LEN - 1);
        out[num].tag[SFIX_MAX_TAG_LEN - 1] = '\0';

        strncpy(out[num].value, separator + 1, SFIX_MAX_VALUE_LEN - 1);
        out[num].value[SFIX_MAX_VALUE_LEN - 1] = '\0';
      } else {
        // Handle case where the tag number is out of range
        fprintf(stderr, "Warning: Tag number %d is out of range.\n", num);
      }
    }
    token = strtok(NULL, sep);
  }
  return 0;
  /*#endregion*/
}

static void _$remplace(int value, char *message, int tag) {
  /*#region*/
  char str[16];
  snprintf(str, sizeof(str), "%03d", value);
  char template[7];
  sprintf(template, "%d=000", tag);
  char *position = strstr(message, template) + _$count_digits(tag) + 1;
  memcpy(position, str, strlen(str));
  /*#endregion*/
}

int SFIX_estimateMessageSize(int pairs_length) {
  /*#region*/
  return SFIX_MSG_SIZE(pairs_length);
  /*#endregion*/
}

int SFIX_compose(char *out, char type, SFIX_Pair pairs[], int pairsLength) {
  /*#region*/
  int msg_size = strlen(out);
  msg_size += _$append_tag(out, 8, PROTOCOL, SOH);
  // Body Length (placeholder, will be replaced later)
  msg_size += _$append_tag(out, 9, "000", SOH);

  char temp[6];
  msg_size += snprintf(temp, sizeof(temp), "35=%c%s", type, SOH);
  strcat(out, temp);

  for (int i = 0; i < pairsLength; i++) {
    SFIX_Pair p = pairs[i];
    msg_size += _$append_tag(out, p.key, p.value, SOH);
  }
  strcat(out, STX);
  msg_size++; // count STX char
  msg_size++; // count null-terminate char

  int body_length = _$body_length(out);
  _$remplace(body_length, out, 9);

  return msg_size;
  /*#endregion*/
}

int SFIX_parse(char *message, SFIX_KeyValue *out) {
  /*#region*/
  int count = 0;
  int size = SFIX_listLength(message);
  char array[size][SFIX_MSG_SIZE(256)];

  int length = strlen(message);
  char copy[length];
  memset(copy, 0, length);
  strcpy(copy, message);

  char *p = strtok(copy, STX);
  while (p != NULL) {
    strcpy(array[count], p);
    p = strtok(NULL, STX);
    count++;
  }

  for (int i = 0; i < size; i++) {
    int index = (256 * i);
    out += index;
    int e = _$parse(array[i], out, SOH);
    if (e) {
      return 1;
    }
  }

  return 0;
  /*#endregion*/
}

int SFIX_listLength(char *message) {
  /*#region*/
  int count = 0;
  while (*message != '\0') {
    if (*message == '\x02') {
      count++;
    }
    message++;
  }
  return count;
  /*#endregion*/
}

void SFIX_print(char *message) {
  /*#region*/
  int length = SFIX_listLength(message);
  SFIX_KeyValue pairs[256 * length];
  memset(pairs, 0, sizeof(SFIX_KeyValue) * 256 * length);
  int err = SFIX_parse(message, pairs);
  if (err) {
    return perror("Unable to parse SFIX message");
  }
  char *s = KEY(Yellow);
  char *e = KEY(Reset);
  for (int i = 0; i < length; i++) {
    printf("%s{%s\n", s, e);
    for (int j = 0; j < 256; j++) {
      int index = (i * 256) + j;
      if (strcmp(pairs[index].tag, "")) {
        char *t = pairs[index].tag;
        char *v = pairs[index].value;
        char *r = KEY(Reset);
        char *a1 = KEY(Red);
        char *a2 = KEY(Yellow);
        char *a3 = KEY(Green);
        printf("  %s\"%s\"%s%s:%s %s\"%s\"%s,\n", a1,t, r, a2,r ,a3,v,r);
      }
    }
    i == length - 1 ? printf("%s}%s\n", s, e) : printf("%s}%s,\n", s, e);
  }
  /*#endregion*/
}

int SFIX_acknowledge(char out[SFIX_MSG_SIZE(0)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'A', pairs, 0);
  /*#endregion*/
}

int SFIX_unknown(char out[SFIX_MSG_SIZE(0)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'U', pairs, 0);
  /*#endregion*/
}
