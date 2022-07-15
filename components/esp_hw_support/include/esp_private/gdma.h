/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN ANY APPLICATIONS
// GDMA driver is not public for end users, but for ESP-IDF developpers.

#pragma once

#include <stdbool.h>
#include "soc/gdma_channel.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of GDMA channel handle
 *
 */
typedef struct gdma_channel_t *gdma_channel_handle_t;

/**
 * @brief Enumeration of peripherals which have the DMA capability
 * @note Some peripheral might not be available on certain chip, please refer to `soc_caps.h` for detail.
 *
 */
typedef enum {
    GDMA_TRIG_PERIPH_M2M,  /*!< GDMA trigger peripheral: M2M */
    GDMA_TRIG_PERIPH_UART, /*!< GDMA trigger peripheral: UART */
    GDMA_TRIG_PERIPH_SPI,  /*!< GDMA trigger peripheral: SPI */
    GDMA_TRIG_PERIPH_I2S,  /*!< GDMA trigger peripheral: I2S */
    GDMA_TRIG_PERIPH_AES,  /*!< GDMA trigger peripheral: AES */
    GDMA_TRIG_PERIPH_SHA,  /*!< GDMA trigger peripheral: SHA */
    GDMA_TRIG_PERIPH_ADC,  /*!< GDMA trigger peripheral: ADC */
    GDMA_TRIG_PERIPH_DAC,  /*!< GDMA trigger peripheral: DAC */
    GDMA_TRIG_PERIPH_LCD,  /*!< GDMA trigger peripheral: LCD */
    GDMA_TRIG_PERIPH_CAM,  /*!< GDMA trigger peripheral: CAM */
    GDMA_TRIG_PERIPH_RMT,  /*!< GDMA trigger peripheral: RMT */
} gdma_trigger_peripheral_t;

/**
 * @brief Enumeration of GDMA channel direction
 *
 */
typedef enum {
    GDMA_CHANNEL_DIRECTION_TX, /*!< GDMA channel direction: TX */
    GDMA_CHANNEL_DIRECTION_RX, /*!< GDMA channel direction: RX */
} gdma_channel_direction_t;

/**
 * @brief Collection of configuration items that used for allocating GDMA channel
 *
 */
typedef struct {
    gdma_channel_handle_t sibling_chan; /*!< DMA sibling channel handle (NULL means having sibling is not necessary) */
    gdma_channel_direction_t direction; /*!< DMA channel direction */
    struct {
        int reserve_sibling: 1; /*!< If set, DMA channel allocator would prefer to allocate new channel in a new pair, and reserve sibling channel for future use */
    } flags;
} gdma_channel_alloc_config_t;

/**
 * @brief GDMA transfer ability
 *
 * @note The alignment set in this structure is **not** a guarantee that gdma driver will take care of the nonalignment cases.
 *       Actually the GDMA driver has no knowledge about the DMA buffer (address and size) used by upper layer.
 *       So it's the responsibility of the **upper layer** to take care of the buffer address and size.
 *
 */
typedef struct {
    size_t sram_trans_align;  /*!< DMA transfer alignment for memory in SRAM, in bytes. The driver enables/disables burst mode based on this value. 0 means no alignment is required */
    size_t psram_trans_align; /*!< DMA transfer alignment for memory in PSRAM, in bytes. The driver sets proper burst block size based on the alignment value. 0 means no alignment is required */
} gdma_transfer_ability_t;

/**
 * @brief Type of GDMA event data
 *
 */
typedef struct {
    union {
        intptr_t rx_eof_desc_addr; /*!< EOF descriptor address of RX channel */
        intptr_t tx_eof_desc_addr; /*!< EOF descriptor address of TX channel */
    };
} gdma_event_data_t;

/**
 * @brief Type of GDMA event callback
 * @param dma_chan GDMA channel handle, created from `gdma_new_channel`
 * @param event_data GDMA event data
 * @param user_data User registered data from `gdma_register_tx_event_callbacks` or `gdma_register_rx_event_callbacks`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 *
 */
typedef bool (*gdma_event_callback_t)(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

/**
 * @brief Group of supported GDMA TX callbacks
 * @note The callbacks are all running under ISR environment
 *
 */
typedef struct {
    gdma_event_callback_t on_trans_eof; /*!< Invoked when TX engine meets EOF descriptor */
} gdma_tx_event_callbacks_t;

/**
 * @brief Group of supported GDMA RX callbacks
 * @note The callbacks are all running under ISR environment
 *
 */
typedef struct {
    gdma_event_callback_t on_recv_eof; /*!< Invoked when RX engine meets EOF descriptor */
} gdma_rx_event_callbacks_t;

/**
 * @brief Type of GDMA engine trigger
 * @note It's recommended to initialize this structure with `GDMA_MAKE_TRIGGER`.
 *
 */
typedef struct {
    gdma_trigger_peripheral_t periph; /*!< Target peripheral which will trigger DMA operations */
    int instance_id;                  /*!< Peripheral instance ID. Supported IDs are listed in `soc/gdma_channel.h`, e.g. SOC_GDMA_TRIG_PERIPH_UART0 */
} gdma_trigger_t;

/**
 * @brief Helper macro to initialize GDMA trigger
 * @note value of `peri` must be selected from `gdma_trigger_peripheral_t` enum.
 *       e.g. GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UART,0)
 *
 */
#define GDMA_MAKE_TRIGGER(peri, id) \
    (gdma_trigger_t) { .periph = peri, .instance_id = SOC_##peri##id }

/**
 * @brief A collection of strategy item that each GDMA channel could apply
 *
 */
typedef struct {
    bool owner_check;      /*!< If set / clear, DMA channel enables / disables checking owner validity */
    bool auto_update_desc; /*!< If set / clear, DMA channel enables / disables hardware to update descriptor automatically (TX channel only) */
} gdma_strategy_config_t;

/**
 * @brief Create GDMA channel
 * @note This API won't install interrupt service for the allocated channel.
 *       If interrupt service is needed, user has to register GDMA event callback by `gdma_register_tx_event_callbacks` or `gdma_register_rx_event_callbacks`.
 *
 * @param[in] config Pointer to a collection of configurations for allocating GDMA channel
 * @param[out] ret_chan Returnned channel handle
 * @return
 *      - ESP_OK: Create DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA channel failed because out of memory
 *      - ESP_FAIL: Create DMA channel failed because of other error
 */
esp_err_t gdma_new_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan);

/**
 * @brief Connect GDMA channel to trigger peripheral
 *
 * @note Suggest to use helper macro `GDMA_MAKE_TRIGGER` to construct parameter `trig_periph`. e.g. GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SHA,0)
 * @note Connecting to a peripheral will also reset the DMA FIFO and FSM automatically
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] trig_periph GDMA trigger peripheral
 * @return
 *      - ESP_OK: Connect GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Connect GDMA channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Connect GDMA channel failed because DMA channel is working with another peripheral
 *      - ESP_FAIL: Connect GDMA channel failed because of other error
 */
esp_err_t gdma_connect(gdma_channel_handle_t dma_chan, gdma_trigger_t trig_periph);

/**
 * @brief Disconnect GMA channel from peripheral
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Disconnect GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Disconnect GDMA channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disconnect GDMA channel failed because DMA channel is not connected to any peripheral
 *      - ESP_FAIL: Disconnect DMA channel failed because of other error
 */
esp_err_t gdma_disconnect(gdma_channel_handle_t dma_chan);

/**
 * @brief Set DMA channel transfer ability
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] ability Transfer ability, e.g. alignment
 * @return
 *      - ESP_OK: Set DMA channel transfer ability successfully
 *      - ESP_ERR_INVALID_ARG: Set DMA channel transfer ability failed because of invalid argument
 *      - ESP_FAIL: Set DMA channel transfer ability failed because of other error
 */
esp_err_t gdma_set_transfer_ability(gdma_channel_handle_t dma_chan, const gdma_transfer_ability_t *ability);

/**
 * @brief Apply channel strategy for GDMA channel
 *
 * @param dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param config Configuration of GDMA channel strategy
 *      - ESP_OK: Apply channel strategy successfully
 *      - ESP_ERR_INVALID_ARG: Apply channel strategy failed because of invalid argument
 *      - ESP_FAIL: Apply channel strategy failed because of other error
 */
esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config);

/**
 * @brief Delete GDMA channel
 * @note If you call `gdma_new_channel` several times for a same peripheral, make sure you call this API the same times.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Delete GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete GDMA channel failed because of invalid argument
 *      - ESP_FAIL: Delete GDMA channel failed because of other error
 */
esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan);

/**
 * @brief Get the channel ID
 *
 * @note This API breaks the encapsulation of GDMA Channel Object.
 *       With the returned channel ID, you can even bypass all other GDMA driver API and access Low Level API directly.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[out] channel_id Returned channel ID
 * @return
 *      - ESP_OK: Get GDMA channel ID successfully
 *      - ESP_ERR_INVALID_ARG: Get GDMA channel ID failed because of invalid argument
 *      - ESP_FAIL: Get GDMA channel ID failed because of other error
 */
esp_err_t gdma_get_channel_id(gdma_channel_handle_t dma_chan, int *channel_id);

/**
 * @brief Set GDMA event callbacks for TX channel
 * @note This API will install GDMA interrupt service for the channel internally
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set GDMA event callbacks for RX channel
 * @note This API will install GDMA interrupt service for the channel internally
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t gdma_register_rx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_rx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set DMA descriptor address and start engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] desc_base_addr Base address of descriptors (usually the descriptors are chained into a link or ring)
 * @return
 *      - ESP_OK: Start DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Start DMA engine failed because of invalid argument
 *      - ESP_FAIL: Start DMA engine failed because of other error
 */
esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr);

/**
 * @brief Stop DMA engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Stop DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Stop DMA engine failed because of invalid argument
 *      - ESP_FAIL: Stop DMA engine failed because of other error
 */
esp_err_t gdma_stop(gdma_channel_handle_t dma_chan);

/**
 * @brief Make the appended descriptors be aware to the DMA engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 * @note This API could also resume a paused DMA engine, make sure new descriptors have been appended to the descriptor chain before calling it.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Send append command to DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Send append command to DMA engine failed because of invalid argument
 *      - ESP_FAIL: Send append command to DMA engine failed because of other error
 */
esp_err_t gdma_append(gdma_channel_handle_t dma_chan);

/**
 * @brief Reset DMA channel FIFO and internal finite state machine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 * @note Resetting a DMA channel won't break the connection with the target peripheral
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: DMA channel reset successfully
 *      - ESP_ERR_INVALID_ARG: DMA channel reset failed due to invalid arguments
 *      - ESP_FAIL: DMA channel reset failed due to other errors
 */
esp_err_t gdma_reset(gdma_channel_handle_t dma_chan);

#ifdef __cplusplus
}
#endif
