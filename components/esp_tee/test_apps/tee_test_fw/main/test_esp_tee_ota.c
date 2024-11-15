/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "unity.h"
#include "esp_log.h"

#include "esp_system.h"
#include "esp_partition.h"
#include "esp_image_format.h"
#include "esp_tee_ota_ops.h"

#include "esp_tee.h"
#include "secure_service_num.h"

#define TEE_IMG_SRC_PART_SUBTYPE  (ESP_PARTITION_SUBTYPE_APP_OTA_1)
#define OTA_BUF_SIZE              (512)

#define FLASH_SECTOR_SIZE       (4096)
#define ALIGN_UP(num, align)    (((num) + ((align)-1)) & ~((align)-1))
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

static const char *TAG = "test_esp_tee_ota";

TEST_CASE("Test TEE OTA - API state check", "[ota_neg_1]")
{
    uint8_t buf[OTA_BUF_SIZE];
    memset(buf, 0xAA, sizeof(buf));

    ESP_LOGI(TAG, "TEE OTA - write without begin");
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_ota_write(0x00, (const void *)buf, sizeof(buf)));

    ESP_LOGI(TAG, "TEE OTA - end without begin and write");
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_tee_ota_end());

    ESP_LOGI(TAG, "TEE OTA - end without write");
    TEST_ESP_OK(esp_tee_ota_begin());
    TEST_ESP_ERR(ESP_ERR_IMAGE_INVALID, esp_tee_ota_end());
}

TEST_CASE("Test TEE OTA - Write out of bounds", "[ota_neg_1]")
{
    uint8_t buf[OTA_BUF_SIZE];
    memset(buf, 0xAA, sizeof(buf));

    const esp_partition_t *tee_next = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
    TEST_ASSERT_NOT_NULL(tee_next);

    TEST_ESP_OK(esp_tee_ota_begin());
    TEST_ESP_ERR(ESP_FAIL, esp_tee_ota_write(tee_next->size, (const void *)buf, sizeof(buf)));
    TEST_ESP_ERR(ESP_FAIL, esp_tee_ota_write(tee_next->size - sizeof(buf) + 1, (const void *)buf, sizeof(buf)));
}

TEST_CASE("Test TEE OTA - Invalid image", "[ota_neg_1]")
{
    uint8_t buf[OTA_BUF_SIZE];
    memset(buf, 0xAA, sizeof(buf));

    TEST_ESP_OK(esp_tee_ota_begin());
    TEST_ESP_OK(esp_tee_ota_write(0x00, (const void *)buf, sizeof(buf)));
    TEST_ESP_ERR(ESP_ERR_IMAGE_INVALID, esp_tee_ota_end());
}

static uint32_t copy_tee_update(void)
{
    /* NOTE: In a real-world scenario, we would get the non-secure app's next update partition */
    const esp_partition_t *tee_update = esp_partition_find_first(ESP_PARTITION_TYPE_APP, TEE_IMG_SRC_PART_SUBTYPE, NULL);
    TEST_ASSERT_NOT_NULL(tee_update);

    const esp_partition_pos_t tee_next_pos = {
        .offset = tee_update->address,
        .size = tee_update->size,
    };
    esp_image_metadata_t tee_next_metadata = {};

    TEST_ESP_OK(esp_image_get_metadata(&tee_next_pos, &tee_next_metadata));

    uint32_t tee_next_img_len = tee_next_metadata.image_len;

#if CONFIG_SECURE_BOOT_V2_ENABLED
    tee_next_img_len = ALIGN_UP(tee_next_img_len, FLASH_SECTOR_SIZE) + FLASH_SECTOR_SIZE;
#endif

    uint32_t curr_write_offset = 0;
    bool image_header_was_checked = false;  /* deal with all receive packet */

    /* an ota data write buffer ready to write to the flash */
    uint8_t ota_write_data[OTA_BUF_SIZE];
    memset(ota_write_data, 0xFF, OTA_BUF_SIZE);

    while (curr_write_offset < tee_next_img_len) {
        uint32_t bytes_to_copy = (curr_write_offset + OTA_BUF_SIZE <= tee_next_img_len)
                                 ? OTA_BUF_SIZE
                                 : (tee_next_img_len - curr_write_offset);

        TEST_ESP_OK(esp_partition_read(tee_update, curr_write_offset, ota_write_data, bytes_to_copy));

        if (image_header_was_checked == false) {
            esp_image_header_t img_hdr;
            memcpy(&img_hdr, ota_write_data, sizeof(esp_image_header_t));
            TEST_ASSERT_EQUAL(img_hdr.chip_id, CONFIG_IDF_FIRMWARE_CHIP_ID);
            image_header_was_checked = true;

            TEST_ESP_OK(esp_tee_ota_begin());
        }

        TEST_ESP_OK(esp_tee_ota_write(curr_write_offset, (const void *)ota_write_data, bytes_to_copy));

        curr_write_offset += bytes_to_copy;
        memset(ota_write_data, 0xFF, OTA_BUF_SIZE);
    }

    ESP_LOGI(TAG, "Done copying the update!");
    return curr_write_offset;
}

TEST_CASE("Test TEE OTA - Corrupted image", "[ota_neg_2]")
{
    ESP_LOGI(TAG, "Starting TEE OTA...");

    uint32_t curr_write_offset = copy_tee_update();
    ESP_LOGI(TAG, "Total binary data written: %lu", curr_write_offset);

    /* Corrupting the image */
    ESP_LOGI(TAG, "Corrupting the image at some offset...");
    uint32_t corrupt[8] = {[0 ... 7] = 0x0BADC0DE};
    curr_write_offset -= (2 * FLASH_SECTOR_SIZE + sizeof(corrupt));
    TEST_ESP_OK(esp_tee_ota_write(curr_write_offset, (const void *)corrupt, sizeof(corrupt)));

    TEST_ESP_ERR(ESP_ERR_IMAGE_INVALID, esp_tee_ota_end());
}

TEST_CASE("Test TEE OTA - Reboot without ending OTA", "[ota_neg_2]")
{
    ESP_LOGI(TAG, "Starting TEE OTA...");

    uint32_t bytes_wr = copy_tee_update();
    ESP_LOGI(TAG, "Total binary data written: %lu", bytes_wr);

    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
}

TEST_CASE("Test TEE OTA - Valid image", "[ota_valid_img]")
{
    ESP_LOGI(TAG, "Starting TEE OTA...");

    uint32_t bytes_wr = copy_tee_update();
    ESP_LOGI(TAG, "Total binary data written: %lu", bytes_wr);

    TEST_ESP_OK(esp_tee_ota_end());
    ESP_LOGI(TAG, "TEE OTA update successful!");

    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
}

TEST_CASE("Test TEE OTA - Rollback", "[ota_rollback]")
{
    ESP_LOGI(TAG, "Starting TEE OTA...");

    uint32_t bytes_wr = copy_tee_update();
    ESP_LOGI(TAG, "Total binary data written: %lu", bytes_wr);

    TEST_ESP_OK(esp_tee_ota_end());

    ESP_LOGI(TAG, "TEE OTA update successful!");
    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
}
