/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/efuse_struct.h"

#define ESP_SOC_GET_CHIP_REV    (EFUSE.rd_mac_sys_3.wafer_version_major * 100 + EFUSE.rd_mac_sys_3.wafer_version_minor)

#define REG_COMPATIBLE_ADDR(rev, before_addr, after_addr)   ((ESP_SOC_GET_CHIP_REV >= (rev)) ? (after_addr) : (before_addr))
