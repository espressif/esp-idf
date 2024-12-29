/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_INTERRUPT_STRUCT_H_
#define _SOC_INTERRUPT_STRUCT_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct interrupt_dev_s {
} interrupt_dev_t;
extern interrupt_dev_t INTERRUPT;
#ifdef __cplusplus
}
#endif

#endif /*_SOC_INTERRUPT_STRUCT_H_ */
