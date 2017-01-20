#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>
#include <esp_flash_encrypt.h>

#include "test_config.h"

static void test_encrypted_write(size_t offset, const uint8_t *data, size_t length);
static void verify_erased_flash(size_t offset, size_t length);

TEST_CASE("test 16 byte encrypted writes", "[spi_flash]")
{
    if (!esp_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption disabled, skipping spi_flash_write_encrypted() tests");
    }

    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                      spi_flash_erase_sector(TEST_REGION_START / SPI_FLASH_SEC_SIZE));

    uint8_t fortyeight_bytes[0x30]; // 0, 1, 2, 3, 4... 47
    for(int i = 0; i < sizeof(fortyeight_bytes); i++) {
        fortyeight_bytes[i] = i;
    }

    /* Verify unaligned start or length fails */
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_ARG,
                      spi_flash_write_encrypted(TEST_REGION_START+1, fortyeight_bytes, 32));

    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_SIZE,
                      spi_flash_write_encrypted(TEST_REGION_START, fortyeight_bytes, 15));

    /* ensure nothing happened to the flash yet */
    verify_erased_flash(TEST_REGION_START, 0x20);

    /* Write 32 byte block, this is the "normal" encrypted write */
    test_encrypted_write(TEST_REGION_START, fortyeight_bytes, 0x20);
    verify_erased_flash(TEST_REGION_START + 0x20, 0x20);

    /* Slip in an unaligned spi_flash_read_encrypted() test */
    uint8_t buf[0x10];
    spi_flash_read_encrypted(TEST_REGION_START+0x10, buf, 0x10);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(fortyeight_bytes+0x10, buf, 16);

    /* Write 16 bytes unaligned */
    test_encrypted_write(TEST_REGION_START + 0x30, fortyeight_bytes, 0x10);
    /* the 16 byte regions before and after the 16 bytes we just wrote should still be 0xFF */
    verify_erased_flash(TEST_REGION_START + 0x20, 0x10);
    verify_erased_flash(TEST_REGION_START + 0x40, 0x10);

    /* Write 48 bytes starting at a 32-byte aligned offset */
    test_encrypted_write(TEST_REGION_START + 0x40, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(TEST_REGION_START + 0x70, 0x10);

    /* Write 48 bytes starting at a 16-byte aligned offset */
    test_encrypted_write(TEST_REGION_START + 0x90, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(TEST_REGION_START + 0x120, 0x10);
}

static void test_encrypted_write(size_t offset, const uint8_t *data, size_t length)
{
    uint8_t readback[length];
    printf("encrypt %d bytes at 0x%x\n", length, offset);
    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                          spi_flash_write_encrypted(offset, data, length));

    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                          spi_flash_read_encrypted(offset, readback, length));

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, readback, length);
}

static void verify_erased_flash(size_t offset, size_t length)
{
    uint8_t readback[length];
    printf("verify erased 0x%x - 0x%x\n", offset, offset + length);
    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                          spi_flash_read(offset, readback, length));
    for (int i = 0; i < length; i++) {
        char message[32];
        sprintf(message, "unerased flash @ 0x%08x", offset + i);
        TEST_ASSERT_EQUAL_HEX_MESSAGE(0xFF, readback[i], message);
    }
}

