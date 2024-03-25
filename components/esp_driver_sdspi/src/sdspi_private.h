/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/sdspi_host.h"

/// Control tokens used to frame data transfers
/// (see section 7.3.3 of SD simplified spec)

/// Token sent before single/multi block reads and single block writes
#define TOKEN_BLOCK_START                   0b11111110
/// Token sent before multi block writes
#define TOKEN_BLOCK_START_WRITE_MULTI       0b11111100
/// Token used to stop multi block write (for reads, CMD12 is used instead)
#define TOKEN_BLOCK_STOP_WRITE_MULTI        0b11111101

/// Data response tokens

/// Mask (high 3 bits are undefined for data response tokens)
#define TOKEN_RSP_MASK        0b11111
/// Data accepted
#define TOKEN_RSP_OK          0b00101
/// Data rejected due to CRC error
#define TOKEN_RSP_CRC_ERR     0b01011
/// Data rejected due to write error
#define TOKEN_RSP_WRITE_ERR   0b01101

/// Data error tokens have format 0b0000xyzw where xyzw are single bit flags.
/// MASK and VAL are used to check if a token is an error token
#define TOKEN_ERR_MASK      0b11110000
#define TOKEN_ERR_VAL       0b00000000

/// Argument is out of range
#define TOKEN_ERR_RANGE     BIT(3)
/// Card internal ECC error
#define TOKEN_ERR_CARD_ECC  BIT(2)
/// Card controller error
#define TOKEN_ERR_INTERNAL  BIT(1)
/// Card is locked
#define TOKEN_ERR_LOCKED    BIT(0)

/// Transfer format in SPI mode. See section 7.3.1.1 of SD simplified spec.
typedef struct {
    // These fields form the command sent from host to the card (6 bytes)
    uint8_t cmd_index : 6;
    uint8_t transmission_bit : 1;
    uint8_t start_bit : 1;
    uint8_t arguments[4];
    uint8_t stop_bit : 1;
    uint8_t crc7 : 7;
    /// Ncr is the dead time between command and response; should be 0xff
    uint8_t ncr;
    /// Response data, should be set by host to 0xff for read operations
    uint8_t r1;
    /// Up to 16 bytes of response. Luckily, this is aligned on 4 byte boundary.
    uint32_t response[4];
    /// response timeout, in milliseconds
    int timeout_ms;
} sdspi_hw_cmd_t;

#define SDSPI_CMD_SIZE      6
#define SDSPI_NCR_MIN_SIZE  1
#define SDSPI_NCR_MAX_SIZE  8

//the size here contains 6 bytes of CMD, 1 bytes of dummy and the actual response
#define SDSPI_CMD_NORESP_SIZE   (SDSPI_CMD_SIZE+0)   //!< Size of the command without any response
#define SDSPI_CMD_R1_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+1)   //!< Size of the command with R1 response
#define SDSPI_CMD_R2_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+2)   //!< Size of the command with R1b response
#define SDSPI_CMD_R3_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+5)  //!< Size of the command with R3 response
#define SDSPI_CMD_R4_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+5)  //!< Size of the command with R4 response
#define SDSPI_CMD_R5_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+2)   //!< Size of the command with R5 response
#define SDSPI_CMD_R7_SIZE       (SDSPI_CMD_SIZE+SDSPI_NCR_MIN_SIZE+5)  //!< Size of the command with R7 response

#define SDSPI_CMD_FLAG_DATA     BIT(0)  //!< Command has data transfer
#define SDSPI_CMD_FLAG_WRITE    BIT(1)  //!< Data is written to the card
#define SDSPI_CMD_FLAG_RSP_R1   BIT(2)  //!< Response format R1 (1 byte)
#define SDSPI_CMD_FLAG_RSP_R1B  BIT(3)  //!< Response format R1 (1 byte), with busy polling
#define SDSPI_CMD_FLAG_RSP_R2   BIT(4)  //!< Response format R2 (2 bytes)
#define SDSPI_CMD_FLAG_RSP_R3   BIT(5)  //!< Response format R3 (5 bytes)
#define SDSPI_CMD_FLAG_RSP_R4   BIT(6)  //!< Response format R4 (5 bytes)
#define SDSPI_CMD_FLAG_RSP_R5   BIT(7)  //!< Response format R5 (2 bytes)
#define SDSPI_CMD_FLAG_RSP_R7   BIT(8)  //!< Response format R7 (5 bytes)
#define SDSPI_CMD_FLAG_NORSP    BIT(9)  //!< Don't expect response (used when sending CMD0 first time).
#define SDSPI_CMD_FLAG_MULTI_BLK BIT(10) //!< For the write multiblock commands, the start token should be different

#define SDSPI_MAX_DATA_LEN      512     //!< Max size of single block transfer

void make_hw_cmd(uint32_t opcode, uint32_t arg, int timeout_ms, sdspi_hw_cmd_t *hw_cmd);

esp_err_t sdspi_host_start_command(sdspi_dev_handle_t handle, sdspi_hw_cmd_t *cmd,
                                   void *data, uint32_t data_size, int flags);
