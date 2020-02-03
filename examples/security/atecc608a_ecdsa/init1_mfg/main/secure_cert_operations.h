/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#pragma once
#include "esp_err.h"

esp_err_t secure_cert_get_priv_key(unsigned char * buffer, size_t *len);
esp_err_t secure_cert_get_device_cert(unsigned char * buffer, size_t *len);
esp_err_t secure_cert_get_ca_cert(unsigned char * buffer, size_t *len);

/* Storage Format:
0-4 Bytes: CRC of the secure cert
5th Byte: Magic Bytes
9-12 Bytes: Key/Cert Length
33rd byte onwards: Actual key/cert
[Max size single cert/key -> 4096 Bytes(Including Length & CRC)]

0xd000 - 0x1000 - Offsets
Offsets:
0-4096 - Private Key
4097-8192 - Device Cert
8193 - 12288 - CA Cert

API Usage:

Fetching the private key:
    #include <string.h>
    #include "esp_log.h"
    #include "secure_cert_operations.h"
    uint32_t len;
    if (secure_cert_get_priv_key(NULL, &len) == ESP_OK) {
        unsigned char * buffer = (unsigned char *)calloc(1, len+1);
        secure_cert_get_priv_key(buffer, &len);
        ESP_LOGI("App", "PEM KEY: \nLength: %d\n%s", strlen((char *)buffer), buffer);
        free(buffer);
    }

Fetching the device cert:
    #include <string.h>
    #include "esp_log.h"
    #include "secure_cert_operations.h"
    uint32_t len;
    if (secure_cert_get_device_cert(NULL, &len) == ESP_OK) {
        unsigned char * buffer = (unsigned char *)calloc(1, len+1);
        secure_cert_get_device_cert(buffer, &len);
        ESP_LOGI("App", "Device Cert: \nLength: %d\n%s", strlen((char *)buffer), buffer);
        free(buffer);
    }

Fetching the ca cert:
    #include <string.h>
    #include "esp_log.h"
    #include "secure_cert_operations.h"
    uint32_t len;
    if (secure_cert_get_ca_cert(NULL, &len) == ESP_OK) {
        unsigned char * buffer = (unsigned char *)calloc(1, len+1);
        secure_cert_get_ca_cert(buffer, &len);
        ESP_LOGI("App", "CA Cert: \nLength: %d\n%s", strlen((char *)buffer), buffer);
        free(buffer);
    }
*/