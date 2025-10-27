/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/gpio_ext_struct.h"
#include "soc/soc_etm_source.h"

#define GPIO_LL_ETM_EVENT_ID_POS_EDGE(ch) (GPIO_EVT_CH0_RISE_EDGE + (ch))
#define GPIO_LL_ETM_EVENT_ID_NEG_EDGE(ch) (GPIO_EVT_CH0_FALL_EDGE + (ch))
#define GPIO_LL_ETM_EVENT_ID_ANY_EDGE(ch) (GPIO_EVT_CH0_ANY_EDGE + (ch))

#define GPIO_LL_ETM_TASK_ID_SET(ch) (GPIO_TASK_CH0_SET + (ch))
#define GPIO_LL_ETM_TASK_ID_CLR(ch) (GPIO_TASK_CH0_CLEAR + (ch))
#define GPIO_LL_ETM_TASK_ID_TOG(ch) (GPIO_TASK_CH0_TOGGLE + (ch))

#define GPIO_LL_ETM_EVENT_CHANNELS_PER_GROUP    8
#define GPIO_LL_ETM_TASK_CHANNELS_PER_GROUP     8

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set which GPIO to be bound to the event channel
 *
 * @note Different channels can be bound to one GPIO
 *
 * @param dev Register base address
 * @param chan GPIO ETM Event channel number
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_etm_event_channel_set_gpio(gpio_etm_dev_t *dev, uint32_t chan, uint32_t gpio_num)
{
    dev->etm_event_chn_cfg[chan].etm_chn_event_sel = gpio_num;
}

/**
 * @brief Whether to enable the event channel
 *
 * @param dev Register base address
 * @param chan GPIO ETM Event channel number
 * @param enable True to enable, false to disable
 */
static inline void gpio_ll_etm_enable_event_channel(gpio_etm_dev_t *dev, uint32_t chan, bool enable)
{
    dev->etm_event_chn_cfg[chan].etm_chn_event_en = enable;
}

/**
 * @brief Get which GPIO is bound to the event channel
 *
 * @param dev Register base address
 * @param chan GPIO ETM Event channel number
 * @return GPIO number
 */
static inline uint32_t gpio_ll_etm_event_channel_get_gpio(gpio_etm_dev_t *dev, uint32_t chan)
{
    return dev->etm_event_chn_cfg[chan].etm_chn_event_sel;
}

/**
 * @brief Set which GPIO to be bound to the task channel
 *
 * @note One channel can be bound to multiple different GPIOs
 *
 * @param dev Register base address
 * @param chan GPIO ETM Task channel number
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_etm_gpio_set_task_channel(gpio_etm_dev_t *dev, uint32_t gpio_num, uint32_t chan)
{
    int g_p = gpio_num / 5;
    int g_idx = gpio_num % 5;
    uint32_t reg_val = dev->etm_task_pn_cfg[g_p].val;
    reg_val &= ~(0x07 << (g_idx * 6));
    reg_val |= ((chan & 0x07) << (g_idx * 6));
    dev->etm_task_pn_cfg[g_p].val = reg_val;
}

/**
 * @brief Whether to enable the GPIO to be managed by the task channel
 *
 * @param dev Register base address
 * @param gpio_num GPIO number
 * @param enable True to enable, false to disable
 */
static inline void gpio_ll_etm_enable_task_gpio(gpio_etm_dev_t *dev, uint32_t gpio_num, bool enable)
{
    int g_p = gpio_num / 5;
    int g_idx = gpio_num % 5;
    uint32_t reg_val = dev->etm_task_pn_cfg[g_p].val;
    reg_val &= ~(0x01 << (g_idx * 6 + 5));
    reg_val |= ((enable & 0x01) << (g_idx * 6 + 5));
    dev->etm_task_pn_cfg[g_p].val = reg_val;
}

/**
 * @brief Check whether a GPIO has been enabled and managed by a task channel
 *
 * @param dev Register base address
 * @param gpio_num GPIO number
 * @return True if enabled, false otherwise
 */
static inline bool gpio_ll_etm_is_task_gpio_enabled(gpio_etm_dev_t *dev, uint32_t gpio_num)
{
    int g_p = gpio_num / 5;
    int g_idx = gpio_num % 5;
    return dev->etm_task_pn_cfg[g_p].val & (0x01 << (g_idx * 6 + 5));
}

/**
 * @brief Get the channel number that the GPIO is bound to
 *
 * @param dev Register base address
 * @param gpio_num GPIO number
 * @return GPIO ETM Task channel number
 */
static inline uint32_t gpio_ll_etm_gpio_get_task_channel(gpio_etm_dev_t *dev, uint32_t gpio_num)
{
    int g_p = gpio_num / 5;
    int g_idx = gpio_num % 5;
    return (dev->etm_task_pn_cfg[g_p].val >> (g_idx * 6)) & 0x07;
}

#ifdef __cplusplus
}
#endif
