/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Target-specific regi2c test configuration
 *
 * For ESP32: test with the APLL regi2c block (I2C_APLL)
 * For other targets: test with the SAR ADC calibration regi2c block (I2C_SAR_ADC / I2C_SARADC)
 *
 * Each target configuration selects:
 *   - A full 8-bit register for testing read/write
 *   - A partial bit-field register for testing read_mask/write_mask
 */

#if CONFIG_IDF_TARGET_ESP32

#include "soc/regi2c_apll.h"
#define TEST_BLOCK          I2C_APLL
#define TEST_HOST_ID        I2C_APLL_HOSTID
/* I2C_APLL_DSDM0: register 9, bits [7:0] — full 8-bit SDM parameter */
#define TEST_REG_FULL       I2C_APLL_DSDM0
/* I2C_APLL_OC_DVDD: register 6, bits [4:0] — 5-bit output voltage divider */
#define TEST_REG_MASK       I2C_APLL_OC_DVDD
#define TEST_REG_MASK_MSB   I2C_APLL_OC_DVDD_MSB
#define TEST_REG_MASK_LSB   I2C_APLL_OC_DVDD_LSB

#elif CONFIG_IDF_TARGET_ESP32H4

#include "soc/regi2c_saradc.h"
#define TEST_BLOCK          I2C_SAR_ADC
#define TEST_HOST_ID        I2C_SAR_ADC_HOSTID
/* ADC_SAR1_INITIAL_CODE_LOW_ADDR: register 0, bits [7:0] — full 8-bit */
#define TEST_REG_FULL       ADC_SAR1_INITIAL_CODE_LOW_ADDR
/* ADC_SAR1_INITIAL_CODE_HIGH_ADDR: register 1, bits [3:0] — 4-bit field */
#define TEST_REG_MASK       ADC_SAR1_INITIAL_CODE_HIGH_ADDR
#define TEST_REG_MASK_MSB   ADC_SAR1_INITIAL_CODE_HIGH_ADDR_MSB
#define TEST_REG_MASK_LSB   ADC_SAR1_INITIAL_CODE_HIGH_ADDR_LSB

#elif CONFIG_IDF_TARGET_ESP32S31

#include "soc/regi2c_saradc.h"
#define TEST_BLOCK          I2C_SARADC
#define TEST_HOST_ID        I2C_SARADC_HOSTID
/* I2C_SARADC_SAR1_INIT_CODE_LSB: register 0, bits [7:0] — full 8-bit */
#define TEST_REG_FULL       I2C_SARADC_SAR1_INIT_CODE_LSB
/* I2C_SARADC_SAR1_INIT_CODE_MSB: register 1, bits [3:0] — 4-bit field */
#define TEST_REG_MASK       I2C_SARADC_SAR1_INIT_CODE_MSB
#define TEST_REG_MASK_MSB   I2C_SARADC_SAR1_INIT_CODE_MSB_MSB
#define TEST_REG_MASK_LSB   I2C_SARADC_SAR1_INIT_CODE_MSB_LSB

#elif CONFIG_IDF_TARGET_ESP32H21 // TODO: IDF-11590 Replace with standard SARADC register field macros once they are defined in esp32h21/regi2c_saradc.h

#include "soc/regi2c_saradc.h"
#define TEST_BLOCK          I2C_SAR_ADC
#define TEST_HOST_ID        I2C_SAR_ADC_HOSTID
/*
 * ESP32-H21 regi2c_saradc.h only defines block and host ID without individual
 * register field macros. Use the standard SARADC register layout directly:
 *   register 0 bits [7:0] = SAR1 initial code low byte
 *   register 1 bits [3:0] = SAR1 initial code high nibble
 */
#define TEST_REG_FULL       0
#define TEST_REG_MASK       1
#define TEST_REG_MASK_MSB   3
#define TEST_REG_MASK_LSB   0

#else
/* Standard SARADC register naming */

#include "soc/regi2c_saradc.h"
#define TEST_BLOCK          I2C_SAR_ADC
#define TEST_HOST_ID        I2C_SAR_ADC_HOSTID
/* ADC_SAR1_INITIAL_CODE_LOW_ADDR: register 0x0, bits [7:0] — full 8-bit */
#define TEST_REG_FULL       ADC_SAR1_INITIAL_CODE_LOW_ADDR
/* ADC_SAR1_DREF_ADDR: register 0x2, bits [6:4] — 3-bit reference voltage */
#define TEST_REG_MASK       ADC_SAR1_DREF_ADDR
#define TEST_REG_MASK_MSB   ADC_SAR1_DREF_ADDR_MSB
#define TEST_REG_MASK_LSB   ADC_SAR1_DREF_ADDR_LSB

#endif

#ifdef __cplusplus
}
#endif
