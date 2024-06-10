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
 * @brief STALL Get short Device Descriptor request (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. STALL EP0
 * 4. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_DEV_DESC_STALLed", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = NULL,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full Device Descriptor request (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. STALL EP0
 * 4. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_DEV_DESC_STALLed", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
        .enumeration = {
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                .desc = (const uint8_t *) &mock_device_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                .desc = NULL,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get short Configuration Descriptor request (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. STALL EP0
 * 6. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_CONFIG_DESC_STALLed", "[mock_enum_device]")
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
                .desc = NULL,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full Configuration Descriptor request (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. STALL EP0
 * 8. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_CONFIG_DESC_STALLed", "[mock_enum_device]")
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
                .desc = mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = NULL,
                .stop_test = true,
            },
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get short String Descriptor - LANGID (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. STALL EP0
 * 10. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_LANGID_TABLE_STALLed", "[mock_enum_device]")
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
                .desc = mock_config_desc,
            },
            .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                .desc = mock_config_desc,
            },
            // String Descriptors
            .stage_config[MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE] = {
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full String Descriptor - LANGID (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. STALL EP0
 * 10. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_LANGID_TABLE_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get short String Descriptor - Manufacturer (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufactirer)
 * 13. STALL EP0
 * 14. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_MANU_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full String Descriptor - Manufacturer (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufactirer)
 * 13. Reply with short String Descriptor (Manufactirer)
 * 14. Wait Get String Descriptor request (Manufactirer)
 * 15. STALL EP0
 * 16. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_MANU_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get short String Descriptor - Product (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. STALL EP0
 * 18. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_PROD_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full String Descriptor - Product (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. Reply with short String Descriptor (Product)
 * 18. Wait Get String Descriptor request (Product)
 * 19. STALL EP0
 * 20. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_PROD_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get short String Descriptor - Serial (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. Reply with short String Descriptor (Product)
 * 18. Wait Get String Descriptor request (Product)
 * 19. Reply with full String Descriptor (Product)
 * 20. Wait Get String Descriptor request (Serial)
 * 21. STALL EP0
 * 22. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_SHORT_SER_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Get full String Descriptor - Serial (S/W teardown)
 *
 * @note STALL Get String Descriptor request is allowed (means device doesn't have string descriptor)
 * Thus, mock_dev_cfg.stop_test parameter is not used.
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. Reply with short String Descriptor (Product)
 * 18. Wait Get String Descriptor request (Product)
 * 19. Reply with full String Descriptor (Product)
 * 20. Wait Get String Descriptor request (Serial)
 * 21. Reply with short String Descriptor (Serial)
 * 22. Wait Get String Descriptor request (Serial)
 * 23. STALL EP0
 * 24. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::GET_FULL_SER_STR_DESC_STALLed", "[mock_enum_device]")
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
                .str_desc = NULL,
            },
            .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                .desc = mock_config_desc,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief STALL Set Configuration (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. Reply with short String Descriptor (Product)
 * 18. Wait Get String Descriptor request (Product)
 * 19. Reply with full String Descriptor (Product)
 * 20. Wait Get String Descriptor request (Serial)
 * 21. Reply with short String Descriptor (Serial)
 * 22. Wait Get String Descriptor request (Serial)
 * 23. Reply with full String Descriptor (Serial)
 * 24. Wait Set Configuration request
 * 25. STALL EP0
 * 26. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::SET_CONFIG_STALLed", "[mock_enum_device]")
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
                .desc = NULL,
                .stop_test = true,
            }
        },
    };

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}

/**
 * @brief Regular Device enumeration routine (S/W teardown)
 *
 * Scenario:
 * 1. Install TinyUSB device stack
 * 2. Wait for Get Device Descriptor request
 * 3. Reply with short Device Descriptor
 * 4. Wait for Get Device Descriptor request
 * 3. Reply with full Device Descriptor
 * 4. Wait Get Configuration Descriptor request
 * 5. Reply with short Configuration Descriptor
 * 6. Wait Get Configuration Descriptor request
 * 7. Reply with full Configuration Descriptor
 * 8. Wait Get String Descriptor request (LANGID)
 * 9. Reply with short String Descriptor (LANGID)
 * 10. Wait Get String Descriptor request (LANGID)
 * 11. Reply with full String Descriptor (LANGID)
 * 12. Wait Get String Descriptor request (Manufacturer)
 * 13. Reply with short String Descriptor (Manufacturer)
 * 14. Wait Get String Descriptor request (Manufacturer)
 * 15. Reply with full String Descriptor (Manufacturer)
 * 16. Wait Get String Descriptor request (Product)
 * 17. Reply with short String Descriptor (Product)
 * 18. Wait Get String Descriptor request (Product)
 * 19. Reply with full String Descriptor (Product)
 * 20. Wait Get String Descriptor request (Serial)
 * 21. Reply with short String Descriptor (Serial)
 * 22. Wait Get String Descriptor request (Serial)
 * 23. Reply with full String Descriptor (Serial)
 * 24. Wait Set Configuration request
 * 25. Acknowledge Configuration setup
 * 26. Uninstall TinyUSB device stack
 */
TEST_CASE("enum::complete", "[mock_enum_device]")
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

    TEST_ASSERT_EQUAL(true, mock_dev_setup(&mock_dev_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, mock_dev_wait_flag());
    mock_dev_release();
}
