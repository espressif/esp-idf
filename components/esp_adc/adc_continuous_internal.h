/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
//For DMA
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "hal/spi_types.h"
#include "esp_private/spi_common_internal.h"
#elif CONFIG_IDF_TARGET_ESP32
#include "driver/i2s_types.h"
#endif

#include "esp_adc/adc_filter.h"
#include "esp_adc/adc_monitor.h"
#include "adc_dma_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADC_FSM_INIT,
    ADC_FSM_STARTED,
} adc_fsm_t;

typedef enum {
    ADC_MONITOR_FSM_INIT,
    ADC_MONITOR_FSM_ENABLED,
} adc_monitor_fsm_t;

/*---------------------------------------------------------------
            Driver Context
---------------------------------------------------------------*/
typedef struct adc_iir_filter_t adc_iir_filter_t;
typedef struct adc_monitor_t adc_monitor_t;
typedef struct adc_continuous_ctx_t adc_continuous_ctx_t;
typedef bool (*adc_dma_intr_func_t)(adc_continuous_ctx_t *adc_digi_ctx);

/**
 * @brief ADC iir filter context
 */
struct adc_iir_filter_t {
    adc_digi_iir_filter_t               filter_id;              // Filter ID
    adc_continuous_iir_filter_config_t  cfg;                    //filter configuration
    adc_continuous_ctx_t                *continuous_ctx;        //ADC continuous driver context
};

/**
 * @brief ADC digi monitor context
 */
struct adc_monitor_t {
    adc_monitor_id_t        monitor_id;         // monitor unit number
    adc_monitor_fsm_t       fsm;                // monitor status indicator
    adc_monitor_config_t    config;             // monitor configuration
    adc_monitor_evt_cbs_t   cbs;                // monitor thresh callbacks
    void                    *user_data;         // user data pointer to use in cb
};

/**
 * @brief ADC continuous driver context
 */
struct adc_continuous_ctx_t {
    uint8_t                         *rx_dma_buf;                //dma buffer
    adc_hal_dma_ctx_t               hal;                        //hal context
    RingbufHandle_t                 ringbuf_hdl;                //RX ringbuffer handler
    void*                           ringbuf_storage;            //Ringbuffer storage buffer
    void*                           ringbuf_struct;             //Ringbuffer structure buffer
    size_t                          ringbuf_size;               //Ringbuffer size
    intptr_t                        rx_eof_desc_addr;           //eof descriptor address of RX channel
    adc_fsm_t                       fsm;                        //ADC continuous mode driver internal states
    bool                            use_adc1;                   //1: ADC unit1 will be used; 0: ADC unit1 won't be used.
    bool                            use_adc2;                   //1: ADC unit2 will be used; 0: ADC unit2 won't be used. This determines whether to acquire sar_adc2_mutex lock or not.
    adc_atten_t                     adc1_atten;                 //Attenuation for ADC1. On this chip each ADC can only support one attenuation.
    adc_atten_t                     adc2_atten;                 //Attenuation for ADC2. On this chip each ADC can only support one attenuation.
    adc_hal_digi_ctrlr_cfg_t        hal_digi_ctrlr_cfg;         //Hal digital controller configuration
    adc_continuous_evt_cbs_t        cbs;                        //Callbacks
    void                            *user_data;                 //User context
    esp_pm_lock_handle_t            pm_lock;                    //For power management
    struct {
        uint32_t flush_pool: 1;     //Flush the internal pool when the pool is full. With this flag, the `on_pool_ovf` event will not happen.
    } flags;
#if SOC_ADC_DIG_IIR_FILTER_SUPPORTED
    adc_iir_filter_t                *iir_filter[SOC_ADC_DIGI_IIR_FILTER_NUM];  //ADC IIR filter context
#endif
#if SOC_ADC_MONITOR_SUPPORTED
    adc_monitor_t                   *adc_monitor[SOC_ADC_DIGI_MONITOR_NUM];    // adc monitor context
#endif
    adc_dma_t                       adc_dma;
    adc_dma_intr_func_t             adc_intr_func;
};

#ifdef __cplusplus
}
#endif
