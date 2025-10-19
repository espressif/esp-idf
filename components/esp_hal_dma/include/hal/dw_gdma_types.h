/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DMA block transfer type
 */
typedef enum {
    DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,  /*!< Contiguous buffer address */
    DW_GDMA_BLOCK_TRANSFER_RELOAD,      /*!< Reload transfer configuration automatically */
    DW_GDMA_BLOCK_TRANSFER_SHADOW,      /*!< Shadow register */
    DW_GDMA_BLOCK_TRANSFER_LIST,        /*!< Link list */
} dw_gdma_block_transfer_type_t;

/**
 * @brief Flow control type
 */
typedef enum {
    DW_GDMA_FLOW_CTRL_SELF, /*!< Flow controller is the DMA engine itself */
    DW_GDMA_FLOW_CTRL_SRC,  /*!< Flow controller is the source peripheral */
    DW_GDMA_FLOW_CTRL_DST,  /*!< Flow controller is the destination peripheral */
} dw_gdma_flow_controller_t;

/**
 * @brief Handshake interface type
 */
typedef enum {
    DW_GDMA_HANDSHAKE_HW, /*!< Transaction requests are initiated by hardware */
    DW_GDMA_HANDSHAKE_SW, /*!< Transaction requests are initiated by software */
} dw_gdma_handshake_type_t;

/**
 * @brief Role of the DMA source/destination
 */
typedef enum {
    DW_GDMA_ROLE_MEM,         /*!< Target is a plain memory which is accessible by the DMA */
    DW_GDMA_ROLE_PERIPH_DSI,  /*!< Target is FIFO memory of peripheral: DSI */
    DW_GDMA_ROLE_PERIPH_CSI,  /*!< Target is FIFO memory of peripheral: CSI */
    DW_GDMA_ROLE_PERIPH_ISP,  /*!< Target is FIFO memory of peripheral: ISP */
} dw_gdma_role_t;

/**
 * @brief Channel lock level
 */
typedef enum {
    DW_GDMA_LOCK_LEVEL_FULL_TRANS,  /*!< Lock over complete DMA transfer */
    DW_GDMA_LOCK_LEVEL_BLOCK_TRANS, /*!< Lock over DMA block transfer */
} dw_gdma_lock_level_t;

/**
 * @brief DW_GDMA transfer width
 */
typedef enum {
    DW_GDMA_TRANS_WIDTH_8,   /*!< Data transfer width: 8 bits */
    DW_GDMA_TRANS_WIDTH_16,  /*!< Data transfer width: 16 bits */
    DW_GDMA_TRANS_WIDTH_32,  /*!< Data transfer width: 32 bits */
    DW_GDMA_TRANS_WIDTH_64,  /*!< Data transfer width: 64 bits */
    DW_GDMA_TRANS_WIDTH_128, /*!< Data transfer width: 128 bits */
    DW_GDMA_TRANS_WIDTH_256, /*!< Data transfer width: 256 bits */
    DW_GDMA_TRANS_WIDTH_512, /*!< Data transfer width: 512 bits */
} dw_gdma_transfer_width_t;

/**
 * @brief DW_GDMA burst mode
 */
typedef enum {
    DW_GDMA_BURST_MODE_INCREMENT,  /*!< The address is increased after each transfer */
    DW_GDMA_BURST_MODE_FIXED,      /*!< The address remains the same after each transfer */
} dw_gdma_burst_mode_t;

/**
 * @brief Number of data items that are contained in one burst transaction
 *
 * @note One item's bit width is set by `dw_gdma_transfer_width_t`
 */
typedef enum {
    DW_GDMA_BURST_ITEMS_1,    /*!< 1 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_4,    /*!< 4 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_8,    /*!< 8 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_16,   /*!< 16 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_32,   /*!< 32 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_64,   /*!< 64 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_128,  /*!< 128 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_256,  /*!< 256 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_512,  /*!< 512 data items in a burst transaction */
    DW_GDMA_BURST_ITEMS_1024, /*!< 1024 data items in a burst transaction */
} dw_gdma_burst_items_t;

#ifdef __cplusplus
}
#endif
