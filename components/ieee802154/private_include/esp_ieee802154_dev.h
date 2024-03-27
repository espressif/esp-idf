/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/soc.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IEEE802154_TAG "ieee802154"

// These three macros are in microseconds, used for transmit_at
#define IEEE802154_ED_TRIG_TX_RAMPUP_TIME_US   256
#define IEEE802154_TX_RAMPUP_TIME_US           98
#define IEEE802154_RX_RAMPUP_TIME_US           146

/**
 * @brief The state of IEEE802154 radio process.
 */
typedef enum {
    IEEE802154_STATE_DISABLE,               /*!< IEEE802154 radio state disable */
    IEEE802154_STATE_IDLE,                  /*!< IEEE802154 radio state idle */
    IEEE802154_STATE_SLEEP,                 /*!< IEEE802154 radio state sleep */
    IEEE802154_STATE_RX,                    /*!< IEEE802154 radio state rx */
    IEEE802154_STATE_TX_ACK,                /*!< IEEE802154 radio state tx ack */
    IEEE802154_STATE_TX_ENH_ACK,            /*!< IEEE802154 radio state tx enh-ack */
    IEEE802154_STATE_TX_CCA,                /*!< IEEE802154 radio state cca trigger tx */
    IEEE802154_STATE_TX,                    /*!< IEEE802154 radio state tx */
    IEEE802154_STATE_TEST_TX,               /*!< IEEE802154 radio state test mode tx */
    IEEE802154_STATE_RX_ACK,                /*!< IEEE802154 radio state rx ack */
    IEEE802154_STATE_ED,                    /*!< IEEE802154 radio state ED */
    IEEE802154_STATE_CCA,                   /*!< IEEE802154 radio state CCA */
} ieee802154_state_t;

/**
 * @brief  Initialize the clock of IEEE 802.15.4 subsystem.
 *
 */
void ieee802154_enable(void);

/**
 * @brief  Deinitialize the clock of IEEE 802.15.4 subsystem.
 *
 */
void ieee802154_disable(void);

/**
 * @brief  Enable the RF.
 *
 */
void ieee802154_rf_enable(void);

/**
 * @brief  Disable the RF.
 *
 */
void ieee802154_rf_disable(void);
/**
 * @brief  Initialize the IEEE 802.15.4 MAC.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 *
 */
esp_err_t ieee802154_mac_init(void);

/**
 * @brief  Deinitialize the IEEE 802.15.4 MAC.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 *
 */
esp_err_t ieee802154_mac_deinit(void);

/**
 * @brief  Transmit the given frame.
 *
 * @param[in]  frame  The pointer to the frame
 * @param[in]  cca    Perform CCA before transmission if it's true, otherwise transmit the frame directly.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t ieee802154_transmit(const uint8_t *frame, bool cca);

/**
 * @brief  Set the IEEE 802.15.4 Radio to receive state.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t ieee802154_receive(void);

/**
 * @brief  Notify the IEEE 802.15.4 Radio that the frame is handled done by upper layer.
 *
 * @param[in]  frame  The pointer to the frame which was passed from the function esp_ieee802154_receive_done.
 *                    or ack frame from esp_ieee802154_transmit_done.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if frame is invalid.
 *
 */
esp_err_t ieee802154_receive_handle_done(const uint8_t* frame);

/**
 * @brief  Transmit the given frame at a specific time.
 *
 * @param[in]  frame  The pointer to the frame. Refer to `esp_ieee802154_transmit()`.
 * @param[in]  cca    Perform CCA before transmission if it's true, otherwise transmit the frame directly.
 * @param[in]  time  A specific timestamp for starting transmission.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 * Note: The transmit result will be reported via esp_ieee802154_transmit_done()
 *       or esp_ieee802154_transmit_failed().
 *
 */
esp_err_t ieee802154_transmit_at(const uint8_t *frame, bool cca, uint32_t time);

/**
 * @brief  Set the IEEE 802.15.4 Radio to receive state at a specific time.
 *
 *
 * @param[in]  time  A specific timestamp for starting receiving.
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure due to invalid state.
 *
 * Note: Radio will start receiving after the timestamp, and continue receiving until it receives a valid frame.
 *       Ref to esp_ieee802154_receive_done().
 *
 */
esp_err_t ieee802154_receive_at(uint32_t time);

/**
 * @brief  Set the IEEE 802.15.4 Radio to sleep state.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t ieee802154_sleep(void);

/**
 * @brief  Perform energy detection(ED).
 *
 * @param[in]  duration  The duration of energy detection, in symbol unit (16 us).
 *                       The result will be reported via esp_ieee802154_energy_detect_done().
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t ieee802154_energy_detect(uint32_t duration);

/**
 * @brief  Perform clear channel assessment(CCA).
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t ieee802154_cca(void);

/**
 * @brief  Get the RSSI of the most recent received frame.
 *
 * @return The value of RSSI.
 *
 */
int8_t ieee802154_get_recent_rssi(void);

/**
 * @brief  Get the LQI of the most recent received frame.
 *
 * @return The value of LQI.
 *
 */
uint8_t ieee802154_get_recent_lqi(void);

/**
 * @brief  Get the IEEE 802.15.4 Radio state.
 *
 * @return
 *      - Current state of IEEE 802.15.4 Radio.
 *
 */
ieee802154_state_t ieee802154_get_state(void);

/** The following three functions are only used for internal test. **/
/**
 * @brief  The clear channel assessment done.
 *
 * @param[in]  channel_free  True if the channel is clear, otherwise false.
 *
 */
extern void esp_ieee802154_cca_done(bool channel_free);

/**
 * @brief  Current receiving process is failed due to some reasons.
 *
 * @param[in]  error  The specific received failed reason.
 *
 */
extern void esp_ieee802154_receive_failed(uint16_t error);

/**
 * @brief  Current energy detection(ED) is failed due to some reasons.
 *
 * @param[in]  error  The specific ED failed reason.
 *
 */
extern void esp_ieee802154_ed_failed(uint16_t error);

#if CONFIG_IEEE802154_TEST
#define IEEE802154_STATIC
#define IEEE802154_INLINE
extern void esp_ieee802154_timer0_done(void);
extern void esp_ieee802154_timer1_done(void);
#else
#define IEEE802154_STATIC  static
#define IEEE802154_INLINE  inline
#endif // CONFIG_IEEE802154_TEST

#ifdef __cplusplus
}
#endif
