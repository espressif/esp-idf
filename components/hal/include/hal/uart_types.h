/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "esp_assert.h"

/**
 * @brief UART port number, can be UART_NUM_0 ~ (UART_NUM_MAX -1).
 */
typedef enum {
    UART_NUM_0,                         /*!< UART port 0 */
    UART_NUM_1,                         /*!< UART port 1 */
#if SOC_UART_HP_NUM > 2
    UART_NUM_2,                         /*!< UART port 2 */
#endif
#if SOC_UART_HP_NUM > 3
    UART_NUM_3,                         /*!< UART port 3 */
#endif
#if SOC_UART_HP_NUM > 4
    UART_NUM_4,                         /*!< UART port 4 */
#endif
#if (SOC_UART_LP_NUM >= 1)
    LP_UART_NUM_0,                      /*!< LP UART port 0 */
#endif
    UART_NUM_MAX,                       /*!< UART port max */
} uart_port_t;

ESP_STATIC_ASSERT(UART_NUM_MAX == SOC_UART_NUM, "UART_NUM_MAX does not match SOC_UART_NUM");

/**
 * @brief UART mode selection
 */
typedef enum {
    UART_MODE_UART = 0x00,                      /*!< mode: regular UART mode*/
    UART_MODE_RS485_HALF_DUPLEX = 0x01,         /*!< mode: half duplex RS485 UART mode control by RTS pin */
    UART_MODE_IRDA = 0x02,                      /*!< mode: IRDA  UART mode*/
    UART_MODE_RS485_COLLISION_DETECT = 0x03,    /*!< mode: RS485 collision detection UART mode (used for test purposes)*/
    UART_MODE_RS485_APP_CTRL = 0x04,            /*!< mode: application control RS485 UART mode (used for test purposes)*/
} uart_mode_t;

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS   = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS   = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS   = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS   = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0x4,
} uart_word_length_t;

/**
 * @brief UART stop bits number
 */
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< stop bit: 1bit*/
    UART_STOP_BITS_1_5 = 0x2,  /*!< stop bit: 1.5bits*/
    UART_STOP_BITS_2   = 0x3,  /*!< stop bit: 2bits*/
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE  = 0x0,  /*!< Disable UART parity*/
    UART_PARITY_EVEN     = 0x2,  /*!< Enable UART even parity*/
    UART_PARITY_ODD      = 0x3   /*!< Enable UART odd parity*/
} uart_parity_t;

/**
 * @brief UART hardware flow control modes
 */
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< disable hardware flow control*/
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< enable RX hardware flow control (rts)*/
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< enable TX hardware flow control (cts)*/
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< enable hardware flow control*/
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

/**
 * @brief UART signal bit map
 */
typedef enum {
    UART_SIGNAL_INV_DISABLE  =  0,            /*!< Disable UART signal inverse*/
    UART_SIGNAL_IRDA_TX_INV  = (0x1 << 0),    /*!< inverse the UART irda_tx signal*/
    UART_SIGNAL_IRDA_RX_INV  = (0x1 << 1),    /*!< inverse the UART irda_rx signal*/
    UART_SIGNAL_RXD_INV      = (0x1 << 2),    /*!< inverse the UART rxd signal*/
    UART_SIGNAL_CTS_INV      = (0x1 << 3),    /*!< inverse the UART cts signal*/
    UART_SIGNAL_DSR_INV      = (0x1 << 4),    /*!< inverse the UART dsr signal*/
    UART_SIGNAL_TXD_INV      = (0x1 << 5),    /*!< inverse the UART txd signal*/
    UART_SIGNAL_RTS_INV      = (0x1 << 6),    /*!< inverse the UART rts signal*/
    UART_SIGNAL_DTR_INV      = (0x1 << 7),    /*!< inverse the UART dtr signal*/
} uart_signal_inv_t;

/**
 * @brief UART source clock
 */
typedef soc_periph_uart_clk_src_legacy_t uart_sclk_t;

#if (SOC_UART_LP_NUM >= 1)
/**
 * @brief LP_UART source clock
 */
typedef soc_periph_lp_uart_clk_src_t lp_uart_sclk_t;
#endif

/**
 * @brief UART AT cmd char configuration parameters
 *        Note that this function may different on different chip. Please refer to the TRM at confirguration.
 */
typedef struct {
    uint8_t  cmd_char;             /*!< UART AT cmd char*/
    uint8_t  char_num;             /*!< AT cmd char repeat number*/
    uint32_t gap_tout;             /*!< gap time(in baud-rate) between AT cmd char*/
    uint32_t pre_idle;             /*!< the idle time(in baud-rate) between the non AT char and first AT char*/
    uint32_t post_idle;            /*!< the idle time(in baud-rate) between the last AT char and the none AT char*/
} uart_at_cmd_t;

/**
 * @brief UART software flow control configuration parameters
 */
typedef struct {
    uint8_t  xon_char;      /*!< Xon flow control char*/
    uint8_t  xoff_char;     /*!< Xoff flow control char*/
    uint8_t xon_thrd;      /*!< If the software flow control is enabled and the data amount in rxfifo is less than xon_thrd, an xon_char will be sent*/
    uint8_t xoff_thrd;       /*!< If the software flow control is enabled and the data amount in rxfifo is more than xoff_thrd, an xoff_char will be sent*/
} uart_sw_flowctrl_t;

#ifdef __cplusplus
}
#endif
