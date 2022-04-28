/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef __ESP_BROWNOUT_H
#define __ESP_BROWNOUT_H

#ifdef __cplusplus
extern "C" {
#endif

void esp_brownout_init(void);

void esp_brownout_disable(void);

#ifdef __cplusplus
}
#endif

#endif
