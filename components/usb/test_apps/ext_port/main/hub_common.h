/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "stdint.h"
#include "hcd.h"
#include "usb/usb_types_ch11.h"

/**
 * @brief Hub set root port handle
 *
 * When external hub connected to the root hub port (USB OTG peripheral), one port handle shared between all downstream pipes.
 *
 * @param[in] root_port_hdl Root port handle
 */
void hub_set_root_port(hcd_port_handle_t root_port_hdl);

/**
 * @brief Hub attach
 *
 * Enables the root port, wait a connection and configure the external Hub
 */
void hub_attach(void);

/**
 * @brief Hub detach
 *
 * Disables the root port by turing the port off and disconnects the external Hub
 */
void hub_detach(void);

// =================================================================================================
// ================================== Hub Child API ================================================
// =================================================================================================
/**
 * @brief Create pipe for the child device
 *
 * @param[in] speed Device speed
 * @return hcd_pipe_handle_t Pipe handle
 */
hcd_pipe_handle_t hub_child_create_pipe(usb_speed_t speed);

/**
 * @brief Free pipe for the child device
 *
 * @param[in] pipe_hdl Pipe handle
 */
void hub_child_pipe_free(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Quick enumeration of the child device
 *
 * Performs quick enumeration of the device, using the ctrl_pipe handle.
 * Quick enumeration includes:
 * - Get Device Descriptor
 * - Set Address
 * - Get Configuration (1)
 *
 * @param[in] ctrl_pipe Control pipe handle
 * @param[in] dev_addr Device address
 * @param[in] config_num Configuration number
 */
void hub_child_quick_enum(hcd_pipe_handle_t ctrl_pipe, uint8_t dev_addr, uint8_t config_num);

// =================================================================================================
// =================================== Hub Getters  ================================================
// =================================================================================================
/**
 * @brief Get hub context
 *
 * @return void* pointer to a Hub context
 */
void* hub_get_context(void);

/**
 * @brief Get number of ports
 *
 * @return uint8_t Number of ports
 */
uint8_t hub_get_port_num(void);

/**
 * @brief Get port power on delay
 *
 * @return uint16_t Power on delay in milliseconds
 */
uint16_t hub_get_port_poweron_delay_ms(void);

/**
 * @brief Get port status
 *
 * @param[in] port1 Port number
 * @param[out] status Port status
 */
void hub_get_port_status(uint8_t port1, usb_port_status_t *status);

/**
 * @brief Get port connection status
 *
 * @param[in] port1 Port number
 * @return bool Port connection status
 */
bool hub_get_port_connection(uint8_t port1);

// =================================================================================================
// =================================== Hub Port Control ============================================
// =================================================================================================
/**
 * @brief Power off the port
 *
 * @param[in] port1 Port number
 */
void hub_port_power_off(uint8_t port1);

/**
 * @brief Power on the port
 *
 * @param[in] port1 Port number
 */
void hub_port_power_on(uint8_t port1);

/**
 * @brief Reset the port
 *
 * @param[in] port1 Port number
 */
void hub_port_reset(uint8_t port1);

/**
 * @brief Suspend the port
 *
 * @param[in] port1 Port number
 */
void hub_port_suspend(uint8_t port1);

/**
 * @brief Resume the port
 *
 * @param[in] port1 Port number
 */
void hub_port_resume(uint8_t port1);

/**
 * @brief Disable the port
 *
 * @param[in] port1 Port number
 */
void hub_port_disable(uint8_t port1);

/**
 * @brief Clear the port connection
 *
 * @param[in] port1 Port number
 */
void hub_port_clear_connection(uint8_t port1);

/**
 * @brief Clear the port reset
 *
 * @param[in] port1 Port number
 */
void hub_port_clear_reset(uint8_t port1);
