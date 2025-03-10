/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BitScrambler transport direction
 */
typedef enum {
    BITSCRAMBLER_DIR_TX = 0,    /*!< Transmit, so mem to device (or mem to mem in loopback mode) */
    BITSCRAMBLER_DIR_RX = 1     /*!< Receive, so device to memory */
} bitscrambler_direction_t;

/**
 * @brief BitScrambler LUT (look up table) width
 */
typedef enum {
    BITSCRAMBLER_LUT_WIDTH_8BIT = 0,  /*!< 8-bit LUT */
    BITSCRAMBLER_LUT_WIDTH_16BIT = 1, /*!< 16-bit LUT */
    BITSCRAMBLER_LUT_WIDTH_32BIT = 2, /*!< 32-bit LUT */
} bitscrambler_lut_width_t;


/**
 * @brief EOF signal generating mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_EOF_MODE_READ = 0,  /*!< EOF is counted by reading the source FIFO */
    BITSCRAMBLER_EOF_MODE_WRITE = 1, /*!< EOF is counted by writing the destination FIFO */
} bitscrambler_eof_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_COND_MODE_LESSTHAN = 0, /*!< Use "<=" to generate the condition */
    BITSCRAMBLER_COND_MODE_NOTEQUAL = 1, /*!< Use "!=" to generate the condition */
} bitscrambler_cond_mode_t;

/**
 * @brief Halt mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_HALT_WAIT_WRITES = 0,   /*!< Wait data write back */
    BITSCRAMBLER_HALT_IGNORE_WRITES = 1, /*!< Ignore write data back */
} bitscrambler_halt_mode_t;

/**
 * @brief Dummy mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_DUMMY_MODE_HALT = 0,  /*!< Halt and wait the read data */
    BITSCRAMBLER_DUMMY_MODE_DUMMY = 1, /*!< Ignore the read data */
} bitscrambler_dummy_mode_t;

/**
 * @brief Commands to set the state of bitscrambler
 *
 * @note Pause->Run, bitscrambler can continue from the last instruction;
 *       Halt->Run, bitscrambler will start from the first instruction;
 */
typedef enum {
    BITSCRAMBLER_SET_STATE_RUN,   /*!< Run */
    BITSCRAMBLER_SET_STATE_HALT,  /*!< Halt */
    BITSCRAMBLER_SET_STATE_PAUSE, /*!< Pause */
} bitscrambler_set_state_t;

/**
 * @brief Status of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_STATE_IDLE,   /*!< Idle (halt) */
    BITSCRAMBLER_STATE_RUN,    /*!< Running */
    BITSCRAMBLER_STATE_WAIT,   /*!< Waiting data write back */
    BITSCRAMBLER_STATE_PAUSED, /*!< Paused */
    BITSCRAMBLER_STATE_UNKNOWN /*!< Invalid, should never be returned */
} bitscrambler_state_t;

#ifdef __cplusplus
}
#endif
