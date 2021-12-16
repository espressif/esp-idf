/*
 * Copyright (c) 2021 Espressif Systems (Shanghai) CO LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "zboss_api.h"
#include "zboss_api_zcl.h"
#include "zb_ha.h"
#include "switch_driver.h"

/* Zigbee configuration */
#define IEEE_CHANNEL_MASK               (1l << 13)  /* ZigBee default setting is channel 13 for light example usage */
#define ERASE_PERSISTENT_CONFIG         ZB_TRUE     /* erase network devices before running example  */

/* ZCL configuration */
#define HA_ONOFF_SWITCH_ENDPOINT             1
#define MATCH_DESC_REQ_START_DELAY          (2 * ZB_TIME_ONE_SECOND)            /* time delay between the light switch startup and light finding procedure */
#define MATCH_DESC_REQ_TIMEOUT              (5 * ZB_TIME_ONE_SECOND)            /* timeout for finding bulb */
#define MATCH_DESC_REQ_ROLE                 ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE    /* find non-sleep Zigbee device */

typedef struct light_switch_bulb_params_s {
    zb_uint8_t  endpoint;
    zb_uint16_t short_addr;
} light_switch_bulb_params_t;

/* light switch device cluster attributes */
typedef struct {
    zb_zcl_basic_attrs_t            basic_attr;
    zb_zcl_identify_attrs_t         identify_attr;
    zb_zcl_on_off_attrs_t           on_off_attr;
    light_switch_bulb_params_t      bulb_params;
} switch_device_ctx_t;
#define ZB_ESP_DEFAULT_RADIO_CONFIG()                           \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#define ZB_ESP_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,      \
    }
