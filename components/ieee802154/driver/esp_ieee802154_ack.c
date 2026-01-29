/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "hal/ieee802154_ll.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_ieee802154_ack.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_pib.h"
#include "esp_ieee802154_types.h"
#include "esp_ieee802154_util.h"

static ieee802154_pending_table_t ieee802154_pending_table[CONFIG_IEEE802154_INTERFACE_NUM];

#define GET_MASK_ITEM_FROM_TABLE(mask, pos) (mask[(pos) / IEEE802154_PENDING_TABLE_MASK_BITS])

#define BIT_SET(mask, pos) (GET_MASK_ITEM_FROM_TABLE(mask, pos) |= (1UL << (pos % IEEE802154_PENDING_TABLE_MASK_BITS)))
#define BIT_CLR(mask, pos) (GET_MASK_ITEM_FROM_TABLE(mask, pos) &= ~(1UL << (pos % IEEE802154_PENDING_TABLE_MASK_BITS)))
#define BIT_IST(mask, pos) (GET_MASK_ITEM_FROM_TABLE(mask, pos) & (1UL << (pos % IEEE802154_PENDING_TABLE_MASK_BITS)))

static IRAM_ATTR bool ieee802154_addr_in_pending_table(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short)
{
    bool ret = false;
    if (is_short) {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (BIT_IST(ieee802154_pending_table[inf_index].short_addr_mask, index) &&
                    memcmp(addr, ieee802154_pending_table[inf_index].short_addr[index], IEEE802154_FRAME_SHORT_ADDR_SIZE) == 0) {
                ret = true;
                break;
            }
        }
    } else {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (BIT_IST(ieee802154_pending_table[inf_index].ext_addr_mask, index) &&
                    memcmp(addr, ieee802154_pending_table[inf_index].ext_addr[index], IEEE802154_FRAME_EXT_ADDR_SIZE) == 0) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

esp_err_t ieee802154_add_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short)
{
    esp_err_t ret = ESP_FAIL;
    int8_t first_empty_index = -1;
    if (is_short) {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (!BIT_IST(ieee802154_pending_table[inf_index].short_addr_mask, index)) {
                // record the first empty index
                first_empty_index = (first_empty_index == -1 ? index : first_empty_index);
            } else if (memcmp(addr, ieee802154_pending_table[inf_index].short_addr[index], IEEE802154_FRAME_SHORT_ADDR_SIZE) == 0) {
                // The address is in the table already.
                ret = ESP_OK;
                return ret;
            }
        }
        if (first_empty_index != -1) {
            memcpy(ieee802154_pending_table[inf_index].short_addr[first_empty_index], addr, IEEE802154_FRAME_SHORT_ADDR_SIZE);
            BIT_SET(ieee802154_pending_table[inf_index].short_addr_mask, first_empty_index);
            ret = ESP_OK;
        }
    } else {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (!BIT_IST(ieee802154_pending_table[inf_index].ext_addr_mask, index)) {
                first_empty_index = (first_empty_index == -1 ? index : first_empty_index);
            } else if (memcmp(addr, ieee802154_pending_table[inf_index].ext_addr[index], IEEE802154_FRAME_EXT_ADDR_SIZE) == 0) {
                // The address is already in the pending table.
                ret = ESP_OK;
                return ret;
            }
        }
        if (first_empty_index != -1) {
            memcpy(ieee802154_pending_table[inf_index].ext_addr[first_empty_index], addr, IEEE802154_FRAME_EXT_ADDR_SIZE);
            BIT_SET(ieee802154_pending_table[inf_index].ext_addr_mask, first_empty_index);
            ret = ESP_OK;
        }
    }
    return ret;
}

esp_err_t ieee802154_clear_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short)
{
    esp_err_t ret = ESP_FAIL;
    // Consider this function may be called in ISR, only clear the mask bits for finishing the process quickly.
    if (is_short) {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (BIT_IST(ieee802154_pending_table[inf_index].short_addr_mask, index) &&
                    memcmp(addr, ieee802154_pending_table[inf_index].short_addr[index], IEEE802154_FRAME_SHORT_ADDR_SIZE) == 0) {
                BIT_CLR(ieee802154_pending_table[inf_index].short_addr_mask, index);
                ret = ESP_OK;
                break;
            }
        }
    } else {
        for (uint8_t index = 0; index < CONFIG_IEEE802154_PENDING_TABLE_SIZE; index++) {
            if (BIT_IST(ieee802154_pending_table[inf_index].ext_addr_mask, index) &&
                    memcmp(addr, ieee802154_pending_table[inf_index].ext_addr[index], IEEE802154_FRAME_EXT_ADDR_SIZE) == 0) {
                BIT_CLR(ieee802154_pending_table[inf_index].ext_addr_mask, index);
                ret = ESP_OK;
                break;
            }
        }
    }

    return ret;
}

void ieee802154_reset_pending_table(esp_ieee802154_multipan_index_t inf_index, bool is_short)
{
    // Consider this function may be called in ISR, only clear the mask bits for finishing the process quickly.
    if (is_short) {
        memset(ieee802154_pending_table[inf_index].short_addr_mask, 0, IEEE802154_PENDING_TABLE_MASK_SIZE);
    } else {
        memset(ieee802154_pending_table[inf_index].ext_addr_mask, 0, IEEE802154_PENDING_TABLE_MASK_SIZE);
    }
}

bool ieee802154_ack_config_pending_bit(const uint8_t *frame, const esp_ieee802154_frame_info_t *frame_info)
{
    esp_ieee802154_multipan_index_t inf_index = 0;
#if CONFIG_IEEE802154_MULTI_PAN_ENABLE
    inf_index = frame_info->mpf_index;
    if (inf_index >= ESP_IEEE802154_MULTIPAN_MAX || inf_index < ESP_IEEE802154_MULTIPAN_0) {
        return false;
    }
#endif
    bool pending_bit = false;
    uint8_t addr[IEEE802154_FRAME_EXT_ADDR_SIZE] = {0};
    uint8_t src_mode = 0;
    // Only set the HW pending bit for the frames with version 0b00 or 0b01.
    bool set_to_hw = (ieee802154_frame_get_version(frame) <= IEEE802154_FRAME_VERSION_1);

    // Check SW should check whether the frame is data request or not.
    bool sw_check_data_req = ieee802154_ll_get_pending_mode();

    if (sw_check_data_req) {
        pending_bit = ieee802154_is_data_request(frame);
    }

    ieee802154_ll_pending_mode_t pending_mode = ieee802154_pib_get_pending_mode(inf_index);

    switch (pending_mode) {
    case IEEE802154_AUTO_PENDING_DISABLE:
        // HW will check whether the frame is data request or not
        pending_bit = true;
        break;
    case IEEE802154_AUTO_PENDING_ENABLE:
    case IEEE802154_AUTO_PENDING_ENHANCED:
        src_mode = ieee802154_frame_get_src_addr(frame, addr);
        if (src_mode == IEEE802154_FRAME_SRC_MODE_SHORT || src_mode == IEEE802154_FRAME_SRC_MODE_EXT) {
            pending_bit = ieee802154_addr_in_pending_table(inf_index, addr, src_mode == IEEE802154_FRAME_SRC_MODE_SHORT);
        }
        break;
    case IEEE802154_AUTO_PENDING_ZIGBEE:
        // If the address type is short and in pending table, set 'pending_bit' false, otherwise set true.
        src_mode = ieee802154_frame_get_src_addr(frame, addr);
        pending_bit = !(src_mode == IEEE802154_FRAME_SRC_MODE_SHORT && ieee802154_addr_in_pending_table(inf_index, addr, src_mode == IEEE802154_FRAME_SRC_MODE_SHORT));
        break;
    default:
        IEEE802154_ASSERT(false);
    }

    if (set_to_hw) {
        ieee802154_ll_set_pending_bit(pending_bit);
    }

    return pending_bit;
}
