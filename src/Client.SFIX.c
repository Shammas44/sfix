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

static int __append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]);
static int __body_length(char *fix_message);
static int __checksum(char *fix_message);
static char *__compose(T *self, char type, SFIX_Pair pairs[], int pairsLength,
                       int *out_length);
static char *__composeInnerMessage(T *self, int message_id, SFIX_Pair pairs[],
                                   int pairsLength);
static char *__concatInnerMessages(T *self, char *pairs[], int length);
static int __parse(T *self, char *message, SFIX_KeyValue *out);
static void __print(T *self, char *message);
static char *__acknowledge(T *self, int *out_length);
static char *__unknown(T *self, int *out_length);
static int _$parse(T *self, char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   char *separator);

static int __count_digits(int num);
static int __isList(T *self, char *message);
static void __remplace(int value, char *message, int tag);
static void __destructor(T *self);
static int __estimateMessageSize(T *self, int pairsLength);

T *SFIX_client_constructor() {
  /*#region*/
  T *self = malloc(sizeof(T));
  if (!self) {
    perror("malloc");
    return NULL;
  }
  self->parse = __parse;
  self->destructor = __destructor;
  self->compose = __compose;
  self->print = __print;
  self->acknowledge = __acknowledge;
  self->unknown = __unknown;
  self->composeInnerMessage = __composeInnerMessage;
  self->concatInnerMessages = __concatInnerMessages;
  self->isList = __isList;
  self->estimateMessageSize = __estimateMessageSize;
  return self;
  /*#endregion*/
}

static void __destructor(T *self) {
  /*#region*/
  free(self);
  /*#endregion*/
}

static int __estimateMessageSize(T *self, int pairsLength) {
  /*#region*/
  (void)self;
  // +2 for (=,EOF)
  int singlePairSize = SFIX_MAX_TAG_LEN + SFIX_MAX_VALUE_LEN + 2;
  // +1 for EOL
  return pairsLength * singlePairSize + 1;
  /*#endregion*/
}

static int __append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]) {
  /*#region*/
  char temp[SFIX_TAGSNUM];
  int writtenChars =
      snprintf(temp, sizeof(temp), "%d=%s%s", tag, value, delimiter);
  if (writtenChars > SFIX_TAGSNUM) {
    perror("sprintf");
    exit(1);
  }
  strcat(buffer, temp);
  return writtenChars;
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

static char *__compose(T *self, char type, SFIX_Pair pairs[], int pairsLength,
                       int *out_length) {
  /*#region*/
  (void)(self);
  int msg_size = 0;
  // TODO: allocate memory on the stack instead
  char *fix_message = malloc(sizeof(char) * 1024);
  memset(fix_message, 0, sizeof(char) * 1024);
  msg_size += __append_tag(fix_message, 8, PROTOCOL, SOH);
  // Body Length (placeholder, will be replaced later)
  msg_size += __append_tag(fix_message, 9, "000", SOH);

  char temp[6];
  msg_size += snprintf(temp, sizeof(temp), "35=%c%s", type, SOH);
  strcat(fix_message, temp);

  for (int i = 0; i < pairsLength; i++) {
    SFIX_Pair p = pairs[i];
    msg_size += __append_tag(fix_message, p.key, p.value, SOH);
  }
  // CheckSum (placeholder, will be replaced later)
  msg_size += __append_tag(fix_message, 10, "000", SOH);
  msg_size++; // count null-terminate char

  int body_length = __body_length(fix_message);
  __remplace(body_length, fix_message, 9);

  int checksum = __checksum(fix_message);
  __remplace(checksum, fix_message, 10);

  *out_length = msg_size;
  return fix_message;
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
  puts("here");
  int e = 0;
  int list_len = __isList(self, message);
  e = _$parse(self, message, out, SOH);
  if (e) {
    puts("here1");
    return 1;
  }

  if (list_len > 1) {
    char *list = out[SFIX_Tag_List].value;
    SFIX_KeyValue messages[SFIX_TAGSNUM] = {0};
    e = _$parse(self, list, messages, ETX);
    if (e) {
      puts("here2");
      return 1;
    }

    for (int i = 0; i < list_len; i++) {
      char *innerMessage = messages[i].value;
      int index = (256 * i);
      out += index;
      e = _$parse(self, innerMessage, out, STX);
      if (e) {
        puts("here3");
        return 1;
      }
    }
  } else {
    puts("here4");
    return 0;
  }
  puts("here5");
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

static char *__acknowledge(T *self, int *out_length) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  int length;
  char *msg = __compose(self, 'a', pairs, 0, &length);
  *out_length = length;
  return msg;
  /*#endregion*/
}

static char *__unknown(T *self, int *out_length) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  int length;
  char *msg = __compose(self, 'u', pairs, 0, &length);
  *out_length = length;
  return msg;
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
