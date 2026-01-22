/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "sdkconfig.h"
#include "soc/system_periph_retention.h"
#include "esp_private/sleep_retention.h"

bool peripheral_domain_pd_allowed(void)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    const sleep_retention_module_bitmap_t inited_modules = sleep_retention_get_inited_modules();
    const sleep_retention_module_bitmap_t created_modules = sleep_retention_get_created_modules();

    sleep_retention_module_bitmap_t mask = RETENTION_MODULE_BITMAP_INIT(NULL);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_SYS_PERIPH);
    // ESP32P4 supports WDT sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG0_WDT);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG1_WDT);

    // ESP32P4 supports Timer sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG0_TIMER0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG1_TIMER0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG0_TIMER1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG1_TIMER1);

    // TODO: IDF-11369
    // RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_ADC);

    // ESP32P4 supports UART sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART2);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART3);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART4);

    // ESP32P4 supports RMT sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_RMT0);

    // TODO: IDF-11371
    // RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH0);
    // RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH1);
    // RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH2);

    // ESP32P4 supports AHB_DMA sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AHB_DMA_CH0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AHB_DMA_CH1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AHB_DMA_CH2);

    // ESP32P4 supports AXI_DMA sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AXI_DMA_CH0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AXI_DMA_CH1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_AXI_DMA_CH2);

    // ESP32P4 supports PAU and I2S sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2S0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2S1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2S2);

    // ESP32P4 supports I2C sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2C0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2C1);

    // ESP32P4 supports ETM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_ETM0);

    // ESP32P4 does not support temperature sensor sleep retention since it is in LP domain

    // ESP32P4 supports TWAI sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TWAI0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TWAI1);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TWAI2);

    // ESP32P4 supports PARLIO sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_PARLIO0);

    // ESP32P4 supports GPSPI2 sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GPSPI2);
    // ESP32P4 supports GPSPI3 sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GPSPI3);

    // ESP32P4 supports LEDC sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_LEDC);

    // ESP32P4 supports MCPWM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_MCPWM0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_MCPWM1);

    // ESP32P4 supports SDM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_SDM0);

    // ESP32P4 supports EMAC sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_EMAC);

    // ESP32P4 supports JPEG sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_JPEG);

    const sleep_retention_module_bitmap_t peripheral_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t peripheral_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    return sleep_retention_module_bitmap_eq(peripheral_domain_inited_modules, peripheral_domain_created_modules);
#else
    return false;
#endif
}
