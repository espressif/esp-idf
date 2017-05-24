#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <spi_flash_lowlevel.h>
#include <esp_attr.h>

#include <test_utils.h>

static uint8_t sector_buf[4096];

TEST_CASE("SPI flash metadata functions", "[spi_flash_ll]")
{
    uint32_t id, size;

    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read_id(NULL, &id) );

    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_detect_size(NULL, &size) );

    printf("Flash ID %08x detected size %d bytes\n", id, size);
}

static uint32_t erase_test_region(int num_sectors)
{
    const esp_partition_t *part = get_test_data_partition();
    uint32_t offs = part->address;

    /* chip should be initialised */
    TEST_ASSERT(esp_flash_default_chip != NULL
                && esp_flash_default_chip->drv != NULL);

    TEST_ASSERT(num_sectors * 4096 <= part->size);

    bzero(sector_buf, sizeof(sector_buf));

    printf("Erase @ 0x%x...\n", offs);
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_erase_region(NULL, offs, num_sectors * 4096) );

    printf("Verify erased...\n");
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, sector_buf, offs, sizeof(sector_buf)) );

    printf("Buffer starts 0x%02x 0x%02x 0x%02x 0x%02x\n", sector_buf[0], sector_buf[1], sector_buf[2], sector_buf[3]);
    for (int i = 0; i < sizeof(sector_buf); i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, sector_buf[i]);
    }

    return offs;
}

TEST_CASE("SPI flash simple read/write", "[spi_flash_ll]")
{
    uint32_t offs = erase_test_region(1);

    for (int i =0 ; i < sizeof(sector_buf); i++) {
        sector_buf[i] = i & 0xFF;
    }

    printf("Write...\n");
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, offs, sector_buf, sizeof(sector_buf)) );

    bzero(sector_buf, sizeof(sector_buf));

    printf("Read back...\n");
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, sector_buf, offs, sizeof(sector_buf)) );

    printf("Buffer starts 0x%02x 0x%02x 0x%02x 0x%02x\n", sector_buf[0], sector_buf[1], sector_buf[2], sector_buf[3]);

    for (int i = 0; i < sizeof(sector_buf); i++) {
        TEST_ASSERT_EQUAL_HEX8(i & 0xFF, sector_buf[i]);
    }
}

TEST_CASE("SPI flash unaligned read/write", "[spi_flash_ll]")
{
    uint32_t offs = erase_test_region(2);

    const char *msg = "i am a message";
    TEST_ASSERT(strlen(msg)+1 % 4 != 0);
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, offs+1, msg, strlen(msg)+1) );

    char buf[strlen(msg) + 1];

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, buf, offs+1, strlen(msg)+1) );
    TEST_ASSERT_EQUAL_STRING_LEN(msg, buf, strlen(msg));
    TEST_ASSERT(memcmp(buf, msg, strlen(msg)+1) == 0);
}


TEST_CASE("SPI flash single byte reads/writes", "[spi_flash_ll]")
{
    uint32_t offs = erase_test_region(2);

    for (unsigned v = 0; v < 512; v++) {
        TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, offs+v, &v, 1) );
    }

    for (unsigned v = 0; v < 512; v++) {
        uint8_t readback;
        TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, &readback, offs+v, 1) );
        TEST_ASSERT_EQUAL_HEX8(v, readback);
    }
}

/* this test is notable because it generates a lot of unaligned reads/writes,
   and also reads/writes across both a sector boundary & many page boundaries.
*/
TEST_CASE("SPI flash three byte reads/writes", "[spi_flash_ll]")
{
    uint32_t offs = erase_test_region(2);

    for (uint32_t v = 0; v < 2000; v++) {
        TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, offs+3*v, &v, 3) );
    }

    for (uint32_t v = 0; v < 2000; v++) {
        uint32_t readback;
        TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, &readback, offs+3*v, 3) );
        TEST_ASSERT_EQUAL_HEX32(v & 0xFFFFFF, readback & 0xFFFFFF);
    }
}

TEST_CASE("SPI flash erase large region", "[spi_flash_ll]")
{
    const esp_partition_t *part = get_test_data_partition();

    /* Write some noise at the start and the end of the region */
    const char *ohai = "OHAI";
    uint32_t readback;
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, part->address, ohai, 5));
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, part->address + part->size - 5, ohai, 5));

    /* sanity check what we just wrote */
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, &readback, part->address + part->size - 5, 4));
    TEST_ASSERT_EQUAL_HEX32(*((const uint32_t*)ohai), readback);

    /* Erase whole region */
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_erase_region(NULL, part->address, part->size));

    /* ensure both areas we wrote are now all-FFs */
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, &readback, part->address, 4));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, readback);

    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(NULL, &readback, part->address + part->size - 5, 4));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, readback);
}

TEST_CASE("SPI flash test reading with all speed/mode permutations", "[spi_flash_ll]")
{
    /* Note: this only works if the SPI flash chip supports all these modes & speeds */

    uint32_t offs = erase_test_region(1);

    /* Write some test data */
    const char *message = "This is some test data.";
    TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_write(NULL, offs, message, strlen(message)+1) );

    // Start by copying the default chip to a structure we can tweak
    esp_flash_chip_t chip = *esp_flash_default_chip;

    for (chip.read_mode = 0;
         chip.read_mode != ESP_FLASH_READ_MODE_MAX;
         chip.read_mode++) {
        for (chip.speed = 0;
             chip.speed != ESP_FLASH_SPEED_MAX;
             chip.speed++) {
            printf("mode %d speed %d\n", chip.read_mode, chip.speed);

            TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_init(&chip) );

            char *buf[strlen(message)+1];
            memset(buf, 0xFF, sizeof(buf));
            TEST_ASSERT_EQUAL(FLASH_OK, esp_flash_read(&chip, buf, offs, sizeof(buf)) );
            TEST_ASSERT_EQUAL_STRING_LEN(message, buf, strlen(message));
        }
    }
}
