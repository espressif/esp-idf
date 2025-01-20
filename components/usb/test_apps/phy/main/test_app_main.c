/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_private/usb_phy.h"
#include "hal/usb_wrap_ll.h" // For USB_WRAP_LL_EXT_PHY_SUPPORTED symbol
#include "soc/soc_caps.h"    // For SOC_USB_UTMI_PHY_NUM symbol

#if USB_WRAP_LL_EXT_PHY_SUPPORTED
#define EXT_PHY_SUPPORTED 1
#else
#define EXT_PHY_SUPPORTED 0
#endif

#if SOC_USB_UTMI_PHY_NUM > 0
#define UTMI_PHY_SUPPORTED 1
#else
#define UTMI_PHY_SUPPORTED 0
#endif

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks();
}

void app_main(void)
{
    //  ____ ___  ___________________    __                   __
    // |    |   \/   _____/\______   \ _/  |_  ____   _______/  |_
    // |    |   /\_____  \  |    |  _/ \   __\/ __ \ /  ___/\   __\.
    // |    |  / /        \ |    |   \  |  | \  ___/ \___ \  |  |
    // |______/ /_______  / |______  /  |__|  \___  >____  > |__|
    //                  \/         \/             \/     \/
    printf(" ____ ___  ___________________    __                   __   \r\n");
    printf("|    |   \\/   _____/\\______   \\ _/  |_  ____   _______/  |_ \r\n");
    printf("|    |   /\\_____  \\  |    |  _/ \\   __\\/ __ \\ /  ___/\\   __\\\r\n");
    printf("|    |  / /        \\ |    |   \\  |  | \\  ___/ \\___ \\  |  |  \r\n");
    printf("|______/ /_______  / |______  /  |__|  \\___  >____  > |__|  \r\n");
    printf("                 \\/         \\/             \\/     \\/        \r\n");

    unity_utils_setup_heap_record(80);
    unity_utils_set_leak_level(128);
    unity_run_menu();
}

/**
 * Test init and deinit of internal FSLS PHY
 */
TEST_CASE("Init internal FSLS PHY", "[phy]")
{
    usb_phy_handle_t phy_handle = NULL;
    const usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_INT,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NOT_NULL(phy_handle);
    TEST_ASSERT_EQUAL(ESP_OK, usb_del_phy(phy_handle));
}

/**
 * Test init and deinit of external FSLS PHY
 */
TEST_CASE("Init external FSLS PHY", "[phy]")
{
    usb_phy_handle_t phy_handle = NULL;
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_EXT,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
#if EXT_PHY_SUPPORTED
    // Init ext PHY without ext_io_conf -> FAIL
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NULL(phy_handle);

    // Init ext PHY with ext_io_conf -> PASS
    const usb_phy_ext_io_conf_t ext_io_conf = { // Some random values
        .vp_io_num = 1,
        .vm_io_num = 1,
        .rcv_io_num = 1,
        .suspend_n_io_num = 1,
        .oen_io_num = 1,
        .vpo_io_num = 1,
        .vmo_io_num = 1,
        .fs_edge_sel_io_num = 1,
    };
    phy_config.ext_io_conf = &ext_io_conf;
    TEST_ASSERT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NOT_NULL(phy_handle);
    TEST_ASSERT_EQUAL(ESP_OK, usb_del_phy(phy_handle));
#else
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NULL(phy_handle);
#endif
}

/**
 * Test init and deinit of internal UTMI PHY
 */
TEST_CASE("Init internal UTMI PHY", "[phy]")
{
    usb_phy_handle_t phy_handle = NULL;
    const usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_UTMI,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
#if UTMI_PHY_SUPPORTED
    TEST_ASSERT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NOT_NULL(phy_handle);
    TEST_ASSERT_EQUAL(ESP_OK, usb_del_phy(phy_handle));
#else
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NULL(phy_handle);
#endif
}

/**
 * Test init and deinit of all PHYs at the same time
 */
TEST_CASE("Init all PHYs", "[phy]")
{
    usb_phy_handle_t phy_handle = NULL;
    usb_phy_handle_t phy_handle_2 = NULL;
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_INT,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,
        .ext_io_conf = NULL,
        .otg_io_conf = NULL,
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle));
    TEST_ASSERT_NOT_NULL(phy_handle);

    // Our current targets support either UTMI or external PHY
    // so if/else suffice here
#if UTMI_PHY_SUPPORTED
    phy_config.target = USB_PHY_TARGET_UTMI;
#else
    const usb_phy_ext_io_conf_t ext_io_conf = { // Some random values
        .vp_io_num = 1,
        .vm_io_num = 1,
        .rcv_io_num = 1,
        .suspend_n_io_num = 1,
        .oen_io_num = 1,
        .vpo_io_num = 1,
        .vmo_io_num = 1,
        .fs_edge_sel_io_num = 1,
    };
    phy_config.target = USB_PHY_TARGET_EXT;
    phy_config.ext_io_conf = &ext_io_conf;
#endif
    TEST_ASSERT_EQUAL(ESP_OK, usb_new_phy(&phy_config, &phy_handle_2));
    TEST_ASSERT_NOT_NULL(phy_handle_2);

    TEST_ASSERT_EQUAL(ESP_OK, usb_del_phy(phy_handle));
    TEST_ASSERT_EQUAL(ESP_OK, usb_del_phy(phy_handle_2));
}
