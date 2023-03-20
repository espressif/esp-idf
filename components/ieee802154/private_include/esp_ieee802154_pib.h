/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/ieee802154_ll.h"
#include "esp_ieee802154_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The PAN information base(PIB) of IEEE802154 radio.
 */
typedef struct {
    bool auto_ack_tx;                                                               /*!< A flag indicates auto-tx ack mode is enabled or not */
    bool auto_ack_rx;                                                               /*!< A flag indicates auto-rx ack mode is enabled or not */
    bool enhance_ack_tx;                                                            /*!< A flag indicates enh-ack timing mode is enabled or not */
    bool promiscuous;                                                               /*!< A flag indicates promiscuous mode is enabled or not */
    bool coordinator;                                                               /*!< A flag indicates the device is coordinator or not*/
    bool rx_when_idle;                                                              /*!< A flag indicates the device is rx on when idle or not */
    int8_t txpower;                                                                 /*!< Tx power configuration */
    uint8_t channel;                                                                /*!< Channel configuration  */
    ieee802154_ll_pending_mode_t pending_mode;                                         /*!< Pending mode configuration  */
    int8_t cca_threshold;                                                           /*!< CCA threshold */
    ieee802154_ll_cca_mode_t cca_mode;                                                 /*!< CCA mode */
} ieee802154_pib_t;

/**
 * @brief  Initialize the PAN information base(PIB) of IEEE802154 radio.
 *
 */
void ieee802154_pib_init(void);

/**
 * @brief  Update the PAN information base(PIB) of IEEE802154 radio.
 *
 */
void ieee802154_pib_update(void);

/**
 * @brief  Get the state of PIB, if is pending, the PIB should be updated to hardware.
 *
 * @return
 *        - True if the PIB is pending, otherwise false.
 */
bool ieee802154_pib_is_pending(void);

/**
 * @brief  Set a specific channel to the PIB.
 *
 * @param[in]  channel  The channel.
 *
 */
void ieee802154_pib_set_channel(uint8_t channel);

/**
 * @brief  Get the channel from the PIB.
 *
 * @return
 *        - The channel has been set in the PIB.
 */
uint8_t ieee802154_pib_get_channel(void);

/**
 * @brief  Set a specific transmission power to the PIB.
 *
 * @param[in]  power  The power.
 *
 */
void ieee802154_pib_set_power(int8_t power);

/**
 * @brief  Get the transmission power from the PIB.
 *
 * @return
 *        - The transmission power has been set in the PIB.
 */
int8_t ieee802154_pib_get_power(void);

/**
 * @brief  Set the promiscuous mode to the PIB.
 *
 * @param[in]  enable  True for enabling the promiscuous mode, otherwise false.
 *
 */
void ieee802154_pib_set_promiscuous(bool enable);

/**
 * @brief  Get the promiscuous mode from the PIB.
 *
 * @return
 *        - The promiscuous mode has been set in the PIB.
 */
bool ieee802154_pib_get_promiscuous(void);

/**
 * @brief  Set a specific CCA threshold to the PIB.
 *
 * @param[in]  cca_threshold  The CCA threshold.
 *
 */
void ieee802154_pib_set_cca_threshold(int8_t cca_threshold);

/**
 * @brief  Get the CCA threshold from the PIB.
 *
 * @return
 *        - The CCA threshold has been set in the PIB.
 */
 int8_t ieee802154_pib_get_cca_threshold(void);

/**
 * @brief  Set the CCA mode to the PIB.
 *
 * @param[in]  cca_mode  The CCA mode.
 *
 */
void ieee802154_pib_set_cca_mode(ieee802154_ll_cca_mode_t cca_mode);

/**
 * @brief  Get the CCA mode from the PIB.
 *
 * @return
 *        - The CCA mode has been set in the PIB.
 */
ieee802154_ll_cca_mode_t ieee802154_pib_get_cca_mode(void);

/**
 * @brief  Configure the auto-ack transmission mode to the PIB.
 *
 * @param[in]  enable  True for enabling the auto-ack transmission mode, otherwise false.
 *
 */
void ieee802154_pib_set_auto_ack_tx(bool enable);

/**
 * @brief  Get the auto-ack transmission mode from the PIB.
 *
 * @return
 *        - The auto-ack transmission mode has been set in the PIB.
 */
bool ieee802154_pib_get_auto_ack_tx(void);

/**
 * @brief  Configure the auto-ack receiving mode to the PIB.
 *
 * @param[in]  enable  True for enabling the auto-ack receiving mode, otherwise false.
 *
 */
void ieee802154_pib_set_auto_ack_rx(bool enable);

/**
 * @brief  Get the auto-ack receiving mode from the PIB.
 *
 * @return
 *        - The auto-ack receiving mode has been set in the PIB.
 */
bool ieee802154_pib_get_auto_ack_rx(void);

/**
 * @brief  Configure the enh-ack timing mode to the PIB.
 *
 * @param[in]  enable  True for enabling the enh-ack timing mode, otherwise false.
 *
 */
void ieee802154_pib_set_enhance_ack_tx(bool enable);

/**
 * @brief  Get the enh-ack timing mode from the PIB.
 *
 * @return
 *        - The enh-ack timing mode has been set in the PIB.
 */
bool ieee802154_pib_get_enhance_ack_tx(void);

/**
 * @brief  Configure this bit to the PIB to indicate the device is coordinator.
 *
 * @param[in]  enable  True for configuring the device coordinator, otherwise false.
 *
 */
void ieee802154_pib_set_coordinator(bool enable);

/**
 * @brief  Get the coordinator configuration of device from the PIB.
 *
 * @return
 *        - The coordinator configuration of device has been set in the PIB.
 */
bool ieee802154_pib_get_coordinator(void);

/**
 * @brief  Configure the pending mode to the PIB.
 *
 * @param[in]  pending_mode  The pending mode.
 *
 */
void ieee802154_pib_set_pending_mode(ieee802154_ll_pending_mode_t pending_mode);

/**
 * @brief  Get the pending mode from the PIB.
 *
 * @return
 *        - The pending mode has been set in the PIB.
 */
ieee802154_ll_pending_mode_t ieee802154_pib_get_pending_mode(void);

/**
 * @brief  Configure the radio mode when the radio is going to enter idle to the PIB.
 *
 * @param[in]  enable  True for continuing to receive when the radio is going to enter ilde, otherwise false.
 *
 */
void ieee802154_pib_set_rx_when_idle(bool enable);

/**
 * @brief  Get the radio mode when the radio is going to enter ilde to the PIB.
 *
 * @return
 *        - True for continuing to receive when the radio is going to enter ilde, otherwise false.
 *
 */
bool ieee802154_pib_get_rx_when_idle(void);

#ifdef __cplusplus
}
#endif
