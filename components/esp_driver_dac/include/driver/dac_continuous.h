/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

/**
 * @brief DAC channel mask
 *
 */
typedef enum {
    DAC_CHANNEL_MASK_CH0  = BIT(0),             /*!< DAC channel 0 is GPIO25(ESP32) / GPIO17(ESP32S2) */
    DAC_CHANNEL_MASK_CH1  = BIT(1),             /*!< DAC channel 1 is GPIO26(ESP32) / GPIO18(ESP32S2) */
    DAC_CHANNEL_MASK_ALL = BIT(0) | BIT(1),     /*!< Both DAC channel 0 and channel 1 */
} dac_channel_mask_t;

typedef struct dac_continuous_s      *dac_continuous_handle_t;    /*!< DAC continuous channel handle */

/**
 * @brief DAC continuous channels' configurations
 *
 */
typedef struct {
    dac_channel_mask_t          chan_mask;      /*!< DAC channels' mask for selecting which channels are used */
    uint32_t                    desc_num;       /*!< The number of DMA descriptor, at least 2 descriptors are required
                                                 *   The number of descriptors is directly proportional to the max data buffer size while converting in cyclic output
                                                 *   but only need to ensure it is greater than '1' in acyclic output
                                                 *   Typically, suggest to set the number bigger than 5, in case the DMA stopped while sending a short buffer
                                                 */
    size_t                      buf_size;       /*!< The DMA buffer size, should be within 32~4092 bytes. Each DMA buffer will be attached to a DMA descriptor,
                                                 *   i.e. the number of DMA buffer will be equal to the DMA descriptor number
                                                 *   The DMA buffer size is not allowed to be greater than 4092 bytes
                                                 *   The total DMA buffer size equal to `desc_num * buf_size`
                                                 *   Typically, suggest to set the size to the multiple of 4
                                                 */
    uint32_t                    freq_hz;        /*!< The frequency of DAC conversion in continuous mode, unit: Hz
                                                 *   The supported range is related to the target and the clock source.
                                                 *   For the clock `DAC_DIGI_CLK_SRC_DEFAULT`: the range is 19.6 KHz to several MHz on ESP32
                                                 *   and 77 Hz to several MHz on ESP32-S2.
                                                 *   For the clock `DAC_DIGI_CLK_SRC_APLL`: the range is 648 Hz to several MHz on ESP32
                                                 *   and 6 Hz to several MHz on ESP32-S2.
                                                 *   Typically not suggest to set the frequency higher than 2 MHz, otherwise the severe distortion will appear
                                                 */
    int8_t                      offset;         /*!< The offset of the DAC digital data. Range -128~127 */
    dac_continuous_digi_clk_src_t    clk_src;        /*!< The clock source of digital controller, which can affect the range of supported frequency
                                                 *   Currently `DAC_DIGI_CLK_SRC_DEFAULT` and `DAC_DIGI_CLK_SRC_APLL` are available
                                                 */
    dac_continuous_channel_mode_t    chan_mode;      /*!< The channel mode of continuous mode, only take effect when multiple channels enabled, depends converting the buffer alternately or simultaneously */
} dac_continuous_config_t;

/**
 * @brief Event structure used in DAC event queue
 */
typedef struct {
    void                *buf;           /*!< The pointer of DMA buffer that just finished sending */
    size_t              buf_size;       /*!< The writable buffer size of the DMA buffer, equal to 'dac_continuous_config_t::buf_size' */
    size_t              write_bytes;    /*!< The number of bytes that be written successfully */
} dac_event_data_t;

/**
 * @brief DAC event callback
 * @param[in] handle    DAC channel handle, created from `dac_continuous_new_channels()`
 * @param[in] event     DAC event data
 * @param[in] user_data User registered context, passed from `dac_continuous_register_event_callback()`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*dac_isr_callback_t)(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data);

/**
 * @brief Group of DAC callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_DAC_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    dac_isr_callback_t on_convert_done;     /**< Callback of data conversion done event
                                             *   An event data buffer previously loaded to the driver has been output and converted.
                                             *   The event data includes DMA buffer address and size that just finished converting.
                                             */
    dac_isr_callback_t on_stop;             /**< Callback of finished sending all the data.
                                             *   All loaded event data buffers are converted. Driver is pending for new data buffers to be loaded.
                                             *   The event data will be NULL in this callback.
                                             */
} dac_event_callbacks_t;

/**
 * @brief Allocate new DAC channels in continuous mode
 * @note  The DAC channels can't be registered to continuous mode separately
 *
 * @param[in]  cont_cfg    Continuous mode configuration
 * @param[out] ret_handle   The returned continuous mode handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel has been registered already
 *      - ESP_ERR_NOT_FOUND     Not found the available dma peripheral, might be occupied
 *      - ESP_ERR_NO_MEM        No memory for the DAC continuous mode resources
 *      - ESP_OK                Allocate the new DAC continuous mode success
 */
esp_err_t dac_continuous_new_channels(const dac_continuous_config_t *cont_cfg, dac_continuous_handle_t *ret_handle);

/**
 * @brief Delete the DAC continuous handle
 *
 * @param[in]  handle       The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels have already been deregistered or not disabled
 *      - ESP_OK                Delete the continuous channels success
 */
esp_err_t dac_continuous_del_channels(dac_continuous_handle_t handle);

/**
 * @brief Enabled the DAC continuous mode
 * @note  Must enable the channels before
 *
 * @param[in]  handle       The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels have been enabled already
 *      - ESP_OK                Enable the continuous output success
 */
esp_err_t dac_continuous_enable(dac_continuous_handle_t handle);

/**
 * @brief Disable the DAC continuous mode
 *
 * @param[in]  handle       The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channels have been enabled already
 *      - ESP_OK                Disable the continuous output success
 */
esp_err_t dac_continuous_disable(dac_continuous_handle_t handle);

/**
 * @brief Write DAC data continuously
 * @note  The data in buffer will only be converted one time,
 *        This function will be blocked until all data loaded or timeout
 *        then the DAC output will keep outputting the voltage of the last data in the buffer
 * @note  Specially, on ESP32, the data bit width of DAC continuous data is fixed to 16 bits while only the high 8 bits are available,
 *        The driver will help to expand the inputted buffer automatically by default,
 *        you can also align the data to 16 bits manually by clearing `CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` in menuconfig.
 *
 * @param[in]  handle   The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[out] bytes_loaded The bytes that has been loaded into DMA buffer, can be NULL if don't need it
 * @param[in]  timeout_ms The timeout time in millisecond, set a minus value means will block forever
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode has not been enabled yet
 *      - ESP_ERR_TIMEOUT       Waiting for semaphore or message queue timeout
 *      - ESP_OK                Success to output the acyclic DAC data
 */
esp_err_t dac_continuous_write(dac_continuous_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, int timeout_ms);

/**
 * @brief Write DAC continuous data cyclically
 * @note  The data in buffer will be converted cyclically using DMA once this function is called,
 *        This function will return once the data loaded into DMA buffers.
 * @note  The buffer size of cyclically output is limited by the descriptor number and
 *        dma buffer size while initializing the continuous mode.
 *        Concretely, in order to load all the data into descriptors,
 *        the cyclic buffer size is not supposed to be greater than `desc_num * buf_size`
 * @note  Specially, on ESP32, the data bit width of DAC continuous data is fixed to 16 bits while only the high 8 bits are available,
 *        The driver will help to expand the inputted buffer automatically by default,
 *        you can also align the data to 16 bits manually by clearing `CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` in menuconfig.
 *
 * @param[in]  handle   The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[out] bytes_loaded The bytes that has been loaded into DMA buffer, can be NULL if don't need it
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC continuous mode has not been enabled yet
 *      - ESP_OK                Success to output the acyclic DAC data
 */
esp_err_t dac_continuous_write_cyclically(dac_continuous_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded);

/**
 * @brief Set event callbacks for DAC continuous mode
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `callbacks` structure to NULL.
 * @note When CONFIG_DAC_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in this function, including the `user_data`, should be in the internal RAM as well.
 *
 * @param[in] handle        The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @param[in] callbacks     Group of callback functions, input NULL to clear the former callbacks
 * @param[in] user_data     User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK                Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG   Set event callbacks failed because of invalid argument
 */
esp_err_t dac_continuous_register_event_callback(dac_continuous_handle_t handle, const dac_event_callbacks_t *callbacks, void *user_data);

/**
 * @brief  Start the async writing
 * @note   When the asynchronous writing start, the DAC will keep outputting '0' until the data are loaded into the DMA buffer.
 *         To loaded the data into DMA buffer, 'on_convert_done' callback is required,
 *         which can be registered by 'dac_continuous_register_event_callback' before enabling
 *
 * @param[in] handle        The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @return
 *      - ESP_OK                Start asynchronous writing successfully
 *      - ESP_ERR_INVALID_ARG   The handle is NULL
 *      - ESP_ERR_INVALID_STATE The channel is not enabled or the 'on_convert_done' callback is not registered
 */
esp_err_t dac_continuous_start_async_writing(dac_continuous_handle_t handle);

/**
 * @brief Stop the sync writing
 *
 * @param[in] handle        The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @return
 *      - ESP_OK                Stop asynchronous writing successfully
 *      - ESP_ERR_INVALID_ARG   The handle is NULL
 *      - ESP_ERR_INVALID_STATE Asynchronous writing has not started
 */
esp_err_t dac_continuous_stop_async_writing(dac_continuous_handle_t handle);

/**
 * @brief Write DAC data asynchronously
 * @note  This function can be called when the asynchronous writing started, and it can be called in the callback directly
 *        but recommend to writing data in a task, referring to :example:`peripherals/dac/dac_continuous/dac_audio`
 *
 * @param[in]  handle        The DAC continuous channel handle that obtained from 'dac_continuous_new_channels'
 * @param[in]  dma_buf       The DMA buffer address, it can be acquired from 'dac_event_data_t' in the 'on_convert_done' callback
 * @param[in]  dma_buf_len   The DMA buffer length, it can be acquired from 'dac_event_data_t' in the 'on_convert_done' callback
 * @param[in]  data          The data that need to be written
 * @param[in]  data_len      The data length the need to be written
 * @param[out] bytes_loaded  The bytes number that has been loaded/written into the DMA buffer
 * @return
 *      - ESP_OK                        Write the data into DMA buffer successfully
 *      - ESP_ERR_INVALID_ARG           NULL pointer
 *      - ESP_ERR_INVALID_STATE         The channels haven't start the asynchronous writing
 *      - ESP_ERR_NOT_FOUND             The param 'dam_buf' not match any existed DMA buffer
 */
esp_err_t dac_continuous_write_asynchronously(dac_continuous_handle_t handle,
                                              uint8_t *dma_buf,
                                              size_t dma_buf_len,
                                              const uint8_t *data,
                                              size_t data_len,
                                              size_t *bytes_loaded);

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
