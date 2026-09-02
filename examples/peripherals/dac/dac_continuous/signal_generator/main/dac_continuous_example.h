/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdint.h>
#include "soc/soc_caps.h"

#define TAG "signal_generator"

#if SOC_IS(ESP32) || SOC_IS(ESP32S2)
/* only support 8-bit code, packed into uint8_t */
typedef uint8_t dac_example_sample_t;
#elif SOC_IS(ESP32S31)
/* support 10-bit and 12-bit codes, both packed into uint16_t */
typedef uint16_t dac_example_sample_t;
#else
#error "Define packed sample type to match selected SOC and DAC channel config"
#endif

#define EXAMPLE_ARRAY_LEN     400  // Length of wave array

typedef enum {
    DAC_SINE_WAVE,
    DAC_TRIANGLE_WAVE,
    DAC_SAWTOOTH_WAVE,
    DAC_SQUARE_WAVE,
    DAC_WAVE_MAX,
} dac_example_wave_type_t;

extern const char* wave_name[DAC_WAVE_MAX];
extern dac_example_sample_t wave_data[DAC_WAVE_MAX][EXAMPLE_ARRAY_LEN];

/**
 * @brief Generate wave tables for the given bit width of the configured DAC channel
 *
 * @param resolution_bits The resolution bits width of the DAC channel
 */
void example_generate_wave(uint8_t resolution_bits);

/**
 * @brief Use DMA to convert continuously
 *
 */
void example_dac_continuous_by_dma(void);

/**
 * @brief Use timer to convert continuously
 *
 */
void example_dac_continuous_by_timer(void);

/**
 * @brief Print the example log information
 *
 * @param update_rate   DAC sample update rate (Hz)
 * @param wave_freq     The frequency of the wave
 */
void example_log_info(uint32_t update_rate, uint32_t wave_freq);
