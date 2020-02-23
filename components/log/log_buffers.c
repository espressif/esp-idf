// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "esp_log.h"
#include "soc/soc_memory_layout.h"  // for esp_ptr_byte_accessible


//print number of bytes per line for esp_log_buffer_char and esp_log_buffer_hex
#define BYTES_PER_LINE 16

void esp_log_buffer_hex_internal(const char *tag, const void *buffer, uint16_t buff_len,
                                 esp_log_level_t log_level)
{
    if (buff_len == 0) {
        return;
    }
    char temp_buffer[BYTES_PER_LINE + 3]; //for not-byte-accessible memory
    char hex_buffer[3 * BYTES_PER_LINE + 1];
    const char *ptr_line;
    int bytes_cur_line;

    do {
        if (buff_len > BYTES_PER_LINE) {
            bytes_cur_line = BYTES_PER_LINE;
        } else {
            bytes_cur_line = buff_len;
        }
        if (!esp_ptr_byte_accessible(buffer)) {
            //use memcpy to get around alignment issue
            memcpy(temp_buffer, buffer, (bytes_cur_line + 3) / 4 * 4);
            ptr_line = temp_buffer;
        } else {
            ptr_line = buffer;
        }

        for (int i = 0; i < bytes_cur_line; i ++) {
            sprintf(hex_buffer + 3 * i, "%02x ", ptr_line[i]);
        }
        ESP_LOG_LEVEL(log_level, tag, "%s", hex_buffer);
        buffer += bytes_cur_line;
        buff_len -= bytes_cur_line;
    } while (buff_len);
}

void esp_log_buffer_char_internal(const char *tag, const void *buffer, uint16_t buff_len,
                                  esp_log_level_t log_level)
{
    if (buff_len == 0) {
        return;
    }
    char temp_buffer[BYTES_PER_LINE + 3]; //for not-byte-accessible memory
    char char_buffer[BYTES_PER_LINE + 1];
    const char *ptr_line;
    int bytes_cur_line;

    do {
        if (buff_len > BYTES_PER_LINE) {
            bytes_cur_line = BYTES_PER_LINE;
        } else {
            bytes_cur_line = buff_len;
        }
        if (!esp_ptr_byte_accessible(buffer)) {
            //use memcpy to get around alignment issue
            memcpy(temp_buffer, buffer, (bytes_cur_line + 3) / 4 * 4);
            ptr_line = temp_buffer;
        } else {
            ptr_line = buffer;
        }

        for (int i = 0; i < bytes_cur_line; i ++) {
            sprintf(char_buffer + i, "%c", ptr_line[i]);
        }
        ESP_LOG_LEVEL(log_level, tag, "%s", char_buffer);
        buffer += bytes_cur_line;
        buff_len -= bytes_cur_line;
    } while (buff_len);
}

void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, esp_log_level_t log_level)
{

    if (buff_len == 0) {
        return;
    }
    char temp_buffer[BYTES_PER_LINE + 3]; //for not-byte-accessible memory
    const char *ptr_line;
    //format: field[length]
    // ADDR[10]+"   "+DATA_HEX[8*3]+" "+DATA_HEX[8*3]+"  |"+DATA_CHAR[8]+"|"
    char hd_buffer[10 + 3 + BYTES_PER_LINE * 3 + 3 + BYTES_PER_LINE + 1 + 1];
    char *ptr_hd;
    int bytes_cur_line;

    do {
        if (buff_len > BYTES_PER_LINE) {
            bytes_cur_line = BYTES_PER_LINE;
        } else {
            bytes_cur_line = buff_len;
        }
        if (!esp_ptr_byte_accessible(buffer)) {
            //use memcpy to get around alignment issue
            memcpy(temp_buffer, buffer, (bytes_cur_line + 3) / 4 * 4);
            ptr_line = temp_buffer;
        } else {
            ptr_line = buffer;
        }
        ptr_hd = hd_buffer;

        ptr_hd += sprintf(ptr_hd, "%p ", buffer);
        for (int i = 0; i < BYTES_PER_LINE; i ++) {
            if ((i & 7) == 0) {
                ptr_hd += sprintf(ptr_hd, " ");
            }
            if (i < bytes_cur_line) {
                ptr_hd += sprintf(ptr_hd, " %02x", ptr_line[i]);
            } else {
                ptr_hd += sprintf(ptr_hd, "   ");
            }
        }
        ptr_hd += sprintf(ptr_hd, "  |");
        for (int i = 0; i < bytes_cur_line; i ++) {
            if (isprint((int)ptr_line[i])) {
                ptr_hd += sprintf(ptr_hd, "%c", ptr_line[i]);
            } else {
                ptr_hd += sprintf(ptr_hd, ".");
            }
        }
        ptr_hd += sprintf(ptr_hd, "|");

        ESP_LOG_LEVEL(log_level, tag, "%s", hd_buffer);
        buffer += bytes_cur_line;
        buff_len -= bytes_cur_line;
    } while (buff_len);
}
