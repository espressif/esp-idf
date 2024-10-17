// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "driver/adc.h"
// #include "esp_adc/adc_continuous.h"
// #include "esp_adc_cal.h"

// #define TAG "ADC_DMA"
// #define ADC_CHANNEL ADC1_CHANNEL_2 // GPIO1
// #define ADC_ATTEN ADC_ATTEN_DB_11  // 11dB attenuation for 3.3V range
// #define READ_LEN 1024              

// static adc_continuous_handle_t adc_handle;

// void init_adc_dma(void)
// {
//     adc_continuous_handle_cfg_t adc_config = {
//         .max_store_buf_size = 1024,     // Buffer size for DMA
//         .conv_frame_size = READ_LEN,    // Size of each ADC frame to read
//     };

//     adc_continuous_new_handle(&adc_config, &adc_handle);

//     adc_continuous_config_t adc_dma_config = {
//         .sample_freq_hz = 20 * 1000, // 20 kHz sample rate
//         .conv_mode = ADC_CONV_SINGLE_UNIT_1, // Single ADC unit (ADC1)
//         .format =  ADC_DIGI_OUTPUT_FORMAT_TYPE2, // 12-bit output format
//     };

//     adc_digi_pattern_config_t adc_pattern = {
//         .atten = ADC_ATTEN_DB_11, // 11 dB attenuation for 0-3.3V input
//         .channel = ADC_CHANNEL,   
//         .bit_width = ADC_WIDTH_BIT_12 // 12-bit resolution
//     };
//     adc_dma_config.adc_pattern = &adc_pattern;
//     adc_dma_config.pattern_num = 1;

//     // Apply configuration and start continuous ADC
//     adc_continuous_config(adc_handle, &adc_dma_config);
//     adc_continuous_start(adc_handle);
// }

// void app_main(void)
// {
//     init_adc_dma(); 

//     uint8_t adc_data[READ_LEN] = {0};  
//     uint32_t out_length = 0; 
//     int raw_adc;
    
//     while (1)
//     {
//         esp_err_t ret = adc_continuous_read(adc_handle, adc_data, READ_LEN, &out_length, portMAX_DELAY);
//         if (ret == ESP_OK )
//         {
//             for (int i = 0; i < 4; i += sizeof(adc_digi_output_data_t))
//             {
//                 adc_digi_output_data_t *adc_output = (adc_digi_output_data_t *)&adc_data[i];
//                 raw_adc = adc_output->type2.data;
//                 ESP_LOGI(TAG, "ADC: %d, Voltage: %.2fV", raw_adc, ((raw_adc * 3.3) / 4095));
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }


/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_continuous.h"

#include "soc/gpio_periph.h"
#include "soc/soc_caps.h"
#include "hal/gpio_ll.h"
#include "hal/gpio_types.h"
#include "soc/soc.h"    
#include "soc/gpio_sig_map.h"

#include "soc/soc.h"
#include "soc/apb_saradc_reg.h"
#include "rom/ets_sys.h"
#include "xtensa/xtensa_api.h"

#define EXAMPLE_READ_LEN   8
#define GET_UNIT(x)        ((x>>3) & 0x1)


#define ADC_CONV_MODE       ADC_CONV_BOTH_UNIT
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2


static adc_channel_t channel[2] = {ADC_CHANNEL_2, (ADC_CHANNEL_0 | 1 << 3)};


uint32_t cb_num = 0; 
static const char *TAG = "EXAMPLE";


void IRAM_ATTR my_int(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    //TOGGLE GPIO
    (&GPIO)->out_w1ts = (1 << 15);
    (&GPIO)->out_w1tc = (1 << 15);
    cb_num++; 
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 32,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };
    adc_continuous_new_handle(&adc_config, &handle);

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 10 * 1000,
        .conv_mode = ADC_CONV_BOTH_UNIT,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        uint8_t unit = GET_UNIT(channel[i]);
        uint8_t ch = channel[i] & 0x7;
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = ch;
        adc_pattern[i].unit = unit;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    dig_cfg.adc_pattern = adc_pattern;
    
    adc_continuous_config(handle, &dig_cfg);

    *out_handle = handle;
}

static bool check_valid_data(const adc_digi_output_data_t *data)
{
    const unsigned int unit = data->type2.unit;
    if (unit > 2) return false;
    if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) return false;

    return true;
}

void app_main(void)
{
    //Set GPIO as OUTPUT
    (&GPIO)->enable_w1ts = (0x1 << 15);
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};
    memset(result, 0xcc, EXAMPLE_READ_LEN);

    adc_continuous_handle_t handle = NULL;
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);

    /*Setting up the Interrupt*/
    /*APB_SARADC_ADC1_DONE_INT: Triggered when SAR ADC1 completes one data conversion.*/
    REG_SET_BIT(APB_SARADC_INT_ENA_REG, APB_SARADC_ADC1_DONE_INT_ENA); 
    xt_ints_off(1<<13);
    intr_matrix_set(0, 65, 13);
    xt_ints_on(1<<13);
    xt_set_interrupt_handler(13,  my_int, NULL); 
    

    ESP_ERROR_CHECK(adc_continuous_start(handle));

    while(1) {
        while (1) {
            ret = adc_continuous_read(handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
            
            if (ret == ESP_OK) {
                ESP_LOGI("TASK", "ret is %d, ret_num is %"PRIu32, ret, ret_num);
                ESP_LOGI("TASK", "cb_num %ld" , cb_num);
                for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                    adc_digi_output_data_t *p = (void*)&result[i];
                    if (ADC_CONV_MODE == ADC_CONV_BOTH_UNIT || ADC_CONV_MODE == ADC_CONV_ALTER_UNIT) {
                        if (check_valid_data(p)) {
                            ESP_LOGI(TAG, "Unit: %d,_Channel: %d, Value: %d", p->type2.unit+1, p->type2.channel, p->type2.data);
                        } else {
                            ESP_LOGI(TAG, "Invalid data [%d_%d_%x]", p->type2.unit+1, p->type2.channel, p->type2.data);
                        }
                    }
                }
                vTaskDelay(1);
            } 
        }
    }  
}
