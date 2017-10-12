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


/****************************************************************************
*
* This file is used to decode eddystone information.
*
****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_gap_ble_api.h"
#include "esp_eddystone_protocol.h"
#include "esp_eddystone_api.h"


/* Declare static functions */
static esp_err_t esp_eddystone_uid_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res);
static esp_err_t esp_eddystone_url_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res);
static char* esp_eddystone_resolve_url_scheme(const uint8_t* url_start, const uint8_t* url_end);
static esp_err_t esp_eddystone_tlm_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res);
static esp_err_t esp_eddystone_get_inform(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res);

/* Eddystone-URL scheme prefixes */
static const char* eddystone_url_prefix[4] = {
    "http://www.",
    "https://www.",
    "http://",
    "https://"
};

/* Eddystone-URL HTTP URL encoding */
static const char* eddystone_url_encoding[14] = {
    ".com/",
    ".org/",
    ".edu/",
    ".net/",
    ".info/",
    ".biz/",
    ".gov/",
    ".com",
    ".org",
    ".edu",
    ".net",
    ".info",
    ".biz",
    ".gov"
 };

/* decode and store received UID */
static esp_err_t esp_eddystone_uid_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res)
{
    uint8_t pos = 0;
    if(len+4 != EDDYSTONE_UID_FRAME_LEN) {
        //ERROR:uid len wrong
        return -1;
    }
    res->inform.uid.ranging_data = buf[pos++];
    for(int i=0; i<10; i++) {
        res->inform.uid.namespace_id[i] = buf[pos++];
    }
    for(int i=0; i<6; i++) {
        res->inform.uid.instance_id[i] = buf[pos++];
    }
    return 0;
}

/* resolve received URL to url_res pointer */
static char* esp_eddystone_resolve_url_scheme(const uint8_t *url_start, const uint8_t *url_end)
{
    int pos = 0;
    static char url_buf[100] = {0};
    const uint8_t *p = url_start;

    pos += sprintf(&url_buf[pos], "%s", eddystone_url_prefix[*p++]);

    for (; p <= url_end; p++) {
        if (esp_eddystone_is_char_invalid((*p))) {
            pos += sprintf(&url_buf[pos], "%s", eddystone_url_encoding[*p]);
        } else {
            pos += sprintf(&url_buf[pos], "%c", *p);
        }
    }
    return url_buf;
}

/* decode and store received URL, the pointer url_res points to the resolved url */
static esp_err_t esp_eddystone_url_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res)
{
    char *url_res = NULL;
    uint8_t pos = 0;
    if(len-1 > EDDYSTONE_URL_MAX_LEN) {
        //ERROR:too long url
        return -1;
    }
    res->inform.url.tx_power = buf[pos++];   
    url_res = esp_eddystone_resolve_url_scheme(buf+pos, buf+len-1);
    memcpy(&res->inform.url.url, url_res, strlen(url_res));
    return 0;
}

/* decode and store received TLM */
static esp_err_t esp_eddystone_tlm_received(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res)
{
    uint8_t pos = 0;
    if(len+4 > EDDYSTONE_TLM_FRAME_LEN) {
        //ERROR:TLM too long
        return -1;
    }
    res->inform.tlm.version = buf[pos++];
    res->inform.tlm.battery_voltage = big_endian_read_16(buf, pos);
    pos += 2;
    uint16_t temp = big_endian_read_16(buf, pos);
    int8_t temp_integral = (int8_t)((temp >> 8) & 0xff);
    float temp_decimal = (temp & 0xff) / 256.0;
    res->inform.tlm.temperature = temp_integral + temp_decimal;
    pos += 2;
    res->inform.tlm.adv_count = big_endian_read_32(buf, pos);
    pos += 4;
    res->inform.tlm.time = big_endian_read_32(buf, pos);
    return 0;
}

static esp_err_t esp_eddystone_get_inform(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res)
{
    static esp_err_t ret=-1;
    switch(res->common.frame_type)
    {
        case EDDYSTONE_FRAME_TYPE_UID: {
            ret = esp_eddystone_uid_received(buf, len, res);
            break;
        }
        case EDDYSTONE_FRAME_TYPE_URL: {
            ret = esp_eddystone_url_received(buf, len, res);
            break;
        }
        case EDDYSTONE_FRAME_TYPE_TLM: {
            ret = esp_eddystone_tlm_received(buf, len, res);
            break;
        }
        default:
            break;
    }
    return ret;
}

esp_err_t esp_eddystone_decode(const uint8_t* buf, uint8_t len, esp_eddystone_result_t* res)
{
    static uint8_t pos=0;
    while(res->common.srv_data_type != EDDYSTONE_SERVICE_UUID) 
    {
        pos++;
        uint8_t ad_type = buf[pos++];
        switch(ad_type) 
        {
            case ESP_BLE_AD_TYPE_FLAG: {
                res->common.flags = buf[pos++];
                break;
            }
            case ESP_BLE_AD_TYPE_16SRV_CMPL: {
                uint16_t uuid = little_endian_read_16(buf, pos);
                if(uuid != EDDYSTONE_SERVICE_UUID) {
                    return -1; 
                }
                res->common.srv_uuid = uuid;
                pos += 2;
                break;
            }
            case ESP_BLE_AD_TYPE_SERVICE_DATA: {
                uint16_t type = little_endian_read_16(buf, pos);
                pos += 2;
                uint8_t frame_type = buf[pos++];
                if(type != EDDYSTONE_SERVICE_UUID || !(frame_type == EDDYSTONE_FRAME_TYPE_UID || frame_type == EDDYSTONE_FRAME_TYPE_URL || 
                   frame_type == EDDYSTONE_FRAME_TYPE_TLM)) {
                    return -1;
                }
                res->common.srv_data_type = type;
                res->common.frame_type = frame_type;
                break;
            }
            default:
                break;
        }
    }
    return esp_eddystone_get_inform(buf+pos, len-pos, res);
}

