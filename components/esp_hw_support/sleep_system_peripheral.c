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
#include "soc/uart_periph.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_check.h"

#include "esp_private/startup_internal.h"
#include "esp_private/sleep_retention.h"

static __attribute__((unused)) const char *TAG = "sleep_sys_periph";

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

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
esp_err_t sleep_sys_periph_l2_cache_retention_init(void)
{
    esp_err_t err = sleep_retention_entries_create(l2_cache_regs_retention, ARRAY_SIZE(l2_cache_regs_retention), REGDMA_LINK_PRI_SYS_PERIPH_HIGH, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (L2 Cache) retention");
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

static __attribute__((unused)) esp_err_t sleep_sys_periph_retention_init(void *arg)
{
    esp_err_t err;
    err = sleep_sys_periph_intr_matrix_retention_init(arg);
    if(err) goto error;
    err = sleep_sys_periph_hp_system_retention_init(arg);
    if(err) goto error;
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    err = sleep_sys_periph_l2_cache_retention_init();
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
#endif

error:
    return err;
}

bool peripheral_domain_pd_allowed(void)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    const sleep_retention_module_bitmap_t inited_modules = sleep_retention_get_inited_modules();
    const sleep_retention_module_bitmap_t created_modules = sleep_retention_get_created_modules();

    sleep_retention_module_bitmap_t mask = RETENTION_MODULE_BITMAP_INIT(NULL);
    mask.bitmap[SLEEP_RETENTION_MODULE_SYS_PERIPH >> 5] |= BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH % 32);
#if SOC_MWDT_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_TG0_WDT >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG0_WDT % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_TG1_WDT >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG1_WDT % 32);
#endif
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_TG0_TIMER0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG0_TIMER0 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_TG1_TIMER0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG1_TIMER0 % 32);
#if (SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1)
    mask.bitmap[SLEEP_RETENTION_MODULE_TG0_TIMER1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG0_TIMER1 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_TG1_TIMER1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TG1_TIMER1 % 32);
#endif
#endif

#if SOC_ADC_SUPPORTED && !CONFIG_IDF_TARGET_ESP32P4 // TODO: IDF-11369
    mask.bitmap[SLEEP_RETENTION_MODULE_ADC >> 5] |= BIT(SLEEP_RETENTION_MODULE_ADC % 32);
#endif

#if SOC_UART_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_UART0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_UART0 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_UART1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_UART1 % 32);
# if (SOC_UART_HP_NUM > 2) && !CONFIG_IDF_TARGET_ESP32C61   // TODO: IDF-11370
    mask.bitmap[SLEEP_RETENTION_MODULE_UART2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_UART2 % 32);
# endif
# if (SOC_UART_HP_NUM > 3)
    mask.bitmap[SLEEP_RETENTION_MODULE_UART3 >> 5] |= BIT(SLEEP_RETENTION_MODULE_UART3 % 32);
# endif
# if (SOC_UART_HP_NUM > 4)
    mask.bitmap[SLEEP_RETENTION_MODULE_UART4 >> 5] |= BIT(SLEEP_RETENTION_MODULE_UART4 % 32);
# endif
#endif /* SOC_UART_SUPPORT_SLEEP_RETENTION */

#if SOC_RMT_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_RMT0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_RMT0 % 32);
#endif

#if SOC_GDMA_SUPPORT_SLEEP_RETENTION
# if SOC_GDMA_SUPPORTED && !CONFIG_IDF_TARGET_ESP32P4 // TODO: IDF-11371
    mask.bitmap[SLEEP_RETENTION_MODULE_GDMA_CH0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_GDMA_CH0 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_GDMA_CH1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_GDMA_CH1 % 32);
#  if (SOC_GDMA_PAIRS_PER_GROUP_MAX > 2)
    mask.bitmap[SLEEP_RETENTION_MODULE_GDMA_CH2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_GDMA_CH2 % 32);
#  endif
# endif /* SOC_GDMA_SUPPORTED */
# if SOC_AHB_GDMA_SUPPORTED && CONFIG_IDF_TARGET_ESP32P4 // TODO: IDF-11372
    mask.bitmap[SLEEP_RETENTION_MODULE_AHB_DMA_CH0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH0 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_AHB_DMA_CH1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH1 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_AHB_DMA_CH2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH2 % 32);
# endif /* SOC_AHB_GDMA_SUPPORTED */
# if SOC_AXI_GDMA_SUPPORTED
    mask.bitmap[SLEEP_RETENTION_MODULE_AXI_DMA_CH0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH0 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_AXI_DMA_CH1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH1 % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_AXI_DMA_CH2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH2 % 32);
# endif /* SOC_AXI_GDMA_SUPPORTED */
#endif /* SOC_GDMA_SUPPORT_SLEEP_RETENTION */

#if SOC_I2S_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_I2S0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_I2S0 % 32);
# if (SOC_I2S_NUM > 1)
    mask.bitmap[SLEEP_RETENTION_MODULE_I2S1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_I2S1 % 32);
# endif
# if (SOC_I2S_NUM > 2)
    mask.bitmap[SLEEP_RETENTION_MODULE_I2S2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_I2S2 % 32);
# endif
#endif /* SOC_I2S_SUPPORT_SLEEP_RETENTION */

#if SOC_I2C_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_I2C0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_I2C0 % 32);
# if (SOC_HP_I2C_NUM > 1)
    mask.bitmap[SLEEP_RETENTION_MODULE_I2C1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_I2C1 % 32);
# endif
#endif /* SOC_I2C_SUPPORT_SLEEP_RETENTION */

#if SOC_ETM_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_ETM0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_ETM0 % 32);
#endif

#if SOC_TEMP_SENSOR_SUPPORTED && !CONFIG_IDF_TARGET_ESP32P4
    mask.bitmap[SLEEP_RETENTION_MODULE_TEMP_SENSOR >> 5] |= BIT(SLEEP_RETENTION_MODULE_TEMP_SENSOR % 32);
#endif

#if SOC_TWAI_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_TWAI0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TWAI0 % 32);
# if (SOC_TWAI_CONTROLLER_NUM > 1)
    mask.bitmap[SLEEP_RETENTION_MODULE_TWAI1 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TWAI1 % 32);
# endif
# if (SOC_TWAI_CONTROLLER_NUM > 2)
    mask.bitmap[SLEEP_RETENTION_MODULE_TWAI2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_TWAI2 % 32);
# endif
#endif /* SOC_TWAI_SUPPORT_SLEEP_RETENTION */

#if SOC_PARLIO_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_PARLIO0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_PARLIO0 % 32);
#endif

#if SOC_GPSPI_SUPPORTED
    mask.bitmap[SLEEP_RETENTION_MODULE_GPSPI2 >> 5] |= BIT(SLEEP_RETENTION_MODULE_GPSPI2 % 32);
# if CONFIG_IDF_TARGET_ESP32P4
    mask.bitmap[SLEEP_RETENTION_MODULE_GPSPI3 >> 5] |= BIT(SLEEP_RETENTION_MODULE_GPSPI3 % 32);
# endif
#endif

#if SOC_LEDC_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_LEDC >> 5] |= BIT(SLEEP_RETENTION_MODULE_LEDC % 32);
#endif

#if SOC_PCNT_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_PCNT0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_PCNT0 % 32);
#endif

#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    mask.bitmap[SLEEP_RETENTION_MODULE_MCPWM0 >> 5] |= BIT(SLEEP_RETENTION_MODULE_MCPWM0 % 32);
#endif

    const sleep_retention_module_bitmap_t peripheral_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t peripheral_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    return sleep_retention_module_bitmap_eq(peripheral_domain_inited_modules, peripheral_domain_created_modules);
#else
    return false;
#endif
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
