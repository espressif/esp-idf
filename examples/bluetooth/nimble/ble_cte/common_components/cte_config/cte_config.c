/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "cte_config.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "soc/gpio_sig_map.h"
#include "esp_rom_gpio.h"

const char direction_finding_logo[] = {
"\n\
███████╗███████╗██████╗     ██████╗ ██╗     ███████╗                                                                      \n\
██╔════╝██╔════╝██╔══██╗    ██╔══██╗██║     ██╔════╝                                                                      \n\
█████╗  ███████╗██████╔╝    ██████╔╝██║     █████╗                                                                        \n\
██╔══╝  ╚════██║██╔═══╝     ██╔══██╗██║     ██╔══╝                                                                        \n\
███████╗███████║██║         ██████╔╝███████╗███████╗                                                                      \n\
╚══════╝╚══════╝╚═╝         ╚═════╝ ╚══════╝╚══════╝                                                                      \n\
                                                                                                                          \n\
██████╗ ██╗██████╗ ███████╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗    ███████╗██╗███╗   ██╗██████╗ ██╗███╗   ██╗ ██████╗  \n\
██╔══██╗██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║    ██╔════╝██║████╗  ██║██╔══██╗██║████╗  ██║██╔════╝  \n\
██║  ██║██║██████╔╝█████╗  ██║        ██║   ██║██║   ██║██╔██╗ ██║    █████╗  ██║██╔██╗ ██║██║  ██║██║██╔██╗ ██║██║  ███╗ \n\
██║  ██║██║██╔══██╗██╔══╝  ██║        ██║   ██║██║   ██║██║╚██╗██║    ██╔══╝  ██║██║╚██╗██║██║  ██║██║██║╚██╗██║██║   ██║ \n\
██████╔╝██║██║  ██║███████╗╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║    ██║     ██║██║ ╚████║██████╔╝██║██║ ╚████║╚██████╔╝ \n\
╚═════╝ ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝    ╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝  \n\
                                                                                                                          \n\
"
};

struct ble_gatt_register_ctxt;

#ifdef CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT
uint8_t antenna_use_gpio[CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT] = {
#if CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT > 0
    CONFIG_EXAMPLE_ANT_GPIO_0,
#endif
#if CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT > 1
    CONFIG_EXAMPLE_ANT_GPIO_1,
#endif
#if CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT > 2
    CONFIG_EXAMPLE_ANT_GPIO_2,
#endif
#if CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT > 3
    CONFIG_EXAMPLE_ANT_GPIO_3
#endif
};
#endif

#ifndef CTE_ANT0_IDX
#ifdef ANT_SEL0_IDX
#define CTE_ANT0_IDX ANT_SEL0_IDX
#define CTE_ANT1_IDX ANT_SEL1_IDX
#define CTE_ANT2_IDX ANT_SEL2_IDX
#define CTE_ANT3_IDX ANT_SEL3_IDX
#ifdef ANT_SEL4_IDX
#define CTE_ANT4_IDX ANT_SEL4_IDX
#define CTE_ANT5_IDX ANT_SEL5_IDX
#define CTE_ANT6_IDX ANT_SEL6_IDX
#define CTE_ANT7_IDX ANT_SEL7_IDX
#define CTE_ANT8_IDX ANT_SEL8_IDX
#define CTE_ANT9_IDX ANT_SEL9_IDX
#define CTE_ANT10_IDX ANT_SEL10_IDX
#define CTE_ANT11_IDX ANT_SEL11_IDX
#define CTE_ANT12_IDX ANT_SEL12_IDX
#define CTE_ANT13_IDX ANT_SEL13_IDX
#define CTE_ANT14_IDX ANT_SEL14_IDX
#define CTE_ANT15_IDX ANT_SEL15_IDX
#endif
#else
#error "CTE Antenna not support."
#endif
#endif


uint8_t antenna_signal_index[4] = {CTE_ANT0_IDX, CTE_ANT1_IDX, CTE_ANT2_IDX, CTE_ANT3_IDX};

int ble_direction_finding_antenna_init(uint8_t* gpio_array,uint8_t gpio_array_len){
    int rc;
    // GPIO configuration
    uint64_t gpio_pin_maks = 0;

    if (!gpio_array || gpio_array_len > 4 || gpio_array_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < gpio_array_len; i++){
        gpio_pin_maks |= (1ULL << gpio_array[i]);
    }
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = gpio_pin_maks,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    rc = gpio_config(&gpio_conf);
    if(rc != 0) {
        ESP_LOGE("DF","config fault GPIO failed");
        return rc;
    }
    // gpio bind signal
    for (int i = 0; i < gpio_array_len; i++){
        ESP_LOGI("DF","bind gpio %d to signal %d\n",gpio_array[i],antenna_signal_index[i]);
        esp_rom_gpio_connect_out_signal(gpio_array[i],antenna_signal_index[i],0,0);
    }
    return 0;
}
