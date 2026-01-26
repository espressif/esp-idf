/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function has different implementations depending on the product type.
 *
 * See func_dev.c and func_prod.c. Which of the files is compiled is determined in
 * CMakeLists.txt,
 */
void func(void);

#ifdef __cplusplus
}
#endif
