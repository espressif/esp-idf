/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __GPIO_PCM_CONFIG_H__
#define __GPIO_PCM_CONFIG_H__

#define ACOUSTIC_ECHO_CANCELLATION_ENABLE    1

void app_gpio_pcm_io_cfg(void);

#if ACOUSTIC_ECHO_CANCELLATION_ENABLE
void app_gpio_aec_io_cfg(void);
#endif /* ACOUSTIC_ECHO_CANCELLATION_ENABLE */

#endif /* #define __GPIO_PCM_CONFIG_H__ */
