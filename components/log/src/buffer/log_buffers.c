/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_private/log_util.h"

#ifndef CONFIG_IDF_TARGET_LINUX
#include "esp_memory_utils.h"  // for esp_ptr_byte_accessible
#else // !CONFIG_IDF_TARGET_LINUX
static inline bool esp_ptr_byte_accessible(const void* ptr)
{
    (void) ptr;
    return true;
}
#endif // !CONFIG_IDF_TARGET_LINUX

/* It represents the number of bytes printed per line when displaying the
 * contents of a buffer in hexadecimal format. It determines how many bytes of
 * the buffer will be shown on each line, making it easier to read and interpret
 * the data. */
#define BYTES_PER_LINE 16

/* Checks if a character is a printable ASCII character.
 * If the provided character falls within the printable ASCII range,
 * which includes characters with ASCII values from 32 (space) to 126 (tilde). */
#define IS_CHAR_PRINTABLE(character) ((character) >= 32 && (character) <= 126)

typedef void (*print_line_t)(uintptr_t, const void *, char *, int);

static void log_buffer_hex_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len);
static void log_buffer_char_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len);
static void log_buffer_hexdump_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len);
static void print_buffer(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level, char *output_str, print_line_t print_line_func);

void esp_log_buffer_hex_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level)
{
    // I (954) log_example: 54 68 65 20 77 61 79 20 74 6f 20 67 65 74 20 73
    // I (962) log_example: 74 61 72 74 65 64 20 69 73 20 74 6f 20 71 75 69
    //                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    char output_str[3 * BYTES_PER_LINE];
    print_buffer(tag, buffer, buff_len, log_level, output_str, log_buffer_hex_line);
}

void esp_log_buffer_char_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level)
{
    // I (980) log_example: The way to get s
    // I (985) log_example: tarted is to qui
    //                      ^^^^^^^^^^^^^^^^^
    char output_str[BYTES_PER_LINE + 1];
    print_buffer(tag, buffer, buff_len, log_level, output_str, log_buffer_char_line);
}

void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level)
{
    // I (1013) log_example: 0x3ffb5bc0   54 68 65 20 77 61 79 20  74 6f 20 67 65 74 20 73  |The way to get s|
    // I (1024) log_example: 0x3ffb5bd0   74 61 72 74 65 64 20 69  73 20 74 6f 20 71 75 69  |tarted is to qui|
    //                       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    char output_str[(2 + sizeof(void *) * 2) + 3 + (BYTES_PER_LINE * 3) + 2 + (1 + BYTES_PER_LINE + 1) + 1];
    print_buffer(tag, buffer, buff_len, log_level, output_str, log_buffer_hexdump_line);
}

static void print_buffer(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level, char *output_str, print_line_t print_line_func)
{
    if (buff_len == 0) {
        return;
    }
    char temp_buffer[BYTES_PER_LINE + 3]; //for not-byte-accessible memory

    do {
        const char *ptr_line = buffer;
        int bytes_cur_line = (buff_len > BYTES_PER_LINE) ? BYTES_PER_LINE : buff_len;
        if (!esp_ptr_byte_accessible(buffer)) {
            //use memcpy to get around alignment issue
            memcpy(temp_buffer, buffer, (bytes_cur_line + 3) / 4 * 4);
            ptr_line = temp_buffer;
        }

        print_line_func((uintptr_t)buffer, ptr_line, output_str, bytes_cur_line);

        ESP_LOG_LEVEL(log_level, tag, "%s", output_str);
        buffer += bytes_cur_line;
        buff_len -= bytes_cur_line;
    } while (buff_len);
}

static void log_buffer_hex_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len)
{
    (void) orig_buff;
    const unsigned char *ptr = (unsigned char *)ptr_line;
    for (int i = 0; i < buff_len; i++) {
        output_str += esp_log_util_cvt_hex(ptr[i], 2, output_str);
        *output_str++ = ' ';
    }
    *--output_str = 0;
}

static void log_buffer_char_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len)
{
    (void) orig_buff;
    const char *ptr = (char *)ptr_line;
    memcpy(output_str, ptr, buff_len);
    output_str[buff_len] = 0;
}

static void log_buffer_hexdump_line(uintptr_t orig_buff, const void *ptr_line, char *output_str, int buff_len)
{
    const unsigned char *ptr = (unsigned char *)ptr_line;
    *output_str++ = '0';
    *output_str++ = 'x';
    output_str += esp_log_util_cvt_hex(orig_buff, sizeof(uintptr_t), output_str);
    *output_str++ = ' ';
    for (int i = 0; i < BYTES_PER_LINE; i++) {
        if ((i & 7) == 0) {
            *output_str++ = ' ';
        }
        *output_str++ = ' ';
        if (i < buff_len) {
            output_str += esp_log_util_cvt_hex(ptr[i], 2, output_str);
        } else {
            *output_str++ = ' ';
            *output_str++ = ' ';
        }
    }
    *output_str++ = ' ';
    *output_str++ = ' ';
    *output_str++ = '|';
    for (int i = 0; i < buff_len; i++) {
        *output_str++ = IS_CHAR_PRINTABLE(ptr[i]) ? ptr[i] : '.';
    }
    *output_str++ = '|';
    *output_str = 0;
}
