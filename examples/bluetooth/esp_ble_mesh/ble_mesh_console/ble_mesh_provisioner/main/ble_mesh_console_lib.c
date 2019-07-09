// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ble_mesh_console_lib.h"

static int hex2num(char c);
static int hex2byte(const char *hex);

static int hex2num(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

static int hex2byte(const char *hex)
{
    int a, b;
    a = hex2num(*hex++);
    if (a < 0) {
        return -1;
    }
    b = hex2num(*hex++);
    if (b < 0) {
        return -1;
    }
    return (a << 4) | b;
}

int hexstr_2_bin(const char *hex, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    int a;
    const char *ipos = hex;
    uint8_t *opos = buf;

    for (i = 0; i < len; i++) {
        a = hex2byte(ipos);
        if (a < 0) {
            return -1;
        }
        *opos ++ = a;
        ipos += 2;
    }
    return 0;
}

int get_value_string(char *value_in, char *buf)
{
    int result = -1;

    uint16_t length = strlen(value_in);

    if (length > 2) {
        if (value_in[0] == '0' && value_in[1] == 'x') {
            buf[(length - 2) / 2] = 0;
            result = hexstr_2_bin(&value_in[2], (uint8_t *)buf, (length - 2) / 2);
            length = (length - 2) / 2;
        } else {
            strcpy(buf, value_in);
            result = 0;
        }
    } else {
        strcpy(buf, value_in);
        result = 0;
    }
    return result;
}

bool str_2_mac(uint8_t *str, uint8_t *dest)
{
    uint8_t loop = 0;
    uint8_t tmp = 0;
    uint8_t *src_p = str;

    if (strlen((char *)src_p) != 17) { // must be like 12:34:56:78:90:AB
        return false;
    }

    for (loop = 0; loop < 17 ; loop++) {
        if (loop % 3 == 2) {
            if (src_p[loop] != ':') {
                return false;
            }

            continue;
        }

        if ((src_p[loop] >= '0') && (src_p[loop] <= '9')) {
            tmp = tmp * 16 + src_p[loop] - '0';
        } else if ((src_p[loop] >= 'A') && (src_p[loop] <= 'F')) {
            tmp = tmp * 16 + src_p[loop] - 'A' + 10;
        } else if ((src_p[loop] >= 'a') && (src_p[loop] <= 'f')) {
            tmp = tmp * 16 + src_p[loop] - 'a' + 10;
        } else {
            return false;
        }

        if (loop % 3 == 1) {
            *dest++ = tmp;
            tmp = 0;
        }
    }

    return true;
}
