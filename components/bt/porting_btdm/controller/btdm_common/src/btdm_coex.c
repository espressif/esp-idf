/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "sdkconfig.h"
#include "btdm_coex.h"

#if CONFIG_SW_COEXIST_ENABLE
#include "private/esp_coexist_internal.h"
#endif

extern esp_err_t coex_iso_start(void *coex_iso_info);
extern esp_err_t coex_iso_stop(uint16_t handle);
extern uint16_t coex_protect_frame_thres_get(void);
extern void coex_start_int_handle(uint16_t handle, uint32_t duration);
extern void coex_end_int_handle(uint16_t handle, uint32_t duration);
extern void coex_ble_idle_time_inform(uint32_t start_offset, uint32_t duration);
extern uint8_t coex_t_ifs_pti_selection_get(void);
extern void coex_ble_channel_map_set(uint8_t *channel_map);
extern esp_err_t coex_register_ble_cb(uint8_t type, void *func);
extern void bt_rf_coex_log_func_register(void *cb);

/*
 ***************************************************************************************************
 * BTDM COEX Adapter Types
 ***************************************************************************************************
 */
typedef void (*coex_schm_cb_t)(uint32_t __event);
typedef void (*coex_wifi_channel_change_btdm_cb_t)(uint8_t primary, uint8_t secondary);
typedef void (*coex_funcs_ble_cb_t)(void);

/*
 ***************************************************************************************************
 * BTDM COEX Adapter Functions
 ***************************************************************************************************
 */
void
wr_btdm_coex_schm_status_bit_set(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_set(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

void
wr_btdm_coex_schm_status_bit_clear(uint32_t type, uint32_t status)
{

#if CONFIG_SW_COEXIST_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif // CONFIG_SW_COEXIST_ENABLE
}

int
wr_btdm_coex_schm_register_btdm_callback(coex_schm_cb_t callback)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_register_callback(COEX_SCHM_CALLBACK_TYPE_BT, callback);
#else
    return 0;
#endif
}

uint32_t
wr_btdm_coex_schm_interval_get(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_interval_get();
#else
    return 0;
#endif
}

uint8_t
wr_btdm_coex_schm_curr_period_get(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_curr_period_get();
#else
    return 1;
#endif
}

void *
wr_btdm_coex_schm_curr_phase_get(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_schm_curr_phase_get();
#else
    return NULL;
#endif
}

int
wr_btdm_coex_register_wifi_channel_change_callback(coex_wifi_channel_change_btdm_cb_t callback)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_register_wifi_channel_change_callback(callback);
#else
    return -1;
#endif
}

int
wr_btdm_coex_wifi_channel_get(uint8_t *primary, uint8_t *secondary)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_wifi_channel_get(primary, secondary);
#else
    return -1;
#endif
}

int
wr_btdm_coex_version_get(unsigned *major, unsigned *minor, unsigned *path)
{
#if CONFIG_SW_COEXIST_ENABLE
    // TODO: enable it after update submodule coexist
#if 0
    coex_version_t version;
    ESP_ERROR_CHECK(coex_version_get_value(&version));
    *major = (unsigned int)version.major;
    *minor = (unsigned int)version.minor;
    *patch = (unsigned int)version.patch;
#endif
    return 0;
#endif
    return -1;
}

int
wr_btdm_coex_iso_start(void *coex_iso_info)
{
#if CONFIG_SW_COEXIST_ENABLE && CONFIG_BT_LE_ISO_SUPPORT
    return coex_iso_start(coex_iso_info);
#else
    return ESP_OK;
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

int
wr_btdm_coex_iso_stop(uint16_t handle)
{
#if CONFIG_SW_COEXIST_ENABLE && CONFIG_BT_LE_ISO_SUPPORT
    return coex_iso_stop(handle);
#else
    return ESP_OK;
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

uint16_t
wr_btdm_coex_iso_protect_frame_thres_get(void)
{
#if CONFIG_SW_COEXIST_ENABLE && CONFIG_BT_LE_ISO_SUPPORT
    return coex_protect_frame_thres_get();
#else
    return 0;
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

void
wr_btdm_coex_iso_start_int_handle(uint16_t handle, uint32_t duration)
{
#if CONFIG_SW_COEXIST_ENABLE && CONFIG_BT_LE_ISO_SUPPORT
    coex_start_int_handle(handle, duration);
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

void
wr_btdm_coex_iso_end_int_handle(uint16_t handle, uint32_t duration)
{
#if CONFIG_SW_COEXIST_ENABLE && CONFIG_BT_LE_ISO_SUPPORT
    coex_end_int_handle(handle, duration);
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

void
wr_btdm_coex_ble_idle_time_inform(uint32_t start_offset, uint32_t duration)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_ble_idle_time_inform(start_offset, duration);
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

int
wr_btdm_coex_register_ble_cb(uint32_t type, coex_funcs_ble_cb_t func)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_register_ble_cb(type, func);
#else
    return ESP_OK;
#endif /* CONFIG_SW_COEXIST_ENABLE */
}

/*
 ***************************************************************************************************
 * BTDM COEX Initialization
 ***************************************************************************************************
 */
esp_err_t
btdm_coex_init(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_init();
#else
    return ESP_OK;
#endif
}

void
btdm_coex_deinit(void)
{

}

esp_err_t
btdm_coex_enable(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    return coex_enable();
#else
    return ESP_OK;
#endif
}

void
btdm_coex_disable(void)
{
#if CONFIG_SW_COEXIST_ENABLE
    coex_disable();
#endif
}
