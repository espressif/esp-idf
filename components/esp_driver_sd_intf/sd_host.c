/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_types.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/sd_host.h"
#include "esp_private/sd_driver_interface.h"

static const char *TAG = "SD_HOST";

#define SD_HOST_ARG_CHECK(arg) do { \
    if (unlikely(!(arg))) { \
        ESP_LOGE(TAG, "invalid argument: null pointer"); \
        return ESP_ERR_INVALID_ARG; \
    } \
} while(0)

#define SD_HOST_FUNC_CHECK(arg) do { \
    if (unlikely(!(arg))) { \
        ESP_LOGE(TAG, "controller driver function not supported"); \
        return ESP_ERR_NOT_SUPPORTED; \
    } \
} while(0)

esp_err_t sd_host_slot_configure(sd_host_slot_handle_t slot, const sd_host_slot_cfg_t *config)
{
    SD_HOST_ARG_CHECK(slot && config);
    SD_HOST_FUNC_CHECK(slot->configure);

    return slot->configure(slot, config);
}

esp_err_t sd_host_slot_do_transaction(sd_host_slot_handle_t slot, sdmmc_command_t *cmdinfo)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->do_transaction);

    return slot->do_transaction(slot, cmdinfo);
}

esp_err_t sd_host_slot_register_event_callbacks(sd_host_slot_handle_t slot, const sd_host_evt_cbs_t *cbs, void *user_data)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->register_cbs);

    return slot->register_cbs(slot, cbs, user_data);
}

esp_err_t sd_host_remove_slot(sd_host_slot_handle_t slot)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->remove_slot);

    return slot->remove_slot(slot);
}

esp_err_t sd_host_del_controller(sd_host_ctlr_handle_t ctlr)
{
    SD_HOST_ARG_CHECK(ctlr);
    SD_HOST_FUNC_CHECK(ctlr->del_ctlr);

    return ctlr->del_ctlr(ctlr);
}

esp_err_t sd_host_slot_set_cclk_always_on(sd_host_slot_handle_t slot, bool always_on)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->set_cclk_always_on);

    return slot->set_cclk_always_on(slot, always_on);
}

esp_err_t sd_host_slot_enable_io_int(sd_host_slot_handle_t slot)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->enable_io_int);

    return slot->enable_io_int(slot);
}

esp_err_t sd_host_slot_wait_io_int(sd_host_slot_handle_t slot, TickType_t timeout_ticks)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->wait_io_int);

    return slot->wait_io_int(slot, timeout_ticks);
}

esp_err_t sd_host_slot_get_info(sd_host_slot_handle_t slot, sd_host_slot_info_t *info)
{
    SD_HOST_ARG_CHECK(slot);
    SD_HOST_FUNC_CHECK(slot->get_info);

    return slot->get_info(slot, info);
}
