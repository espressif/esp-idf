/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN ANY APPLICATIONS
// DMA2D driver is not public for end users, but for ESP-IDF developers.

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "hal/dma2d_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of 2D-DMA pool handle
 */
typedef struct dma2d_group_t *dma2d_pool_handle_t;

/**
 * @brief Type of 2D-DMA transaction context (queue element)
 */
typedef struct dma2d_trans_s dma2d_trans_t;

/**
 * @brief Helper macro to get the size for struct `dma2d_trans_t`
 */
#define SIZEOF_DMA2D_TRANS_T  dma2d_get_trans_elm_size()

/**
 * @brief Get the size for struct `dma2d_trans_t`
 *
 * @return size_t Size of struct `dma2d_trans_t`
 */
size_t dma2d_get_trans_elm_size(void);

/**
 * @brief A collection of configuration items that used for allocating a 2D-DMA pool
 */
typedef struct {
    uint32_t pool_id;           /*!< The ID number of the 2D-DMA pool to allocate */
    uint32_t intr_priority;     /*!< 2D-DMA interrupt priority,
                                     if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
} dma2d_pool_config_t;

/**
 * @brief Acquire a 2D-DMA pool
 *
 * @param[in] config Pointer to a collection of configurations for the 2D-DMA pool
 * @param[out] ret_pool Returned pool handle
 * @return
 *      - ESP_OK: Acquire the 2D-DMA pool successfully
 *      - ESP_ERR_INVALID_ARG: Acquire the 2D-DMA pool failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Acquire the 2D-DMA pool failed because out of memory
 *      - ESP_FAIL: Acquire the 2D-DMA pool failed because of other error
 */
esp_err_t dma2d_acquire_pool(const dma2d_pool_config_t *config, dma2d_pool_handle_t *ret_pool);

/**
 * @brief Release a 2D-DMA pool
 *
 * @warning Upper driver should make sure there is no pending transaction (enqueued by the driver, but haven't be
 *          processed) before calling this function.
 *
 * @param[in] dma2d_pool 2D-DMA pool handle, allocated by `dma2d_acquire_pool`
 * @return
 *      - ESP_OK: Release the 2D-DMA pool successfully
 *      - ESP_ERR_INVALID_ARG: Release the 2D-DMA pool failed because of invalid argument
 *      - ESP_ERR_NOT_ALLOWED: Release the 2D-DMA pool failed because there is pending transactions in the pool,
 *                             pool can not be destroyed
 */
esp_err_t dma2d_release_pool(dma2d_pool_handle_t dma2d_pool);

/**
 * @brief Type of 2D-DMA channel handle
 */
typedef struct dma2d_channel_t *dma2d_channel_handle_t;

/**
 * @brief Struct to save the necessary information of a 2D-DMA channel for upper drivers to configure the channels
 */
typedef struct {
    dma2d_channel_direction_t dir;      /*!< Direction of the DMA channel */
    dma2d_channel_handle_t chan;        /*!< Handle of the DMA channel */
} dma2d_trans_channel_info_t;

/**
 * @brief Callback function to start a 2D-DMA transaction. This callback is being called when all necessary channels to
 * do the transaction have been acquired.
 *
 * Inside this function, usually configure the DMA channels, and then call `dma2d_start` to start the transaction.
 *
 * @note This function could run in current thread, or in other threads, or in ISR context (needs to follow ISR rules!)
 *
 * @param[in] num_chans Number of DMA channels acquired for the transaction
 * @param[in] dma2d_chans List of the channels acquired
 * @param[in] user_config User registered data (usually dma channel configurations) from `dma2d_trans_config_t::user_config` which is passed into `dma2d_enqueue`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*dma2d_trans_on_picked_callback_t)(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);

/**
 * @brief 2D-DMA channel special function flags
 *
 * These flags are supposed to be used to specify the 2D-DMA channel requirements for a transaction.
 */
#define DMA2D_CHANNEL_FUNCTION_FLAG_TX_REORDER   (1 << 0)       /*!< TX channel that has reorder functionality */
#define DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER   (1 << 1)       /*!< RX channel that has reorder functionality */
#define DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC       (1 << 2)       /*!< TX channel that has color space conversion functionality */
#define DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC       (1 << 3)       /*!< RX channel that has color space conversion functionality */
#define DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING      (1 << 4)       /*!< TX and RX channel with same channel ID */

/**
 * @brief A collection of configuration items for a 2D-DMA transaction
 */
typedef struct {
    uint32_t tx_channel_num;                /*!< Number of 2D-DMA TX channels required */
    uint32_t rx_channel_num;                /*!< Number of 2D-DMA RX channels required */

    // Special function requirements for channels
    uint32_t channel_flags;                 /*!< Bitwise OR of `DMA2D_CHANNEL_FUNCTION_FLAG_*` flags indicating the required functions on the channels for the transaction */

    // Specified to use reserved channels
    uint32_t specified_tx_channel_mask;     /*!< Bit mask of the specific TX channels to be used, the specified TX channels should have been reserved */
    uint32_t specified_rx_channel_mask;     /*!< Bit mask of the specific RX channels to be used, the specified RX channels should have been reserved */

    dma2d_trans_on_picked_callback_t on_job_picked;   /*!< Callback function to be called when all necessary channels to do the transaction have been acquired */
    void *user_config;                      /*!< User registered data to be passed into `on_job_picked` callback */
} dma2d_trans_config_t;

/**
 * @brief Enqueue a 2D-DMA transaction to be picked up by a certain 2D-DMA pool
 *
 * @param[in] dma2d_pool 2D-DMA pool handle, allocated by `dma2d_acquire_pool`
 * @param[in] trans_desc Pointer to a collection of configurations for a transaction
 *                       The context must exist at least until `on_job_picked` callback function is called.
 * @param[in] trans_placeholder Address to the memory for storing this transaction context
 *                              Caller must malloc a placeholder for storing the 2D-DMA transaction, and pass it into the function.
 *                              Size of the placeholder can be get from `SIZEOF_DMA2D_TRANS_T` macro.
 *                              Freeing the 2D-DMA transaction placeholder should also be taken care by the upper driver.
 *                              It can be freed when `on_job_picked` callback function is called or anytime later.
 * @return
 *      - ESP_OK: Enqueue the 2D-DMA transaction successfully
 *      - ESP_ERR_INVALID_ARG: Enqueue the 2D-DMA transaction failed because of invalid argument
 */
esp_err_t dma2d_enqueue(dma2d_pool_handle_t dma2d_pool, const dma2d_trans_config_t *trans_desc, dma2d_trans_t *trans_placeholder);

/**
 * @brief Force end an in-flight 2D-DMA transaction
 *
 * This API is useful when the error was caused by the DMA consumer (such as JPEG). The error can only be detected
 * by the consumer module, and the error info will only be propagated to the consumer driver. The 2D-DMA channels being
 * involved to transfer the data has no way to be informed about the error at its upstream, it will keep waiting for
 * the data.
 *
 * Therefore, when the consumer driver is doing the error handling, it is required to call this API to end the on-going
 * transaction and release the taken TX and RX channels. It will stop and free the TX and RX channels that are bundled
 * together to process the transaction.
 *
 * @param[in] trans Pointer to the 2D-DMA transaction context
 * @param[out] need_yield Pointer to a status flag to record whether a task switch is needed if this API is being called in an ISR
 * @return
 *      - ESP_OK: Force end an in-flight transaction successfully
 *      - ESP_ERR_INVALID_ARG: Force end failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Force end failed because the transaction is not yet in-flight
 */
esp_err_t dma2d_force_end(dma2d_trans_t *trans, bool *need_yield);

/*********************************************** DMA CHANNEL OPERATIONS ***********************************************/

/**
 * @brief Type of 2D-DMA engine trigger
 */
typedef struct {
    dma2d_trigger_peripheral_t periph;  /*!< Target peripheral which will trigger DMA operations */
    int periph_sel_id;                  /*!< Peripheral selection ID. Supported IDs are listed in `soc/dma2d_channel.h` */
} dma2d_trigger_t;

/**
 * @brief Connect 2D-DMA channel to trigger peripheral, and configure all other channel settings to a certain state (the channel will be reset first)
 *
 * Usually only to be called in `on_job_picked` callback, and is the first step to do inside the callback, since it resets other configurations to a default mode.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] trig_periph 2D-DMA trigger peripheral
 * @return
 *      - ESP_OK: Connect 2D-DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Connect 2D-DMA channel failed because of invalid argument
 */
esp_err_t dma2d_connect(dma2d_channel_handle_t dma2d_chan, const dma2d_trigger_t *trig_periph);

/**
 * @brief A collection of strategy items that each 2D-DMA channel could apply
 */
typedef struct {
    bool owner_check;           /*!< If set / clear, DMA channel enables / disables checking owner validity */
    bool auto_update_desc;      /*!< If set / clear, DMA channel enables / disables hardware to update descriptor automatically (TX channel only) */
    bool eof_till_data_popped;  /*!< If set, EOF flag is generated until the data needs to read has been popped from the DMA FIFO (TX channel only) */
} dma2d_strategy_config_t;

/**
 * @brief Apply channel strategy for 2D-DMA channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] config Configuration of 2D-DMA channel strategy
 * @return
 *      - ESP_OK: Apply channel strategy successfully
 *      - ESP_ERR_INVALID_ARG: Apply channel strategy failed because of invalid argument
 */
esp_err_t dma2d_apply_strategy(dma2d_channel_handle_t dma2d_chan, const dma2d_strategy_config_t *config);

/**
 * @brief A collection of transfer ability items that each 2D-DMA channel could apply to improve transfer efficiency
 *
 * @note The 2D-DMA driver has no knowledge about the DMA buffer (address and size) used by upper layer.
 *       So it's the responsibility of the **upper layer** to take care of the buffer address and size.
 *       Usually RX buffer at least requires 4-byte alignment to avoid overwriting other data by DMA write PSRAM process
 *       or its data being overwritten.
 */
typedef struct {
    bool desc_burst_en;                             /*!< If set / clear, DMA channel enables / disables burst reading descriptor link */
    dma2d_data_burst_length_t data_burst_length;    /*!< Configure the DMA channel burst reading data length */
    dma2d_macro_block_size_t mb_size;               /*!< Configure the DMA channel macro block size (only useful in DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE mode) */
} dma2d_transfer_ability_t;

/**
 * @brief Configure 2D-DMA channel transfer ability for transfer efficiency
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] ability Configuration of 2D-DMA channel transfer ability
 * @return
 *      - ESP_OK: Set channel transfer ability successfully
 *      - ESP_ERR_INVALID_ARG: Set channel transfer ability failed because of invalid argument
 */
esp_err_t dma2d_set_transfer_ability(dma2d_channel_handle_t dma2d_chan, const dma2d_transfer_ability_t *ability);

/**
 * @brief A collection of color space conversion (CSC) items that each 2D-DMA channel could apply
 */
typedef struct {
    union {
        dma2d_csc_tx_option_t tx_csc_option;    /*!< TX direction (into DMA) pixel format conversion option */
        dma2d_csc_rx_option_t rx_csc_option;    /*!< RX direction (out from DMA) pixel format conversion option */
    };
    dma2d_scramble_order_t pre_scramble;        /*!< DMA channel data scramble order before color conversion */
    dma2d_scramble_order_t post_scramble;       /*!< DMA channel data scramble order after color conversion (only available for RX channels) */
} dma2d_csc_config_t;

/**
 * @brief Configure color space conversion setting for 2D-DMA channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] config Configuration of 2D-DMA channel color space conversion
 * @return
 *      - ESP_OK: Configure DMA color space conversion successfully
 *      - ESP_ERR_INVALID_ARG: Configure DMA color space conversion failed because of invalid argument
 */
esp_err_t dma2d_configure_color_space_conversion(dma2d_channel_handle_t dma2d_chan, const dma2d_csc_config_t *config);

/**
 * @brief A collection of configurations apply to 2D-DMA channel DSCR-PORT mode
 */
typedef struct {
    uint32_t block_h;                           /*!< Horizontal width of the block in dscr-port mode (unit: pixel) */
    uint32_t block_v;                           /*!< Vertical height of the block in dscr-port mode (unit: pixel) */
} dma2d_dscr_port_mode_config_t;

/**
 * @brief Configure 2D-DMA channel DSCR-PORT mode
 *
 * @note This API only targets PPA SRM, which uses 2D-DMA DSCR-PORT mode.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] config Configuration of 2D-DMA channel DSCR-PORT mode
 * @return
 *      - ESP_OK: Configure 2D-DMA dscr-port mode successfully
 *      - ESP_ERR_INVALID_ARG: Configure 2D-DMA dscr-port mode failed because of invalid argument
 */
esp_err_t dma2d_configure_dscr_port_mode(dma2d_channel_handle_t dma2d_chan, const dma2d_dscr_port_mode_config_t *config);

/**
 * @brief Type of 2D-DMA event data
 */
typedef struct {
    union {
        intptr_t rx_eof_desc_addr;      /*!< EOF descriptor address of RX channel */
        intptr_t tx_eof_desc_addr;      /*!< EOF descriptor address of TX channel */
    };
    dma2d_trans_t *transaction;         /*!< Pointer to the transaction context processed */
} dma2d_event_data_t;

/**
 * @brief Type of 2D-DMA event callback
 *
 * @param dma2d_chan 2D-DMA channel handle. Depends on the callback events, sometimes will pass NULL to this parameter.
 * @param event_data 2D-DMA event data
 * @param user_data User registered data from `dma2d_register_tx/rx_event_callbacks`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*dma2d_event_callback_t)(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data);

/**
 * @brief Group of supported 2D-DMA TX callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    dma2d_event_callback_t on_desc_done;    /*!< Invoked when TX engine completes processing all data in a descriptor.
                                                 When TX_DONE interrupt gets triggered but not EOF, it is considered
                                                 to be still in the middle of a complete transaction (partially done),
                                                 you are allowed to configure 2D-DMA channel hardware/descriptor in this
                                                 callback, and let the channels start running again */
} dma2d_tx_event_callbacks_t;

/**
 * @brief Group of supported 2D-DMA RX callbacks
 * @note The callbacks are all running under ISR environment
 *
 * Users should be clear on the unique responsibility of each callback when writing the callback functions, such as
 * where to free the transaction memory.
 */
typedef struct {
    dma2d_event_callback_t on_recv_eof;     /*!< Invoked when RX engine meets EOF descriptor.
                                                 Note that in this callback, RX channel handle will not be given.
                                                 This is because at the moment of `on_recv_eof` callback is called,
                                                 the channels are returned to the pool and may have already been used to start another new transaction */
    dma2d_event_callback_t on_desc_done;    /*!< Invoked when RX engine completes processing all data in a descriptor.
                                                 When RX_DONE interrupt gets triggered but not EOF, it is considered
                                                 to be still in the middle of a complete transaction (partially done),
                                                 you are allowed to configure 2D-DMA channel hardware/descriptor in this
                                                 callback, and let the channels start running again */
    dma2d_event_callback_t on_desc_empty;   /*!< Invoked when the buffer size pointed to by the received linked list
                                                 descriptor is less than the length of the data to be received */
} dma2d_rx_event_callbacks_t;

/**
 * @brief Set 2D-DMA event callbacks for TX channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA TX channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 */
esp_err_t dma2d_register_tx_event_callbacks(dma2d_channel_handle_t dma2d_chan, dma2d_tx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set 2D-DMA event callbacks for RX channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA RX channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 */
esp_err_t dma2d_register_rx_event_callbacks(dma2d_channel_handle_t dma2d_chan, dma2d_rx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set descriptor address for 2D-DMA channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @param[in] desc_base_addr Base address of descriptors
 * @return
 *      - ESP_OK: Set 2D-DMA descriptor addr successfully
 *      - ESP_ERR_INVALID_ARG: Set 2D-DMA descriptor addr failed because of invalid argument
 */
esp_err_t dma2d_set_desc_addr(dma2d_channel_handle_t dma2d_chan, intptr_t desc_base_addr);

/**
 * @brief Start engine for 2D-DMA channel
 *
 * Usually only to be called in `on_job_picked` callback.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle, get from the `on_job_picked` callback input argument `dma2d_chans`
 * @return
 *      - ESP_OK: Start 2D-DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Start 2D-DMA engine failed because of invalid argument
 */
esp_err_t dma2d_start(dma2d_channel_handle_t dma2d_chan);

/**
 * @brief Stop engine for 2D-DMA channel
 *
 * Usually to be called in ISR context.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle
 * @return
 *      - ESP_OK: Stop 2D-DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Stop 2D-DMA engine failed because of invalid argument
 */
esp_err_t dma2d_stop(dma2d_channel_handle_t dma2d_chan);

/**
 * @brief Make the appended descriptors be aware to the 2D-DMA engine
 *
 * Usually to be called in ISR context.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle
 * @return
 *      - ESP_OK: Send append command to 2D-DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Send append command to 2D-DMA engine failed because of invalid argument
 */
esp_err_t dma2d_append(dma2d_channel_handle_t dma2d_chan);

/**
 * @brief Reset engine for 2D-DMA channel
 *
 * Usually to be called in ISR context.
 *
 * @param[in] dma2d_chan 2D-DMA channel handle
 * @return
 *      - ESP_OK: Reset 2D-DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Reset 2D-DMA engine failed because of invalid argument
 */
esp_err_t dma2d_reset(dma2d_channel_handle_t dma2d_chan);

#ifdef __cplusplus
}
#endif
