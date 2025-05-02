/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "unity.h"
#include "sdkconfig.h"
// Internal common test files
#include "ext_port_common.h"
#include "hub_common.h"
// Cross-test common files
#include "hcd_common.h"

#define TEST_PORT_NUM_DEVICE_FSHS           CONFIG_USB_HOST_TEST_HUB_PORT_NUM_DEVICE_FSHS
#define TEST_PORT_NUM_DEVICE_LS             CONFIG_USB_HOST_TEST_HUB_PORT_NUM_DEVICE_LS
#define TEST_PORT_NUM_EMPTY                 CONFIG_USB_HOST_TEST_HUB_PORT_NUM_EMPTY
#define TEST_CHILD_DEVICE_ADDR              2
#define TEST_CHILD_DEVICE_CONFIG            1

/**
 * @brief Test the Port rconnection based on the test configuration
 *
 * @param port1 Port number
 */
static inline void test_port_connection(uint8_t port1)
{
    switch (port1) {
    case TEST_PORT_NUM_DEVICE_FSHS:
        TEST_ASSERT_TRUE_MESSAGE(hub_get_port_connection(port1), "Configured port doesn't have a device. Reconfigure the port number via menuconfig and run the test again.");
        break;
    case TEST_PORT_NUM_EMPTY:
        TEST_ASSERT_FALSE_MESSAGE(hub_get_port_connection(port1), "Configured port doesn't have a device. Reconfigure the port number via menuconfig and run the test again.");
        break;
    default:
        // Nothing to verify
        break;
    }
}

/*
Test the External Port reachability by getting the number of ports and checking the port status after powering the port on.

Purpose:
    - Verify the root hub port connection/disconnection
    - Verify the port power on/off
    - Verify the port status and print it out

Procedure:
    - Attach the hub
    - Get the number of ports
    - Power on all ports
    - Get the port status
    - Power off all ports
    - Detach the hub
*/
TEST_CASE("Port: all power on then off", "[ext_port][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    usb_port_status_t port_status;
    for (uint8_t i = 1; i <= port_num; i++) {
        hub_port_power_on(i);
        hub_get_port_status(i, &port_status);
        test_port_connection(i);
        hub_port_power_off(i);
    }
    hub_detach();
}

/*
Test the port in disconnected state.

Purpose:
    - Verify the process how the External Port Driver handles the port without a connection

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON
    - Handling the port: POWERED_ON -> DISCONNECTED -> IDLE
    - Detach the hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: disconnected", "[low_speed][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_EMPTY <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_EMPTY,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_EMPTY, port_hdl);
    printf("Handle disconnected port...\n");
    test_ext_port_disconnected(TEST_PORT_NUM_EMPTY, port_hdl);
    // Detach parent hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

/*
Test the port with a connection and perform the enumeration of the child USB Low-speed device.

To configure the port to which the device is connected, change the TEST_PORT_NUM_DEVICE_LS value in the menuconfig.

Purpose:
    - Verify the process how the External Port Driver handles the port with a connection
    - Verify the connectivity with the Low-speed USB device, connected via hub

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Enumerate the child device, speed: Low
    - Handling the port: CONNECTED -> IDLE -> DISCONNECTED
    - Recycle the port
    - Detach the hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: enumerate child device Low-speed", "[ext_port][low_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_LS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_LS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_LS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_LS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    // Check the device speed
    TEST_ASSERT_EQUAL_MESSAGE(USB_SPEED_LOW, port_speed, "Invalid USB device speed");
    // Enumerate device
    hcd_pipe_handle_t ctrl_pipe = hub_child_create_pipe(port_speed);
    hub_child_quick_enum(ctrl_pipe, TEST_CHILD_DEVICE_ADDR, TEST_CHILD_DEVICE_CONFIG);
    hub_child_pipe_free(ctrl_pipe);
    // Wait disconnection
    test_ext_port_imitate_disconnection(TEST_PORT_NUM_DEVICE_LS, port_hdl);
    // Recylce the port
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_PRESENT);
    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

/*
Test the port with a connection and perform the enumeration of the child USB Full-speed device.

To configure the port to which the device is connected, change the TEST_PORT_NUM_DEVICE_FSHS value in the menuconfig.

Purpose:
    - Verify the process how the External Port Driver handles the port with a connection
    - Verify the connectivity with the Low-speed USB device, connected via hub

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Enumerate the child device, speed: Full
    - Handling the port: CONNECTED -> IDLE -> DISCONNECTED
    - Recycle the port
    - Detach the hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: enumerate child device Full-speed", "[ext_port][full_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    // Check the device speed
    TEST_ASSERT_EQUAL_MESSAGE(USB_SPEED_FULL, port_speed, "Invalid USB device speed");
    // Enumerate device
    hcd_pipe_handle_t ctrl_pipe = hub_child_create_pipe(port_speed);
    hub_child_quick_enum(ctrl_pipe, TEST_CHILD_DEVICE_ADDR, TEST_CHILD_DEVICE_CONFIG);
    hub_child_pipe_free(ctrl_pipe);
    // Wait disconnection
    test_ext_port_imitate_disconnection(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Recylce the port
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_PRESENT);
    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

/*
Test the port with a connection and perform the enumeration of the child USB High-speed device.

To configure the port to which the device is connected, change the TEST_PORT_NUM_DEVICE_FSHS value in the menuconfig.

Purpose:
    - Verify the process how the External Port Driver handles the port with a connection
    - Verify the connectivity with the Low-speed USB device, connected via hub

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Enumerate the child device, speed: High
    - Handling the port: CONNECTED -> IDLE -> DISCONNECTED
    - Recycle the port
    - Detach the hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: enumerate child device High-speed", "[ext_port][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    // Check the device speed
    TEST_ASSERT_EQUAL_MESSAGE(USB_SPEED_HIGH, port_speed, "Invalid USB device speed");
    // Enumerate device
    hcd_pipe_handle_t ctrl_pipe = hub_child_create_pipe(port_speed);
    hub_child_quick_enum(ctrl_pipe, TEST_CHILD_DEVICE_ADDR, TEST_CHILD_DEVICE_CONFIG);
    hub_child_pipe_free(ctrl_pipe);
    // Wait disconnection
    test_ext_port_imitate_disconnection(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Recylce the port
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_PRESENT);
    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

/*
Test the port recycle procedure.

Purpose:
    - Verify the port recycle procedure

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Wait disconnection
    - Recycle the port
    - Repower the port
    - Verify the port is still available
    - Wait disconnection
    - Recycle the port
    - Detach the hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: recycle", "[ext_port][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    // Wait disconnection
    test_ext_port_imitate_disconnection(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Recylce the port
    printf("Recycle the port...\n");
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_PRESENT);
    // Repower the port as we use imitation for disconnection. Port state in the External Port Driver is not changed during that.
    hub_port_power_off(TEST_PORT_NUM_DEVICE_FSHS);
    hub_port_power_on(TEST_PORT_NUM_DEVICE_FSHS);
    // Verify that port still available
    printf("Verify the port is still available...\n");
    port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    // Wait disconnection
    test_ext_port_imitate_disconnection(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Recylce the port
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_PRESENT);
    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

TEST_CASE("Port: recycle when port is gone", "[ext_port][low_speed][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);

    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_PRESENT);
    // Recylce the port
    test_ext_port_recycle(port_hdl, RECYCLE_PORT_IS_GONE);
    // test_ext_port_delete(port_hdl);
}

/*
Test the port disable procedure.

Purpose:
    - Verify the port disable procedure

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Disable the port
    - Detach the parent hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: disable", "[ext_port][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    printf("Disable the port ...\n");
    test_ext_port_disable(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Detach hub
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);
}

/*
Test the port gone procedure, when the port is in POWERED_ON state.

Purpose:
    - Verify the port gone procedure

Procedure:
    - Attach the hub
    - Create External Port
    - Power on the port
    - Detach the parent hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: gone in state - powered on", "[ext_port][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    //
    printf("Port Gone...\n");
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_NOT_PRESENT);
    test_ext_port_delete(port_hdl);

}

/*
Test the port gone procedure, when the port is in ENABLED state.

Purpose:
    - Verify the port gone procedure

Procedure:
    - Attach the hub
    - Create External Port
    - Handling the port: NOT_CONFIGURED -> POWERED_OFF -> POWERED_ON -> CONNECTED
    - Detach the parent hub
    - Notify the port
    - Delete the port
*/
TEST_CASE("Port: gone in state - enabled", "[ext_port][full_speed][high_speed]")
{
    hub_attach();
    uint8_t port_num = hub_get_port_num();
    TEST_ASSERT_TRUE(TEST_PORT_NUM_DEVICE_FSHS <= port_num);
    // Create External Port
    ext_port_config_t port_config = {
        .context = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_dev_hdl = (void*) hub_get_context() /* use any before IDF-10023 */,
        .parent_port_num = TEST_PORT_NUM_DEVICE_FSHS,
        .port_power_delay_ms = hub_get_port_poweron_delay_ms(),
    };
    ext_port_hdl_t port_hdl = test_ext_port_new(&port_config);
    // After adding the port, it is in POWERED_OFF state
    test_ext_port_power_on(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    // Wait connection
    usb_speed_t port_speed = test_ext_port_connected(TEST_PORT_NUM_DEVICE_FSHS, port_hdl);
    printf("Hub port: %s speed device \n", (char*[]) {
        "Low", "Full", "High"
    }[port_speed]);
    //
    printf("Port Gone...\n");
    hub_detach();
    // Notify port
    test_ext_port_gone(port_hdl, GONE_DEVICE_PRESENT);
    test_ext_port_delete(port_hdl);
}
