/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "unity.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "ulp.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_io_reg.h"
#include "hal/misc.h"
#include "driver/rtc_io.h"

#include "sdkconfig.h"
#include "esp_rom_sys.h"

#include "ulp_test_app.h"

/* Test cases that require manual interaction, not run in CI */

void ulp_fsm_controls_rtc_io(void)
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    hal_memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

    /* ULP co-processor program to toggle LED */
    const ulp_insn_t program[] = {
        I_MOVI(R0, 0),                                  // r0 is LED state
        I_MOVI(R2, 16),                                 // loop r2 from 16 down to 0
        M_LABEL(4),                                     // define label 4
        I_SUBI(R2, R2, 1),                          // r2 = r2 - 1
        M_BXZ(6),                                   // branch to label 6 if r2 = 0
        I_ADDI(R0, R0, 1),                          // r0 = (r0 + 1) % 2
        I_ANDI(R0, R0, 0x1),
        M_BL(0, 1),                                 // if r0 < 1 goto 0
        M_LABEL(1),                                 // define label 1
        I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 1),  // RTC_GPIO12 = 1
        M_BX(2),                                // goto 2
        M_LABEL(0),                                 // define label 0
        I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 0),  // RTC_GPIO12 = 0
        M_LABEL(2),                                 // define label 2
        I_MOVI(R1, 100),                        // loop R1 from 100 down to 0
        M_LABEL(3),                                 // define label 3
        I_SUBI(R1, R1, 1),                      // r1 = r1 - 1
        M_BXZ(5),                               // branch to label 5 if r1 = 0
        I_DELAY(32000),                         // delay for a while
        M_BX(3),                                // goto 3
        M_LABEL(5),                                 // define label 5
        M_BX(4),                                // loop back to label 4
        M_LABEL(6),                                     // define label 6
        I_WAKE(),                                   // wake up the SoC
        I_END(),                                    // stop ULP program timer
        I_HALT()
    };

    /* Configure LED GPIOs */
    const gpio_num_t led_gpios[] = {
        GPIO_NUM_2,
        GPIO_NUM_0,
        GPIO_NUM_4
    };
    for (size_t i = 0; i < sizeof(led_gpios) / sizeof(led_gpios[0]); ++i) {
        rtc_gpio_init(led_gpios[i]);
        rtc_gpio_set_direction(led_gpios[i], RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(led_gpios[i], 0);
    }

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program) / sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

void ulp_fsm_power_consumption(void)
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 4 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    hal_memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

    /* Put the ULP coprocessor in halt state */
    ulp_insn_t insn = I_HALT();
    hal_memcpy(RTC_SLOW_MEM, &insn, sizeof(insn));

    /* Set ULP timer */
    ulp_set_wakeup_period(0, 0x8000);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

#if !DISABLED_FOR_TARGETS(ESP32)
void ulp_fsm_temp_sens(void)
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    hal_memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

    // Allow TSENS to be controlled by the ULP
    SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
#if CONFIG_IDF_TARGET_ESP32S2
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, SENS_FORCE_XPD_SAR_FSM, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL2_REG, SENS_TSENS_CLKGATE_EN);
#elif CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_TSENS_CLK_EN);
#endif
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);

    // data start offset
    size_t offset = 20;
    // number of samples to collect
    RTC_SLOW_MEM[offset] = (CONFIG_ULP_COPROC_RESERVE_MEM) / 4 - offset - 8;
    // sample counter
    RTC_SLOW_MEM[offset + 1] = 0;

    /* ULP co-processor program to record temperature sensor readings */
    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),             // r1 <- offset
        I_LD(R2, R1, 1),                // r2 <- counter
        I_LD(R3, R1, 0),                // r3 <- length
        I_SUBI(R3, R3, 1),              // end = length - 1
        I_SUBR(R3, R3, R2),             // r3 = length - counter
        M_BXF(1),                       // if overflow goto 1:
        I_TSENS(R0, 16383),         // r0 <- tsens
        I_ST(R0, R2, offset + 4),   // mem[r2 + offset +4] <- r0
        I_ADDI(R2, R2, 1),          // counter += 1
        I_ST(R2, R1, 1),            // save counter
        I_HALT(),                   // enter sleep
        M_LABEL(1),                     // done with measurements
        I_END(),                    // stop ULP timer
        I_WAKE(),                   // initiate wakeup
        I_HALT()
    };

    size_t size = sizeof(program) / sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
#endif //#if !DISABLED_FOR_TARGETS(ESP32)

void ulp_fsm_adc(void)
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    const int adc = 0;
    const int channel = 0;
    const int atten = 0;

    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    hal_memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

#if defined(CONFIG_IDF_TARGET_ESP32)
    // Configure SAR ADCn resolution
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH, 3, SENS_SAR1_BIT_WIDTH_S);
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR2_BIT_WIDTH, 3, SENS_SAR2_BIT_WIDTH_S);
    SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_BIT, 0x3, SENS_SAR1_SAMPLE_BIT_S);
    SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_BIT, 0x3, SENS_SAR2_SAMPLE_BIT_S);

    // SAR ADCn is started by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_FORCE);

    // Use ULP FSM to power up SAR ADCn
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_AMP, 2, SENS_FORCE_XPD_AMP_S);

    // SAR ADCn invert result
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DATA_INV);
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR2_DATA_INV);

    // Set SAR ADCn pad enable bitmap to be controlled by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD_FORCE_M);
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // SAR ADCn is started by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_FORCE);

    // Use ULP FSM to power up/down SAR ADCn
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS1_CTRL1_REG, SENS_FORCE_XPD_AMP, 2, SENS_FORCE_XPD_AMP_S);

    // SAR1 invert result
    SET_PERI_REG_MASK(SENS_SAR_READER1_CTRL_REG, SENS_SAR1_DATA_INV);
    SET_PERI_REG_MASK(SENS_SAR_READER2_CTRL_REG, SENS_SAR2_DATA_INV);

    // Set SAR ADCn pad enable bitmap to be controlled by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD_FORCE_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD_FORCE_M);

    // Enable SAR ADCn clock gate on esp32s3
#if CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_SARADC_CLK_EN);
#endif
#endif

    SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, 3, atten, 2 * channel); //set SAR1 attenuation
    SET_PERI_REG_BITS(SENS_SAR_ATTEN2_REG, 3, atten, 2 * channel); //set SAR2 attenuation

    // data start offset
    size_t offset = 20;
    // number of samples to collect
    RTC_SLOW_MEM[offset] = (CONFIG_ULP_COPROC_RESERVE_MEM) / 4 - offset - 8;
    // sample counter
    RTC_SLOW_MEM[offset + 1] = 0;

    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),             // r1 <- offset
        I_LD(R2, R1, 1),                // r2 <- counter
        I_LD(R3, R1, 0),                // r3 <- length
        I_SUBI(R3, R3, 1),              // end = length - 1
        I_SUBR(R3, R3, R2),             // r3 = length - counter
        M_BXF(1),                       // if overflow goto 1:
        I_ADC(R0, adc, channel),    // r0 <- ADC
        I_ST(R0, R2, offset + 4),   // mem[r2 + offset +4] = r0
        I_ADDI(R2, R2, 1),          // counter += 1
        I_ST(R2, R1, 1),            // save counter
        I_HALT(),                   // enter sleep
        M_LABEL(1),                     // done with measurements
        I_END(),                    // stop ULP program timer
        I_HALT()
    };

    size_t size = sizeof(program) / sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
