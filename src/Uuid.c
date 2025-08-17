#include "Uuid.SFIX.h"
#include <uuid/uuid.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define T SFIX_Uuid

static bool __compare(T self, T b);
static void __get(T self, char buffer[SFIX_UUID_LENGTH]);

T SFIX_uuid_constructor(void) {
  /* #region*/
  T self = {};
  SFIX_uuid_by_custom_length_string((char*)self.__value, sizeof(self.__value), SFIX_UUID_LENGTH -1);
  self.compare = __compare;
  self.get = __get;
  return self;
  /* #endregion*/
}

T SFIX_uuid_by_string_constructor(char id[SFIX_UUID_LENGTH]) {
  /* #region*/
  T self = {};
  snprintf((char*)self.__value, SFIX_UUID_LENGTH, "%s", id );
  self.compare = __compare;
  self.get = __get;
  return self;
  /* #endregion*/
}

static bool __compare(T self, T b) {
  /* #region*/
  return strcmp(self.__value, b.__value) == 0;
  /* #endregion*/
}

static void __get(T self, char buffer[SFIX_UUID_LENGTH]) {
  /* #region*/
  memset(buffer, 0x00, SFIX_UUID_LENGTH);
  memcpy(buffer ,self.__value, sizeof(self.__value));
  /* #endregion*/
}

/**
 * @brief Generates a unique identifier string of a specified length without
 * using heap memory.
 *
 * This function constructs a unique ID by concatenating the string
 * representations of multiple standard UUIDs (each 36 characters long) until
 * the desired length is met or exceeded. The final string is then truncated to
 * the exact desired_length and null-terminated.
 *
 * All internal buffers are allocated on the stack. The caller must provide
 * a buffer large enough to hold the desired_length characters plus a null
 * terminator.
 *
 * @param buffer A pointer to a character array (buffer) where the custom length
 * UUID string will be stored. This buffer MUST be at least
 * (desired_length + 1) bytes long.
 * @param buffer_size The total size of the provided buffer in bytes. Used for
 * safety.
 * @param desired_length The exact number of characters the generated UUID
 * string should have (excluding the null terminator).
 */
void SFIX_uuid_by_custom_length_string(char *buffer,
                                                 size_t buffer_size,
                                                 size_t desired_length) {
  /*#region*/
  // 1. Input Validation
  if (buffer == NULL) {
    fprintf(
        stderr,
        "Error: SFIX_uuid_by_custom_length_string received NULL buffer.\n");
    return;
  }
  if (desired_length == 0) {
    if (buffer_size > 0) {
      buffer[0] = '\0'; // Ensure it's an empty string
    }
    return;
  }
  if (buffer_size < (desired_length + 1)) {
    fprintf(stderr,
            "Error: Buffer too small (%zu bytes provided, need at least %zu) "
            "for desired length %zu.\n",
            buffer_size, desired_length + 1, desired_length);
    if (buffer_size > 0) {
      buffer[0] = '\0'; // Ensure buffer is an empty string if invalid
    }
    return;
  }

  // Initialize the output buffer to ensure it's clean
  memset(buffer, 0, buffer_size);

  size_t current_len = 0;
  char temp_uuid_str[37]; // Buffer for a single standard UUID string (36 chars
                          // + null)
  uuid_t b_uuid;          // Binary representation of a UUID (16 bytes)

  // 2. Loop for concatenation and truncation
  while (current_len < desired_length) {
    // Generate a standard UUID
    uuid_generate(b_uuid);
    uuid_unparse_lower(b_uuid, temp_uuid_str); // Convert to lowercase string

    // Calculate how many characters to copy from the current UUID string
    size_t chars_to_copy = desired_length - current_len;
    if (chars_to_copy > 36) { // A standard UUID string is 36 chars long
      chars_to_copy = 36;
    }

    // Safely append the characters to the main buffer
    // snprintf is used to append and ensure null termination within bounds
    // The '%s' format specifier with a precision (e.g., '%.*s') allows
    // printing a specific number of characters from a string.
    // We need to calculate the remaining space in the buffer for snprintf.
    size_t remaining_buffer_space = buffer_size - current_len;
    if (remaining_buffer_space == 0) { // Should not happen if buffer_size check
                                       // is correct, but for safety
      break;
    }

    // Use snprintf to append, ensuring we don't write beyond buffer_size
    // and that it's always null-terminated.
    // The 'chars_to_copy' parameter ensures we only take the needed part of
    // temp_uuid_str.
    int written = snprintf(buffer + current_len, remaining_buffer_space, "%.*s",
                           (int)chars_to_copy, temp_uuid_str);

    if (written < 0 || (size_t)written >= remaining_buffer_space) {
      // Error or truncation occurred that wasn't intended by desired_length
      fprintf(stderr, "Warning: snprintf error or unexpected truncation during "
                      "UUID generation.\n");
      break; // Exit loop on error
    }

    current_len += written;
  }

  // 3. Final Null-termination (redundant if snprintf worked, but good practice)
  if (current_len < buffer_size) {
    buffer[current_len] = '\0';
  } else if (buffer_size > 0) {
    // If buffer_size was exactly desired_length + 1, ensure the last byte is
    // null
    buffer[buffer_size - 1] = '\0';
  }
  /*#endregion*/
}
