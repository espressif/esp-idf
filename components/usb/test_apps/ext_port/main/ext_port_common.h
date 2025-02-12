/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "ext_port.h"

/**
 * @brief External Port setup test
 * Create queues for events/callbacks and install External Port driver
 */
void test_ext_port_setup(void);

/**
 * @brief External Port teardown test
 * Uninstall External Port driver and delete queues
 */
void test_ext_port_teardown(void);

/**
 * @brief Create External Port
 *
 * Creates the port.
 * Verifies that creating new port triggers the process request callback.
 * Verifies that processing newly added port requires the Get Port Status request on the parent hub.
 *
 * @param[in] config External Port configuration
 * @return
 *    - External Port handle if port is created successfully
 */
ext_port_hdl_t test_ext_port_new(ext_port_config_t *config);

/**
 * @brief Test power on the Port
 *
 * Requests actual port status from parent hub.
 * Set status to the External Port Driver.
 * Verifies that setting the port status triggers the process request callback.
 * Verifies that processing the port after setting the status triggers the Get Port Status request on the parent hub.
 *
 * @param[in] port1 Port number
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_power_on(uint8_t port1, ext_port_hdl_t port_hdl);

/**
 * @brief Test Port disconnected
 *
 * Handles the port that doesn't have a connection.
 *
 * @param[in] port1 Port number
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_disconnected(uint8_t port1, ext_port_hdl_t port_hdl);

/**
 * @brief Test External Port connected
 *
 * Handles the port that has a connection
 *
 * @param[in] port1 Port number
 * @param[in] port_hdl External Port handle
 * @return
 *    - usb_speed_t Port speed
 */
usb_speed_t test_ext_port_connected(uint8_t port1, ext_port_hdl_t port_hdl);

/**
 * @brief Test External Port imitate disconnection
 *
 * Imitates the port disconnection via sending changed status to the External Port Driver
 *
 * @param[in] port1 Port number
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_imitate_disconnection(uint8_t port1, ext_port_hdl_t port_hdl);

/**
 * @brief Test External Port disable
 *
 * @param[in] port1 Port number
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_disable(uint8_t port1, ext_port_hdl_t port_hdl);

/**
 * @brief Test External Port recycle
 *
 * @param[in] port_hdl External Port handle
 * @param[in] has_device Device presence
 */
void test_ext_port_gone(ext_port_hdl_t port_hdl, bool has_device);

/**
 * @brief Test External Port delete
 *
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_delete(ext_port_hdl_t port_hdl);

/**
 * @brief Test External Port recycle
 *
 * @param[in] port_hdl External Port handle
 */
void test_ext_port_recycle(ext_port_hdl_t port_hdl);
