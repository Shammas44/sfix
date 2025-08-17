#include "Client.SFIX.h"
#include "PairConvertor.SFIX.h"
#include "Tag.SFIX.h"
#include <ctype.h>
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

static int __append_tag(char *buffer, int tag, const char *value,
                        char delimiter[2]);
// static bool __is_numeric(const char *str);
static int __body_length(char *fix_message);
static int __parse(const char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
                   const char *sep);
static void __remplace(int value, char *message, int tag);

EMSCRIPTEN_KEEPALIVE
int SFIX_estimateMessageSize(int pairs_length) {
  /*#region*/
  return SFIX_MSG_SIZE(pairs_length);
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
int SFIX_compose(char *out, char type, SFIX_Pair pairs[], int pairsLength) {
  /*#region*/
  int offset = strlen(out);       // Find where to start appending
  char *msg_start = out + offset; // Pointer to start of new message

  int msg_size = 0;

  // Tag 8: Protocol
  msg_size += __append_tag(msg_start + msg_size, 8, PROTOCOL, SOH);

  // Tag 9: Placeholder for body length
  msg_size += __append_tag(msg_start + msg_size, 9, "000", SOH);

  // Tag 35: Message Type
  char temp[16];
  snprintf(temp, sizeof(temp), "35=%c%s", type, SOH);
  strcat(msg_start + msg_size, temp);
  msg_size += strlen(temp);

  // Append user-defined fields
  for (int i = 0; i < pairsLength; i++) {
    SFIX_Pair p = pairs[i];
    msg_size += __append_tag(msg_start + msg_size, p.key, p.value, SOH);
  }

  // End marker (STX)
  strcat(msg_start + msg_size, STX);
  msg_size++; // STX
  msg_size++; // null terminator

  // Compute body length of this message only
  int body_length = __body_length(msg_start);
  __remplace(body_length, msg_start, 9); // only affect current message

  return offset + msg_size; // total buffer size
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
int SFIX_parse(char *message, SFIX_KeyValue *out) {
  /*#region*/
  int count = 0;
  int size = SFIX_listLength(message);

  if (size <= 0) {
    fprintf(stderr, "sfix message contains no message delimiter");
    // LOG_ERROR("sfix message contains no message delimiter");
    return 1; // Invalid input
  }

  // Allocate token array on the stack
  char array[size][SFIX_MSG_SIZE(256)];

  // Safe copy of the input message
  size_t length = strlen(message);
  char copy[length + 1]; // +1 for null terminator
  strcpy(copy, message); // now safe

  // Tokenize the message into fields
  char *p = strtok(copy, STX);
  while (p != NULL && count < size) {
    strncpy(array[count], p, SFIX_MSG_SIZE(256) - 1);
    array[count][SFIX_MSG_SIZE(256) - 1] = '\0'; // Ensure null-termination
    count++;
    p = strtok(NULL, STX);
  }

  // Parse each field into out buffer (256 entries per message)
  for (int i = 0; i < count; i++) {
    SFIX_KeyValue *message_out = out + (256 * i);
    int e = __parse(array[i], message_out, SOH);
    if (e) {
      fprintf(stderr, "Unable to parse sfix message");
      // LOG_ERROR("Unable to parse sfix message");
      return 1;
    }
  }

  return 0;
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
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

EMSCRIPTEN_KEEPALIVE
void SFIX_print(char *message, bool showLabels) {
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
    size_t validPairCount = 0;
    for (int j = 0; j < 256; j++) {
      int index = (i * 256) + j;
      if (strcmp(pairs[index].tag, "")) {
        validPairCount++;
        int tag = atoi(pairs[index].tag);
        char *v = pairs[index].value;
        const char **labels = SFIX_ENUM(tag);
        if (labels != NULL) {
          const char *label = labels[atoi(v)];
          if (label != NULL) {
            strncpy(v, label, 64 - 1);
            v[64 - 1] = '\0'; // ensure null termination
          }
        }
        char *t = showLabels ? SFIX_LABEL(j) : pairs[index].tag;
        char *r = KEY(Reset);
        char *a1 = KEY(Red);
        char *a2 = KEY(Yellow);
        char *a3 = KEY(Green);
        if (validPairCount == 1) {
          printf("  %s\"%s\"%s%s:%s %s\"%s\"%s", a1, t, r, a2, r, a3, v, r);
        } else {
          printf(",\n  %s\"%s\"%s%s:%s %s\"%s\"%s", a1, t, r, a2, r, a3, v, r);
        }
      }
    }
    i == length - 1 ? printf("\n%s}%s\n", s, e) : printf("\n%s}%s,\n", s, e);
  }
  /*#endregion*/
}

// int SFIX_toJson(char *message, bool showLabels, char *out_buffer,
//                 int out_buffer_size) {
//   /*#region*/
//   if (!message || !out_buffer || out_buffer_size <= 0) {
//     if (out_buffer && out_buffer_size > 0) {
//       fprintf(stderr, "{\"error\":\"Invalid input parameters\"}");
//       snprintf(out_buffer, out_buffer_size,
//                "{\"error\":\"Invalid input parameters\"}");
//     }
//     return -1;
//   }

//   memset(out_buffer, 0, out_buffer_size); // Clear the output buffer
//   int current_offset = 0;
//   int remaining_size = out_buffer_size;

//   int length = SFIX_listLength(message);
//   if (length <= 0) {
//     snprintf(out_buffer, out_buffer_size, "{}"); // Empty object if no messages
//     return strlen(out_buffer);
//   }

//   // Allocate SFIX_KeyValue array on the heap for parsing
//   SFIX_KeyValue *pairs =
//       (SFIX_KeyValue *)malloc(sizeof(SFIX_KeyValue) * SFIX_TAGSNUM * length);
//   if (!pairs) {
//     fprintf(stderr, "{\"error\":\"Memory allocation failed\"}");
//     snprintf(out_buffer, out_buffer_size,
//              "{\"error\":\"Memory allocation failed\"}");
//     return -1;
//   }
//   memset(pairs, 0, sizeof(SFIX_KeyValue) * SFIX_TAGSNUM * length);

//   int err = SFIX_parse(message, pairs);
//   if (err) {
//     fprintf(stderr, "{\"error\":\"Unable to parse SFIX message\"}");
//     snprintf(out_buffer, out_buffer_size,
//              "{\"error\":\"Unable to parse SFIX message: %d\"}", err);
//     free(pairs);
//     return -1;
//   }

//   // Start JSON array if multiple messages
//   if (length > 1) {
//     current_offset += snprintf(out_buffer + current_offset,
//                                remaining_size - current_offset, "[");
//     remaining_size = out_buffer_size - current_offset;
//   }

//   for (int i = 0; i < length; i++) {
//     current_offset += snprintf(out_buffer + current_offset,
//                                remaining_size - current_offset, "{");
//     remaining_size = out_buffer_size - current_offset;

//     // Collect valid key-value pairs for the current message
//     typedef struct {
//       const char *tag_str;
//       const char *value_str;
//       bool is_value_numeric; // New field to track if value is numeric
//     } JsonPair;
//     JsonPair current_message_pairs[SFIX_TAGSNUM];
//     int valid_pairs_count = 0;

//     for (int j = 0; j < SFIX_TAGSNUM; j++) {
//       int index = (i * SFIX_TAGSNUM) + j;      // Corrected index calculation
//       if (strcmp(pairs[index].tag, "") != 0) { // If tag is not empty
//         const char *tag_key = pairs[index].tag;
//         char *value_val =
//             pairs[index].value; // Use a mutable copy for label replacement

//         // Apply label logic for value if showLabels is true and SFIX_ENUM
//         // provides labels
//         if (showLabels) {
//           int tag_enum_val =
//               atoi(tag_key); // Convert tag string to int for enum lookup
//           const char **labels = SFIX_ENUM(tag_enum_val);
//           if (labels != NULL) {
//             int value_int =
//                 atoi(value_val); // Convert value string to int for label lookup
//             const char *label = labels[value_int];
//             if (label != NULL) {
//               strncpy(value_val, label, SFIX_MAX_VALUE_LEN - 1);
//               value_val[SFIX_MAX_VALUE_LEN - 1] = '\0';
//             }
//           }
//         }

//         // Apply label logic for tag if showLabels is true and SFIX_LABEL
//         // provides labels
//         const char *final_tag_label =
//             showLabels ? SFIX_LABEL(atoi(tag_key)) : tag_key;
//         if (final_tag_label == NULL) {
//           final_tag_label = tag_key;
//         }

//         current_message_pairs[valid_pairs_count].tag_str = final_tag_label;
//         current_message_pairs[valid_pairs_count].value_str = value_val;
//         current_message_pairs[valid_pairs_count].is_value_numeric =
//             __is_numeric(value_val); // Check if numeric
//         valid_pairs_count++;
//       }
//     }

//     // Write key-value pairs, handling commas and numeric values
//     for (int k = 0; k < valid_pairs_count; k++) {
//       // Check for buffer overflow before writing
//       if (remaining_size <= 0) {
//         fprintf(stderr,
//                 "{\"error\":\"Buffer overflow during JSON generation\"}");
//         snprintf(out_buffer, out_buffer_size,
//                  "{\"error\":\"Buffer overflow during JSON generation\"}");
//         free(pairs);
//         return -1;
//       }

//       // Format based on whether the value is numeric
//       if (current_message_pairs[k].is_value_numeric) {
//         current_offset += snprintf(
//             out_buffer + current_offset, remaining_size - current_offset,
//             "\"%s\":%s%s", // No quotes around %s for numeric value
//             current_message_pairs[k].tag_str,
//             current_message_pairs[k].value_str,
//             (k == valid_pairs_count - 1) ? "" : ",");
//       } else {
//         current_offset += snprintf(out_buffer + current_offset,
//                                    remaining_size - current_offset,
//                                    "\"%s\":\"%s\"%s", // Quotes for string value
//                                    current_message_pairs[k].tag_str,
//                                    current_message_pairs[k].value_str,
//                                    (k == valid_pairs_count - 1) ? "" : ",");
//       }
//       remaining_size = out_buffer_size - current_offset;
//     }

//     if (remaining_size <= 0) { // Final check after writing pairs
//       fprintf(stderr, "{\"error\":\"Buffer overflow during JSON generation\"}");
//       snprintf(out_buffer, out_buffer_size,
//                "{\"error\":\"Buffer overflow during JSON generation\"}");
//       free(pairs);
//       return -1;
//     }

//     current_offset += snprintf(
//         out_buffer + current_offset, remaining_size - current_offset, "}%s",
//         (i == length - 1 && length > 1) ? "" : ((length > 1) ? "," : ""));
//     remaining_size = out_buffer_size - current_offset;
//     if (remaining_size <= 0) { // Final check after closing object
//       fprintf(stderr, "{\"error\":\"Buffer overflow during JSON generation\"}");
//       snprintf(out_buffer, out_buffer_size,
//                "{\"error\":\"Buffer overflow during JSON generation\"}");
//       free(pairs);
//       return -1;
//     }
//   }

//   // End JSON array if multiple messages
//   if (length > 1) {
//     current_offset += snprintf(out_buffer + current_offset,
//                                remaining_size - current_offset, "]");
//     remaining_size = out_buffer_size - current_offset;
//   }

//   free(pairs);           // Free the dynamically allocated pairs array
//   return current_offset; // Return the actual length written
//   /*#endregion*/
// }


EMSCRIPTEN_KEEPALIVE
int SFIX_toJson(char *message, bool showLabels, char *out, size_t outSize) {
  if (!message || !out || outSize == 0) return -1;

  int msgCount = SFIX_listLength(message);
  if (msgCount <= 0 || msgCount > 64) return -2;

  SFIX_KeyValue pairs[256 * msgCount];
  memset(pairs, 0, sizeof(pairs));

  if (SFIX_parse(message, pairs) != 0) return -3;

  size_t offset = 0;
  offset += snprintf(out + offset, outSize - offset, "[");

  for (int i = 0; i < msgCount; i++) {
    if (offset >= outSize - 1) return -4;  // buffer full

    offset += snprintf(out + offset, outSize - offset, "{");

    bool first = true;
    for (int j = 0; j < 256; j++) {
      int idx = i * 256 + j;
      if (pairs[idx].tag[0] == '\0') continue;

      char *tag = pairs[idx].tag;
      char *val = pairs[idx].value;

      // Replace enum value with label if available
      int tagInt = atoi(tag);
      const char **labels = SFIX_ENUM(tagInt);
      if (labels) {
        int valIndex = atoi(val);
        const char *label = labels[valIndex];
        if (label) {
          strncpy(val, label, sizeof(pairs[idx].value) - 1);
          val[sizeof(pairs[idx].value) - 1] = '\0';
        }
      }

      const char *key = showLabels ? SFIX_LABEL(tagInt) : tag;

      // Determine if value is numeric
      bool isNumber = true;
      for (size_t k = 0; val[k]; k++) {
        if (!isdigit(val[k]) && val[k] != '.') {
          isNumber = false;
          break;
        }
      }

      if (!first) {
        offset += snprintf(out + offset, outSize - offset, ",");
      }

      offset += snprintf(out + offset, outSize - offset, "\"%s\":", key);
      if (isNumber) {
        offset += snprintf(out + offset, outSize - offset, "%s", val);
      } else {
        offset += snprintf(out + offset, outSize - offset, "\"%s\"", val);
      }

      if (offset >= outSize - 1) return -5;  // buffer full
      first = false;
    }

    offset += snprintf(out + offset, outSize - offset, "}");
    if (i < msgCount - 1) {
      offset += snprintf(out + offset, outSize - offset, ",");
    }
  }

  offset += snprintf(out + offset, outSize - offset, "]");
  return 0;
}

EMSCRIPTEN_KEEPALIVE
int SFIX_acknowledge(char out[SFIX_MSG_SIZE(0)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'A', pairs, 0);
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
int SFIX_strategy(const char strategy_name[64], char out[SFIX_MSG_SIZE(1)]) {
  /*#region*/
  SFIX_Pair pairs[1] = {0};
  int i = 0;
  SFIX_string_to_string(strategy_name, SFIX_Tag_Strategy, &pairs[i++]);
  return SFIX_compose(out, 'T', pairs, 1);
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
int SFIX_unknown(char out[SFIX_MSG_SIZE(0)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_TAGSNUM] = {0};
  return SFIX_compose(out, 'U', pairs, 0);
  /*#endregion*/
}

EMSCRIPTEN_KEEPALIVE
int SFIX_error(int status, const char description[64],
               char out[SFIX_MSG_SIZE(2)]) {
  /*#region*/
  SFIX_Pair pairs[2] = {0};
  int i = 0;
  SFIX_int_to_string(status, SFIX_Tag_Status, &pairs[i++]);
  SFIX_string_to_string(description, SFIX_Tag_Description, &pairs[i++]);
  return SFIX_compose(out, 'E', pairs, 2);
  /*#endregion*/
}

static int __append_tag(char *buffer, int tag, const char *value,
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

static int __body_length(char *fix_message) {
  /*#region*/
  char *body_start = strstr(fix_message, "35=") + 5; // Start of Body
  int body_length = strlen(body_start) - 1;          // -1 for STX
  return body_length;
  /*#endregion*/
}

static int __parse(const char *message, SFIX_KeyValue out[SFIX_TAGSNUM],
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

// Replace the value of a FIX tag in `out` with the given integer value (no
// leading zeros)
static void __remplace(int value, char *out, int tag) {
  /*#region*/
  char search[16];
  snprintf(search, sizeof(search), "%d=", tag);

  char *start = strstr(out, search);
  if (!start)
    return;

  char *value_start = start + strlen(search);
  char *value_end = strchr(value_start, '\001'); // SOH is ASCII 1

  if (!value_end)
    return;

  // Construct the new tag=value␁ string
  char replacement[32];
  snprintf(replacement, sizeof(replacement), "%d=%d%c", tag, value,
           1); // 1 is SOH char

  // Calculate lengths
  int old_len = value_end - start + 1; // include SOH
  int new_len = strlen(replacement);
  int tail_len = strlen(value_end + 1);

  // If new value is shorter, move tail left
  // If longer, move tail right to make room
  if (new_len != old_len) {
    memmove(start + new_len, value_end + 1, tail_len + 1); // move + null
  }

  // Copy new tag=value␁ into place
  memcpy(start, replacement, new_len);
  /*#endregion*/
}

// static bool __is_numeric(const char *str) {
//   /*#region*/
//   if (str == NULL || *str == '\0') {
//     return false;
//   }

//   char *endptr;
//   // Try converting to double (handles both integers and floats)
//   strtod(str, &endptr);

//   // Check if the entire string was consumed and there are no non-whitespace
//   // characters left
//   while (isspace((unsigned char)*endptr)) {
//     endptr++;
//   }
//   return (*endptr == '\0');
//   /*#endregion*/
// }
