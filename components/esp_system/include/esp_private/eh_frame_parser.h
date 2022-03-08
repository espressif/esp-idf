/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EH_FRAME_PARSER_H
#define EH_FRAME_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print backtrace for the given execution frame.
 *
 * @param frame_or Snapshot of the CPU registers when the program stopped its
 *                 normal execution. This frame is usually generated on the
 *                 stack when an exception or an interrupt occurs.
 */
void esp_eh_frame_print_backtrace(const void *frame_or);

#ifdef __cplusplus
}
#endif

#endif
