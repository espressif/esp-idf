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
#include "light_driver.h"

/* Zigbee configuration */
#define IEEE_CHANNEL_MASK               (1l << 13)  /* Zigbee default setting is channel 13 for light example usage */
#define ERASE_PERSISTENT_CONFIG         ZB_TRUE     /* erase network devices before running example */

/* groups cluster attributes */
typedef struct {
    zb_uint8_t name_support;
} zb_zcl_groups_attrs_t;

/* scene cluster attributes */
typedef struct {
    zb_uint8_t  scene_count;
    zb_uint8_t  current_scene;
    zb_uint8_t  scene_valid;
    zb_uint8_t  name_support;
    zb_uint16_t current_group;
} zb_zcl_scenes_attrs_t;

/* light bulb device cluster attributes */
typedef struct {
    zb_zcl_basic_attrs_t            basic_attr;
    zb_zcl_identify_attrs_t         identify_attr;
    zb_zcl_groups_attrs_t           groups_attr;
    zb_zcl_scenes_attrs_t           scenes_attr;
    zb_zcl_on_off_attrs_t           on_off_attr;
} bulb_device_ctx_t;

#define HA_ESP_LIGHT_ENDPOINT        10    /* esp light bulb device endpoint, used to process light controlling commands */
#define ZB_ESP_DEFAULT_RADIO_CONFIG()                           \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#define ZB_ESP_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,      \
    }
