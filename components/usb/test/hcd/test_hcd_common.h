// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "usb.h"
#include "hcd.h"

#define IRP_CONTEXT_VAL ((void *)0xDEADBEEF)

// ------------------------------------------------- HCD Event Test ----------------------------------------------------

/**
 * @brief Expect (wait) for an HCD port event
 *
 * @param port_hdl Port handle to expect event from
 * @param expected_event Port event to expect
 */
void test_hcd_expect_port_event(hcd_port_handle_t port_hdl, hcd_port_event_t expected_event);

/**
 * @brief Expect (wait) for an HCD pipe event
 *
 * @param pipe_hdl Pipe handle to expect event from
 * @param expected_event Pipe event to expect
 */
void test_hcd_expect_pipe_event(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t expected_event);

/**
 * @brief Get the current number of queued port events (dequeued using test_hcd_expect_port_event())
 *
 * @param port_hdl Port handle
 * @return int Number of port events currently queued
 */
int test_hcd_get_num_port_events(hcd_port_handle_t port_hdl);

/**
 * @brief Get the current number of queued pipe events (dequeued using test_hcd_expect_pipe_event())
 *
 * @param pipe_hdl Pipe handle
 * @return int Number of pipe events currently queued
 */
int test_hcd_get_num_pipe_events(hcd_pipe_handle_t pipe_hdl);

// ----------------------------------------------- Driver/Port Related -------------------------------------------------

/**
 * @brief For the USB PHY into the connected or disconnected state
 *
 * @param connected For into connected state if true, disconnected if false
 * @param delay_ticks Delay in ticks before forcing state
 */
void test_hcd_force_conn_state(bool connected, TickType_t delay_ticks);

/**
 * @brief Sets up the HCD and initializes an HCD port.
 *
 * @return hcd_port_handle_t Port handle
 */
hcd_port_handle_t test_hcd_setup(void);

/**
 * @brief Frees and HCD port and uninstalls the HCD
 *
 * @param port_hdl Port handle
 */
void test_hcd_teardown(hcd_port_handle_t port_hdl);

/**
 * @brief Wait for a connection on an HCD port
 *
 * @note This function will internally call test_hcd_force_conn_state() to allow for a connection
 *
 * @param port_hdl Port handle
 * @return usb_speed_t Speed of the connected device
 */
usb_speed_t test_hcd_wait_for_conn(hcd_port_handle_t port_hdl);

/**
 * @brief Wait for a disconnection on an HCD port
 *
 * @note This fucntion will internally call test_hcd_force_conn_state() to force a disconnection
 *
 * @param port_hdl Port handle
 * @param already_disabled Whether the HCD port is already in the disabled state
 */
void test_hcd_wait_for_disconn(hcd_port_handle_t port_hdl, bool already_disabled);

// ------------------------------------------------- Pipe alloc/free ---------------------------------------------------

/**
 * @brief Test the allocation of a pipe
 *
 * @param port_hdl Port handle
 * @param ep_desc Endpoint descriptor
 * @param dev_addr Device address of the pipe
 * @param dev_speed Device speed of the pipe
 * @return hcd_pipe_handle_t Pipe handle
 */
hcd_pipe_handle_t test_hcd_pipe_alloc(hcd_port_handle_t port_hdl, const usb_desc_ep_t *ep_desc, uint8_t dev_addr, usb_speed_t dev_speed);

/**
 * @brief Test the freeing of a pipe
 *
 * @param pipe_hdl Pipe handle
 */
void test_hcd_pipe_free(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Allocate an IRP
 *
 * @param num_iso_packets Number of isochronous packets
 * @param data_buffer_size Size of the data buffer of the IRP
 * @return usb_irp_t* IRP
 */
usb_irp_t *test_hcd_alloc_irp(int num_iso_packets, size_t data_buffer_size);

/**
 * @brief Free an IRP
 *
 * @param irp IRP
 */
void test_hcd_free_irp(usb_irp_t *irp);

// --------------------------------------------------- Enumeration -----------------------------------------------------

/**
 * @brief Do some basic enumeration of the device
 *
 * For tests that need a device to have been enumerated (such as bulk tests). This function will enumerate that device
 * using the device's default pipe. The minimal enumeration will include
 *
 * - Getting the device's descriptor and updating the default pipe's MPS
 * - Setting the device's address and updating the default pipe to use that address
 * - Setting the device to configuration 1 (i.e., the first configuration found
 *
 * @param default_pipe The connected device's default pipe
 * @return uint8_t The address of the device after enumeration
 */
uint8_t test_hcd_enum_devc(hcd_pipe_handle_t default_pipe);
