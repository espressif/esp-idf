/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <sys/select.h>
#include "esp_ieee802154_types.h"
#include "driver/uart.h"
#include "soc/gpio_num.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_SPINEL_LOG_TAG "ESP_RADIO_SPINEL"

#define SPINEL_PROP_VENDOR_ESP_SET_COORDINATOR (SPINEL_PROP_VENDOR_ESP__BEGIN + 1)  /* Vendor command for coordinator.*/

#define SPINEL_PROP_VENDOR_ESP_SET_PENDINGMODE (SPINEL_PROP_VENDOR_ESP__BEGIN + 2)  /* Vendor command for pending mode.*/

typedef enum {
    ESP_RADIO_SPINEL_ZIGBEE = 0x0,      /* The index of Zigbee.*/
    ESP_RADIO_SPINEL_OPENTHREAD = 0x1,  /* The index of OpenThread.*/
    ESP_RADIO_SPINEL_MAX,
} esp_radio_spinel_idx_t;               /* The index of 802.15.4 related protocol stack for ESP radio spinel.*/

typedef struct {
    fd_set         read_fds;    /* The read file descriptors.*/
    fd_set         write_fds;   /* The write file descriptors.*/
    fd_set         error_fds;   /* The error file descriptors.*/
    int            max_fd;      /* The max file descriptor.*/
    struct timeval timeout;     /* The timeout.*/
} esp_radio_spinel_mainloop_context_t;

typedef struct {
    uart_port_t port;               /*!< UART port number */
    uart_config_t uart_config;      /*!< UART configuration, see uart_config_t docs */
    gpio_num_t rx_pin;              /*!< UART RX pin */
    gpio_num_t tx_pin;              /*!< UART TX pin */
} esp_radio_spinel_uart_config_t;   /*This structure represents a context for ESP radio spinel. */

typedef void (*esp_radio_spinel_rcp_failure_handler)(void);                                                                     /* The handler for rcp failure.*/
typedef esp_err_t (*esp_radio_spinel_uart_init_handler)(const esp_radio_spinel_uart_config_t *uart_config_t, int *uart_fd);     /* The handler for UART initialization.*/
typedef esp_err_t (*esp_radio_spinel_uart_deinit_handler)(const esp_radio_spinel_uart_config_t *uart_config_t, int *uart_fd);   /* The handler for UART deinitialization.*/
typedef void (*esp_radio_spinel_compatibility_error_callback)(void);

typedef struct
{
    void (*receive_done)(const uint8_t *frame, esp_ieee802154_frame_info_t *frame_info);                            /* Callback for Receive Done.*/
    void (*transmit_done)(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info);   /* Callback for Transmit Done.*/
    void (*transmit_failed)(esp_ieee802154_tx_error_t error);                                                       /* Callback for Transmit Failed.*/
    void (*energy_scan_done)(int8_t max_rssi);                                                                      /* Callback for Energy Scan Done.*/
    void (*transmit_started)(const uint8_t *frame);                                                                 /* Callback for Transmit Started.*/
    void (*switchover_done)(bool success);                                                                          /* Callback for Switchover Done.*/

#if CONFIG_OPENTHREAD_DIAG
    void (*diag_receive_done)(const uint8_t *frame, esp_ieee802154_frame_info_t *frame_info);                       /* Callback for Receive Done (diag).*/
    void (*diag_transmit_done)(const uint8_t *frame, esp_ieee802154_frame_info_t *frame_info);                      /* Callback for Transmit Done (diag).*/
    void (*diag_transmit_failed)(esp_ieee802154_tx_error_t error);                                                  /* Callback for Transmit Failed (diag).*/
#endif // CONFIG_OPENTHREAD_DIAG
} esp_radio_spinel_callbacks_t;                                                                                     /* ESP Radio Spinel Callbacks.*/

/**
 * @brief Set callkbacks of ESP radio spinel.
 *
 * @note This function should be called before `esp_radio_spinel_init`.
 *
 * @param[in]  aCallbacks   The callbacks struct.
 * @param[in]  idx          The index of 802.15.4 related protocol stack.
 *
 */
void esp_radio_spinel_set_callbacks(const esp_radio_spinel_callbacks_t aCallbacks, esp_radio_spinel_idx_t idx);

/**
 * @brief Enable the UART interface for ESP radio spinel
 *
 * @note This function should be called before `esp_radio_spinel_init`.
 *
 * @param[in]  radio_uart_config    The config of UART for radio spinel.
 * @param[in]  aUartInitHandler     The function for UART initialization
 * @param[in]  aUartDeinitHandler   The function for UART deinitialization
 * @param[in]  idx                  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_uart_interface_enable(const esp_radio_spinel_uart_config_t *radio_uart_config,
                                                 esp_radio_spinel_uart_init_handler aUartInitHandler,
                                                 esp_radio_spinel_uart_deinit_handler aUartDeinitHandler,
                                                 esp_radio_spinel_idx_t idx);

/**
 * @brief Initialize ESP radio spinel.
 *
 * @note This function should be called after `esp_radio_spinel_set_callbacks` and `esp_radio_spinel_uart_interface_enable`.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 */
void esp_radio_spinel_init(esp_radio_spinel_idx_t idx);

/**
 * @brief Enavle ESP radio spinel.
 *
 * @note This function should be called after `esp_radio_spinel_init`.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_enable(esp_radio_spinel_idx_t idx);

/**
 * @brief Set the pending mode.
 *
 * @param[in]  pending_mode The pending mode.
 * @param[in]  idx          The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_pending_mode(esp_ieee802154_pending_mode_t pending_mode, esp_radio_spinel_idx_t idx);

/**
 * @brief Get the EUI-64.
 *
 * @param[in]  eui64    A pointer to the EUI-64.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_get_eui64(uint8_t *eui64, esp_radio_spinel_idx_t idx);

/**
 * @brief Set the panid.
 *
 * @param[in]  panid    The panid.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_panid(uint16_t panid, esp_radio_spinel_idx_t idx);

/**
 * @brief Set the short address.
 *
 * @param[in]  short_address    The short address.
 * @param[in]  idx              The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_short_address(uint16_t short_address, esp_radio_spinel_idx_t idx);

/**
 * @brief Set the extended address.
 *
 * @param[in]  ext_address  The extended address.
 * @param[in]  idx          The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_extended_address(uint8_t *ext_address, esp_radio_spinel_idx_t idx);

/**
 * @brief Set the coordinator mode.
 *
 * @param[in]  enable   Enable or disable the coordinator mode.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_pan_coord(bool enable, esp_radio_spinel_idx_t idx);

/**
 * @brief Enable the RCP reception.
 *
 * @param[in]  channel  The channel of reception.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_receive(uint8_t channel, esp_radio_spinel_idx_t idx);

/**
 * @brief Perform the energy scan.
 *
 * @param[in]  scan_channel     The channel of energy scan (usually the range is 11~26, scan all channels if it's set to 0).
 * @param[in]  scan_duration    The duration for energy scan.
 * @param[in]  idx              The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_energy_scan(uint8_t scan_channel, uint16_t scan_duration, esp_radio_spinel_idx_t idx);

/**
 * @brief Perform the transmission.
 *
 * @param[in]  frame    A pointer to the frame.
 * @param[in]  channel  The channel to use for transmitting.
 * @param[in]  cca      Perform clear channel assessment(if it's true) or not(if it's false)
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_transmit(uint8_t *frame, uint8_t channel, bool cca, esp_radio_spinel_idx_t idx);

/**
 * @brief Clear all short addresses from the source address match table.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_clear_short_entries(esp_radio_spinel_idx_t idx);

/**
 * @brief Add a short address to the source address match table.
 *
 * @param[in]  short_address    The short address to be added.
 * @param[in]  idx              The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_add_short_entry(uint16_t short_address, esp_radio_spinel_idx_t idx);

/**
 * @brief Clear the pending table, remove all extended/long addresses.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_clear_extended_entries(esp_radio_spinel_idx_t idx);

/**
 * @brief Add an extended address to the source address match table.
 *
 * @param[in]  ext_address  The extended address to be added.
 * @param[in]  idx          The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_add_extended_entry(uint8_t *ext_address, esp_radio_spinel_idx_t idx);

/**
 * @brief Sets the status of promiscuous mode.
 *
 * @param[in]  enable   Whether to enable or disable promiscuous mode.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_promiscuous_mode(bool enable, esp_radio_spinel_idx_t idx);

/**
 * @brief Update the ESP radio spinel.
 *
 * @param[in]  mainloop_context The context for ESP radio spinel.
 * @param[in]  idx              The index of 802.15.4 related protocol stack.
 *
 */
void esp_radio_spinel_radio_update(esp_radio_spinel_mainloop_context_t *mainloop_context, esp_radio_spinel_idx_t idx);

/**
 * @brief Process the ESP radio spinel.
 *
 * @param[in]  mainloop_context The context for ESP radio spinel.
 * @param[in]  idx              The index of 802.15.4 related protocol stack.
 *
 */
void esp_radio_spinel_radio_process(esp_radio_spinel_mainloop_context_t *mainloop_context, esp_radio_spinel_idx_t idx);

/**
 * @brief Switch the radio state to Sleep.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_sleep(esp_radio_spinel_idx_t idx);

/**
 * @brief Set the radio's transmit power in dBm.
 *
 * @param[in]  power    The transmit power in dBm.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_set_tx_power(int8_t power, esp_radio_spinel_idx_t idx);

/**
 * @brief Get the radio's transmit power in dBm.
 *
 * @param[in]  power    A pointer to the transmit power.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_get_tx_power(int8_t *power, esp_radio_spinel_idx_t idx);

/**
 * @brief Register a handler to process the RCP failure.
 *
 * @param[in]  handler  The RCP failure handler.
 * @param[in]  idx      The index of 802.15.4 related protocol stack.
 *
 */
void esp_radio_spinel_register_rcp_failure_handler(esp_radio_spinel_rcp_failure_handler handler, esp_radio_spinel_idx_t idx);

/**
 * @brief Deinitialize the RCP.
 *
 * @param[in]  idx  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_rcp_deinit(esp_radio_spinel_idx_t idx);

/**
 * @brief Get the version of RCP.
 *
 * @param[in]  running_rcp_version  A pointer to the RCP version string.
 * @param[in]  idx                  The index of 802.15.4 related protocol stack.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failures
 *
 */
esp_err_t esp_radio_spinel_rcp_version_get(char *running_rcp_version, esp_radio_spinel_idx_t idx);

/**
 * @brief   Registers the callback for spinel compatibility error.
 *
 * @note This function must be called before esp_radio_spinel_init.
 *
 * @param[in]  callback   The callback.
 *
 */
void esp_radio_spinel_set_compatibility_error_callback(esp_radio_spinel_compatibility_error_callback callback);

#ifdef __cplusplus
}
#endif
