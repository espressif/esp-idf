/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_ieee802154_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initialize the IEEE 802.15.4 subsystem.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 *
 */
esp_err_t esp_ieee802154_enable(void);

/**
 * @brief  Deinitialize the IEEE 802.15.4 subsystem.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_disable(void);

/**
 * @brief  Get the operational channel.
 *
 * @return The channel number (11~26).
 *
 */
uint8_t esp_ieee802154_get_channel(void);

/**
 * @brief  Set the operational channel.
 *
 * @param[in]  channel  The channel number (11-26).
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_channel(uint8_t channel);

/**
 * @brief  Get the transmit power.
 *
 * @return The transmit power in dBm.
 *
 */
int8_t esp_ieee802154_get_txpower(void);

/**
 * @brief  Set the transmit power.
 *
 * @param[in]  power  The transmit power in dBm.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_txpower(int8_t power);

/**
 * @brief  Get the promiscuous mode.
 *
 * @return
 *      - True   The promiscuous mode is enabled.
 *      - False  The promiscuous mode is disabled.
 *
 */
bool esp_ieee802154_get_promiscuous(void);

/**
 * @brief  Set the promiscuous mode.
 *
 * @param[in]  enable  The promiscuous mode to be set.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_promiscuous(bool enable);

/**
 * @brief  Get the IEEE 802.15.4 Radio state.
 *
 * @return  The IEEE 802.15.4 Radio state, refer to esp_ieee802154_state_t.
 *
 */
esp_ieee802154_state_t esp_ieee802154_get_state(void);

/**
 * @brief  Set the IEEE 802.15.4 Radio to sleep state.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_sleep(void);

/**
 * @brief  Set the IEEE 802.15.4 Radio to receive state.
 *
 * @note Radio will continue receiving until it receives a valid frame.
 *       Refer to `esp_ieee802154_receive_done()`.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_receive(void);

/**
 * @brief  Transmit the given frame.
 *         The transmit result will be reported via `esp_ieee802154_transmit_done()`
 *         or `esp_ieee802154_transmit_failed()`.
 *
 * @param[in]  frame  The pointer to the frame, the frame format:
 *                    |-----------------------------------------------------------------------|
 *                    | Len | MHR |              MAC Payload                          |  FCS  |
 *                    |-----------------------------------------------------------------------|
 * @param[in]  cca    Perform CCA before transmission if it's true, otherwise transmit the frame directly.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_transmit(const uint8_t *frame, bool cca);

/**
 * @brief  Set the time to wait for the ack frame.
 *
 * @param[in]  timeout  The time to wait for the ack frame, in symbol unit (16 us).
 *                      Default: 0x006C, Range: 0x0000 - 0xFFFF.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_ack_timeout(uint32_t timeout);

/**
 * @brief  Get the device PAN ID.
 *
 * @return  The device PAN ID.
 *
 */
uint16_t esp_ieee802154_get_panid(void);

/**
 * @brief  Set the device PAN ID.
 *
 * @param[in]  panid  The device PAN ID.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_panid(uint16_t panid);

/**
 * @brief  Get the device short address.
 *
 * @return  The device short address.
 *
 */
uint16_t esp_ieee802154_get_short_address(void);

/**
 * @brief  Set the device short address.
 *
 * @param[in]  short_address  The device short address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_short_address(uint16_t short_address);

/**
 * @brief  Get the device extended address.
 *
 * @param[out]  ext_addr  The pointer to the device extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_get_extended_address(uint8_t *ext_addr);

/**
 * @brief  Set the device extended address.
 *
 * @param[in]  ext_addr  The pointer to the device extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_extended_address(const uint8_t *ext_addr);

/**
 * @brief  Get the device PAN ID for specific interface.
 *
 * @param[in]  index  The interface index.
 *
 * @return  The device PAN ID.
 *
 */
uint16_t esp_ieee802154_get_multipan_panid(esp_ieee802154_multipan_index_t index);

/**
 * @brief  Set the device PAN ID for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  panid  The device PAN ID.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_panid(esp_ieee802154_multipan_index_t index, uint16_t panid);

/**
 * @brief  Get the device short address for specific interface.
 *
 * @param[in]  index  The interface index.
 *
 * @return  The device short address.
 *
 */
uint16_t esp_ieee802154_get_multipan_short_address(esp_ieee802154_multipan_index_t index);

/**
 * @brief  Set the device short address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  short_address  The device short address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_short_address(esp_ieee802154_multipan_index_t index, uint16_t short_address);

/**
 * @brief  Get the device extended address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[out]  ext_addr  The pointer to the device extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_get_multipan_extended_address(esp_ieee802154_multipan_index_t index, uint8_t *ext_addr);

/**
 * @brief  Set the device extended address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  ext_addr  The pointer to the device extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_extended_address(esp_ieee802154_multipan_index_t index, const uint8_t *ext_addr);

/**
 * @brief  Get the device current multipan interface enable mask.
 *
 * @return  Current multipan interface enable mask.
 *
 */
uint8_t esp_ieee802154_get_multipan_enable(void);

/**
 * @brief Enable specific interface for the device.
 *
 * As an example, call `esp_ieee802154_set_multipan_enable(BIT(ESP_IEEE802154_MULTIPAN_0) | BIT(ESP_IEEE802154_MULTIPAN_1));`
 * to enable multipan interface 0 and 1.
 *
 * @param[in]  mask  The multipan interface bit mask.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_enable(uint8_t mask);

/**
 * @brief  Get the device coordinator.
 *
 * @return
 *         - True   The coordinator is enabled.
 *         - False  The coordinator is disabled.
 *
 */
bool esp_ieee802154_get_coordinator(void);

/**
 * @brief  Set the device coordinator role.
 *
 * @param[in]  enable  The coordinator role to be set.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_coordinator(bool enable);

/**
 * @brief  Get the auto frame pending mode.
 *
 * @return  The auto frame pending mode, refer to esp_ieee802154_pending_mode_t.
 *
 */
esp_ieee802154_pending_mode_t esp_ieee802154_get_pending_mode(void);

/**
 * @brief  Set the auto frame pending mode.
 *
 * @param[in]  pending_mode  The auto frame pending mode, refer to esp_ieee802154_pending_mode_t.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_pending_mode(esp_ieee802154_pending_mode_t pending_mode);

/**
 * @brief  Add address to the source matching table.
 *
 * @param[in]  addr      The pointer to the address.
 * @param[in]  is_short  Short address or Extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_NO_MEM if the pending table is full.
 *
 */
esp_err_t esp_ieee802154_add_pending_addr(const uint8_t *addr, bool is_short);

/**
 * @brief  Remove address from the source matching table.
 *
 * @param[in]  addr      The pointer to the address.
 * @param[in]  is_short  Short address or Extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_NOT_FOUND if the address was not found from the source matching table.
 *
 */
esp_err_t esp_ieee802154_clear_pending_addr(const uint8_t *addr, bool is_short);

/**
 * @brief  Clear the source matching table to empty.
 *
 * @param[in]  is_short  Clear Short address table or Extended address table.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_reset_pending_table(bool is_short);

/**
 * @brief  Get the CCA threshold.
 *
 * @return  The CCA threshold in dBm.
 *
 */
int8_t esp_ieee802154_get_cca_threshold(void);

/**
 * @brief  Set the CCA threshold.
 *
 * @param[in]  cca_threshold  The CCA threshold in dBm.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_cca_threshold(int8_t cca_threshold);

/**
 * @brief  Get the CCA mode.
 *
 * @return  The CCA mode, refer to esp_ieee802154_cca_mode_t.
 *
 */
esp_ieee802154_cca_mode_t esp_ieee802154_get_cca_mode(void);

/**
 * @brief  Set the CCA mode.
 *
 * @param[in]  cca_mode  The CCA mode, refer to esp_ieee802154_cca_mode_t.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_cca_mode(esp_ieee802154_cca_mode_t cca_mode);

/**
 * @brief  Enable rx_on_when_idle mode, radio will receive during idle.
 *
 * @param[in]  enable  Enable/Disable rx_on_when_idle mode.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_rx_when_idle(bool enable);

/**
 * @brief  Get the rx_on_when_idle mode.
 *
 * @return  rx_on_when_idle mode.
 *
 */
bool esp_ieee802154_get_rx_when_idle(void);

/**
 * @brief  Perform energy detection.
 *
 * @param[in]  duration  The duration of energy detection, in symbol unit (16 us).
 *                       The result will be reported via esp_ieee802154_energy_detect_done().
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_energy_detect(uint32_t duration);

/**
 * @brief  Notify the IEEE 802.15.4 Radio that the frame is handled done by upper layer.
 *
 * @param[in]  frame  The pointer to the frame which was passed from the function `esp_ieee802154_receive_done()`
 *                    or ack frame from `esp_ieee802154_transmit_done()`.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if frame is invalid.
 *
 */
esp_err_t esp_ieee802154_receive_handle_done(const uint8_t *frame);

/** Below are the events generated by IEEE 802.15.4 subsystem, which are in ISR context **/
/**
 * @brief  A Frame was received.
 *
 * @note   User must call the function `esp_ieee802154_receive_handle_done()` to notify 802.15.4 driver after the received frame is handled.
 *
 * @param[in]  frame  The point to the received frame, frame format:
 *                    |-----------------------------------------------------------------------|
 *                    | Len | MHR |              MAC Payload                       (no FCS)   |
 *                    |-----------------------------------------------------------------------|
 * @param[in]  frame_info  More information of the received frame, refer to esp_ieee802154_frame_info_t.
 *
 */
extern void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info);

/**
 * @brief  The SFD field of the frame was received.
 *
 */
extern void esp_ieee802154_receive_sfd_done(void);

/**
 * @brief  The Frame Transmission succeeded.
 *
 * @note   If the ack frame is not null, user must call the function `esp_ieee802154_receive_handle_done()` to notify 802.15.4 driver
 *         after the ack frame is handled.
 *
 * @param[in]  frame           The pointer to the transmitted frame.
 * @param[in]  ack             The received ACK frame, it could be NULL if the transmitted frame's AR bit is not set.
 * @param[in]  ack_frame_info  More information of the ACK frame, refer to esp_ieee802154_frame_info_t.
 *
 */
extern void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info);

/**
 * @brief  The Frame Transmission failed. Refer to `esp_ieee802154_transmit()`.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[in]  error  The transmission failure reason, refer to esp_ieee802154_tx_error_t.
 *
 */
extern void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error);

/**
 * @brief  The SFD field of the frame was transmitted.
 *
 */
extern void esp_ieee802154_transmit_sfd_done(uint8_t *frame);

/**
 * @brief  The energy detection done. Refer to `esp_ieee802154_energy_detect()`.
 *
 * @param[in]  power  The detected power level, in dBm.
 *
 */
extern void esp_ieee802154_energy_detect_done(int8_t power);

/**
 * @brief  Set the IEEE 802.15.4 Radio to receive state at a specific time.
 *
 * @note   Radio will start receiving after the timestamp, and continue receiving until it receives a valid frame.
 *         Refer to `esp_ieee802154_receive_done()`.
 *
 * @param[in]  time  A specific timestamp for starting receiving.
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_receive_at(uint32_t time);

/**
 * @brief  Transmit the given frame at a specific time.
 *         The transmit result will be reported via `esp_ieee802154_transmit_done()`
 *         or `esp_ieee802154_transmit_failed()`.
 *
 * @param[in]  frame  The pointer to the frame. Refer to `esp_ieee802154_transmit()`.
 * @param[in]  cca    Perform CCA before transmission if it's true, otherwise transmit the frame directly.
 * @param[in]  time  A specific timestamp for starting transmission.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to invalid state.
 *
 */
esp_err_t esp_ieee802154_transmit_at(const uint8_t *frame, bool cca, uint32_t time);

/**
 * @brief  Get the RSSI of the most recent received frame.
 *
 * @return The value of RSSI.
 *
 */
int8_t esp_ieee802154_get_recent_rssi(void);

/**
 * @brief  Get the LQI of the most recent received frame.
 *
 * @return The value of LQI.
 *
 */
uint8_t esp_ieee802154_get_recent_lqi(void);

/**
 * @brief  Set the key and addr for a frame needs to be encrypted by HW.
 *
 * @param[in]  frame  A frame needs to be encrypted. Refer to `esp_ieee802154_transmit()`.
 * @param[in]  key    A 16-bytes key for encryption.
 * @param[in]  addr   An 8-bytes addr for HW to generate nonce, in general, is the device extended address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_transmit_security(uint8_t *frame, uint8_t *key, uint8_t *addr);

/**
 * @brief  This function will be called when a received frame needs to be acked with Enh-Ack, the upper
 *         layer should generate the Enh-Ack frame in this callback function.
 *
 * @param[in]  frame          The received frame.
 * @param[in]  frame_info     The frame information. Refer to `esp_ieee802154_frame_info_t`.
 * @param[out] enhack_frame   The Enh-ack frame need to be generated via this function, HW will send it back after AIFS.
 *
 * @return
 *        - ESP_OK if Enh-Ack generates done.
 *        - ESP_FAIL if Enh-Ack generates failed.
 *
 */
esp_err_t esp_ieee802154_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info, uint8_t* enhack_frame);

/**
 * The configurable definitions via Kconfig
 */
#if CONFIG_IEEE802154_TXRX_STATISTIC

/**
 * @brief  Clear the current IEEE802.15.4 statistic.
 *
 */
void esp_ieee802154_txrx_statistic_clear(void);

/**
 * @brief  Print the current IEEE802.15.4 statistic.
 *
 */
void esp_ieee802154_txrx_statistic_print(void);
#endif // CONFIG_IEEE802154_TXRX_STATISTIC

#ifdef __cplusplus
}
#endif
