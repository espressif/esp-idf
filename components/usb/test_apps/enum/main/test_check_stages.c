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

static const tusb_desc_device_t mock_device_desc_invalid_bDescriptor_type = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = 0x00,
};

static const tusb_desc_device_t mock_device_desc_bNumConfiguration_2 = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bNumConfigurations = 0x02,
};

static const tusb_desc_configuration_t mock_config_desc_unexpected_response_length_less = {
    .bLength = sizeof(tusb_desc_configuration_t),
    .wTotalLength = 0,
};

static const tusb_desc_configuration_t mock_config_desc_invalid_bDescriptor_type = {
    .bLength = sizeof(tusb_desc_configuration_t),
    .wTotalLength = sizeof(tusb_desc_configuration_t),
};

static const tusb_desc_configuration_t mock_config_desc = {
    .bLength = sizeof(tusb_desc_configuration_t),
    .bDescriptorType = TUSB_DESC_CONFIGURATION,
    .wTotalLength = sizeof(tusb_desc_configuration_t),
    .bNumInterfaces = 0,
    .bConfigurationValue = 1,
    .bmAttributes = TUSB_DESC_CONFIG_ATT_SELF_POWERED | TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
    .bMaxPower = 0x50,
};

//
// Test cases
//

/**
 * @brief Cancel enumeration: SHORT_DEV_DESC stage, invalid bDescriptorType field in the Device Descriptor
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return Device Descriptor with invalid bDescriptorType filed
 * 4. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_SHORT_DEV_DESC_invalid_bDescriptorType", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc_invalid_bDescriptor_type,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: FULL_DEV_DESC stage, invalid bDescriptorType field in the Device Descriptor
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return full Device Descriptor with invalid bDescriptorType filed
 * 6. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_FULL_DEV_DESC_invalid_bDescriptorType", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc_invalid_bDescriptor_type,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: Device has more that 1 Configuration
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return full Device Descriptor with bNumConfiguration=2
 * 6. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_FULL_DEV_DESC_bNumConfigurations=2", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc_bNumConfiguration_2,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: short Configuration Descriptor unexpected length
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return correct full Device Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Return short Configuration Descriptor with unexpected length (less), wTotalLength=0
 * 8. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_SHORT_CONFIG_DESC_unexpected_response_length_(less)", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc_unexpected_response_length_less,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: full Configuration Descriptor unexpected length
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return correct full Device Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Return short Configuration Descriptor
 * 8. Wait Get Configuration Descriptor request
 * 9. Return full Configuration Descriptor with unexpected length (less), wTotalLength=0
 * 10. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_FULL_CONFIG_DESC_unexpected_response_length_(less)", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc_unexpected_response_length_less,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: short Configuration Descriptor wrong bDescriptorType
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return correct full Device Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Return short Configuration Descriptor with wrong bDescriptorType=0
 * 8. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_SHORT_CONFIG_DESC_invalid_bDescriptorType", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc_invalid_bDescriptor_type,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Cancel enumeration: full Configuration Descriptor wrong bDescriptorType
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Return correct short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 5. Return correct full Device Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Return short Configuration Descriptor
 * 8. Wait Get Configuration Descriptor request
 * 7. Return full Configuration Descriptor with wrong bDescriptorType=0
 * 8. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::CHECK_FULL_CONFIG_DESC_invalid_bDescriptorType", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = (const uint8_t *) &mock_config_desc_invalid_bDescriptor_type,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}
