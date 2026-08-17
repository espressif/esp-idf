/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_lock.h"
#include "esp_radio_spinel_multipan.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_spinel.h"
#include "esp_openthread_types.h"
#include "esp_timer.h"
#include "esp_vfs_eventfd.h"
#include "esp_zigbee.h"
#include "ezbee/zha.h"
#include "mdns.h"
#include "nvs_flash.h"
#include "ot_examples_common.h"
#include "esp_multipan_config.h"
#if CONFIG_OPENTHREAD_BORDER_ROUTER && CONFIG_OPENTHREAD_NETWORK_AUTO_START
#include "ot_examples_br.h"
#endif
#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
#include "esp_ot_cli_extension.h"
#endif

static const char *TAG = "ot_zb_multipan";

static esp_timer_handle_t s_zb_retry_timer;
static uint8_t s_zb_retry_mode;

static void zb_retry_timer_cb(void *arg)
{
    (void)arg;
    if (esp_zigbee_lock_acquire(portMAX_DELAY)) {
        (void)ezb_bdb_start_top_level_commissioning(s_zb_retry_mode);
        esp_zigbee_lock_release();
    }
}

static void zb_schedule_commissioning(uint8_t mode, uint32_t delay_ms)
{
    s_zb_retry_mode = mode;
    if (s_zb_retry_timer == NULL) {
        const esp_timer_create_args_t args = {
            .callback = zb_retry_timer_cb,
            .name = "zb_retry",
        };
        ESP_ERROR_CHECK(esp_timer_create(&args, &s_zb_retry_timer));
    } else {
        esp_timer_stop(s_zb_retry_timer);
    }
    ESP_ERROR_CHECK(esp_timer_start_once(s_zb_retry_timer, (uint64_t)delay_ms * 1000));
}

static bool zb_app_signal_handler(const ezb_app_signal_t *app_signal)
{
    ezb_app_signal_type_t signal_type = ezb_app_signal_get_type(app_signal);

    switch (signal_type) {
    case EZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Initialize Zigbee stack");
        ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_INITIALIZATION);
        break;
    case EZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case EZB_BDB_SIGNAL_DEVICE_REBOOT: {
        ezb_bdb_comm_status_t status = *((ezb_bdb_comm_status_t *)ezb_app_signal_get_params(app_signal));
        if (status == EZB_BDB_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Zigbee started (%s factory-new)", ezb_bdb_is_factory_new() ? "" : "not");
            if (ezb_bdb_is_factory_new()) {
                ESP_ZIGBEE_ERROR_CHECK(ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_NETWORK_FORMATION));
            } else {
                ESP_ZIGBEE_ERROR_CHECK(ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_NETWORK_STEERING));
            }
        } else {
            ESP_LOGW(TAG, "Zigbee start failed: status(0x%02x)", status);
            zb_schedule_commissioning(EZB_BDB_MODE_INITIALIZATION, 1000);
        }
    } break;
    case EZB_BDB_SIGNAL_FORMATION: {
        ezb_bdb_comm_status_t status = *((ezb_bdb_comm_status_t *)ezb_app_signal_get_params(app_signal));
        if (status == EZB_BDB_STATUS_SUCCESS) {
            ezb_extpanid_t extended_pan_id;
            ezb_nwk_get_extended_panid(&extended_pan_id);
            ESP_LOGI(TAG, "Zigbee network formed: pan_id=0x%04hx ext=0x%llx channel=%d",
                     ezb_nwk_get_panid(), extended_pan_id.u64, ezb_nwk_get_current_channel());
            ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_NETWORK_STEERING);
        } else {
            ESP_LOGW(TAG, "Zigbee formation retry: status(0x%02x)", status);
            zb_schedule_commissioning(EZB_BDB_MODE_NETWORK_FORMATION, 1000);
        }
    } break;
    case EZB_BDB_SIGNAL_STEERING: {
        ezb_bdb_comm_status_t status = *((ezb_bdb_comm_status_t *)ezb_app_signal_get_params(app_signal));
        if (status == EZB_BDB_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Zigbee network steering completed");
        } else {
            ESP_LOGW(TAG, "Zigbee steering failed: status(0x%02x)", status);
        }
    } break;
    case EZB_ZDO_SIGNAL_DEVICE_ANNCE: {
        const ezb_zdo_signal_device_annce_params_t *dev_annce_params = ezb_app_signal_get_params(app_signal);
        ESP_LOGI(TAG, "Zigbee device announced: short=0x%04hx", dev_annce_params->short_addr);
    } break;
    case EZB_NWK_SIGNAL_PERMIT_JOIN_STATUS: {
        uint8_t duration = *(uint8_t *)ezb_app_signal_get_params(app_signal);
        if (duration) {
            ESP_LOGI(TAG, "Zigbee network open for %d seconds", duration);
        } else {
            ESP_LOGW(TAG, "Zigbee network closed");
        }
    } break;
    default:
        ESP_LOGI(TAG, "Zigbee signal: %s (0x%x)", ezb_app_signal_to_string(signal_type), signal_type);
        break;
    }
    return true;
}

static esp_err_t zb_create_gateway_device(void)
{
    ezb_af_device_desc_t dev_desc = ezb_af_create_device_desc();
    ezb_zha_custom_gateway_config_t gateway_cfg = EZB_ZHA_CUSTOM_GATEWAY_CONFIG();
    ezb_af_ep_desc_t ep_desc = ezb_zha_create_custom_gateway(ESP_ZIGBEE_CUSTOM_GATEWAY_EP_ID, &gateway_cfg);
    ezb_zcl_cluster_desc_t basic_desc = ezb_af_endpoint_get_cluster_desc(ep_desc, EZB_ZCL_CLUSTER_ID_BASIC, EZB_ZCL_CLUSTER_SERVER);

    ESP_ZIGBEE_ERROR_CHECK(
        ezb_zcl_basic_cluster_desc_add_attr(basic_desc, EZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)ESP_MANUFACTURER_NAME));
    ESP_ZIGBEE_ERROR_CHECK(
        ezb_zcl_basic_cluster_desc_add_attr(basic_desc, EZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)ESP_MODEL_IDENTIFIER));
    ESP_ZIGBEE_ERROR_CHECK(ezb_af_device_add_endpoint_desc(dev_desc, ep_desc));
    ESP_ZIGBEE_ERROR_CHECK(ezb_af_device_desc_register(dev_desc));
    return ESP_OK;
}

static void zigbee_task(void *arg)
{
    (void)arg;
    esp_zigbee_config_t zb_config = ESP_ZIGBEE_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(esp_zigbee_init(&zb_config));
    ezb_aps_secur_enable_distributed_security(false);
    ESP_ZIGBEE_ERROR_CHECK(ezb_bdb_set_primary_channel_set(ESP_ZIGBEE_PRIMARY_CHANNEL_MASK));
    ESP_ZIGBEE_ERROR_CHECK(ezb_bdb_set_secondary_channel_set(ESP_ZIGBEE_SECONDARY_CHANNEL_MASK));
    ESP_ZIGBEE_ERROR_CHECK(ezb_app_signal_add_handler(zb_app_signal_handler));
    ESP_ERROR_CHECK(zb_create_gateway_device());
    ESP_ERROR_CHECK(esp_zigbee_start(false));
    ESP_LOGI(TAG, "Zigbee stack started");
    esp_zigbee_launch_mainloop();
    esp_zigbee_deinit();
    vTaskDelete(NULL);
}

void app_main(void)
{
    /* Used eventfds:
     *   1: lwip netif
     *   1: OpenThread task queue
     *   1: OpenThread border router
     *   1: Zigbee task queue
     *   1: multipan UART RX worker stop fd
     *   2: one per multipan client (OT + ZB)
     *   +1 when TREL is enabled */
    size_t max_eventfd = 7;
#if CONFIG_OPENTHREAD_RADIO_TREL
    max_eventfd++;
#endif
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = max_eventfd,
    };

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("esp-ot-zb-multipan"));
    static const esp_radio_spinel_multipan_radio_config_t multipan_radio_config = {
        .radio_mode = RADIO_MODE_UART_RCP,
        .radio_uart_config = ESP_MULTIPAN_DEFAULT_UART_RCP_CONFIG(),
    };
    ESP_ERROR_CHECK(esp_radio_spinel_multipan_init(&multipan_radio_config));

#if CONFIG_OPENTHREAD_CLI
    ot_console_start();
    ot_register_external_commands();
#endif

    BaseType_t zb_task_ret = xTaskCreate(zigbee_task, "zigbee_task", 8192, NULL, 5, NULL);
    ESP_ERROR_CHECK(zb_task_ret == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);

    static esp_openthread_config_t ot_config = {
        .netif_config = ESP_NETIF_DEFAULT_OPENTHREAD(),
        .platform_config = {
            .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
            .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
            .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
        },
    };

    ESP_ERROR_CHECK(esp_openthread_start(&ot_config));
    ESP_LOGI(TAG, "OpenThread stack started");
#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
    esp_cli_custom_command_init();
#endif
#if CONFIG_OPENTHREAD_BORDER_ROUTER && CONFIG_OPENTHREAD_NETWORK_AUTO_START
    ESP_ERROR_CHECK(esp_openthread_border_router_start());
#elif CONFIG_OPENTHREAD_NETWORK_AUTO_START
    ot_network_auto_start();
#endif
}
