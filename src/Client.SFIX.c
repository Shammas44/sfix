#include "Client.SFIX.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOH "\001" // Regular tag delimiter
#define STX "\002" // Inner tag delimiter
#define ETX "\003" // Inner message delimiter
#define PROTOCOL "SFIX.1"

static int _$append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]);
static int _$body_length(char *fix_message);
static int _$checksum(char *fix_message);
static int _$count_digits(int num);
static int _$parse(char *message, SFIX_KeyValue out[SFIX_TAGSNUM], char *sep);
static void _$remplace(int value, char *message, int tag);

static int _$append_tag(char *buffer, int tag, const char *value,
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

static int _$body_length(char *fix_message) {
  /*#region*/
  char *body_start = strstr(fix_message, "35=") + 5; // Start of Body
  int body_length = strlen(body_start);
  return body_length;
  /*#endregion*/
}

static int _$checksum(char *fix_message) {
  /*#region*/
  int checksum = 0;
  for (char *p = fix_message; *p; p++) {
    checksum += *p;
  }
  checksum %= 256;
  return checksum;
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

static int _$parse(char *message, SFIX_KeyValue out[SFIX_TAGSNUM], char *sep) {
  /*#region*/
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

int SFIX_estimateMessageSize(SFIX_Pair body[], int length) {
  /*#region*/
  int size = 0;
  for (int i = 0; i < length; i++) {
    // +2 for (=,EOF)
    size += strlen(body[i].value) + SFIX_MAX_TAG_LEN + 2;
  }
  // +2 for (=,EOF); *4 for protocol, type, body_length,checksum
  int regularPairSize = (SFIX_MAX_TAG_LEN + SFIX_MAX_VALUE_LEN + 2) * 4;
  // +1 for EOL
  return size + regularPairSize + 1;
  /*#endregion*/
}

int SFIX_compose(char *out, char type, SFIX_Pair pairs[], int pairsLength) {
  /*#region*/
  int msg_size = 0;
  // TODO: allocate memory on the stack instead
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
  // CheckSum (placeholder, will be replaced later)
  msg_size += _$append_tag(out, 10, "000", SOH);
  msg_size++; // count null-terminate char

  int body_length = _$body_length(out);
  _$remplace(body_length, out, 9);

  int checksum = _$checksum(out);
  _$remplace(checksum, out, 10);
  return msg_size;
  /*#endregion*/
}

int SFIX_composeInnerMessage(char *out, int rows, int cols, SFIX_Pair pairs[]) {
  /*#region*/
  int size = 0;
  for (int i = 0; i < rows; i++) {
    char id[4];
    sprintf(id, "%d=", i);
    strcat(out, id);
    for (int j = 0; j < cols; j++) {
      int index = i * cols + j;
      SFIX_Pair p = pairs[index];
      size += _$append_tag(out, p.key, p.value, STX);
    }
    strcat(out, ETX);
  }
  return size + 1;
  /*#endregion*/
}

int SFIX_parse(char *message, SFIX_KeyValue *out) {
  /*#region*/
  int e = 0;
  int list_len = SFIX_isList(message);
  e = _$parse(message, out, SOH);

  if (e) {
    return 1;
  }

  if (list_len >= 1) {
    char *list = out[SFIX_Tag_List].value;
    SFIX_KeyValue messages[SFIX_TAGSNUM] = {0};
    e = _$parse(list, messages, ETX);
    if (e) {
      return 1;
    }

    for (int i = 0; i < list_len; i++) {
      char *innerMessage = messages[i].value;
      int index = (256 * i);
      out += index;
      e = _$parse(innerMessage, out, STX);
      if (e) {
        return 1;
      }
    }
  }
  return 0;
  /*#endregion*/
}

int SFIX_isList(char *message) {
  /*#region*/
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

void SFIX_print(char *message) {
  /*#region*/
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

int SFIX_acknowledge(char out[83]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'A', pairs, 0);
  /*#endregion*/
}

int SFIX_unknown(char out[83]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'U', pairs, 0);
  /*#endregion*/
}
