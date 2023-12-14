/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_platform.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread_alarm.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_radio.h"
#include "esp_openthread_spi_slave.h"
#include "esp_openthread_task_queue.h"
#include "esp_openthread_types.h"
#include "esp_openthread_uart.h"
#include "esp_partition.h"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "core/instance/instance.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

static esp_openthread_platform_config_t s_platform_config;
static bool s_openthread_platform_initialized = false;
static esp_openthread_platform_workflow_t *s_workflow_list = NULL;

esp_err_t esp_openthread_platform_workflow_register(esp_openthread_update_func update_func,
                                                    esp_openthread_process_func process_func, const char *name)
{
    uint8_t name_len = strnlen(name, WORKFLOW_MAX_NAMELEN);
    esp_openthread_platform_workflow_t *current_workflow = s_workflow_list;
    esp_openthread_platform_workflow_t *before_workflow = NULL;
    esp_openthread_platform_workflow_t *add_workflow =
        static_cast<esp_openthread_platform_workflow_t *>(malloc(sizeof(esp_openthread_platform_workflow_t)));
    ESP_RETURN_ON_FALSE(add_workflow != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG,
                        "Failed to alloc memory for esp_openthread_workflow");
    strncpy(add_workflow->name, name, name_len);
    add_workflow->update_func = update_func;
    add_workflow->process_func = process_func;
    add_workflow->next = NULL;
    while (current_workflow) {
        if (strncmp(current_workflow->name, name, name_len) == 0) {
            free(add_workflow);
            add_workflow = NULL; // handle has be added in the list.
            break;
        } else {
            before_workflow = current_workflow;
            current_workflow = current_workflow->next;
        }
    }
    if (add_workflow) {
        if (before_workflow) {
            before_workflow->next = add_workflow; // Add handle to the list end
        } else {
            s_workflow_list = add_workflow; // No handle in the list
        }
    }
    return ESP_OK;
}

void esp_openthread_platform_workflow_unregister(const char *name)
{
    esp_openthread_platform_workflow_t *current_workflow = s_workflow_list;
    esp_openthread_platform_workflow_t *before_workflow = NULL;
    while (current_workflow) {
        if (strncmp(current_workflow->name, name, strnlen(name, WORKFLOW_MAX_NAMELEN)) == 0) {
            if (before_workflow) {
                before_workflow->next = current_workflow->next;
            } else {
                s_workflow_list = current_workflow->next;
            }
            free(current_workflow);
            break;
        }
        before_workflow = current_workflow;
        current_workflow = current_workflow->next;
    }
}

static inline esp_openthread_host_connection_mode_t get_host_connection_mode(void)
{
    return s_platform_config.host_config.host_connection_mode;
}

static esp_err_t esp_openthread_host_interface_init(const esp_openthread_platform_config_t *config)
{
    esp_openthread_host_connection_mode_t host_mode = get_host_connection_mode();
    switch (host_mode) {
    case HOST_CONNECTION_MODE_RCP_SPI:
        ESP_RETURN_ON_ERROR(esp_openthread_host_rcp_spi_init(config), OT_PLAT_LOG_TAG,
                          "esp_openthread_host_rcp_spi_init failed");
        break;
    case HOST_CONNECTION_MODE_RCP_UART:
        ESP_RETURN_ON_ERROR(esp_openthread_host_rcp_uart_init(config), OT_PLAT_LOG_TAG,
                          "esp_openthread_host_rcp_uart_init failed");
        break;
#if CONFIG_OPENTHREAD_CONSOLE_TYPE_UART
    case HOST_CONNECTION_MODE_CLI_UART:
        ESP_RETURN_ON_ERROR(esp_openthread_host_cli_uart_init(config), OT_PLAT_LOG_TAG,
                          "esp_openthread_host_cli_uart_init failed");
        break;
#endif
#if CONFIG_OPENTHREAD_CONSOLE_TYPE_USB_SERIAL_JTAG
    case HOST_CONNECTION_MODE_CLI_USB:
        ESP_RETURN_ON_ERROR(esp_openthread_host_cli_usb_init(config), OT_PLAT_LOG_TAG,
                          "esp_openthread_host_cli_usb_init failed");
        break;
#endif
    case HOST_CONNECTION_MODE_NONE:
        ESP_LOGI(OT_PLAT_LOG_TAG, "Host connection mode none");
        break;
    default:
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_openthread_platform_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_FALSE(config->radio_config.radio_mode < RADIO_MODE_MAX,
                        ESP_ERR_INVALID_ARG, OT_PLAT_LOG_TAG, "Radio mode not supported");
    ESP_RETURN_ON_FALSE(config->host_config.host_connection_mode < HOST_CONNECTION_MODE_MAX,
                        ESP_ERR_INVALID_ARG, OT_PLAT_LOG_TAG, "Host connection mode not supported");
    ESP_RETURN_ON_FALSE(!s_openthread_platform_initialized, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread platform already initialized");

    s_openthread_platform_initialized = true;
    esp_err_t ret = ESP_OK;

    s_platform_config = *config;
    ESP_GOTO_ON_ERROR(esp_openthread_lock_init(), exit, OT_PLAT_LOG_TAG, "esp_openthread_lock_init failed");
    ESP_GOTO_ON_ERROR(esp_openthread_alarm_init(), exit, OT_PLAT_LOG_TAG, "esp_openthread_alarm_init failed");

    esp_openthread_set_storage_name(config->port_config.storage_partition_name);

    ESP_GOTO_ON_ERROR(esp_openthread_host_interface_init(config), exit, OT_PLAT_LOG_TAG,
                      "esp_openthread_host_interface_init failed");

    ESP_GOTO_ON_ERROR(esp_openthread_task_queue_init(config), exit, OT_PLAT_LOG_TAG,
                      "esp_openthread_task_queue_init failed");
    ESP_GOTO_ON_ERROR(esp_openthread_radio_init(config), exit, OT_PLAT_LOG_TAG, "esp_openthread_radio_init failed");

exit:
    if (ret != ESP_OK) {
        esp_openthread_platform_deinit();
    }

    return ret;
}

otInstance *esp_openthread_get_instance(void)
{
    return (otInstance *)&ot::Instance::Get();
}

esp_err_t esp_openthread_platform_deinit(void)
{
    ESP_RETURN_ON_FALSE(s_openthread_platform_initialized, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread platform not initialized");
    s_openthread_platform_initialized = false;
    esp_openthread_task_queue_deinit();
    esp_openthread_radio_deinit();

    if (get_host_connection_mode() == HOST_CONNECTION_MODE_RCP_SPI){
        esp_openthread_spi_slave_deinit();
    } else if (get_host_connection_mode() == HOST_CONNECTION_MODE_CLI_UART ||
        get_host_connection_mode() == HOST_CONNECTION_MODE_RCP_UART) {
        esp_openthread_uart_deinit();
    }

    esp_openthread_lock_deinit();
    esp_openthread_alarm_deinit();
    return ESP_OK;
}

void esp_openthread_platform_update(esp_openthread_mainloop_context_t *mainloop)
{
    esp_openthread_platform_workflow_t *current_workflow = s_workflow_list;
    while (current_workflow) {
        current_workflow->update_func(mainloop);
        current_workflow = current_workflow->next;
    }
}

esp_err_t esp_openthread_platform_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    esp_openthread_platform_workflow_t *current_workflow = s_workflow_list;
    while (current_workflow) {
        ESP_RETURN_ON_ERROR(current_workflow->process_func(instance, mainloop), OT_PLAT_LOG_TAG, "process %s failed",
                            current_workflow->name);
        current_workflow = current_workflow->next;
    }
    return ESP_OK;
}
