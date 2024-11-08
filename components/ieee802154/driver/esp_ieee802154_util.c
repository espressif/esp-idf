/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/esp_modem_clock.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "hal/ieee802154_ll.h"
#include "esp_coex_i154.h"
#include "esp_ieee802154_util.h"

uint8_t ieee802154_freq_to_channel(uint8_t freq)
{
    return (freq - 3) / 5 + 11;
}

uint8_t ieee802154_channel_to_freq(uint8_t channel)
{
    return (channel - 11) * 5 + 3;
}

#if !CONFIG_IEEE802154_TEST && (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
void ieee802154_set_txrx_pti(ieee802154_txrx_scene_t txrx_scene)
{

    switch (txrx_scene) {
    case IEEE802154_SCENE_IDLE:
        esp_coex_ieee802154_txrx_pti_set(IEEE802154_IDLE);
        break;
    case IEEE802154_SCENE_TX:
    case IEEE802154_SCENE_RX:
        esp_coex_ieee802154_txrx_pti_set(IEEE802154_LOW);
        break;
    case IEEE802154_SCENE_TX_AT:
    case IEEE802154_SCENE_RX_AT:
        esp_coex_ieee802154_txrx_pti_set(IEEE802154_MIDDLE);
        break;
    default:
        assert(false);
        break;
    }
}
#endif // !CONFIG_IEEE802154_TEST && CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE

// TZ-97: implement these two functions using ETM common interface
void ieee802154_etm_channel_clear(uint32_t channel)
{
    if (!(REG_READ(ETM_CHEN_AD0_REG) & (1 << channel))) {
        REG_WRITE(ETM_CHENCLR_AD0_REG, (REG_READ(ETM_CHENCLR_AD0_REG)) | 1 << channel);
    }
}

void ieee802154_etm_set_event_task(uint32_t channel, uint32_t event, uint32_t task)
{
    ieee802154_etm_channel_clear(channel);

    REG_WRITE((ETM_CH0_EVT_ID_REG + ETM_CH_OFFSET * channel), event);
    REG_WRITE((ETM_CH0_TASK_ID_REG + ETM_CH_OFFSET * channel), task);

    REG_WRITE(ETM_CHENSET_AD0_REG, (REG_READ(ETM_CHENSET_AD0_REG) | 1 << channel));
}
