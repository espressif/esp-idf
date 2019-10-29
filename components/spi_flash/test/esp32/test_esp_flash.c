#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_flash.h"
#include "driver/spi_common_internal.h"
#include "esp_flash_spi_init.h"
#include <esp_attr.h>
#include "esp_log.h"

#include <test_utils.h>

#include "unity.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

#define FUNC_SPI    1

static uint8_t sector_buf[4096];

#define TEST_SPI_SPEED      ESP_FLASH_10MHZ
#define TEST_SPI_READ_MODE  SPI_FLASH_FASTRD
//#define FORCE_GPIO_MATRIX

#define EXTRA_SPI1_CLK_IO   17  //the pin which is usually used by the PSRAM clk

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

#define ALL_TEST_NUM (sizeof(config_list)/sizeof(flashtest_config_t))
typedef void (*flash_test_func_t)(esp_flash_t* chip);

#define FLASH_TEST_CASE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash]") {flash_test_func(FUNC_TO_RUN, 1);}
#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
// These tests needs external flash, right on the place of psram
#define FLASH_TEST_CASE_3(STR, FUNCT_TO_RUN)
#else
#define FLASH_TEST_CASE_3(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", 3 chips", "[esp_flash][test_env=UT_T1_ESP_FLASH]") {flash_test_func(FUNC_TO_RUN, ALL_TEST_NUM);}
#endif

//currently all the configs are the same with esp_flash_spi_device_config_t, no more information required
typedef esp_flash_spi_device_config_t flashtest_config_t;

static const char TAG[] = "test_esp_flash";

flashtest_config_t config_list[] = {
    // 0 always reserved for main flash
    {
        .host_id = -1, // no need to init
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .speed = TEST_SPI_SPEED,
        .host_id = SPI_HOST,
        .cs_id = 1,
        .cs_io_num = 16, //the pin which is usually used by the PSRAM
        .input_delay_ns = 0,
    },
    /* current runner doesn't have a flash on HSPI
    {
        .io_mode = TEST_SPI_READ_MODE,
        .speed = TEST_SPI_SPEED,
        .host = HSPI_HOST,
        .cs_id = 0,
        .cs_io_num = HSPI_IOMUX_PIN_NUM_CS,
        .input_delay_ns = 20,
    },
    */
    {
        .io_mode = TEST_SPI_READ_MODE,
        .speed = TEST_SPI_SPEED,
        .host_id = VSPI_HOST,
        .cs_id = 0,
        .cs_io_num = VSPI_IOMUX_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};

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

static void setup_new_chip(const flashtest_config_t* test_cfg, esp_flash_t** out_chip)
{
    //the bus should be initialized before the flash is attached to the bus
    if (test_cfg->host_id == -1) {
        *out_chip = NULL;
        return;
    }
    setup_bus(test_cfg->host_id);

    esp_flash_spi_device_config_t dev_cfg = {
        .host_id = test_cfg->host_id,
        .io_mode = test_cfg->io_mode,
        .speed = test_cfg->speed,
        .cs_id = test_cfg->cs_id,
        .cs_io_num = test_cfg->cs_io_num,
        .input_delay_ns = test_cfg->input_delay_ns,
    };
    esp_flash_t* init_chip;
    esp_err_t err = spi_bus_add_flash_device(&init_chip, &dev_cfg);
    TEST_ESP_OK(err);
    err = esp_flash_init(init_chip);
    TEST_ESP_OK(err);
    *out_chip = init_chip;
}

void teardown_test_chip(esp_flash_t* chip, spi_host_device_t host)
{
    //happen to work when chip==NULL
    spi_bus_remove_flash_device(chip);
    release_bus(host);
}

static void flash_test_func(flash_test_func_t func, int test_num)
{
    for (int i = 0; i < test_num; i++) {
        flashtest_config_t* config = &config_list[i];
        esp_flash_t* chip;
        setup_new_chip(config, &chip);
        (*func)(chip);
        teardown_test_chip(chip, config->host_id);
    }
}

/* ---------- Test code start ------------*/

static void test_metadata(esp_flash_t *chip)
{
    ESP_LOGI(TAG, "Testing chip %p...", chip);
    uint32_t id, size;
    TEST_ESP_OK(esp_flash_read_id(chip, &id));
    TEST_ESP_OK(esp_flash_get_size(chip, &size));
    printf("Flash ID %08x detected size %d bytes\n", id, size);
}

FLASH_TEST_CASE("SPI flash metadata functions", test_metadata);
FLASH_TEST_CASE_3("SPI flash metadata functions", test_metadata);

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

void test_simple_read_write(esp_flash_t *chip)
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

FLASH_TEST_CASE("SPI flash simple read/write", test_simple_read_write);
FLASH_TEST_CASE_3("SPI flash simple read/write", test_simple_read_write);

void test_unaligned_read_write(esp_flash_t *chip)
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

FLASH_TEST_CASE("SPI flash unaligned read/write", test_unaligned_read_write);
FLASH_TEST_CASE_3("SPI flash unaligned read/write", test_unaligned_read_write);

void test_single_read_write(esp_flash_t* chip)
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

FLASH_TEST_CASE("SPI flash single byte reads/writes", test_single_read_write);
FLASH_TEST_CASE_3("SPI flash single byte reads/writes", test_single_read_write);


/* this test is notable because it generates a lot of unaligned reads/writes,
   and also reads/writes across both a sector boundary & many page boundaries.
*/
void test_three_byte_read_write(esp_flash_t *chip)
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

FLASH_TEST_CASE("SPI flash three byte reads/writes", test_three_byte_read_write);
FLASH_TEST_CASE_3("SPI flash three byte reads/writes", test_three_byte_read_write);

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

FLASH_TEST_CASE("SPI flash erase large region", test_erase_large_region);
FLASH_TEST_CASE_3("SPI flash erase large region", test_erase_large_region);

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

FLASH_TEST_CASE("Test esp_flash can enable/disable write protetion", test_write_protection);
FLASH_TEST_CASE_3("Test esp_flash can enable/disable write protetion", test_write_protection);

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

// Internal functions for testing, from esp_flash_api.c
esp_err_t esp_flash_set_io_mode(esp_flash_t* chip, bool qe);
esp_err_t esp_flash_get_io_mode(esp_flash_t* chip, bool* qe);
esp_err_t esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* flash_id);

static bool check_winbond_chip(esp_flash_t* chip)
{
    uint32_t flash_id;
    esp_err_t ret = esp_flash_read_chip_id(chip, &flash_id);
    TEST_ESP_OK(ret);
    if ((flash_id >> 16) == 0xEF) {
        return true;
    } else {
        return false;
    }
}

static void test_toggle_qe(esp_flash_t* chip)
{
    bool qe;
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    }
    esp_flash_io_mode_t io_mode_before = chip->read_mode;
    esp_err_t ret = esp_flash_get_io_mode(chip, &qe);
    TEST_ESP_OK(ret);

    bool is_winbond_chip = check_winbond_chip(chip);

    for (int i = 0; i < 4; i ++) {
        ESP_LOGI(TAG, "write qe: %d->%d", qe, !qe);
        qe = !qe;
        chip->read_mode = qe? SPI_FLASH_QOUT: SPI_FLASH_SLOWRD;
        ret = esp_flash_set_io_mode(chip, qe);
        if (is_winbond_chip && !qe && ret == ESP_ERR_FLASH_NO_RESPONSE) {
            //allows clear qe failure for Winbond chips
            ret = ESP_OK;
        }
        TEST_ESP_OK(ret);

        bool qe_read;
        ret = esp_flash_get_io_mode(chip, &qe_read);
        TEST_ESP_OK(ret);
        ESP_LOGD(TAG, "qe read: %d", qe_read);
        if (qe != qe_read && !qe && is_winbond_chip) {
            ESP_LOGE(TAG, "cannot clear QE bit, this may be normal for Winbond chips.");
            chip->read_mode = io_mode_before;
            return;
        }
        TEST_ASSERT_EQUAL(qe, qe_read);
    }
    //restore the io_mode after test
    chip->read_mode = io_mode_before;
}

FLASH_TEST_CASE("Test esp_flash_write can toggle QE bit", test_toggle_qe);
FLASH_TEST_CASE_3("Test esp_flash_write can toggle QE bit", test_toggle_qe);


void test_permutations(flashtest_config_t* config)
{
    //replace config pointer with pointer to internal temporary config
    flashtest_config_t temp_cfg;
    memcpy(&temp_cfg, config, sizeof(flashtest_config_t));
    flashtest_config_t* cfg = &temp_cfg;
    esp_flash_t* chip;

    const int length = sizeof(large_const_buffer);
    uint8_t *source_buf = malloc(length);
    TEST_ASSERT_NOT_NULL(source_buf);
    srand(778);
    for (int i = 0; i < length; i++) {
        source_buf[i] = rand();
    }

    const esp_partition_t *part = get_test_data_partition();
    TEST_ASSERT(part->size > length + 2 + SPI_FLASH_SEC_SIZE);

    //write data to be read, and use the lowest speed to write and read to make sure success
    cfg->io_mode = SPI_FLASH_READ_MODE_MIN;
    cfg->speed = ESP_FLASH_SPEED_MIN;
    setup_new_chip(cfg, &chip);
    write_large_buffer(chip, part, source_buf, length);
    read_and_check(chip, part, source_buf, length);
    teardown_test_chip(chip, cfg->host_id);


    if (config->host_id != -1) {
        esp_flash_speed_t speed = ESP_FLASH_SPEED_MIN;
        while (speed != ESP_FLASH_SPEED_MAX) {
            //test io_mode in the inner loop to test QE set/clear function, since
            //the io mode will switch frequently.
            esp_flash_io_mode_t io_mode = SPI_FLASH_READ_MODE_MIN;
            while (io_mode != SPI_FLASH_READ_MODE_MAX) {
                ESP_LOGI(TAG, "test flash io mode: %d, speed: %d", io_mode, speed);
                cfg->io_mode = io_mode;
                cfg->speed = speed;
                setup_new_chip(cfg, &chip);
                read_and_check(chip, part, source_buf, length);
                teardown_test_chip(chip, cfg->host_id);
                io_mode++;
            }
            speed++;
        }
    } else {
        //test main flash
        write_large_buffer(NULL, part, source_buf, length);
        read_and_check(NULL, part, source_buf, length);
    }

    free(source_buf);
}

TEST_CASE("SPI flash test reading with all speed/mode permutations", "[esp_flash]")
{
    test_permutations(&config_list[0]);
}

#ifndef CONFIG_ESP32_SPIRAM_SUPPORT
TEST_CASE("SPI flash test reading with all speed/mode permutations, 3 chips", "[esp_flash][test_env=UT_T1_ESP_FLASH]")
{
    for (int i = 0; i < ALL_TEST_NUM; i++) {
        test_permutations(&config_list[i]);
    }
}
#endif

static void test_write_large_const_buffer(esp_flash_t* chip)
{
    test_write_large_buffer(chip, large_const_buffer, sizeof(large_const_buffer));
}

FLASH_TEST_CASE("Test esp_flash_write large const buffer", test_write_large_const_buffer);
FLASH_TEST_CASE_3("Test esp_flash_write large const buffer", test_write_large_const_buffer);

static void test_write_large_ram_buffer(esp_flash_t* chip)
{
    // buffer in RAM
    uint8_t *source_buf = malloc(sizeof(large_const_buffer));
    TEST_ASSERT_NOT_NULL(source_buf);
    memcpy(source_buf, large_const_buffer, sizeof(large_const_buffer));
    test_write_large_buffer(chip, source_buf, sizeof(large_const_buffer));
    free(source_buf);
}

FLASH_TEST_CASE("Test esp_flash_write large RAM buffer", test_write_large_ram_buffer);
FLASH_TEST_CASE_3("Test esp_flash_write large RAM buffer", test_write_large_ram_buffer);

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