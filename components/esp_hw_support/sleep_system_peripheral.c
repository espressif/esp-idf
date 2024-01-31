/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "soc/system_periph_retention.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_check.h"

#include "esp_private/startup_internal.h"
#include "esp_private/sleep_retention.h"

static __attribute__((unused)) const char *TAG = "sleep_sys_periph";

esp_err_t sleep_sys_periph_intr_matrix_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(intr_matrix_regs_retention, ARRAY_SIZE(intr_matrix_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_INTR_MATRIX);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (Interrupt matrix) retention");
    ESP_LOGI(TAG, "Interrupt Matrix sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_hp_system_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(hp_system_regs_retention, ARRAY_SIZE(hp_system_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_HP_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (HP system) retention");
    ESP_LOGI(TAG, "HP System sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_tee_apm_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(tee_apm_regs_retention, ARRAY_SIZE(tee_apm_regs_retention), REGDMA_LINK_PRI_NON_CRITICAL_TEE_APM, SLEEP_RETENTION_MODULE_TEE_APM);
    if (err == ESP_OK) {
        err = sleep_retention_entries_create(tee_apm_highpri_regs_retention, ARRAY_SIZE(tee_apm_highpri_regs_retention), REGDMA_LINK_PRI_CRITICAL_TEE_APM, SLEEP_RETENTION_MODULE_TEE_APM);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (TEE/APM) retention");
    ESP_LOGI(TAG, "TEE/APM sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_uart0_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(uart_regs_retention, ARRAY_SIZE(uart_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_UART0);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (UART) retention");
    ESP_LOGI(TAG, "UART sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_tg0_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(tg_regs_retention, ARRAY_SIZE(tg_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_TG0);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (Timer Group) retention");
    ESP_LOGI(TAG, "Timer Group sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_iomux_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(iomux_regs_retention, ARRAY_SIZE(iomux_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_IOMUX);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (IO Matrix) retention");
    ESP_LOGI(TAG, "IO Matrix sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_spimem_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(spimem_regs_retention, ARRAY_SIZE(spimem_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SPIMEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (SPI mem) retention");
    ESP_LOGI(TAG, "SPI Mem sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_systimer_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(systimer_regs_retention, ARRAY_SIZE(systimer_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYSTIMER);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (SysTimer) retention");
    ESP_LOGI(TAG, "SysTimer sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_retention_init(void)
{
    esp_err_t err;
    err = sleep_sys_periph_intr_matrix_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_hp_system_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_tee_apm_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_uart0_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_tg0_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_iomux_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_spimem_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_systimer_retention_init();

error:
    return err;
}

bool peripheral_domain_pd_allowed(void)
{
    const uint32_t modules = sleep_retention_get_modules();
    const uint32_t mask = (const uint32_t) (
            SLEEP_RETENTION_MODULE_INTR_MATRIX | \
            SLEEP_RETENTION_MODULE_HP_SYSTEM   | \
            SLEEP_RETENTION_MODULE_TEE_APM     | \
            SLEEP_RETENTION_MODULE_UART0       | \
            SLEEP_RETENTION_MODULE_TG0         | \
            SLEEP_RETENTION_MODULE_IOMUX       | \
            SLEEP_RETENTION_MODULE_SPIMEM      | \
            SLEEP_RETENTION_MODULE_SYSTIMER);
    return ((modules & mask) == mask);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
ESP_SYSTEM_INIT_FN(sleep_sys_periph_startup_init, SECONDARY, BIT(0), 107)
{
    sleep_sys_periph_retention_init();
    return ESP_OK;
}
#endif
