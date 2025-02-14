/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "tusb.h"
#include "mock_dev.h"

#define DEVICE_CONN_DCONN_DELAY_MS    50

//
// Mock Device Descriptors definitions
//

static const tusb_desc_device_t mock_device_desc = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x0001,
    .idProduct = 0x0002,
    .bcdDevice = 0x0000,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

static const uint8_t mock_config_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 0, 0, TUD_CONFIG_DESC_LEN, TUSB_DESC_CONFIG_ATT_SELF_POWERED | TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
};

static const char *mock_langid_str_desc = {
    (char[]){0x09, 0x04},       // 0: LANGID
};

static const char *mock_manu_str_desc = {
    "Espressif",                // 1: Manufacturer
};

static const char *mock_prod_str_desc = {
    "Enum Testing",             // 2: Product
};

static const char *mock_ser_str_desc = {
    "123456",                   // 3: Serials, should use chip ID
};

//
// Test cases
//

/**
 * @brief Test Device enumeration process: 200 attempts in a row
 *
 * @note Test takes around 2 minutes
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait enumeration complete
 * 3. Uninstall TinyUSB device stack
 * 4. Increase counter
 * 5. Repeat step 1 if i < 200
 * 6. assert(counter == 200)
 */
TEST_CASE("enum::assign_device_address_200", "[mock_enum_device]")
{
    uint16_t counter = 0;
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            // String Descriptors
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    for (uint16_t i = 0; i < 200; i++) {
        TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
        TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
        mock_dev_release();
        counter++;
    }

    TEST_ASSERT_EQUAL(counter, 200);
}

/**
 * @brief Test Device enumeration process: 2000 attempts in a row
 *
 * @note: Test takes around 10 minutes
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait enumeration complete
 * 3. Uninstall TinyUSB device stack
 * 4. Increase counter
 * 5. Repeat step 1 if i < 2000
 * 6. assert(counter == 2000)
 */
TEST_CASE("enum::assign_device_address_2000", "[mock_enum_device_manual]")
{
    uint16_t counter = 0;
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            // String Descriptors
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    for (uint16_t i = 0; i < 2000; i++) {
        TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
        TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
        mock_dev_release();
        counter++;
    }

    TEST_ASSERT_EQUAL(counter, 2000);
}

/**
 * @brief Test Device enumeration process: 200 attempts in a row + emulate disconnection via bvalid signal
 *
 * @note: Test takes around 2 minutes
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait enumeration complete
 * 3. Uninstall TinyUSB device stack
 * 4. Increase counter
 * 5. Repeat step 1 if i < 200
 * 6. assert(counter == 200)
 */
TEST_CASE("enum::assign_device_address_200_with_power_cycle", "[mock_enum_device]")
{
    uint16_t counter = 0;
    mock_dev_cfg_t mock_dev_cfg = {
        .conn_dconn = {
            .enabled = true,
            .delay_ms = DEVICE_CONN_DCONN_DELAY_MS,
        },
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            // String Descriptors
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_LANGID_TABLE] = {
                .str_desc = mock_langid_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_MANU_STR_DESC] = {
                .str_desc = mock_manu_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_PROD_STR_DESC] = {
                .str_desc = mock_prod_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_SER_STR_DESC] = {
                .str_desc = mock_ser_str_desc,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    for (uint16_t i = 0; i < 200; i++) {
        TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
        TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
        mock_dev_release();
        counter++;
    }

    TEST_ASSERT_EQUAL(counter, 200);
}
