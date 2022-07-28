/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include "esp_gdbstub_common.h"

// GDB command input buffer
static unsigned char s_cmd[GDBSTUB_CMD_BUFLEN];

// Running checksum of the output packet
static char s_chsum;

// Send the start of a packet; reset checksum calculation.
void esp_gdbstub_send_start(void)
{
    s_chsum = 0;
    esp_gdbstub_putchar('$');
}

// Send a char as part of a packet
void esp_gdbstub_send_char(char c)
{
    if (c == '#' || c == '$' || c == '}' || c == '*') {
        esp_gdbstub_putchar('}');
        esp_gdbstub_putchar(c ^ 0x20);
        s_chsum += (c ^ 0x20) + '}';
    } else {
        esp_gdbstub_putchar(c);
        s_chsum += c;
    }
}

// Send a string as part of a packet
void esp_gdbstub_send_str(const char *c)
{
    while (*c != 0) {
        esp_gdbstub_send_char(*c);
        c++;
    }
}

// Send a hex val as part of a packet.
// 'bits'/4 dictates the number of hex chars sent.
void esp_gdbstub_send_hex(int val, int bits)
{
    // sanity check, in case the following (i - 4) is a negative value
    assert(bits >= 4);
    const char *hex_chars = "0123456789abcdef";
    for (int i = bits; i > 0; i -= 4) {
        esp_gdbstub_send_char(hex_chars[(val >> (i - 4)) & 0xf]);
    }
}

// Finish sending a packet.
void esp_gdbstub_send_end(void)
{
    esp_gdbstub_putchar('#');
    esp_gdbstub_send_hex(s_chsum, 8);
    esp_gdbstub_flush();
}

// Send a packet with a string as content
void esp_gdbstub_send_str_packet(const char *str)
{
    esp_gdbstub_send_start();
    if (str != NULL) {
        esp_gdbstub_send_str(str);
    }
    esp_gdbstub_send_end();
}

// Grab a hex value from the gdb packet. Ptr will get positioned on the end
// of the hex string, as far as the routine has read into it. Bits/4 indicates
// the max amount of hex chars it gobbles up. Bits can be -1 to eat up as much
// hex chars as possible.
uint32_t esp_gdbstub_gethex(const unsigned char **ptr, int bits)
{
    int i;
    int no;
    uint32_t v = 0;
    char c;
    no = bits / 4;
    if (bits == -1) {
        no = 64;
    }
    for (i = 0; i < no; i++) {
        c = **ptr;
        (*ptr)++;
        if (c >= '0' && c <= '9') {
            v <<= 4;
            v |= (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            v <<= 4;
            v |= (c - 'A') + 10;
        } else if (c >= 'a' && c <= 'f') {
            v <<= 4;
            v |= (c - 'a') + 10;
        } else if (c == '#') {
            if (bits == -1) {
                (*ptr)--;
                return v;
            }
            return GDBSTUB_ST_ENDPACKET;
        } else {
            if (bits == -1) {
                (*ptr)--;
                return v;
            }
            return GDBSTUB_ST_ERR;
        }
    }
    return v;
}


// Lower layer: grab a command packet and check the checksum
// Calls gdbHandleCommand on the packet if the checksum is OK
// Returns GDBSTUB_ST_OK on success, GDBSTUB_ST_ERR when checksum fails, a
// character if it is received instead of the GDB packet
// start char.
int esp_gdbstub_read_command(unsigned char **out_cmd, size_t *out_size)
{
    unsigned char c;
    unsigned char chsum = 0;
    unsigned char sentchs[2];
    int p = 0;
    c = esp_gdbstub_getchar();
    if (c != '$') {
        return c;
    }
    while (1) {
        c = esp_gdbstub_getchar();
        if (c == '#') {
            // end of packet, checksum follows
            s_cmd[p] = 0;
            break;
        }
        chsum += c;
        if (c == '$') {
            // restart packet?
            chsum = 0;
            p = 0;
            continue;
        }
        if (c == '}') {
            //escape the next char
            c = esp_gdbstub_getchar();
            chsum += c;
            c ^= 0x20;
        }
        s_cmd[p++] = c;
        if (p >= GDBSTUB_CMD_BUFLEN) {
            return GDBSTUB_ST_ERR;
        }
    }
    // A # has been received. Get and check the received chsum.
    sentchs[0] = esp_gdbstub_getchar();
    sentchs[1] = esp_gdbstub_getchar();
    const unsigned char *c_ptr = &sentchs[0];
    unsigned char rchsum = esp_gdbstub_gethex(&c_ptr, 8);
    if (rchsum != chsum) {
        esp_gdbstub_putchar('-');
        return GDBSTUB_ST_ERR;
    } else {
        esp_gdbstub_putchar('+');
        *out_cmd = s_cmd;
        *out_size = p;
        return GDBSTUB_ST_OK;
    }
}
