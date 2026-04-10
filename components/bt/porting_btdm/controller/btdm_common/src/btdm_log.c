/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "assert.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
void r_btdm_sched_list_details_dump(void);
void r_ble_phy_hw_state_dump(uint8_t);
void esp_ble_controller_log_dump_all(bool);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

/*
 ***************************************************************************************************
 * Public Function Definitions
 ***************************************************************************************************
 */
void IRAM_ATTR
wr_btdm_log_set_flags(uint8_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_set_buf_index_flag(uint32_t p0);
    r_ble_log_set_buf_index_flag(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_reset_flags(uint8_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_reset_buf_index_flag(uint32_t p0);
    r_ble_log_reset_buf_index_flag(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x0(uint32_t p0)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x0(uint32_t p0);
    r_ble_log_internal_x0(p0);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x1(uint32_t p0, uint32_t p1)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x1(uint32_t p0, uint32_t p1);
    r_ble_log_internal_x1(p0, p1);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x2(uint32_t p0, uint32_t p1, uint32_t p2)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x2(uint32_t p0, uint32_t p1, uint32_t p2);
    r_ble_log_internal_x2(p0, p1, p2);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_x3(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_x3(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);
    r_ble_log_internal_x3(p0, p1, p2, p3);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_internal_hex(uint32_t p0, uint32_t p1, uint32_t p2)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_internal_hex(uint32_t p0, uint32_t p1, uint32_t p2);
    r_ble_log_internal_hex(p0, p1, p2);
#endif // BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_log_raw_export(uint16_t p0, uint32_t p1, uint32_t p2)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    extern void r_ble_log_raw_export(uint16_t p0, uint32_t p1, uint32_t p2);
    r_ble_log_raw_export(p0, p1, p2);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
}

void IRAM_ATTR
wr_btdm_assert(const char *file, int line, int p0, int p1, uint8_t type)
{
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED
    r_ble_phy_hw_state_dump(false);
    r_btdm_sched_list_details_dump();
    esp_ble_controller_log_dump_all(true);
#endif // CONFIG_BT_LE_CONTROLLER_LOG_ENABLED

    esp_rom_printf("assert %s,%d, param:0x%x,0x%x\n", file, line, p0, p1);
    assert(0);
}

/*
 ***************************************************************************************************
 * BTDM LOG Initialization
 ***************************************************************************************************
 */
int
btdm_log_init(void)
{
    return 0;
}

void
btdm_log_deinit(void)
{

}
