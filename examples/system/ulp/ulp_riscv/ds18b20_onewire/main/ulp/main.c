/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"

#define EXAMPLE_1WIRE_GPIO GPIO_NUM_4

#define WAKEUP_TEMP_C 32.5
#define TEMP_ALARM_LIMIT ( (int)(WAKEUP_TEMP_C*16) )

typedef enum {
    SENSOR_CONVERSION_INIT,
    SENSOR_CONVERSION_READ,
} sensor_state_t;

sensor_state_t state = SENSOR_CONVERSION_INIT;
int32_t temp_reg_val = INT32_MIN;

static void ds18b20_write_bit(bool bit)
{
    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 0);
    if (bit) {
        /* Must pull high within 15 us, without delay this takes 5 us */
        ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 1);
    }

    /* Write slot duration at least 60 us */
    ulp_riscv_delay_cycles(60 * ULP_RISCV_CYCLES_PER_US);
    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 1);
}

static bool ds18b20_read_bit(void)
{
    bool bit;

    /* Pull low minimum 1 us */
    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 0);
    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 1);

    /* Must sample within 15 us of the failing edge */
    ulp_riscv_delay_cycles(5 * ULP_RISCV_CYCLES_PER_US);
    bit = ulp_riscv_gpio_get_level(EXAMPLE_1WIRE_GPIO);

    /* Read slot duration at least 60 us */
    ulp_riscv_delay_cycles(55 * ULP_RISCV_CYCLES_PER_US);

    return bit;
}

static void ds18b20_write_byte(uint8_t data)
{
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit((data >> i) & 0x1);
    }
}

static uint8_t ds18b20_read_byte(void)
{
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data |=  ds18b20_read_bit() << i;
    }
    return data;
}

bool ds18b20_reset_pulse(void)
{
    bool presence_pulse;
    /* min 480 us reset pulse + 480 us reply time is specified by datasheet */
    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 0);
    ulp_riscv_delay_cycles(480 * ULP_RISCV_CYCLES_PER_US);

    ulp_riscv_gpio_output_level(EXAMPLE_1WIRE_GPIO, 1);

    /* Wait for ds18b20 to pull low before sampling */
    ulp_riscv_delay_cycles(60 * ULP_RISCV_CYCLES_PER_US);
    presence_pulse = ulp_riscv_gpio_get_level(EXAMPLE_1WIRE_GPIO) == 0;

    ulp_riscv_delay_cycles(420 * ULP_RISCV_CYCLES_PER_US);

    return presence_pulse;
}

int main (void)
{
    uint8_t temp_high_byte;
    uint8_t temp_low_byte;

    /* Setup GPIO used for 1wire */
    ulp_riscv_gpio_init(EXAMPLE_1WIRE_GPIO);
    ulp_riscv_gpio_input_enable(EXAMPLE_1WIRE_GPIO);
    ulp_riscv_gpio_output_enable(EXAMPLE_1WIRE_GPIO);
    ulp_riscv_gpio_set_output_mode(EXAMPLE_1WIRE_GPIO, RTCIO_MODE_OUTPUT_OD);
    ulp_riscv_gpio_pullup(EXAMPLE_1WIRE_GPIO);
    ulp_riscv_gpio_pulldown_disable(EXAMPLE_1WIRE_GPIO);


    switch (state) {
    case SENSOR_CONVERSION_INIT:
        if (!ds18b20_reset_pulse()) {
            temp_reg_val = INT32_MIN;
            break;
        }
        /* Start conversion */
        ds18b20_write_byte(0xCC);
        ds18b20_write_byte(0x44);

        /* shutdown and wait for next period (750ms) where the data is ready for reading */
        state = SENSOR_CONVERSION_READ;
        break;

    case SENSOR_CONVERSION_READ:
        if (!ds18b20_reset_pulse()) {
            temp_reg_val = INT32_MIN;
            state = SENSOR_CONVERSION_INIT;
            break;
        }

        /* Read scratchpad */
        ds18b20_write_byte(0xCC);
        ds18b20_write_byte(0xBE);

        temp_low_byte = ds18b20_read_byte();
        temp_high_byte = ds18b20_read_byte();

        temp_reg_val = temp_high_byte << 8;
        temp_reg_val |= temp_low_byte;
        state = SENSOR_CONVERSION_INIT;

        /* Wakes up the main CPU if the temperature exceeds the limit */
        if (temp_reg_val > TEMP_ALARM_LIMIT) {
            ulp_riscv_wakeup_main_processor();
        }

        break;
    }

    /* ulp_riscv_halt() is called automatically when main exits,
       main will be executed again at the next timeout period,
       according to ulp_set_wakeup_period()
     */
    return 0;
}
