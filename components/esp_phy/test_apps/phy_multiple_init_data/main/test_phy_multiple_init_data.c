/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#include "esp_rom_crc.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "phy_init_data.h"
#include "sdkconfig.h"

#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED
extern const uint8_t multi_phy_init_data_bin_start[] asm("_binary_phy_multiple_init_data_bin_start");
extern const uint8_t multi_phy_init_data_bin_end[]   asm("_binary_phy_multiple_init_data_bin_end");
#else
#include "esp_partition.h"
#endif

/* phy_multiple_init_data.bin layout:
 *   [magic][default init data][magic][control info][certified init data] * number
 */
#define PHY_INIT_DATA_LEN      (sizeof(esp_phy_init_data_t))
#define CONTROL_INFO_OFFSET    (PHY_INIT_MAGIC_LEN + PHY_INIT_DATA_LEN + PHY_INIT_MAGIC_LEN)
#define MULTIPLE_DATA_OFFSET   (CONTROL_INFO_OFFSET + sizeof(phy_control_info_data_t))

static uint8_t *multiple_init_data_load(size_t *out_len)
{
    uint8_t *blob = NULL;
    size_t len = 0;

#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED
    len = (size_t)(multi_phy_init_data_bin_end - multi_phy_init_data_bin_start);
    blob = malloc(len);
    TEST_ASSERT_NOT_NULL(blob);
    memcpy(blob, multi_phy_init_data_bin_start, len);
#else
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(partition, "no phy data partition in the partition table");
    len = partition->size;
    blob = malloc(len);
    TEST_ASSERT_NOT_NULL(blob);
    TEST_ESP_OK(esp_partition_read(partition, 0, blob, len));
#endif

    *out_len = len;
    return blob;
}

/* The checksums in the bin are stored big-endian, see phy_crc_check_init_data() */
static bool crc32_matches(const uint8_t *data, size_t len, const uint8_t *expected)
{
    uint32_t crc = esp_rom_crc32_le(0, data, len);
    const uint8_t crc_be[4] = { crc >> 24, crc >> 16, crc >> 8, crc };

    return memcmp(crc_be, expected, sizeof(crc_be)) == 0;
}

static void control_info_get(const uint8_t *blob, phy_control_info_data_t *out_info)
{
    memcpy(out_info, blob + CONTROL_INFO_OFFSET, sizeof(*out_info));
}

static void wifi_start(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        TEST_ESP_OK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ESP_OK(err);

    TEST_ESP_OK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());
}

static void wifi_stop(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(nvs_flash_deinit());
}

TEST_CASE("multiple PHY init data bin matches the PHY init data of this target", "[phy_multiple_init_data]")
{
    size_t blob_len = 0;
    uint8_t *blob = multiple_init_data_load(&blob_len);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(MULTIPLE_DATA_OFFSET, blob_len);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(PHY_INIT_MAGIC, blob, PHY_INIT_MAGIC_LEN, "leading magic");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(PHY_INIT_MAGIC, blob + CONTROL_INFO_OFFSET - PHY_INIT_MAGIC_LEN,
                                     PHY_INIT_MAGIC_LEN, "trailing magic");
    /* Without this flag esp_phy_update_country_info() rejects every country code */
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, blob[PHY_INIT_MAGIC_LEN + PHY_SUPPORT_MULTIPLE_BIN_OFFSET],
                                  "bin is not marked as supporting multiple init data");

    phy_control_info_data_t info;
    control_info_get(blob, &info);
    TEST_ASSERT_EQUAL(PHY_CRC_ALGORITHM, info.check_algorithm);
    TEST_ASSERT_GREATER_THAN(0, info.number);
    /* Big-endian per-entry length, a mismatch means the bin was built for another PHY */
    TEST_ASSERT_EQUAL_MESSAGE(PHY_INIT_DATA_LEN, ((uint16_t)info.length[0] << 8) | info.length[1],
                              "per-entry length differs from sizeof(esp_phy_init_data_t)");

    TEST_ASSERT_TRUE_MESSAGE(crc32_matches(info.multiple_bin_checksum,
                                           sizeof(info) - sizeof(info.control_info_checksum),
                                           info.control_info_checksum),
                             "control info checksum mismatch");

    size_t multiple_data_len = PHY_INIT_DATA_LEN * info.number;
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(MULTIPLE_DATA_OFFSET + multiple_data_len, blob_len);
    TEST_ASSERT_TRUE_MESSAGE(crc32_matches(blob + MULTIPLE_DATA_OFFSET, multiple_data_len,
                                           info.multiple_bin_checksum),
                             "certified init data checksum mismatch");

    free(blob);
}

TEST_CASE("certified PHY init data types are known and unique", "[phy_multiple_init_data]")
{
    size_t blob_len = 0;
    uint8_t *blob = multiple_init_data_load(&blob_len);
    phy_control_info_data_t info;
    control_info_get(blob, &info);

    /* A bin may carry certifications that phy_init_data_type_t does not name yet,
       so the type byte is only bounded by its own width */
    bool seen[256] = { false };
    unsigned unnamed = 0;
    for (uint8_t i = 0; i < info.number; i++) {
        uint8_t type = blob[MULTIPLE_DATA_OFFSET + i * PHY_INIT_DATA_LEN + PHY_INIT_DATA_TYPE_OFFSET];
        /* The DEFAULT type is the block in front of the control info, not a certified entry */
        TEST_ASSERT_NOT_EQUAL(ESP_PHY_INIT_DATA_TYPE_DEFAULT, type);
        TEST_ASSERT_FALSE_MESSAGE(seen[type], "duplicated init data type");
        seen[type] = true;
        if (type >= ESP_PHY_INIT_DATA_TYPE_NUMBER) {
            unnamed++;
        }
    }
    printf("%u certified entries, %u of them unnamed by phy_init_data_type_t\n", info.number, unnamed);

    /* The country codes exercised by this app have to be certified in the bin */
    static const phy_init_data_type_t required[] = {
        ESP_PHY_INIT_DATA_TYPE_SRRC,
        ESP_PHY_INIT_DATA_TYPE_FCC,
        ESP_PHY_INIT_DATA_TYPE_CE,
        ESP_PHY_INIT_DATA_TYPE_NCC,
        ESP_PHY_INIT_DATA_TYPE_KCC,
        ESP_PHY_INIT_DATA_TYPE_MIC,
    };
    for (size_t i = 0; i < sizeof(required) / sizeof(required[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(seen[required[i]], "a country code tested below is not certified");
    }

    free(blob);
}

TEST_CASE("esp_phy_get_init_data loads the multiple PHY init data bin", "[phy_multiple_init_data]")
{
    const esp_phy_init_data_t *init_data = esp_phy_get_init_data();
    TEST_ASSERT_NOT_NULL(init_data);
    esp_phy_release_init_data(init_data);
}

TEST_CASE("country code selects the certified PHY init data", "[phy_multiple_init_data]")
{
    static const struct {
        const char *country;
        phy_init_data_type_t type;
    } certified[] = {
        { "CN", ESP_PHY_INIT_DATA_TYPE_SRRC },
        { "US", ESP_PHY_INIT_DATA_TYPE_FCC },
        { "DE", ESP_PHY_INIT_DATA_TYPE_CE },
        { "TW", ESP_PHY_INIT_DATA_TYPE_NCC },
        { "KR", ESP_PHY_INIT_DATA_TYPE_KCC },
        { "JP", ESP_PHY_INIT_DATA_TYPE_MIC },
    };

    wifi_start();

    for (size_t i = 0; i < sizeof(certified) / sizeof(certified[0]); i++) {
        TEST_ESP_OK(esp_phy_update_country_info(certified[i].country));
        TEST_ASSERT_EQUAL_MESSAGE(certified[i].type, esp_phy_get_init_data_type(), certified[i].country);
    }

    /* Re-applying the same country keeps the PHY on the same init data */
    TEST_ESP_OK(esp_phy_update_country_info("JP"));
    TEST_ASSERT_EQUAL(ESP_PHY_INIT_DATA_TYPE_MIC, esp_phy_get_init_data_type());

    wifi_stop();
}

TEST_CASE("uncertified country codes fall back to the default PHY init data", "[phy_multiple_init_data]")
{
    wifi_start();

    TEST_ESP_OK(esp_phy_update_country_info("CN"));
    TEST_ASSERT_EQUAL(ESP_PHY_INIT_DATA_TYPE_SRRC, esp_phy_get_init_data_type());

    /* AU maps to ACMA, for which the bin carries no certified init data */
    TEST_ESP_OK(esp_phy_update_country_info("AU"));
    TEST_ASSERT_EQUAL(ESP_PHY_INIT_DATA_TYPE_DEFAULT, esp_phy_get_init_data_type());

    /* An unmapped country code is answered with the default init data as well */
    TEST_ESP_OK(esp_phy_update_country_info("ZZ"));
    TEST_ASSERT_EQUAL(ESP_PHY_INIT_DATA_TYPE_DEFAULT, esp_phy_get_init_data_type());

    wifi_stop();
}

TEST_CASE("esp_wifi_set_country_code updates the PHY init data", "[phy_multiple_init_data]")
{
    wifi_start();

    TEST_ESP_OK(esp_wifi_set_country_code("JP", false));

    /* The Wi-Fi task drives the PHY update, so the type may lag the API call */
    for (int i = 0; i < 100 && esp_phy_get_init_data_type() != ESP_PHY_INIT_DATA_TYPE_MIC; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    TEST_ASSERT_EQUAL(ESP_PHY_INIT_DATA_TYPE_MIC, esp_phy_get_init_data_type());

    /* wifi_country_t.cc is 3 octets (ISO code + environment), not a C string */
    char country_code[3] = { 0 };
    TEST_ESP_OK(esp_wifi_get_country_code(country_code));
    TEST_ASSERT_EQUAL('J', country_code[0]);
    TEST_ASSERT_EQUAL('P', country_code[1]);

    wifi_stop();
}
