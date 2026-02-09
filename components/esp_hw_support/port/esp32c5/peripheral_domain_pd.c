/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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

    // ESP32C5 supports WDT sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG0_WDT);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG1_WDT);

    // ESP32C5 supports Timer sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG0_TIMER0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TG1_TIMER0);
    /* ESP32C5 does not support Timer1 */

    // ESP32C5 supports ADC sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_ADC);

    /* ESP32C5 supports UART sleep retention */
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_UART1);
    /* ESP32C5 only supports UART0 and UART1 */

    /* ESP32C5 supports RMT0 */
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_RMT0);

    // ESP32C5 supports GDMA sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH0);
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH1);
    /* ESP32C5 supports GDMA_CH2 */
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GDMA_CH2);

    // ESP32C5 supports PAU and I2S sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2S0);
    /* ESP32C5 only supports I2S0*/

    // ESP32C5 supports I2C sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_I2C0);
    /* ESP32C5 only supports I2C0*/

    // ESP32C5 supports ETM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_ETM0);

    // ESP32C5 supports temperature sensor sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_TEMP_SENSOR);

    // ESP32C5 does not support TWAI sleep retention yet: IDF-13001

    // ESP32C5 supports PARLIO sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_PARLIO0);

    // ESP32C5 supports GPSPI2 sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_GPSPI2);

    // ESP32C5 supports LEDC sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_LEDC);

    // ESP32C5 supports MCPWM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_MCPWM0);
    /* ESP32C5 only supports MCPWM0 */

    // ESP32C5 supports SDM sleep retention
    RETENTION_MODULE_BITMAP_SET(&mask, SLEEP_RETENTION_MODULE_SDM0);

    const sleep_retention_module_bitmap_t peripheral_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t peripheral_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    return sleep_retention_module_bitmap_eq(peripheral_domain_inited_modules, peripheral_domain_created_modules);
#else
    return false;
#endif
}
