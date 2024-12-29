/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "esp_err.h"
#include "esp_eth_mac.h"

/**
 * @brief Indicate to ::emac_esp_dma_receive_frame that receive frame buffer was allocated by ::emac_esp_dma_alloc_recv_buf
 *
 */
#define EMAC_DMA_BUF_SIZE_AUTO 0

/**
 * @brief EMAC DMA handle
 *
 */
typedef struct emac_esp_dma_t *emac_esp_dma_handle_t;


/**
 * @brief EMAC DMA configuration
 * @note Currently just a placeholder
 *
 */
typedef struct emac_esp_dma_config_t emac_esp_dma_config_t;

/**
 * @brief Supplementary properties for the ESP EMAC DMA transmit buffer
 *
 */
typedef struct {
    uint8_t* buf;
    uint32_t size;
} emac_esp_dma_transmit_buff_t;

/**
 * @brief Reset DMA
 * @note This function should be called prior each EMAC start
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 */
void emac_esp_dma_reset(emac_esp_dma_handle_t emac_esp_dma);

/**
 * @brief Transmit data from buffer over EMAC
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] buf buffer to be transmitted
 * @param[in] length length of the buffer
 * @return number of transmitted bytes on success
 *         zero on fail
 */
uint32_t emac_esp_dma_transmit_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t length);
/**
 * @brief Extended version of Transmit data function. It is capable to transmit from multiple buffers to appear as single Ethernet frame.
 *        The function also provides hardware time stamp of the transmission on supported targets.
 *
 * @note Data is joint into single frame in order in which the buffers are stored in input array.
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] buffs_array array of buffers to be transmitted
 * @param[in] buffs_cnt number of buffers (i.e. buffs array sizes can be 1 to n)
 * @param[out] ts time stamp at which the frame was transmitted by EMAC. Valid time stamp returned only on supported targets. Pass NULL
 *                if time stamp is not required.
 *
 * @return number of transmitted bytes on success
 *         zero on fail
 *
 */
uint32_t emac_esp_dma_transmit_frame_ext(emac_esp_dma_handle_t emac_esp_dma, emac_esp_dma_transmit_buff_t *buffs_array, uint32_t buffs_cnt, eth_mac_time_t *ts);

/**
 * @brief Allocate buffer with size equal to actually received Ethernet frame size.
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in, out] size as an input defines maximum size of buffer to be allocated. As an output, indicates actual size of received
 *                      Ethernet frame which is waiting to be processed. Returned size may be 0 when there is no waiting valid frame.
 *
 * @note If maximum allowed size of buffer to be allocated is less than actual size of received Ethernet frame, the buffer
 *       is allocated with that limit and the frame will be truncated by emac_hal_receive_frame.
 *
 * @return Pointer to allocated buffer
 *         NULL when allocation fails (returned @p size is non-zero)
 *         NULL when there is no waiting Ethernet frame (returned @p size is zero)
 */
uint8_t *emac_esp_dma_alloc_recv_buf(emac_esp_dma_handle_t emac_esp_dma, uint32_t *size);

/**
 * @brief Copy received Ethernet frame from EMAC DMA memory space to application.
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] buf buffer into which the Ethernet frame is to be copied
 * @param[in] size buffer size. When buffer was allocated by ::emac_esp_dma_alloc_recv_buf, this parameter needs to be set
 *                 to @c EMAC_DMA_BUF_SIZE_AUTO
 * @param[out] ts time stamp at which the frame was received by EMAC. Only available on supported targets. Can be NULL
 *                when time stamp is not required.
 *
 * @return - number of copied bytes when success
 *         - number of bytes of received Ethernet frame when maximum allowed buffer @p size is less than actual size of
 *         received Ethernet frame and @p size is NOT set to @c EMAC_DMA_BUF_SIZE_AUTO
 *         - 0 when there is no waiting Ethernet frame or on frame error when @p size is NOT set to @c EMAC_DMA_BUF_SIZE_AUTO
 *
 * @note When this function is called with @c EMAC_DMA_BUF_SIZE_AUTO size option (preferred), buffer needs to be
 *       successfully allocated by ::emac_esp_dma_alloc_recv_buf function at first.
 * @note When this function is NOT called with @c EMAC_DMA_BUF_SIZE_AUTO size option and maximum allowed buffer @p size
 *       is less than actual size of received Ethernet frame, the frame will be truncated.
 * @note FCS field is never copied
 */
uint32_t emac_esp_dma_receive_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t size, eth_mac_time_t *ts);

/**
 * @brief Flush frame stored in Rx DMA
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 */
void emac_esp_dma_flush_recv_frame(emac_esp_dma_handle_t emac_esp_dma);

/**
 * @brief Get number of frames remaining in Rx DMA
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[out] frames_remain number of frames remaining to be processed
 * @param[out] free_desc number of free DMA Rx descriptors
 */
void emac_esp_dma_get_remain_frames(emac_esp_dma_handle_t emac_esp_dma, uint32_t *remain_frames, uint32_t *used_descs);

/**
 * @brief Set the Transmit Descriptor Word 0 (TDES0) control bits
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] bit_mask mask of control bits to be set
 */
void emac_esp_dma_set_tdes0_ctrl_bits(emac_esp_dma_handle_t emac_esp_dma, uint32_t bit_mask);

/**
 * @brief Clear the Transmit Descriptor Word 0 (TDES0) control bits
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] bit_mask mask of control bits to be cleared
 */
void emac_esp_dma_clear_tdes0_ctrl_bits(emac_esp_dma_handle_t emac_esp_dma, uint32_t bit_mask);

/**
 * @brief Enables DMA time stamping feature
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @param[in] enable enable when true
 */
void emac_esp_dma_ts_enable(emac_esp_dma_handle_t emac_esp_dma, bool enable);

/**
 * @brief Creates a new instance of the ESP EMAC DMA
 *
 * @param config ESP EMAC DMA configuration
 * @param[out] ret_handle EMAC DMA handle
 * @return esp_err_t
 *          ESP_OK on success
 *          ESP_ERR_NO_MEM when there is not enough memory to allocate instance
 */
esp_err_t emac_esp_new_dma(const emac_esp_dma_config_t* config, emac_esp_dma_handle_t *ret_handle);

/**
 * @brief Deletes the ESP EMAC DMA instance
 *
 * @param[in] emac_esp_dma EMAC DMA handle
 * @return esp_err_t
 *          ESP_OK on success
 */
esp_err_t emac_esp_del_dma(emac_esp_dma_handle_t emac_esp_dma);

#ifdef __cplusplus
}
#endif
