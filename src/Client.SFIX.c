#include "Client.SFIX.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOH "\001" // Regular tag delimiter
#define STX "\002" // Inner tag delimiter
#define ETX "\003" // Inner message delimiter
#define T SFIX_Client
#define PROTOCOL "SFIX.1"

static void __append_tag(char *buffer, int tag, const char *value,
                         char delimiter[2]);
static int __body_length(char *fix_message);
static int __checksum(char *fix_message);
static char *__compose(T *self, char type, SFIX_Pair pairs[], int pairsLength);
static char *__composeInnerMessage(T *self, int message_id, SFIX_Pair pairs[],
                                   int pairsLength);
static char *__concatInnerMessages(T *self, char *pairs[], int length);
static int __parse(T *self, char *message, SFIX_KeyValue *out);
static void __print(T *self, char *message);
static char *__acknowledge(T *self);
static char *__unknown(T *self);
static int _$parse(T *self, char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   char *separator);

static int __count_digits(int num);
static int __isList(T *self, char *message);
static void __remplace(int value, char *message, int tag);
static void __destructor(T *self);

T *SFIX_client_constructor() {
  /*#region*/
  T *self = malloc(sizeof(T));
  self->parse = __parse;
  self->destructor = __destructor;
  self->compose = __compose;
  self->print = __print;
  self->acknowledge = __acknowledge;
  self->unknown = __unknown;
  self->composeInnerMessage = __composeInnerMessage;
  self->concatInnerMessages = __concatInnerMessages;
  self->isList = __isList;
  return self;
  /*#endregion*/
}

static void __destructor(T *self) {
  /*#region*/
  free(self);
  /*#endregion*/
}

static void __append_tag(char *buffer, int tag, const char *value,
                         char delimiter[2]) {
  /*#region*/
  char temp[SFIX_TAGSNUM];
  snprintf(temp, sizeof(temp), "%d=%s%s", tag, value, delimiter);
  strcat(buffer, temp);
  /*#endregion*/
}

static int __body_length(char *fix_message) {
  /*#region*/
  char *body_start = strstr(fix_message, "35=") + 5; // Start of Body
  int body_length = strlen(body_start);
  return body_length;
  /*#endregion*/
}

static int __checksum(char *fix_message) {
  /*#region*/
  int checksum = 0;
  for (char *p = fix_message; *p; p++) {
    checksum += *p;
  }
  checksum %= 256;
  return checksum;
  /*#endregion*/
}

static int __count_digits(int num) {
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

static char *__compose(T *self, char type, SFIX_Pair pairs[], int pairsLength) {
  /*#region*/
  (void)(self);
  char fix_message[1024] = {0};
  __append_tag(fix_message, 8, PROTOCOL, SOH);
  // Body Length (placeholder, will be replaced later)
  __append_tag(fix_message, 9, "000", SOH);
  char strType[2];
  sprintf(strType, "%c", type);
  __append_tag(fix_message, 35, strType, SOH);

  for (int i = 0; i < pairsLength; i++) {
    SFIX_Pair p = pairs[i];
    __append_tag(fix_message, p.key, p.value, SOH);
  }
  // CheckSum (placeholder, will be replaced later)
  __append_tag(fix_message, 10, "000", SOH);

  int body_length = __body_length(fix_message);
  __remplace(body_length, fix_message, 9);

  int checksum = __checksum(fix_message);
  __remplace(checksum, fix_message, 10);

  return strdup(fix_message);
  /*#endregion*/
}

static char *__composeInnerMessage(T *self, int message_id, SFIX_Pair pairs[],
                                   int pairsLength) {
  /*#region*/
  (void)(self);
  char fix_message[1024] = {0};
  char out[1024] = {0};
  for (int i = 0; i < pairsLength; i++) {
    SFIX_Pair p = pairs[i];
    __append_tag(fix_message, p.key, p.value, STX);
  }
  __append_tag(out, message_id, fix_message, ETX);
  return strdup(out);
  /*#endregion*/
}

static char *__concatInnerMessages(T *self, char *pairs[], int length) {
/*#region*/
  (void)(self);
  char fix_message[1024] = {0};
  for (int i = 0; i < length; i++) {
    char *p = pairs[i];
    char temp[SFIX_TAGSNUM];
    snprintf(temp, sizeof(temp), "%s", p);
    strcat(fix_message, temp);
  }
  return strdup(fix_message);
/*#endregion*/
}

static void __remplace(int value, char *message, int tag) {
  /*#region*/
  char str[16];
  snprintf(str, sizeof(str), "%03d", value);
  char template[7];
  sprintf(template, "%d=000", tag);
  char *position = strstr(message, template) + __count_digits(tag) + 1;
  memcpy(position, str, strlen(str));
  /*#endregion*/
}

static int __parse(T *self, char *message, SFIX_KeyValue *out) {
  /*#region*/
  int e = 0;
  int list_len = __isList(self, message);
  e = _$parse(self, message, out, SOH);
  if (e)
    return 1;

  if (list_len > 1) {
    char *list = out[SFIX_Tag_List].value;
    SFIX_KeyValue messages[SFIX_TAGSNUM] = {0};
    e = _$parse(self, list, messages, ETX);
    if (e)
      return 1;

    for (int i = 0; i < list_len; i++) {
      char *innerMessage = messages[i].value;
      int index = (256 * i);
      out += index;
      e = _$parse(self, innerMessage, out, STX);
      if (e)
        return 1;
    }
  } else {
    return 0;
  }
  return 0;
  /*#endregion*/
}

static int __isList(T *self, char *message) {
  /*#region*/
  (void)self;
  const char search[] = "\x01"
                        "20=";
  char *p = strstr(message, search);
  if (!p) {
    return -1;
  }
  p += 4; // Move past "\x01" and "20="

  // Check if the next characters are digits and followed by SOH character
  char *end = p;
  while (*end >= '0' && *end <= '9') {
    end++;
  }

  if (*end != '\x01') {
    return -1; // Not followed by SOH character
  }

  // Temporarily null-terminate the string to isolate the number
  *end = '\0';
  int value = atoi(p);
  *end = '\x01'; // Restore the original character

  return value;
  /*#endregion*/
}

static void __print(T *self, char *message) {
  /*#region*/
  (void)(self);
  size_t length = strlen(message);
  char buffer[length + 1]; // +1 for the null terminator
  for (size_t i = 0; i < length; i++) {
    if (message[i] == '\x01' || message[i] == '\x02' || message[i] == '\x03') {
      buffer[i] = ' ';
    } else {
      buffer[i] = message[i];
    }
  }
  buffer[length] = '\0';
  printf("%s\n", buffer);
  /*#endregion*/
}

static char *__acknowledge(T *self) {
  /*#region*/
  (void)self;
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return __compose(self, 'A', pairs, sizeof(pairs) / sizeof(pairs[0]));
  /*#endregion*/
}

static char *__unknown(T *self) {
  /*#region*/
  (void)self;
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return __compose(self, 'U', pairs, sizeof(pairs) / sizeof(pairs[0]));
  /*#endregion*/
}

static int _$parse(T *self, char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   char *sep) {
  /*#region*/
  (void)(self);
  char *msg_copy = strdup(message);
  if (!msg_copy) {
    perror("strdup failed");
    return 1;
  }

  char *token = strtok(msg_copy, sep);
  while (token) {
    char *separator = strchr(token, '=');
    if (separator) {
      *separator = '\0'; // Split tag and value
      int num = atoi(token);
      strncpy(out[num].tag, token, SFIX_MAX_TAG_LEN - 1);
      out[num].tag[SFIX_MAX_TAG_LEN - 1] = '\0';
      strncpy(out[num].value, separator + 1, SFIX_MAX_VALUE_LEN - 1);
      out[num].value[SFIX_MAX_VALUE_LEN - 1] = '\0';
    }
    token = strtok(NULL, sep);
  }
  free(msg_copy); // Free the duplicated message
  return 0;
  /*#endregion*/
}
