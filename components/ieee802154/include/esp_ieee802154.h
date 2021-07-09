// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_ieee802154_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initialize the IEEE 802.15.4 subsystem.
 *
 */
void esp_ieee802154_enable(void);

/**
 * @brief  Deinitialize the IEEE 802.15.4 subsystem.
 *
 */
void esp_ieee802154_disable(void);

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
 */
void esp_ieee802154_set_channnel(uint8_t channel);

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
 */
void esp_ieee802154_set_txpower(int8_t power);

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
 */
void esp_ieee802154_set_promiscuous(bool enable);

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
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure due to invalid state.
 *
 * Note: Radio will continue receiving until it receives a valid frame.
 *       Ref to esp_ieee802154_receive_done().
 *
 */
esp_err_t esp_ieee802154_receive(void);

/**
 * @brief  Transmit the given frame.
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
 * Note: The transmit result will be reported via esp_ieee802154_transmit_done()
 *       or esp_ieee802154_transmit_failed().
 *
 */
esp_err_t esp_ieee802154_transmit(const uint8_t *frame, bool cca);

/**
 * @brief  Set the time to wait for the ack frame.
 *
 * @param[in]  timeout  The time to wait for the ack frame, in symbol unit (16 us).
 *                      Default: 0x006C, Range: 0x0000 - 0xFFFF.
 *
 */
void esp_ieee802154_set_ack_timeout(uint32_t timeout);

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
 */
void esp_ieee802154_set_panid(uint16_t panid);

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
 */
void esp_ieee802154_set_short_address(uint16_t short_address);

/**
 * @brief  Get the device extended address.
 *
 * @param[out]  ext_addr  The pointer to the device extended address.
 *
 */
void esp_ieee802154_get_extended_address(uint8_t *ext_addr);

/**
 * @brief  Set the device extended address.
 *
 * @param[in]  ext_addr  The pointer to the device extended address.
 *
 */
void esp_ieee802154_set_extended_address(const uint8_t *ext_addr);

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
 */
void esp_ieee802154_set_pending_mode(esp_ieee802154_pending_mode_t pending_mode);

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
 */
void esp_ieee802154_reset_pending_table(bool is_short);

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
 */
void esp_ieee802154_set_cca_threshold(int8_t cca_threshold);

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
 */
void esp_ieee802154_set_cca_mode(esp_ieee802154_cca_mode_t cca_mode);

/**
 * @brief  Enable rx_on_when_idle mode, radio will receive during idle.
 *
 * @param[in]  enable  Enable/Disable rx_on_when_idle mode.
 *
 */
void esp_ieee802154_set_rx_when_idle(bool enable);

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


/** Below are the events generated by IEEE 802.15.4 subsystem, which are in ISR context **/
/**
 * @brief  A Frame was received.
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
 * @param[in]  frame           The pointer to the transmitted frame.
 * @param[in]  ack             The received ACK frame, it could be NULL if the transmitted frame's AR bit is not set.
 * @param[in]  ack_frame_info  More information of the ACK frame, refer to esp_ieee802154_frame_info_t.
 *
 * Note: refer to esp_ieee802154_transmit().
 *
 */
extern void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info);

/**
 * @brief  The Frame Transmission failed.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[in]  error  The transmission failure reason, refer to esp_ieee802154_tx_error_t.
 *
 * Note: refer to esp_ieee802154_transmit().
 *
 */
extern void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error);

/**
 * @brief  The SFD field of the frame was transmitted.
 *
 */
extern void esp_ieee802154_transmit_sfd_done(uint8_t *frame);

/**
 * @brief  The energy detection done.
 *
 * @param[in]  power  The detected power level, in dBm.
 *
 * Note: refer to esp_ieee802154_energy_detect().
 *
 */
extern void esp_ieee802154_energy_detect_done(int8_t power);

#ifdef __cplusplus
}
#endif
