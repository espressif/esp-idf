/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "hal/i2s_types.h"

#include "esp_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef I2S_DRIVER_SELECTED
    #error "Only one set of APIs is allowed to use"
#else
    #define I2S_DRIVER_SELECTED
#endif

/**
 * @brief get default I2S property
 */
#define I2S_CHANNEL_DEFAULT_CONFIG(i2s_num, i2s_role) { \
    .id = i2s_num, \
    .role = i2s_role, \
    .dma_desc_num = 3, \
    .dma_frame_num = 500, \
    .auto_clear = false, \
}

#define I2S_GPIO_UNUSED         GPIO_NUM_NC         /*!< Used in i2s_gpio_config_t for signals which are not used */

/**
 * @brief I2S controller channel configuration
*/
typedef struct {
    i2s_port_t              id;     /*!< I2S port id */
    i2s_role_t              role;   /*!< I2S role, I2S_ROLE_MASTER or I2S_ROLE_SLAVE */

    /* DMA configurations */
    uint32_t                dma_desc_num;   /*!< I2S DMA buffer number, it is also the number of DMA descriptor */
    uint32_t                dma_frame_num;  /*!< I2S frame number in one DMA buffer. One frame means one-time sample data in all slots */
    bool                    auto_clear;     /*!< Set to auto clear DMA TX descriptor, i2s will always send zero automatically if no data to send */

} i2s_chan_config_t;

/**
 * @brief Allocate a new i2s channel
 * @note  The new i2s channel handle will at REGISTERED state after it is allocated successfully.
 * @note  When the port id in channel configuration is I2S_NUM_AUTO, driver will allocate i2s port automatically
 *        Otherwise driver will try to allocate the new channel on the selected port.
 * @note  If both tx_handle and rx_handle are not NULL, it means this I2S port will work at duplex mode,
 *        and these rx and tx channel will be allocated on a same I2S port as well.
 *        Note that tx/rx channel will be affected by each other on ESP32 and ESP32S2,
 *        so please make sure them are working in same condition and have same status(start/stop).
 * @note  If tx_handle or rx_handle is NULL, it means this I2S port will work at simplex mode,
 *        For ESP32 and ESP32S2, the whole I2S port will be occupied as well even if only simplex is working.
 *        For the other targets, another channel on this port will still available.
 *
 * @param[in]   chan_cfg    I2S controller channel configurations
 * @param[out]  tx_handle   I2S channel handler used for managing the sending channel(optional)
 * @param[out]  rx_handle   I2S channel handler used for managing the receiving channel(optional)
 * @return
 *      - ESP_OK    Allocate a new channel success
 *      - ESP_ERR_NOT_SUPPORTED The communication mode is not supported on the current chip
 *      - ESP_ERR_INVALID_ARG   NULL pointer or illegal parameter in i2s_chan_config_t
 *      - ESP_ERR_NOT_FOUND     No available I2S channel found
 */
esp_err_t i2s_new_channel(const i2s_chan_config_t *chan_cfg, i2s_chan_handle_t *tx_handle, i2s_chan_handle_t *rx_handle);

/**
 * @brief Delete the i2s channel
 * @note  The i2s channel will be stopped to ensure the i2s channel is not at READING or WRITING state.
 * @note  Resource will be free automatically if all channels in one port are deleted
 *
 * @param[in]   handle      I2S channel handler
 *      - ESP_OK    Delete successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 */
esp_err_t i2s_del_channel(i2s_chan_handle_t handle);

/**
 * @brief Initialize i2s channel
 * @note  Only allowed to be called when the channel state is REGISTERED, (i.e., channel has been allocated, but not initialized)
 *        and the state will be updated to INITIALIZING while during the initialization,
 *        finally it will be READY if initialization is success, otherwise the state will return to REGISTERED.
 * @note  Will initialize i2s channel according to the i2s mode
 *        For different modes, we should input corresponding configurations.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   clk_config  Clock configuration, should input correct type of clock configuration according to i2s communication mode that set in 'i2s_chan_config_t'.
 *                          For I2S_COMM_MODE_STD mode, please input 'i2s_std_clk_config_t' type.
 *                          For I2S_COMM_MODE_PDM mode, please input 'i2s_pdm_tx_clk_config_t' type for tx channel and 'i2s_pdm_rx_clk_config_t' type for rx channel.
 *                          For I2S_COMM_MODE_TDM mode, please input 'i2s_tdm_clk_config_t' type.
 * @param[in]   slot_config Slot configuration, should input correct type of slot configuration according to i2s communication mode that set in 'i2s_chan_config_t'.
 *                          For I2S_COMM_MODE_STD mode, please input 'i2s_std_slot_config_t' type.
 *                          For I2S_COMM_MODE_PDM mode, please input 'i2s_pdm_tx_slot_config_t' type for tx channel and 'i2s_pdm_rx_slot_config_t' type for rx channel.
 *                          For I2S_COMM_MODE_TDM mode, please input 'i2s_tdm_slot_config_t' type.
 * @return
 *      - ESP_OK    Initialize successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE This channel is not registered
 */
// esp_err_t i2s_init_channel(i2s_chan_handle_t handle, const void *clk_config, const void *slot_config);

/**
 * @brief Reconfigure the I2S clock
 * @note  Only allowed to be called when the channel state is READY, (i.e., channel has been initialized, but not started)
 *        this function won't change the state.
 * @note  Normally the clock has been configured after 'i2s_init_channel' is called
 *        This function is for re-configuring the clock.
 *        'i2s_stop_channel' should be called before calling this function if i2s has started.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   clk_config  Clock configuration, should input correct type of clock configuration according to i2s communication mode
 *                          For I2S_COMM_MODE_STD mode, please input 'i2s_std_clk_config_t' type.
 *                          For I2S_COMM_MODE_PDM mode, please input 'i2s_pdm_tx_clk_config_t' type for tx channel and 'i2s_pdm_rx_clk_config_t' type for rx channel.
 *                          For I2S_COMM_MODE_TDM mode, please input 'i2s_tdm_clk_config_t' type.
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_NOT_SUPPORTED The input communication mode is not supported
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE This channel is not initialized
 */
// esp_err_t i2s_set_clock(i2s_chan_handle_t handle, const void *clk_config);

/**
 * @brief Reconfigure the I2S slot
 * @note  Only allowed to be called when the channel state is READY, (i.e., channel has been initialized, but not started)
 *        this function won't change the state.
 * @note  Normally the slot has been configured after 'i2s_init_channel' is called
 *        This function is for re-configuring the slot
 *        'i2s_stop_channel' should be called before calling this function if i2s has started.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   slot_config Slot configuration, should input correct type of clock configuration according to i2s communication mode
 *                          For I2S_COMM_MODE_STD mode, please input 'i2s_std_slot_config_t' type.
 *                          For I2S_COMM_MODE_PDM mode, please input 'i2s_pdm_tx_slot_config_t' type for tx channel and 'i2s_pdm_rx_slot_config_t' type for rx channel.
 *                          For I2S_COMM_MODE_TDM mode, please input 'i2s_tdm_slot_config_t' type.
 * @return
 *      - ESP_OK    Set slot successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer or unmatched slot configuration type
 *      - ESP_ERR_INVALID_STATE This channel is not initialized
 */
// esp_err_t i2s_set_slot(i2s_chan_handle_t handle, const void *slot_config);

/**
 * @brief Get I2S event queue handler
 * @note  Can be called at any time
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   que_len     Queue length (if the queue has not been created yet)
 * @return
 *      - NULL  Failed to create the event queue
 *      - else  Event queue handler
 */
QueueHandle_t i2s_get_event_queue(i2s_chan_handle_t handle, uint32_t que_len);

/**
 * @brief Start i2s channel
 * @note  Only allowed to be called when the channel state is READY, (i.e., channel has been initialized, but not started)
 *        the channel will enter IDLE state once it is started successfully.
 *
 * @param[in]   handle      I2S channel handler
 *      - ESP_OK    Start successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE This channel has not initialized or already started
 */
esp_err_t i2s_start_channel(i2s_chan_handle_t handle);

/**
 * @brief Stop i2s channel
 * @note  Only allowed to be called when the channel state is READY / IDLE / WRITING / READING, (i.e., channel has been initialized)
 *        the channel will enter READY state once it is stopped successfully.
 * @note  It will stop bclk and ws signal but not mclk signal
 *
 * @param[in]   handle      I2S channel handler
 * @return
 *      - ESP_OK    Stop successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE This channel has not stated
 */
esp_err_t i2s_stop_channel(i2s_chan_handle_t handle);

/**
 * @brief I2S write data
 * @note  Only allowed to be called when the channel state is IDLE, (i.e., tx channel has been started and is not writing now)
 *        the channel will enter WRITING state once start to write,
 *        and it will switch back to IDLE when quit the writing,
 *        but the IDLE only stands for the software state, it doesn't mean there is no the signal transporting on line.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   src         The pointer of sent data buffer
 * @param[in]   size        Max data buffer length
 * @param[out]  bytes_written   Byte number that actually be sent
 * @param[in]   ticks_to_wait   Max block time
 * @return
 *      - ESP_OK    Write successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer or this handle is not tx handle
 *      - ESP_ERR_TIMEOUT       Writing timeout, no writing event received from ISR within ticks_to_wait
 *      - ESP_ERR_INVALID_STATE I2S is not ready to write
 */
esp_err_t i2s_write_channel(i2s_chan_handle_t handle, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait);

/**
 * @brief I2S read data
 * @note  Only allowed to be called when the channel state is IDLE
 *        the channel will enter READING state once start to read,
 *        and it will switch back to IDLE when quit the reading,
 *        but the IDLE only stands for the software state, it doesn't mean there is no the signal transporting on line.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   dest        The pointer of receiving data buffer
 * @param[in]   size        Max data buffer length
 * @param[out]  bytes_read      Byte number that actually be read
 * @param[in]   ticks_to_wait   Max block time
 * @return
 *      - ESP_OK    Read successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer or this handle is not rx handle
 *      - ESP_ERR_TIMEOUT       Reading timeout, no reading event received from ISR within ticks_to_wait
 *      - ESP_ERR_INVALID_STATE I2S is not ready to read
 */
esp_err_t i2s_read_channel(i2s_chan_handle_t handle, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);

/**
 * @brief Clear the DMA buffer
 * @note  Only allowed to be called when the channel state is READY / IDLE / WRITING / READING, (i.e., channel has been initialized)
 *        it won't change the current channel state.
 * @note  The legacy data in DMA buffer will be cleared immediately once it is called
 *        That means i2s will keep send zero if no other data to send
 *        It has same effect with 'auto_clear' field in slot configuration struct
 *        but it should be called manually and won't lower the interrupt performance
 *
 * @param[in]   handle      I2S channel handler
 * @return
 *      - ESP_OK    Clear successfully
 *      - ESP_ERR_INVALID_STATE I2S is not initialized
 */
esp_err_t i2s_clear_dma_buffer(i2s_chan_handle_t handle);

/**
 * @brief Abort i2s reading or writing function
 * @note  Only allowed to be called when the channel state is IDLE / WRITING / READING, (i.e., channel has been started)
 *        it will change to IDLE after aborting the current reading or writing.
 * @note  Since reading or writing will be blocked for a long time while transporting a large quantity of data，
 *        This function can help to terminate reading/writing in its next reading/writing loop,
 *        but if reading/writing is bolcked on receiving dma queue(i.e. dma can't send or receive data), this function won't take effect
 *        And it will only abort for one time, so reading/writing thread won't stop though it is called
 *
 * @param[in]   handle      I2S channel handler
 * @return
 *      - ESP_OK    Abort successfully
 *      - ESP_ERR_INVALID_STATE I2S is stopped or not initialized
 */
esp_err_t i2s_abort_reading_writing(i2s_chan_handle_t handle);


#ifdef __cplusplus
}
#endif
