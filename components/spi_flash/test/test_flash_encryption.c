#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <test_utils.h>
#include <esp_spi_flash.h>
#include <esp_attr.h>
#include <esp_flash_encrypt.h>
#include <string.h>


#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED

static void test_encrypted_write(size_t offset, const uint8_t *data, size_t length);
static void test_encrypted_write_new_impl(size_t offset, const uint8_t *data, size_t length);
static void verify_erased_flash(size_t offset, size_t length);

static size_t start;

static void setup_tests(void)
{
    if (start == 0) {
        const esp_partition_t *part = get_test_data_partition();
        start = part->address;
        printf("Test data partition @ 0x%x\n", start);
    }
}

TEST_CASE("test 16 byte encrypted writes", "[flash_encryption][test_env=UT_T1_FlashEncryption]")
{
    setup_tests();

    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                      spi_flash_erase_sector(start / SPI_FLASH_SEC_SIZE));

    uint8_t fortyeight_bytes[0x30]; // 0, 1, 2, 3, 4... 47
    for(int i = 0; i < sizeof(fortyeight_bytes); i++) {
        fortyeight_bytes[i] = i;
    }

    /* Verify unaligned start or length fails */
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_ARG,
                      spi_flash_write_encrypted(start+1, fortyeight_bytes, 32));

    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_SIZE,
                      spi_flash_write_encrypted(start, fortyeight_bytes, 15));

    /* ensure nothing happened to the flash yet */
    verify_erased_flash(start, 0x20);

    /* Write 32 byte block, this is the "normal" encrypted write */
    test_encrypted_write(start, fortyeight_bytes, 0x20);
    verify_erased_flash(start + 0x20, 0x20);

    /* Slip in an unaligned spi_flash_read_encrypted() test */
    uint8_t buf[0x10];
    spi_flash_read_encrypted(start+0x10, buf, 0x10);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(fortyeight_bytes+0x10, buf, 16);

    /* Write 16 bytes unaligned */
    test_encrypted_write(start + 0x30, fortyeight_bytes, 0x10);
    /* the 16 byte regions before and after the 16 bytes we just wrote should still be 0xFF */
    verify_erased_flash(start + 0x20, 0x10);
    verify_erased_flash(start + 0x40, 0x10);

    /* Write 48 bytes starting at a 32-byte aligned offset */
    test_encrypted_write(start + 0x40, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(start + 0x70, 0x10);

    /* Write 48 bytes starting at a 16-byte aligned offset */
    test_encrypted_write(start + 0x90, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(start + 0x120, 0x10);
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

TEST_CASE("test 16 byte encrypted writes (esp_flash)", "[flash_encryption][esp_flash_enc][test_env=UT_T1_FlashEncryption]")
{
    setup_tests();

    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                      spi_flash_erase_sector(start / SPI_FLASH_SEC_SIZE));

    uint8_t fortyeight_bytes[0x30]; // 0, 1, 2, 3, 4... 47
    for(int i = 0; i < sizeof(fortyeight_bytes); i++) {
        fortyeight_bytes[i] = i;
    }

    /* Verify unaligned start or length fails */
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_ARG,
                      esp_flash_write_encrypted(NULL, start+1, fortyeight_bytes, 32));

    TEST_ASSERT_EQUAL_HEX(ESP_ERR_INVALID_SIZE,
                      esp_flash_write_encrypted(NULL, start, fortyeight_bytes, 15));

    /* ensure nothing happened to the flash yet */
    verify_erased_flash(start, 0x20);

    /* Write 32 byte block, this is the "normal" encrypted write */
    test_encrypted_write_new_impl(start, fortyeight_bytes, 0x20);
    verify_erased_flash(start + 0x20, 0x20);

    /* Slip in an unaligned esp_flash_read_encrypted() test */
    uint8_t buf[0x10];
    esp_flash_read_encrypted(NULL, start+0x10, buf, 0x10);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(fortyeight_bytes+0x10, buf, 16);

    /* Write 16 bytes unaligned */
    test_encrypted_write_new_impl(start + 0x30, fortyeight_bytes, 0x10);
    /* the 16 byte regions before and after the 16 bytes we just wrote should still be 0xFF */
    verify_erased_flash(start + 0x20, 0x10);
    verify_erased_flash(start + 0x40, 0x10);

    /* Write 48 bytes starting at a 32-byte aligned offset */
    test_encrypted_write_new_impl(start + 0x40, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(start + 0x70, 0x10);

    /* Write 48 bytes starting at a 16-byte aligned offset */
    test_encrypted_write_new_impl(start + 0x90, fortyeight_bytes, 0x30);
    /* 16 bytes after this write should still be 0xFF -unencrypted- */
    verify_erased_flash(start + 0x120, 0x10);
}

static void test_encrypted_write_new_impl(size_t offset, const uint8_t *data, size_t length)
{
    uint8_t readback[length];
    printf("encrypt %d bytes at 0x%x\n", length, offset);
    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                          esp_flash_write_encrypted(NULL, offset, data, length));

    TEST_ASSERT_EQUAL_HEX(ESP_OK,
                          esp_flash_read_encrypted(NULL, offset, readback, length));

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

TEST_CASE("test read & write random encrypted data", "[flash_encryption][test_env=UT_T1_FlashEncryption]")
{
    const int MAX_LEN = 192;
    //buffer to hold the read data
    WORD_ALIGNED_ATTR uint8_t buffer_to_write[MAX_LEN+4];
    //test with unaligned buffer
    uint8_t* data_buf = &buffer_to_write[3];

    setup_tests();

    esp_err_t err = spi_flash_erase_sector(start / SPI_FLASH_SEC_SIZE);
    TEST_ESP_OK(err);

    //initialize the buffer to compare
    uint8_t *cmp_buf = heap_caps_malloc(SPI_FLASH_SEC_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    assert(((intptr_t)cmp_buf % 4) == 0);
    err = spi_flash_read_encrypted(start, cmp_buf, SPI_FLASH_SEC_SIZE);
    TEST_ESP_OK(err);

    srand(789);

    uint32_t offset = 0;
    do {
        //the encrypted write only works at 16-byte boundary
        int skip = (rand() % 4) * 16;
        int len = ((rand() % (MAX_LEN/16)) + 1) * 16;

        for (int i = 0; i < MAX_LEN; i++) {
            data_buf[i] = rand();
        }

        offset += skip;
        if (offset + len > SPI_FLASH_SEC_SIZE) {
            if (offset > SPI_FLASH_SEC_SIZE) {
                break;
            }
            len = SPI_FLASH_SEC_SIZE - offset;
        }

        printf("write %d bytes to 0x%08x...\n", len, start + offset);
        err = spi_flash_write_encrypted(start + offset, data_buf, len);
        TEST_ESP_OK(err);

        memcpy(cmp_buf + offset, data_buf, len);
        offset += len;
    } while (offset < SPI_FLASH_SEC_SIZE);

    offset = 0;
    do {
        int len = ((rand() % (MAX_LEN/16)) + 1) * 16;
        if (offset + len > SPI_FLASH_SEC_SIZE) {
            len = SPI_FLASH_SEC_SIZE - offset;
        }

        err = spi_flash_read_encrypted(start + offset, data_buf, len);
        TEST_ESP_OK(err);

        printf("compare %d bytes at 0x%08x...\n", len, start + offset);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(cmp_buf + offset, data_buf, len);
        offset += len;
    } while (offset < SPI_FLASH_SEC_SIZE);

    free(cmp_buf);
}

#endif // CONFIG_SECURE_FLASH_ENC_ENABLED
