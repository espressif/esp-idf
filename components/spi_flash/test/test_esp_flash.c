#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_flash.h"
#include "driver/spi_common.h"
#include "esp_flash_spi_init.h"
#include <esp_attr.h>
#include "esp_log.h"

#include <test_utils.h>

#include "unity.h"
#include "driver/spi_common.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"


#define FUNC_SPI    1

static uint8_t sector_buf[4096];

// #define TEST_SPI1_CS1
// #define TEST_SPI2_CS0
// #define TEST_SPI3_CS0
#define TEST_SPI_SPEED      ESP_FLASH_10MHZ
#define TEST_SPI_READ_MODE  SPI_FLASH_FASTRD
//#define FORCE_GPIO_MATRIX

#define HSPI_PIN_NUM_MOSI   HSPI_IOMUX_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   HSPI_IOMUX_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    HSPI_IOMUX_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     HSPI_IOMUX_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     HSPI_IOMUX_PIN_NUM_WP

#define VSPI_PIN_NUM_MOSI   VSPI_IOMUX_PIN_NUM_MOSI
#define VSPI_PIN_NUM_MISO   VSPI_IOMUX_PIN_NUM_MISO
#define VSPI_PIN_NUM_CLK    VSPI_IOMUX_PIN_NUM_CLK
#define VSPI_PIN_NUM_HD     VSPI_IOMUX_PIN_NUM_HD
#define VSPI_PIN_NUM_WP     VSPI_IOMUX_PIN_NUM_WP

#if defined TEST_SPI1_CS1
#  define TEST_HOST   SPI_HOST
#  define TEST_CS     1
// #define TEST_CS_PIN 14
#  define TEST_CS_PIN 16  //the pin which is usually used by the PSRAM
// #define TEST_CS_PIN 27
#  define TEST_INPUT_DELAY    0
#  define EXTRA_SPI1_CLK_IO   17  //the pin which is usually used by the PSRAM clk

#elif defined TEST_SPI2_CS0

#  define TEST_HOST           HSPI_HOST
#  define TEST_CS             0
#  define TEST_CS_PIN         HSPI_IOMUX_PIN_NUM_CS
#  define TEST_INPUT_DELAY    20

#elif defined TEST_SPI3_CS0

#  define TEST_HOST           VSPI_HOST
#  define TEST_CS             0
#  define TEST_CS_PIN         VSPI_IOMUX_PIN_NUM_CS
#  define TEST_INPUT_DELAY    0

#else
#  define SKIP_EXTENDED_CHIP_TEST
#endif


static const char TAG[] = "test_esp_flash";


#ifndef SKIP_EXTENDED_CHIP_TEST

static esp_flash_t *test_chip = NULL;

static void setup_bus(spi_host_device_t host_id)
{
    if (host_id == SPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...\n");
        //no need to initialize the bus, however the CLK may need one more output if it's on the usual place of PSRAM
#ifdef EXTRA_SPI1_CLK_IO
        gpio_matrix_out(EXTRA_SPI1_CLK_IO, SPICLK_OUT_IDX, 0, 0);
#endif
        //currently the SPI bus for main flash chip is initialized through GPIO matrix
    } else if (host_id == HSPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI2 (HSPI) CS0...\n");
        spi_bus_config_t hspi_bus_cfg = {
            .mosi_io_num = HSPI_PIN_NUM_MOSI,
            .miso_io_num = HSPI_PIN_NUM_MISO,
            .sclk_io_num = HSPI_PIN_NUM_CLK,
            .quadhd_io_num = HSPI_PIN_NUM_HD,
            .quadwp_io_num = HSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
#ifdef FORCE_GPIO_MATRIX
        hspi_bus_cfg.quadhd_io_num = 23;
#endif
        esp_err_t ret = spi_bus_initialize(host_id, &hspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
    } else if (host_id == VSPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI3 (VSPI) CS0...\n");
        spi_bus_config_t vspi_bus_cfg = {
            .mosi_io_num = VSPI_PIN_NUM_MOSI,
            .miso_io_num = VSPI_PIN_NUM_MISO,
            .sclk_io_num = VSPI_PIN_NUM_CLK,
            .quadhd_io_num = VSPI_PIN_NUM_HD,
            .quadwp_io_num = VSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
#ifdef FORCE_GPIO_MATRIX
        vspi_bus_cfg.quadhd_io_num = 23;
#endif
        esp_err_t ret = spi_bus_initialize(host_id, &vspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
    } else {
        ESP_LOGE(TAG, "invalid bus");
    }
}

static void release_bus(int host_id)
{
    if (host_id == HSPI_HOST || host_id == VSPI_HOST) {
        spi_bus_free(host_id);
    }
}

static void setup_new_chip(esp_flash_read_mode_t io_mode, esp_flash_speed_t speed)
{
    //the bus should be initialized before the flash is attached to the bus
    setup_bus(TEST_HOST);

    esp_flash_spi_device_config_t dev_cfg = {
        .host_id = TEST_HOST,
        .io_mode = io_mode,
        .speed = speed,
        .cs_id = TEST_CS,
        .cs_io_num = TEST_CS_PIN,
        .input_delay_ns = TEST_INPUT_DELAY,
    };
    esp_err_t err = spi_bus_add_flash_device(&test_chip, &dev_cfg);
    TEST_ESP_OK(err);
    err = esp_flash_init(test_chip);
    TEST_ESP_OK(err);
}

void teardown_test_chip()
{
    spi_bus_remove_flash_device(test_chip);
    test_chip = NULL;
    release_bus(TEST_HOST);
}

#endif

static void test_metadata(esp_flash_t *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t id, size;
    TEST_ESP_OK(esp_flash_read_id(chip, &id));
    TEST_ESP_OK(esp_flash_get_size(chip, &size));
    printf("Flash ID %08x detected size %d bytes\n", id, size);
}

TEST_CASE("SPI flash metadata functions", "[esp_flash]")
{
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_metadata(test_chip);
    teardown_test_chip();
#endif
    test_metadata(NULL);
}

static uint32_t erase_test_region(esp_flash_t *chip, int num_sectors)
{
    const esp_partition_t *part = get_test_data_partition();
    uint32_t offs = part->address;

    /* chip should be initialised */
    TEST_ASSERT(esp_flash_default_chip != NULL
                && esp_flash_chip_driver_initialized(esp_flash_default_chip));

    TEST_ASSERT(num_sectors * 4096 <= part->size);

    bzero(sector_buf, sizeof(sector_buf));

    printf("Erase @ 0x%x...\n", offs);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, esp_flash_erase_region(chip, offs, num_sectors * 4096) );

    printf("Verify erased...\n");
    for (int i = 0; i < num_sectors; i++) {
        TEST_ASSERT_EQUAL_HEX32(ESP_OK, esp_flash_read(chip, sector_buf, offs + i * 4096, sizeof(sector_buf)));

        printf("Buffer starts 0x%02x 0x%02x 0x%02x 0x%02x\n", sector_buf[0], sector_buf[1], sector_buf[2], sector_buf[3]);
        for (int i = 0; i < sizeof(sector_buf); i++) {
            TEST_ASSERT_EQUAL_HEX8(0xFF, sector_buf[i]);
        }
    }

    return offs;
}

void test_simple_read_write(void *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t offs = erase_test_region(chip, 1);

    const int test_seed = 778;
    srand(test_seed);
    for (int i = 0 ; i < sizeof(sector_buf); i++) {
        sector_buf[i] = rand();
    }

    printf("Write %p...\n", (void *)offs);
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, sector_buf, offs, sizeof(sector_buf)) );

    bzero(sector_buf, sizeof(sector_buf));

    printf("Read back...\n");
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, sector_buf, offs, sizeof(sector_buf)) );

    printf("Buffer starts 0x%02x 0x%02x 0x%02x 0x%02x\n", sector_buf[0], sector_buf[1], sector_buf[2], sector_buf[3]);

    srand(test_seed);
    for (int i = 0; i < sizeof(sector_buf); i++) {
        TEST_ASSERT_EQUAL_HEX8(rand() & 0xFF, sector_buf[i]);
    }
}

TEST_CASE("SPI flash simple read/write", "[esp_flash]")
{
    test_simple_read_write(NULL);
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_simple_read_write(test_chip);
    teardown_test_chip();
#endif
}

void test_unaligned_read_write(void *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t offs = erase_test_region(chip, 2);

    const char *msg = "i am a message";
    TEST_ASSERT(strlen(msg) + 1 % 4 != 0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, msg, offs + 1, strlen(msg) + 1) );

    char buf[strlen(msg) + 1];

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, buf, offs + 1, strlen(msg) + 1) );
    TEST_ASSERT_EQUAL_STRING_LEN(msg, buf, strlen(msg));
    TEST_ASSERT(memcmp(buf, msg, strlen(msg) + 1) == 0);
}

TEST_CASE("SPI flash unaligned read/write", "[esp_flash]")
{
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_unaligned_read_write(test_chip);
    teardown_test_chip();
#endif
    test_unaligned_read_write(NULL);
}

void test_single_read_write(void *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t offs = erase_test_region(chip, 2);

    for (unsigned v = 0; v < 512; v++) {
        TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_flash_write(chip, &v, offs + v, 1) );
    }

    for (unsigned v = 0; v < 512; v++) {
        uint8_t readback;
        TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_flash_read(chip, &readback, offs + v, 1) );
        TEST_ASSERT_EQUAL_HEX8(v, readback);
    }
}

TEST_CASE("SPI flash single byte reads/writes", "[esp_flash]")
{
    test_single_read_write(NULL);
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_single_read_write(test_chip);
    teardown_test_chip();
#endif
}


/* this test is notable because it generates a lot of unaligned reads/writes,
   and also reads/writes across both a sector boundary & many page boundaries.
*/
void test_three_byte_read_write(void *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t offs = erase_test_region(chip, 2);
    ets_printf("offs:%X\n", offs);

    for (uint32_t v = 0; v < 2000; v++) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, &v, offs + 3 * v, 3) );
    }

    for (uint32_t v = 0; v < 2000; v++) {
        uint32_t readback;
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, offs + 3 * v, 3) );
        TEST_ASSERT_EQUAL_HEX32(v & 0xFFFFFF, readback & 0xFFFFFF);
    }
}

TEST_CASE("SPI flash three byte reads/writes", "[esp_flash]")
{
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_three_byte_read_write(test_chip);
    teardown_test_chip();
#endif
    test_three_byte_read_write(NULL);
}

void test_erase_large_region(esp_flash_t *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);

    const esp_partition_t *part = get_test_data_partition();

    /* Write some noise at the start and the end of the region */
    const char *ohai = "OHAI";
    uint32_t readback;
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, ohai, part->address, 5));
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, ohai, part->address + part->size - 5, 5));

    /* sanity check what we just wrote. since the partition may haven't been erased, we only check the part which is written to 0. */
    uint32_t written_data = *((const uint32_t *)ohai);
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, part->address + part->size - 5, 4));
    TEST_ASSERT_EQUAL_HEX32(0, readback & (~written_data));
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, part->address, 4));
    TEST_ASSERT_EQUAL_HEX32(0, readback & (~written_data));

    /* Erase whole region */
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_erase_region(chip, part->address, part->size));

    /* ensure both areas we wrote are now all-FFs */
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, part->address, 4));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, readback);

    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, part->address + part->size - 5, 4));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, readback);
}

TEST_CASE("SPI flash erase large region", "[esp_flash]")
{
    test_erase_large_region(NULL);
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_erase_large_region(test_chip);
    teardown_test_chip();
#endif
}

static void test_write_protection(esp_flash_t* chip)
{
    bool wp = true;
    esp_err_t ret = ESP_OK;
    ret = esp_flash_get_chip_write_protect(chip, &wp);
    TEST_ESP_OK(ret);

    for (int i = 0; i < 4; i ++) {
        bool wp_write = !wp;
        ret = esp_flash_set_chip_write_protect(chip, wp_write);
        TEST_ESP_OK(ret);

        bool wp_read;
        ret = esp_flash_get_chip_write_protect(chip, &wp_read);
        TEST_ESP_OK(ret);
        TEST_ASSERT(wp_read == wp_write);
        wp = wp_read;
    }
}

TEST_CASE("Test esp_flash can enable/disable write protetion", "[esp_flash]")
{
    test_write_protection(NULL);
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_write_protection(test_chip);
    teardown_test_chip();
#endif
}

static const uint8_t large_const_buffer[16400] = {
    203, // first byte
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    [50 ... 99] = 2,
    [1600 ... 2000] = 3,
    [8000 ... 9000] = 77,
    [15000 ... 16398] = 8,
    43 // last byte
};

static void test_write_large_buffer(esp_flash_t *chip, const uint8_t *source, size_t length);
static void write_large_buffer(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length);
static void read_and_check(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length);

TEST_CASE("SPI flash test reading with all speed/mode permutations", "[esp_flash]")
{
    const int length = sizeof(large_const_buffer);
    uint8_t *source_buf = malloc(length);
    TEST_ASSERT_NOT_NULL(source_buf);
    srand(778);
    for (int i = 0; i < length; i++) {
        source_buf[i] = rand();
    }

    const esp_partition_t *part = get_test_data_partition();
    TEST_ASSERT(part->size > length + 2 + SPI_FLASH_SEC_SIZE);

#ifndef SKIP_EXTENDED_CHIP_TEST
    //use the lowest speed to write and read to make sure success
    setup_new_chip(TEST_SPI_READ_MODE, ESP_FLASH_SPEED_MIN);
    write_large_buffer(test_chip, part, source_buf, length);
    read_and_check(test_chip, part, source_buf, length);
    teardown_test_chip();

    esp_flash_read_mode_t io_mode = SPI_FLASH_READ_MODE_MIN;
    while (io_mode != SPI_FLASH_READ_MODE_MAX) {
        esp_flash_speed_t speed = ESP_FLASH_SPEED_MIN;
        while (speed != ESP_FLASH_SPEED_MAX) {
            ESP_LOGI(TAG, "test flash io mode: %d, speed: %d", io_mode, speed);
            setup_new_chip(io_mode, speed);
            read_and_check(test_chip, part, source_buf, length);
            teardown_test_chip();
            speed++;
        }
        io_mode++;
    }
#endif

    //test main flash BTW
    write_large_buffer(NULL, part, source_buf, length);
    read_and_check(NULL, part, source_buf, length);

    free(source_buf);
}

TEST_CASE("Test esp_flash_write large const buffer", "[esp_flash]")
{
    //buffer in flash
    test_write_large_buffer(NULL, large_const_buffer, sizeof(large_const_buffer));
#ifndef SKIP_EXTENDED_CHIP_TEST
    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_write_large_buffer(test_chip, large_const_buffer, sizeof(large_const_buffer));
    teardown_test_chip();
#endif
}

#ifndef SKIP_EXTENDED_CHIP_TEST
TEST_CASE("Test esp_flash_write large RAM buffer", "[esp_flash]")
{
    // buffer in RAM
    uint8_t *source_buf = malloc(sizeof(large_const_buffer));
    TEST_ASSERT_NOT_NULL(source_buf);
    memcpy(source_buf, large_const_buffer, sizeof(large_const_buffer));

    setup_new_chip(TEST_SPI_READ_MODE, TEST_SPI_SPEED);
    test_write_large_buffer(test_chip, source_buf, sizeof(large_const_buffer));
    teardown_test_chip();

    free(source_buf);
}
#endif

static void write_large_buffer(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length)
{
    printf("Writing chip %p, %d bytes from source %p\n", chip, length, source);

    ESP_ERROR_CHECK( esp_flash_erase_region(chip, part->address, (length + SPI_FLASH_SEC_SIZE) & ~(SPI_FLASH_SEC_SIZE - 1)) );

    // note writing to unaligned address
    ESP_ERROR_CHECK( esp_flash_write(chip, source, part->address + 1, length) );
}

static void read_and_check(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length)
{
    printf("Checking chip %p, %d bytes\n", chip, length);
    uint8_t *buf = malloc(length);
    TEST_ASSERT_NOT_NULL(buf);
    ESP_ERROR_CHECK( esp_flash_read(chip, buf, part->address + 1, length) );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(source, buf, length);
    free(buf);

    // check nothing was written at beginning or end
    uint8_t ends[8];

    ESP_ERROR_CHECK( esp_flash_read(chip, ends, part->address, sizeof(ends)) );
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[0]);
    TEST_ASSERT_EQUAL_HEX8(source[0], ends[1]);

    ESP_ERROR_CHECK( esp_flash_read(chip, ends, part->address + length, sizeof(ends)) );

    TEST_ASSERT_EQUAL_HEX8(source[length - 1], ends[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[2]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[3]);
}

static void test_write_large_buffer(esp_flash_t *chip, const uint8_t *source, size_t length)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    const esp_partition_t *part = get_test_data_partition();
    TEST_ASSERT(part->size > length + 2 + SPI_FLASH_SEC_SIZE);

    write_large_buffer(chip, part, source, length);
    read_and_check(chip, part, source, length);
}