/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 * This file contains SPI Master private/internal APIs. Private/Internal APIs are:
 * - Visible to other IDF components
 * - Suggest NOT to use these APIs in your applications
 * - We don't provide backward compatibility, and safety on these APIs either
 */

#pragma once

#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SPI_SCT_SUPPORTED
/**
 * @Backgrounds: `SCT Mode`
 * Segmented-Configure-Transfer Mode
 *
 * In this mode, you could pre-configure multiple SPI transactions.
 * - These whole transaction is called one `Segmented-Configure-Transaction` or one `SCT`.
 * - Each of the transactions in one `SCT` is called one `Segment`.
 *
 * Per segment can have different SPI phase configurations
 */

/**
 * SPI SCT Mode transaction flags
 */
#define SPI_MULTI_TRANS_PREP_LEN_UPDATED    (1<<0)  ///< Use `spi_multi_transaction_t: cs_ena_pretrans` in this segment.
#define SPI_MULTI_TRANS_CMD_LEN_UPDATED     (1<<1)  ///< Use `spi_multi_transaction_t: command_bits` in this segment.
#define SPI_MULTI_TRANS_ADDR_LEN_UPDATED    (1<<2)  ///< Use `spi_multi_transaction_t: address_bits` in this segment.
#define SPI_MULTI_TRANS_DUMMY_LEN_UPDATED   (1<<3)  ///< Use `spi_multi_transaction_t: dummy_bits` in this segment.
#define SPI_MULTI_TRANS_DONE_LEN_UPDATED    (1<<4)  ///< Use `spi_multi_transaction_t: cs_ena_posttrans` in this segment.

/**
 * This struct is for SPI SCT (Segmented-Configure-Transfer) Mode.
 *
 * By default, length of each SPI Phase will not change per segment. Each segment will use the phase length you set when `spi_bus_add_device()`
 * However, you could force a segment to use its custom phase length. To achieve this, set the `SPI_SEG_TRANS_XX` flags, to customize phase length.
 */
typedef struct {
    struct spi_transaction_t base;  ///< Transaction data, so that pointer to spi_transaction_t can be converted into spi_multi_transaction_t
    uint8_t cs_ena_pretrans;        ///< Amount of SPI bit-cycles the cs should be activated before the transmission
    uint8_t cs_ena_posttrans;       ///< Amount of SPI bit-cycles the cs should stay active after the transmission
    uint8_t command_bits;           ///< The command length in this transaction, in bits.
    uint8_t address_bits;           ///< The address length in this transaction, in bits.
    uint8_t dummy_bits;             ///< The dummy length in this transaction, in bits.
    uint32_t sct_gap_len;           ///< The len of CS inactive time between segments, in clocks.
    uint32_t seg_trans_flags;       ///< SCT specific flags. See `SPI_SEG_TRANS_XXX` macros.
} spi_multi_transaction_t;

/**
 * @brief Enable/Disable Segmented-Configure-Transfer (SCT) mode
 *
 * Search for `@Backgrounds: `SCT Mode`` in this header file to know what is SCT mode
 *
 * @note This API isn't thread safe. Besides, after enabling this, current SPI host will be switched into SCT mode.
 *       Therefore, never call this API when in multiple threads, or when an SPI transaction is ongoing (on this SPI host).
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param enable True: to enable SCT mode; False: to disable SCT mode
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Invalid states, e.g.: an SPI polling transaction is ongoing, SPI internal Queue isn't empty, etc.
 */
esp_err_t spi_bus_multi_trans_mode_enable(spi_device_handle_t handle, bool enable);

/**
 * @brief Queue an SPI Segmented-Configure-Transaction (SCT) list for interrupt transaction execution.
 *
 * Search for `@Backgrounds: `SCT Mode`` in this header file to know what is SCT mode
 *
 * @note After calling this API, call `spi_device_get_multi_trans_result` to get the transaction results.
 *
 * @param handle         Device handle obtained using spi_host_add_dev
 * @param seg_trans_desc Pointer to the transaction segments list head (a one-segment-list is also acceptable)
 * @param trans_num      Transaction number in this segment
 * @param ticks_to_wait  Ticks to wait until there's room in the queue; use portMAX_DELAY to never time out.
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Invalid states, e.g.: an SPI polling transaction is ongoing, SCT mode isn't enabled, DMA descriptors not enough, etc.
 *        - ESP_ERR_TIMEOUT:       Timeout, this SCT transaction isn't queued successfully
 */
esp_err_t spi_device_queue_multi_trans(spi_device_handle_t handle, spi_multi_transaction_t *seg_trans_desc, uint32_t trans_num, TickType_t ticks_to_wait);

/**
 * @brief Get the result of an SPI Segmented-Configure-Transaction (SCT).
 *
 * Search for `@Backgrounds: `SCT Mode`` in this header file to know what is SCT mode
 *
 * @note Until this API returns (with `ESP_OK`), you can now recycle the memory used for this SCT list (pointed by `seg_trans_desc`).
 *       You must maintain the SCT list related memory before this API returns, otherwise the SCT transaction may fail
 *
 * @param handle              Device handle obtained using spi_host_add_dev
 * @param[out] seg_trans_desc Pointer to the completed SCT list head (then you can recycle this list of memory).
 * @param ticks_to_wait       Ticks to wait until there's a returned item; use portMAX_DELAY to never time out.
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Invalid states, e.g.: SCT mode isn't enabled, etc.
 *        - ESP_ERR_TIMEOUT:       Timeout, didn't get a completed SCT transaction
 */
esp_err_t spi_device_get_multi_trans_result(spi_device_handle_t handle, spi_multi_transaction_t **seg_trans_desc, TickType_t ticks_to_wait);
#endif  //#if SOC_SPI_SCT_SUPPORTED

#ifdef __cplusplus
}
#endif
