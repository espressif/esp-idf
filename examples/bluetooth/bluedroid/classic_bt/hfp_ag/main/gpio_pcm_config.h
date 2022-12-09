/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __GPIO_PCM_CONFIG_H__
#define __GPIO_PCM_CONFIG_H__

#define ACOUSTIC_ECHO_CANCELLATION_ENABLE    1

void app_gpio_pcm_io_cfg(void);

#if ACOUSTIC_ECHO_CANCELLATION_ENABLE
void app_gpio_aec_io_cfg(void);
#endif /* ACOUSTIC_ECHO_CANCELLATION_ENABLE */

#endif /* #define __GPIO_PCM_CONFIG_H__ */
