/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief BitScrambler LUT width
 */
typedef enum {
    BITSCRAMBLER_LUT_WIDTH_8BIT = 0,
    BITSCRAMBLER_LUT_WIDTH_16BIT = 1,
    BITSCRAMBLER_LUT_WIDTH_32BIT = 2
} bitscrambler_lut_width_t;


/**
 * @brief BitScrambler loopback select
 */
typedef enum {
    BITSCRAMBLER_LOOP_DISABLE = 0,
    BITSCRAMBLER_LOOP_ENABLE = 1
} bitscrambler_loop_mode_t;


/**
 * @brief EOF mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_EOF_MODE_READ = 0,
    BITSCRAMBLER_EOF_MODE_WRITE = 1
} bitscrambler_eof_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_COND_MODE_LESSTHAN = 0,
    BITSCRAMBLER_COND_MODE_NOTEQUAL = 1
} bitscrambler_cond_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_PREFETCH_ENABLED = 0,
    BITSCRAMBLER_PREFETCH_DISABLED = 1
} bitscrambler_prefetch_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_HALT_WAIT_WRITES = 0,
    BITSCRAMBLER_HALT_IGNORE_WRITES = 1
} bitscrambler_halt_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_DUMMY_MODE_HALT = 0,
    BITSCRAMBLER_DUMMY_MODE_DUMMY = 1
} bitscrambler_dummy_mode_t;

/**
 * @brief Condition mode of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_SET_STATE_RUN,
    BITSCRAMBLER_SET_STATE_HALT,
    BITSCRAMBLER_SET_STATE_PAUSE
} bitscrambler_set_state_t;

/**
 * @brief Status of bitscrambler
 */
typedef enum {
    BITSCRAMBLER_STATE_IDLE,
    BITSCRAMBLER_STATE_RUN,
    BITSCRAMBLER_STATE_WAIT,
    BITSCRAMBLER_STATE_PAUSED,
    BITSCRAMBLER_STATE_UNKNOWN /*!< Invalid, should never be returned */
} bitscrambler_state_t;


#ifdef __cplusplus
}
#endif
