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
#include <stdint.h>

#define SECURE_CERT_OFFSET           4096        /* Secure certificate size allocated for each certificate in the partition */
#define SECURE_CERT_PARTITION_TYPE   0x3F        /* Custom partition type */
#define SECURE_CERT_PARTITION_NAME   "sec_cert"  /* Name of the custom secure cert partition */

#define SECURE_CERT_CRC_BYTES         4
#define SECURE_CERT_LEN_BYTES         4
#define SECURE_CERT_LEN_MAGIC_BYTE    1

#define SECURE_CERT_PKEY_MAGIC_BYTE   0xC1   /* Magic byte of the generated private key */
#define SECURE_DEVICE_CERT_MAGIC_BYTE 0xC2   /* Magic byte of the generated device certificate */
#define SECURE_CA_CERT_MAGIC_BYTE     0xC3   /* Magic byte of the CA certificate */

#define METADATA_SIZE                 32     /* 32 bytes are reserved for the metadata (Must be a multiple of 32)*/

typedef struct {
    uint32_t secure_cert_crc;                   /* CRC of only the data of the certificate */
    uint8_t magic_byte;                         /* The magic byte of the cert as mentioned above */
    uint32_t secure_cert_len;                   /* The actual length of the cert[The length before the 32 byte alignment] */
} secure_cert_metadata;                         /* The sizeof struct is 12 bytes, 3 bytes after magic_byte are unused because of struct packing */