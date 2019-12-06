// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "assert.h"
#include "sdkconfig.h"
#include "esp_efuse_table.h"

const static char *TAG = "efuse";

// Sets a write protection for the whole block.
esp_err_t esp_efuse_set_write_protect(esp_efuse_block_t blk)
{
    if (blk == EFUSE_BLK1) {
       return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_BLK1, 1);
    } else if (blk == EFUSE_BLK2) {
       return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_SYS_DATA_PART1, 1);
    } else if (blk == EFUSE_BLK3) {
       return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_USER_DATA, 1);
    } else if (blk == EFUSE_BLK4) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY0, 1);
    } else if (blk == EFUSE_BLK5) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY1, 1);
    } else if (blk == EFUSE_BLK6) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY2, 1);
    } else if (blk == EFUSE_BLK7) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY3, 1);
    } else if (blk == EFUSE_BLK8) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY4, 1);
    } else if (blk == EFUSE_BLK9) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_KEY5, 1);
    } else if (blk == EFUSE_BLK10) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_SYS_DATA_PART2, 1);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

// read protect for blk.
esp_err_t esp_efuse_set_read_protect(esp_efuse_block_t blk)
{
    if (blk == EFUSE_BLK4) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY0, 1);
    } else if (blk == EFUSE_BLK5) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY1, 1);
    } else if (blk == EFUSE_BLK6) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY2, 1);
    } else if (blk == EFUSE_BLK7) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY3, 1);
    } else if (blk == EFUSE_BLK8) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY4, 1);
    } else if (blk == EFUSE_BLK9) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_KEY5, 1);
    } else if (blk == EFUSE_BLK10) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_SYS_DATA_PART2, 1);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

// get efuse coding_scheme.
esp_efuse_coding_scheme_t esp_efuse_get_coding_scheme(esp_efuse_block_t blk)
{
    esp_efuse_coding_scheme_t scheme;
    if (blk == EFUSE_BLK0) {
        scheme = EFUSE_CODING_SCHEME_NONE;
    } else {
        scheme = EFUSE_CODING_SCHEME_RS;
    }
    ESP_LOGD(TAG, "coding scheme %d", scheme);
    return scheme;
}
