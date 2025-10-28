/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "nvs_partition.hpp"    // For nvs::Partition
#include "nvs_flash.h"          // For nvs_sec_cfg_t

namespace nvs {

namespace partition_lookup {

esp_err_t lookup_nvs_partition(const char* label, NVSPartition **p);

esp_err_t lookup_nvs_encrypted_partition(const char* label, nvs_sec_cfg_t* cfg, NVSPartition **p);

} // partition_lookup

} // nvs
