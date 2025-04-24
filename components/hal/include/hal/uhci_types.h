/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UHCI escape sequence
 */
typedef struct {
    uint8_t seper_chr;        /*!< escape sequence character */
    uint8_t sub_chr1;         /*!< escape sequence sub-character 1 */
    uint8_t sub_chr2;         /*!< escape sequence sub-character 2 */
    bool sub_chr_en;          /*!< enable use of sub-chaacter of escape sequence */
} uhci_seper_chr_t;

/**
 * @brief UHCI software flow control
 */
typedef struct {
    uint8_t xon_chr;          /*!< character for XON */
    uint8_t xon_sub1;         /*!< sub-character 1 for XON */
    uint8_t xon_sub2;         /*!< sub-character 2 for XON */
    uint8_t xoff_chr;         /*!< character 2 for XOFF */
    uint8_t xoff_sub1;        /*!< sub-character 1 for XOFF */
    uint8_t xoff_sub2;        /*!< sub-character 2 for XOFF */
    uint8_t flow_en;          /*!< enable use of software flow control */
} uhci_swflow_ctrl_sub_chr_t;

#ifdef __cplusplus
}
#endif
