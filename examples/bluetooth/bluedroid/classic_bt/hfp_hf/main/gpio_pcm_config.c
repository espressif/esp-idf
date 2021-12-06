/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "gpio_pcm_config.h"
#include "esp_rom_gpio.h"

#define GPIO_OUTPUT_PCM_FSYNC      (25)
#define GPIO_OUTPUT_PCM_CLK_OUT    (5)
#define GPIO_OUTPUT_PCM_DOUT       (26)
#define GPIO_INPUT_PCM_DIN         (35)

#define GPIO_OUTPUT_PCM_PIN_SEL  ((1ULL<<GPIO_OUTPUT_PCM_FSYNC) | (1ULL<<GPIO_OUTPUT_PCM_CLK_OUT) | (1ULL<<GPIO_OUTPUT_PCM_DOUT))

#define GPIO_INPUT_PCM_PIN_SEL (1ULL<<GPIO_INPUT_PCM_DIN)

void app_gpio_pcm_io_cfg(void)
{
    gpio_config_t io_conf;
    /// configure the PCM output pins
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PCM_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    /// configure the PCM input pin
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PCM_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    /// matrix out | in the internal PCM signals to the GPIOs
    esp_rom_gpio_connect_out_signal(GPIO_OUTPUT_PCM_FSYNC, PCMFSYNC_OUT_IDX, false, false);
    esp_rom_gpio_connect_out_signal(GPIO_OUTPUT_PCM_CLK_OUT, PCMCLK_OUT_IDX, false, false);
    esp_rom_gpio_connect_out_signal(GPIO_OUTPUT_PCM_DOUT, PCMDOUT_IDX, false, false);
    esp_rom_gpio_connect_in_signal(GPIO_INPUT_PCM_DIN, PCMDIN_IDX, false);
}

#if ACOUSTIC_ECHO_CANCELLATION_ENABLE

#define GPIO_OUTPUT_AEC_1      (19)
#define GPIO_OUTPUT_AEC_2      (21)
#define GPIO_OUTPUT_AEC_3      (22)
#define GPIO_OUTPUT_AEC_PIN_SEL  ((1ULL<<GPIO_OUTPUT_AEC_1) | (1ULL<<GPIO_OUTPUT_AEC_2) | (1ULL<<GPIO_OUTPUT_AEC_3))

void app_gpio_aec_io_cfg(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_AEC_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // set the output pins
    gpio_set_level(GPIO_OUTPUT_AEC_2, 0);

    gpio_set_level(GPIO_OUTPUT_AEC_1, 0);

    gpio_set_level(GPIO_OUTPUT_AEC_1, 1);

    gpio_set_level(GPIO_OUTPUT_AEC_3, 1);
}
#endif /* ACOUSTIC_ECHO_CANCELLATION_ENABLE */
