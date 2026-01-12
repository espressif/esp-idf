/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/system_periph_retention.h"
#include "hal/uart_periph.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/startup_internal.h"
#include "esp_private/sleep_retention.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_sys_periph");

static __attribute__((unused)) esp_err_t sleep_sys_periph_intr_matrix_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(intr_matrix_regs_retention, ARRAY_SIZE(intr_matrix_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "Interrupt matrix");
    ESP_LOGD(TAG, "Interrupt Matrix sleep retention initialization");
    return ESP_OK;
}

static __attribute__((unused)) esp_err_t sleep_sys_periph_hp_system_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(hp_system_regs_retention, ARRAY_SIZE(hp_system_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "HP system");
    ESP_LOGD(TAG, "HP System sleep retention initialization");
    return ESP_OK;
}

#if SOC_APM_SUPPORTED
static __attribute__((unused)) esp_err_t sleep_sys_periph_tee_apm_retention_init(void *arg)
{
/* TBD for ESP32P4 IDF-10020. */
#ifndef CONFIG_IDF_TARGET_ESP32P4
    esp_err_t err = sleep_retention_entries_create(tee_apm_regs_retention, ARRAY_SIZE(tee_apm_regs_retention), REGDMA_LINK_PRI_NON_CRITICAL_TEE_APM, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    if (err == ESP_OK) {
        err = sleep_retention_entries_create(tee_apm_highpri_regs_retention, ARRAY_SIZE(tee_apm_highpri_regs_retention), REGDMA_LINK_PRI_CRITICAL_TEE_APM, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "TEE/APM");
    ESP_LOGD(TAG, "TEE/APM sleep retention initialization");
#endif
    return ESP_OK;
}
#endif

#if CONFIG_ESP_CONSOLE_UART
static __attribute__((unused)) esp_err_t sleep_sys_periph_stdout_console_uart_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(uart_reg_retention_info[CONFIG_ESP_CONSOLE_UART_NUM].regdma_entry_array,
                                                   uart_reg_retention_info[CONFIG_ESP_CONSOLE_UART_NUM].array_size,
                                                   REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "UART");
    ESP_LOGD(TAG, "stdout console UART sleep retention initialization");
    return ESP_OK;
}
#endif

static __attribute__((unused)) esp_err_t sleep_sys_periph_iomux_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(iomux_regs_retention, ARRAY_SIZE(iomux_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "IO Matrix");
    ESP_LOGD(TAG, "IO Matrix sleep retention initialization");
    return ESP_OK;
}

static __attribute__((unused)) esp_err_t sleep_sys_periph_flash_spimem_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(flash_spimem_regs_retention, ARRAY_SIZE(flash_spimem_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "Flash SPI mem");
    ESP_LOGD(TAG, "Flash SPI Mem sleep retention initialization");
    return ESP_OK;
}

#if CONFIG_SPIRAM && CONFIG_IDF_TARGET_ESP32P4
/* TODO: PM-205, In the ESP32C5, Flash and PSRAM use the same set of SPIMEM hardware, while in P4, Flash and PSRAM each have their own SPIMEM hardware.
 * It’s necessary to confirm whether the ESP32C5 can independently manage SPIMEM retention for Flash and PSRAM in software. */
static __attribute__((unused)) esp_err_t sleep_sys_periph_psram_spimem_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(psram_spimem_regs_retention, ARRAY_SIZE(psram_spimem_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "PSRAM SPI mem");
    ESP_LOGD(TAG, "PSRAM SPI Mem sleep retention initialization");
    return ESP_OK;
}
#endif

static __attribute__((unused)) esp_err_t sleep_sys_periph_systimer_retention_init(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(systimer_regs_retention, ARRAY_SIZE(systimer_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (%s) retention", "SysTimer");
    ESP_LOGD(TAG, "SysTimer sleep retention initialization");
    return ESP_OK;
}

#if SOC_PM_CACHE_RETENTION_BY_PAU
esp_err_t sleep_sys_periph_cache_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(cache_regs_retention, ARRAY_SIZE(cache_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (Cache) retention");
    ESP_LOGI(TAG, "L2 Cache sleep retention initialization");
    return ESP_OK;
}
#endif

#if SOC_PAU_IN_TOP_DOMAIN
esp_err_t sleep_pau_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(pau_regs_retention, ARRAY_SIZE(pau_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PAU) retention");
    ESP_LOGI(TAG, "PAU sleep retention initialization");
    return ESP_OK;
}
#endif

#if CONFIG_ESP_ENABLE_PVT
esp_err_t sleep_pvt_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(pvt_regs_retention, ARRAY_SIZE(pvt_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_LOW, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PVT) retention");
    ESP_LOGI(TAG, "PVT sleep retention initialization");
    return ESP_OK;
}
#endif

static __attribute__((unused)) esp_err_t sleep_sys_periph_retention_init(void *arg)
{
    esp_err_t err;
    err = sleep_sys_periph_intr_matrix_retention_init(arg);
    if(err) goto error;
    err = sleep_sys_periph_hp_system_retention_init(arg);
    if(err) goto error;
#if SOC_PM_CACHE_RETENTION_BY_PAU
    err = sleep_sys_periph_cache_retention_init();
    if(err) goto error;
#endif
#if SOC_APM_SUPPORTED
    err = sleep_sys_periph_tee_apm_retention_init(arg);
    if(err) goto error;
#endif
#if CONFIG_ESP_CONSOLE_UART
    err = sleep_sys_periph_stdout_console_uart_retention_init(arg);
    if(err) goto error;
#endif
    err = sleep_sys_periph_iomux_retention_init(arg);
    if(err) goto error;
    err = sleep_sys_periph_flash_spimem_retention_init(arg);
    if(err) goto error;
#if CONFIG_SPIRAM && CONFIG_IDF_TARGET_ESP32P4
    err = sleep_sys_periph_psram_spimem_retention_init(arg);
    if(err) goto error;
#endif
    err = sleep_sys_periph_systimer_retention_init(arg);
    if(err) goto error;
#if SOC_PAU_IN_TOP_DOMAIN
    err = sleep_pau_retention_init();
    if(err) goto error;
#endif
#if CONFIG_ESP_ENABLE_PVT && SOC_PVT_RETENTION_BY_REGDMA
    err = sleep_pvt_retention_init();
    if(err) goto error;
#endif

error:
    return err;
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
ESP_SYSTEM_INIT_FN(sleep_sys_periph_startup_init, SECONDARY, BIT(0), 107)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs = { .create = { .handle = sleep_sys_periph_retention_init, .arg = NULL } },
        .depends.bitmap[SLEEP_RETENTION_MODULE_CLOCK_SYSTEM >> 5] = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM % 32)
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_SYS_PERIPH, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_SYS_PERIPH);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "failed to allocate sleep retention linked list for system peripherals retention");
        }
    }
    return ESP_OK;
}
#endif

/**
 * @brief Dummy function used to force linking this file.
 * This works via -u sleep_system_peripheral_dummy flag in CMakeLists.txt
 */
void sleep_system_peripheral_dummy(void)
{
    return;
}
