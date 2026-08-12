/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_eth_spec.h"

typedef struct {
    uint8_t da[ETH_ADDR_LEN];
    uint8_t sa[ETH_ADDR_LEN];
    uint16_t ether_type;
} __attribute__((packed)) eth_hdr_t;

typedef struct {
    uint16_t tpid;
    uint16_t tci;
} __attribute__((packed)) eth_vlan_tag_t;

typedef struct {
    uint8_t da[ETH_ADDR_LEN];
    uint8_t sa[ETH_ADDR_LEN];
    eth_vlan_tag_t vlan_tag;
    uint16_t ether_type;
} __attribute__((packed)) eth_vlan_hdr_t;
