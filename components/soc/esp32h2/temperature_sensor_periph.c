/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/regdma.h"
#include "soc/temperature_sensor_periph.h"
#include "soc/apb_saradc_reg.h"


const temperature_sensor_attribute_t temperature_sensor_attributes[TEMPERATURE_SENSOR_ATTR_RANGE_NUM] = {
    /*Offset   reg_val  min  max  error */
    {-2,     5,    50,  125,   3},
    {-1,     7,    20,  100,   2},
    { 0,    15,   -10,   80,   1},
    { 1,    11,   -30,   50,   2},
    { 2,    10,   -40,   20,   3},
};

// Temperature sensor sleep retention entries
// Temperature sensor registers require set the reg_update bit to make the configuration take effect

/* Temperature sensor Registers Context
   Include: APB_SARADC_INT_ENA_REG /
            APB_SARADC_APB_TSENS_CTRL_REG / APB_SARADC_TSENS_CTRL2_REG / APB_TSENS_WAKE_REG / APB_TSENS_SAMPLE_REG
*/
#define TEMPERATURE_SENSOR_RETENTION_REGS_CNT  5
#define TEMPERATURE_SENSOR_RETENTION_MAP_BASE  APB_SARADC_INT_ENA_REG
static const uint32_t temperature_sensor_regs_map[4] = {0x6c1, 0, 0, 0};
static const regdma_entries_config_t temperature_sensor_regs_entries[] = {
    [0] = {.config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_TSENS_LINK(0x00), TEMPERATURE_SENSOR_RETENTION_MAP_BASE, TEMPERATURE_SENSOR_RETENTION_MAP_BASE, TEMPERATURE_SENSOR_RETENTION_REGS_CNT, 0, 0, temperature_sensor_regs_map[0], temperature_sensor_regs_map[1], temperature_sensor_regs_map[2], temperature_sensor_regs_map[3]), \
           .owner = ENTRY(0) | ENTRY(2) }, \
};

const temperature_sensor_reg_ctx_link_t temperature_sensor_regs_retention = {
    .link_list = temperature_sensor_regs_entries,
    .link_num = ARRAY_SIZE(temperature_sensor_regs_entries),
    .module_id = SLEEP_RETENTION_MODULE_TEMP_SENSOR,
};
