// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_EDDYSTONE_PROTOCOL_H__
#define __ESP_EDDYSTONE_PROTOCOL_H__

#include "stdbool.h"
#include "stdint.h"

/* Eddystone definitions */
#define EDDYSTONE_SERVICE_UUID          0xFEAA

#define EDDYSTONE_FRAME_TYPE_UID        0x00
#define EDDYSTONE_FRAME_TYPE_URL        0x10
#define EDDYSTONE_FRAME_TYPE_TLM        0x20
#define EDDYSTONE_FRAME_TYPE_EID        0x30

#define EDDYSTONE_UID_FRAME_LEN         0x17
#define EDDYSTONE_TLM_FRAME_LEN         0x11
#define EDDYSTONE_URL_MAX_LEN           18


/* Eddystone UID frame */
typedef struct {
    int8_t    ranging_data;     /*<! calibrated Tx power at 0m */
    uint8_t   namespace_id[10];
    uint8_t   instance_id[6];
    uint8_t   reserved[2];
} __attribute__((packed))esp_eddystone_uid_t;

/* Eddystone URL frame */
typedef struct { 
    int8_t    tx_power;         /*<! calibrated Tx power at 0m */    
    uint8_t   url_scheme;       /*<! encoded scheme prefix */
    uint8_t   encoded_url[0];   /*<! length 1-17 */
} __attribute__((packed))esp_eddystone_url_t;

/* Eddystone TLM frame */
typedef struct {
    uint8_t    version;        /*<! TLM version,0x00 for now */
    uint16_t   batt;           /*<! battery voltage, 1mV/bit */
    uint16_t   temp;           /*<! beacon temperature */
    uint32_t   adv_count;      /*<! adv pdu count since power-on or reboot */
    uint32_t   time;           /*<! time sence power-on or reboot, a 0.1 second resolution counter */
} __attribute__((packed)) esp_eddystone_tlm_t;

/*  AD Structure of flags */
typedef struct {
    uint8_t     len;
    uint8_t     type;
    uint8_t     flags;
} __attribute__((packed)) esp_eddystone_flags_t;

/* AD Structure of complete 16-bit service uuid */
typedef struct {
    uint8_t     len;
    uint8_t     type;
    uint16_t    uuid;       /*<! complete list of 16-bit service UUIDs data type value */
} __attribute__((packed)) esp_eddystone_uuid_t;

/* AD Structure of eddystone frame*/
typedef struct {
    uint8_t     len;        /*<! length of eddystone data */
    uint8_t     type;       /*<! service data type,must be 0x16 */
    uint16_t    uuid;       /*<! 16-bit eddystone uuid */
    uint8_t     frame_type;
    union {
        esp_eddystone_uid_t     uid;
        esp_eddystone_url_t     url;
        esp_eddystone_tlm_t     tlm;
    } u[0];
} __attribute__((packed)) esp_eddystone_frame_t;

/* eddystone packet type */
typedef struct {
    esp_eddystone_flags_t   flags;
    esp_eddystone_uuid_t    uuid;
    esp_eddystone_frame_t   frame;
} __attribute__((packed)) esp_eddystone_packet_t;

/* 
 * URLs are written only with the graphic printable characters of the US-ASCII coded character set. 
 * The octets 00-20 and 7F-FF hexadecimal are not used. 
 * See “Excluded US-ASCII Characters” in RFC 2936.
 *
 */
static inline bool esp_eddystone_is_char_invalid(int ch)
{
    return (ch >= 0x00 && ch <= 0x20) || (ch >= 0x7f && ch <= 0xff);
}

#endif /* __ESP_EDDYSTONE_PROTOCOL_H__ */
