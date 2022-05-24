/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"
#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

/**
 * @brief DAC channel configuration
 *
 */
typedef struct {
    dac_channel_mask_t          chan_sel;       /*!< Using DAC channel mask to select the channels */
} dac_channels_config_t;

/**
 * @brief DAC continuous mode configration
 *
 */
typedef struct {
    uint32_t                    freq_hz;        /*!< The frequency of DAC conversion in continuous mode, unit: Hz
                                                 *   The supported range is related to the target and the clock source.
                                                 *   For the clock `DAC_DIGI_CLK_SRC_DEFAULT`: the range is 19.6 KHz to several MHz on ESP32
                                                 *   and 77 Hz to several MHz on ESP32-S2.
                                                 *   For the clock `DAC_DIGI_CLK_SRC_APLL`: the range is 648 Hz to several MHz on ESP32
                                                 *   and 6 Hz to several MHz on ESP32-S2.
                                                 *   Typically not suggest to set the frequency higher than 2 MHz, otherwise the severe distortion will appear
                                                 */
    dac_conti_clk_src_t         clk_src;        /*!< The clock source of digital controller, which can affect the range of supported frequency
                                                 *   Currently `DAC_DIGI_CLK_SRC_DEFAULT` and `DAC_DIGI_CLK_SRC_APLL` are available
                                                 */
    uint32_t                    desc_num;       /*!< The number of DMA descriptor, at least 2 descriptors are required
                                                 *   The number of descriptors is directly proportional to the max data buffer size while converting in cyclic output
                                                 *   but only need to ensure it is greater than '1' in acyclic output
                                                 *   Typically, suggest to set the number bigger than 5, in case the DMA stopped while sending a short buffer
                                                 */
    uint32_t                    buf_size;       /*!< The DMA buffer size, should be within 4092 bytes. Each DMA buffer will be attached to a DMA descriptor,
                                                 *   i.e. the number of DMA buffer will be equal to the DMA descriptor number
                                                 *   The DMA buffer size is not allowed to be greater than 4092 bytes
                                                 *   The total DMA buffer size equal to `desc_num * buf_size`
                                                 *   Typically, suggest to set the size to the multiple of 4
                                                 */
    dac_conti_channel_mode_t    chan_mode;      /*!< The channel mode of continuous mode, only take effect when multiple channels enabled, depends converting the buffer alternately or simultaneously */
} dac_conti_config_t;

/**
 * @brief DAC cosine wave gnerator configuration
 *
 */
typedef struct {
    uint32_t                    freq_hz;        /*!< The frequency of cosine wave, unit: Hz.
                                                 *   The cosine wave generator is driven by RTC clock which is about SOC_CLK_RC_FAST_FREQ_APPROX Hz by default,
                                                 *   With the default RTC clock, the minimun frequency of cosine wave is about 130 Hz,
                                                 *   Although it can support up to serveral MHz frequency theoretically,
                                                 *   the waveform will distort at high frequency due to the hardware limitation.
                                                 *   Typically not suggest to set the frequency higher than 200 KHz
                                                 */
    dac_cosine_clk_src_t        clk_src;        /*!< The clock source of the cosine wave generator, currently only support `DAC_COSINE_CLK_SRC_DEFAULT` which comes from RTC FAST clock */
    dac_cosine_scale_t          scale;          /*!< The scale of cosine wave amplitude */
    dac_cosine_phase_t          phase;          /*!< The phase of cosine wave */
    int8_t                      offset;         /*!< The DC offset of cosine wave */
} dac_cosine_config_t;

typedef struct dac_channels_s     *dac_channels_handle_t;     /*!< DAC channels' handle of DAC peripheral, one or multiple DAC channels can be controlled by this handle */

/*--------------------------------------------------------------------------
                            DAC common APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Allocate a new DAC channels' handle
 * @note The driver supports to manage one single channel by enabling only one channel in the channel mask,
 *       or multiple channels together as a whole by enabling multiple channels in the channel mask.
 *       Moreover, the channels can also be managed separately if they are allocated separately.
 *
 * @param[in]  dac_cfg   DAC basic configuration
 * @param[out] handle    DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_NO_MEM        No memory for a new DAC handle
 *      - ESP_ERR_INVALID_STATE The specified DAC channel is occupied already
 *      - ESP_OK                Success to allocate DAC channels
 */
esp_err_t dac_new_channels(const dac_channels_config_t *dac_cfg, dac_channels_handle_t *handle);

/**
 * @brief Delete and free the DAC channels
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels are not disabled
 *      - ESP_OK                Success to delete the channels
 */
esp_err_t dac_del_channels(dac_channels_handle_t handle);

/**
 * @brief Enabled the DAC channels in the channels
 * @note GPIOs of DAC channles will be enabled in this step
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels has enabled already or the channels are running
 *      - ESP_OK                Success to enable the channels
 */
esp_err_t dac_channels_enable(dac_channels_handle_t handle);

/**
 * @brief Disable the DAC channels in the channels
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels has disabled already or the channels are running
 *      - ESP_OK                Success to enable the channels
 */
esp_err_t dac_channels_disable(dac_channels_handle_t handle);

/*--------------------------------------------------------------------------
                    DAC direct voltage outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief DAC channels output a voltage directly
 * @note  This function is available when DAC chennels is enbled
 * @note  Please enable 'DAC ISR IRAM-Safe' in memuconfig when it is called in an IRAM safe ISR
 *
 * @param[in]  handle   DAC channels handle
 * @param[in]  value    The digital value of the voltage
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels are not enabled
 *      - ESP_OK                Success to enable the channels
 */
esp_err_t dac_channels_set_voltage(dac_channels_handle_t handle, uint8_t value);

/*--------------------------------------------------------------------------
                          DAC continuous outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Initialize the DAC channels to continuous mode
 * @note  DAC can convert digital data continuously in continuous mode
 *
 * @param[in]  handle   DAC channels handle
 * @param[in]  conti_cfg  DAC continuous mode configuration
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channels has been initialized already
 *      - ESP_ERR_NO_MEM        No memory for DAC continuous mode
 *      - ESP_OK                Success to initializing the DAC channels to continuous mode
 */
esp_err_t dac_channels_init_continuous_mode(dac_channels_handle_t handle, const dac_conti_config_t *conti_cfg);

/**
 * @brief Deinitialize the continuous mode of the DAC channels
 * @note  It can only be deinitialized when the continuous output is disabled
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode is not disabled yet
 *      - ESP_OK                Success to deinitialize the DAC continuous mode
 */
esp_err_t dac_channels_deinit_continuous_mode(dac_channels_handle_t handle);

/**
 * @brief Enable the DAC continuous output
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode has been enabled already
 *      - ESP_OK                Success to start the continuous output
 */
esp_err_t dac_channels_enable_continuous_mode(dac_channels_handle_t handle);

/**
 * @brief Disable the DAC continuous output
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode is disabled already
 *      - ESP_OK                Success to stop the continuous output
 */
esp_err_t dac_channels_disable_continuous_mode(dac_channels_handle_t handle);

/**
 * @brief Write DAC continuous data continuously
 * @note  The data in buffer will only be converted one time,
 *        This function will be blocked until all data loaded or timeout
 *        then the DAC output will keep outputting the voltage of the last data in the buffer
 * @note  On ESP32, the data bit width of DAC continuous data is fixed to 16 bits while only the high 8 bits are available,
 *        you can align the DAC data to 16 bits manually or set `CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` to get the correct wave.
 *        But the data bit width is already 8 bits on ESP32-S2, each byte stands for an vailable voltage,
 *        no need to do any alignment.
 *
 * @param[in]  handle   DAC channels handle
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[out] bytes_loaded The bytes that has been loaded into DMA buffer, can be NULL if don't need it
 * @param[in]  timeout_ms The timeout time in mili-second
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode has not been enabled yet
 *      - ESP_ERR_TIMEOUT       Waiting for semaphore or message queue timeout
 *      - ESP_OK                Success to output the acyclic DAC data
 */
esp_err_t dac_channels_write_continuously(dac_channels_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, uint32_t timeout_ms);

/**
 * @brief Write DAC continuous data cyclically
 * @note  The data in buffer will be converted cyclically once this function is called,
 *        This function won't be blocked, it will return once the data loaded into DMA buffers
 * @note  The buffer size of cyclically output is limited by the descriptor number while initializing the continuous mode,
 *        Concretely, in order to load all the data into descriptors,
 *        the cyclic buffer size is not supposed to be greater than `desc_num * 4092`
 * @note  On ESP32, the data bit width of DAC continuous data is fixed to 16 bits while only the high 8 bits are available,
 *        you can align the DAC data to 16 bits manually or set `CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` to get the correct wave.
 *        But the data bit width is already 8 bits on ESP32-S2, each byte stands for an vailable voltage,
 *        no need to do any alignment.
 *
 * @param[in]  handle   DAC channels handle
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[out] bytes_loaded The bytes that has been loaded into DMA buffer, can be NULL if don't need it
 * @param[in]  timeout_ms The timeout time in mili-second
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode has not been enabled yet
 *      - ESP_ERR_TIMEOUT       Waiting for semaphore or message queue timeout
 *      - ESP_OK                Success to output the acyclic DAC data
 */
esp_err_t dac_channels_write_cyclically(dac_channels_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, uint32_t timeout_ms);

/*--------------------------------------------------------------------------
                        DAC cosine wave outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Initialize the DAC channels to cosine wave mode
 *
 * @param[in]  handle   DAC channels handle
 * @param[in]  cw_cfg   DAC cosine wave generater configuration
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channels has been initialized already
 *      - ESP_OK                Success to initialize the DAC channels into cosine wave mode
 */
esp_err_t dac_channels_init_cosine_mode(dac_channels_handle_t handle, const dac_cosine_config_t *cw_cfg);

/**
 * @brief Deinitialize the DAC channels to cosine wave mode
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC cosine wave generator is not stopped yet
 *      - ESP_OK                Success to deinitialize the DAC cosine mode
 */
esp_err_t dac_channels_deinit_cosine_mode(dac_channels_handle_t handle);

/**
 * @brief Start the DAC cosine wave generator output
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channels has not been enabled yet or started already
 *      - ESP_OK                Success to start cosine wave generator
 */
esp_err_t dac_channels_start_cosine_output(dac_channels_handle_t handle);

/**
 * @brief Stop the DAC cosine wave generator output
 *
 * @param[in]  handle   DAC channels handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channels has not been enabled yet or stoppped already
 *      - ESP_OK                Success to stop cosine wave generator
 */
esp_err_t dac_channels_stop_cosine_output(dac_channels_handle_t handle);

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
