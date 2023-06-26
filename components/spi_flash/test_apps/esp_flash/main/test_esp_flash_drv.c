/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_flash.h"
#include "esp_private/spi_common_internal.h"
#include "esp_flash_spi_init.h"
#include "memspi_host_driver.h"
#include "spi_flash_mmap.h"
#include <esp_attr.h>
#include "esp_log.h"
#include "test_utils.h"
#include "unity.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

#include "esp_partition.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "test_esp_flash_def.h"
#include "spi_flash_mmap.h"
#include "esp_private/spi_flash_os.h"
#include "ccomp_timer.h"

#define FUNC_SPI    1

static uint8_t sector_buf[4096];

static void get_chip_host(esp_flash_t* chip, spi_host_device_t* out_host_id, int* out_cs_id)
{
    spi_host_device_t host_id;
    int cs_id;
    if (chip == NULL) {
        host_id = SPI1_HOST;
        cs_id = 0;
    } else {
        spi_flash_hal_context_t* host_data = (spi_flash_hal_context_t*)chip->host;
        host_id = spi_flash_ll_hw_get_id(host_data->spi);
        cs_id = host_data->cs_num;
    }
    if (out_host_id) {
        *out_host_id = host_id;
    }
    if (out_cs_id) {
        *out_cs_id = cs_id;
    }
}

#if CONFIG_IDF_TARGET_ESP32
static void setup_bus(spi_host_device_t host_id)
{
    if (host_id == SPI1_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...");
        //no need to initialize the bus, however the CLK may need one more output if it's on the usual place of PSRAM
        esp_rom_gpio_connect_out_signal(EXTRA_SPI1_CLK_IO, SPICLK_OUT_IDX, 0, 0);
        //currently the SPI bus for main flash chip is initialized through GPIO matrix
    } else if (host_id == SPI2_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%u (HSPI) CS0...", host_id + 1);
        spi_bus_config_t hspi_bus_cfg = {
            .mosi_io_num = HSPI_PIN_NUM_MOSI,
            .miso_io_num = HSPI_PIN_NUM_MISO,
            .sclk_io_num = HSPI_PIN_NUM_CLK,
            .quadhd_io_num = HSPI_PIN_NUM_HD,
            .quadwp_io_num = HSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
        esp_err_t ret = spi_bus_initialize(host_id, &hspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
    } else if (host_id == SPI3_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%u (VSPI) CS0...", host_id + 1);
        spi_bus_config_t vspi_bus_cfg = {
            .mosi_io_num = VSPI_PIN_NUM_MOSI,
            .miso_io_num = VSPI_PIN_NUM_MISO,
            .sclk_io_num = VSPI_PIN_NUM_CLK,
            .quadhd_io_num = VSPI_PIN_NUM_HD,
            .quadwp_io_num = VSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
        esp_err_t ret = spi_bus_initialize(host_id, &vspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
    } else {
        ESP_LOGE(TAG, "invalid bus");
    }
}
#else // FOR ESP32-S2, ESP32-S3, ESP32-C3
static void setup_bus(spi_host_device_t host_id)
{
    if (host_id == SPI1_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...");
#if !CONFIG_ESPTOOLPY_FLASHMODE_QIO && !CONFIG_ESPTOOLPY_FLASHMODE_QOUT
        //Initialize the WP and HD pins, which are not automatically initialized on ESP32-S2.
        int wp_pin = spi_periph_signal[host_id].spiwp_iomux_pin;
        int hd_pin = spi_periph_signal[host_id].spihd_iomux_pin;
        gpio_iomux_in(wp_pin, spi_periph_signal[host_id].spiwp_in);
        gpio_iomux_out(wp_pin, spi_periph_signal[host_id].func, false);
        gpio_iomux_in(hd_pin, spi_periph_signal[host_id].spihd_in);
        gpio_iomux_out(hd_pin, spi_periph_signal[host_id].func, false);
#endif //CONFIG_ESPTOOLPY_FLASHMODE_QIO || CONFIG_ESPTOOLPY_FLASHMODE_QOUT
        //currently the SPI bus for main flash chip is initialized through GPIO matrix
    } else if (host_id == SPI2_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%u (FSPI) CS0...", host_id + 1);
        spi_bus_config_t fspi_bus_cfg = {
            .mosi_io_num = FSPI_PIN_NUM_MOSI,
            .miso_io_num = FSPI_PIN_NUM_MISO,
            .sclk_io_num = FSPI_PIN_NUM_CLK,
            .quadhd_io_num = FSPI_PIN_NUM_HD,
            .quadwp_io_num = FSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
        esp_err_t ret = spi_bus_initialize(host_id, &fspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
    }
#if SOC_SPI_PERIPH_NUM > 2
    else if (host_id == SPI3_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%u (HSPI) CS0...", host_id + 1);
        spi_bus_config_t hspi_bus_cfg = {
            .mosi_io_num = HSPI_PIN_NUM_MOSI,
            .miso_io_num = HSPI_PIN_NUM_MISO,
            .sclk_io_num = HSPI_PIN_NUM_CLK,
            .quadhd_io_num = HSPI_PIN_NUM_HD,
            .quadwp_io_num = HSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
        esp_err_t ret = spi_bus_initialize(host_id, &hspi_bus_cfg, 0);
        TEST_ESP_OK(ret);

        // HSPI have no multiline mode, use GPIO to pull those pins up
        gpio_set_direction(HSPI_PIN_NUM_HD, GPIO_MODE_OUTPUT);
        gpio_set_level(HSPI_PIN_NUM_HD, 1);

        gpio_set_direction(HSPI_PIN_NUM_WP, GPIO_MODE_OUTPUT);
        gpio_set_level(HSPI_PIN_NUM_WP, 1);
    }
#endif
    else {
        ESP_LOGE(TAG, "invalid bus");
    }
}
#endif // CONFIG_IDF_TARGET_ESP32

static void release_bus(int host_id)
{
    //SPI1 bus can't be deinitialized
#if SOC_SPI_PERIPH_NUM > 2
    if (host_id == SPI2_HOST || host_id == SPI3_HOST)
#else
    if (host_id == SPI2_HOST)
#endif
    {
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
        .freq_mhz = test_cfg->freq_mhz,
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

static void teardown_test_chip(esp_flash_t* chip)
{
    spi_host_device_t host_id;
    get_chip_host(chip, &host_id, NULL);
    //happen to work when chip==NULL
    spi_bus_remove_flash_device(chip);
    release_bus(host_id);
}

static void flash_test_core(flash_test_func_t func, const flashtest_config_t* config)
{
    esp_flash_t* chip;
    setup_new_chip(config, &chip);

    uint32_t size;
    esp_err_t err = esp_flash_get_size(chip, &size);
    TEST_ESP_OK(err);
    ESP_LOGI(TAG, "Flash size: 0x%08lX", size);

    const esp_partition_t* test_part = get_test_data_partition();
    TEST_ASSERT_NOT_EQUAL(NULL, test_part->flash_chip);

    esp_partition_t part = *test_part;
    part.flash_chip = chip;

    ESP_LOGI(TAG, "Testing chip %p, address 0x%08lX...", part.flash_chip, part.address);
    (*func)(&part);

    // For flash with size over 16MB, add one extra round of test for the 32-bit address area
    if (size > MAX_ADDR_24BIT) {
        part.address = 0x1030000;
        part.size = 0x0010000;
        ESP_LOGI(TAG, "Testing chip %p, address 0x%08lX...", part.flash_chip, part.address);
        (*func)(&part);
    }

    teardown_test_chip(chip);
}

static void flash_test_func(flash_test_func_t func, int test_num)
{
    esp_log_level_set("gpio", ESP_LOG_NONE);
    for (int i = 0; i < test_num; i++) {
        ESP_LOGI(TAG, "Testing config %u/%u", i+1, test_num);
        flash_test_core(func, &config_list[i]);
    }
    ESP_LOGI(TAG, "Completed %u configs", test_num);
}

/* ---------- Test code start ------------*/

static void test_metadata(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t id, size, actual_size;
    TEST_ESP_OK(esp_flash_read_id(chip, &id));
    TEST_ESP_OK(esp_flash_get_size(chip, &size));
    TEST_ESP_OK(esp_flash_get_physical_size(chip, &actual_size));
    printf("Flash ID %08lx, size %" PRIu32 "bytes can be mapped, actual flash size is %" PRIu32 "bytes\n", id, size, actual_size);
}

TEST_CASE_FLASH("SPI flash metadata functions", test_metadata);
TEST_CASE_MULTI_FLASH("SPI flash metadata functions", test_metadata);

static uint32_t erase_test_region(const esp_partition_t *part, int num_sectors)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = part->address;

    /* chip should be initialised */
    TEST_ASSERT(esp_flash_default_chip != NULL
                && esp_flash_chip_driver_initialized(esp_flash_default_chip));

    TEST_ASSERT(num_sectors * 4096 <= part->size);

    bzero(sector_buf, sizeof(sector_buf));

    printf("Erase @ 0x%lx...\n", offs);
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

void test_simple_read_write(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = erase_test_region(part, 1);

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
        uint8_t data = rand();
        TEST_ASSERT_EQUAL_HEX8(data, sector_buf[i]);
    }
}

TEST_CASE_FLASH("SPI flash simple read/write", test_simple_read_write);
TEST_CASE_MULTI_FLASH("SPI flash simple read/write", test_simple_read_write);

void test_unaligned_read_write(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = erase_test_region(part, 2);

    const char *msg = "i am a message";
    TEST_ASSERT(strlen(msg) + 1 % 4 != 0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, msg, offs + 1, strlen(msg) + 1) );

    char buf[strlen(msg) + 1];

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, buf, offs + 1, strlen(msg) + 1) );
    TEST_ASSERT_EQUAL_STRING_LEN(msg, buf, strlen(msg));
    TEST_ASSERT(memcmp(buf, msg, strlen(msg) + 1) == 0);
}

TEST_CASE_FLASH("SPI flash unaligned read/write", test_unaligned_read_write);
TEST_CASE_MULTI_FLASH("SPI flash unaligned read/write", test_unaligned_read_write);

void test_single_read_write(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = erase_test_region(part, 2);

    const int seed = 699;
    srand(seed);
    for (unsigned v = 0; v < 512; v++) {
        uint32_t data = rand();
        TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_flash_write(chip, &data, offs + v, 1) );
    }

    srand(seed);
    for (unsigned v = 0; v < 512; v++) {
        uint8_t readback;
        uint32_t data = rand();
        TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_flash_read(chip, &readback, offs + v, 1) );
        TEST_ASSERT_EQUAL_HEX8(data, readback);
    }
}

TEST_CASE_FLASH("SPI flash single byte reads/writes", test_single_read_write);
TEST_CASE_MULTI_FLASH("SPI flash single byte reads/writes", test_single_read_write);


/* this test is notable because it generates a lot of unaligned reads/writes,
   and also reads/writes across both a sector boundary & many page boundaries.
*/
void test_three_byte_read_write(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = erase_test_region(part, 2);

    const int seed = 700;
    esp_rom_printf("offs:%X\n", offs);

    srand(seed);
    for (uint32_t v = 0; v < 86; v++) {
        uint32_t data = rand();
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, &data, offs + 3 * v, 3) );
    }

    srand(seed);
    for (uint32_t v = 0; v < 1; v++) {
        uint32_t readback;
        uint32_t data = rand();
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, &readback, offs + 3 * v, 3) );
        TEST_ASSERT_EQUAL_HEX32(data & 0xFFFFFF, readback & 0xFFFFFF);
    }
}

TEST_CASE_FLASH("SPI flash three byte reads/writes", test_three_byte_read_write);
TEST_CASE_MULTI_FLASH("SPI flash three byte reads/writes", test_three_byte_read_write);

void test_erase_large_region(const esp_partition_t *part)
{
    esp_flash_t* chip = part->flash_chip;
    erase_test_region(part, 2);

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

    /* Erase zero bytes, check that nothing got erased */
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_erase_region(chip, part->address, 0));
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

TEST_CASE_FLASH("SPI flash erase large region", test_erase_large_region);
TEST_CASE_MULTI_FLASH("SPI flash erase large region", test_erase_large_region);

static void test_flash_erase_not_trigger_wdt(const esp_partition_t *part)
{
    spi_host_device_t out_host_id;
    get_chip_host(part->flash_chip, &out_host_id, NULL);
    // don't erase main flash (spi1), test only spi2 & spi3
    if (out_host_id != SPI1_HOST) {
        ESP_ERROR_CHECK(esp_flash_erase_chip(part->flash_chip));
    } else {
        ESP_LOGI(TAG, "spi1 skipped");
    }
}

TEST_CASE_MULTI_FLASH_LONG("Test erasing flash chip not triggering WDT", test_flash_erase_not_trigger_wdt);

static void test_write_protection(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;

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

TEST_CASE_FLASH("Test esp_flash can enable/disable write protetion", test_write_protection);
TEST_CASE_MULTI_FLASH("Test esp_flash can enable/disable write protetion", test_write_protection);

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO && SOC_SPI_MEM_SUPPORT_WRAP
// Only under QIO mode and mspi support wrap this feature makes sense.
static uint8_t wrap_buf[32];

void test_flash_wrap(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = erase_test_region(part, 1);

    const int test_seed = 778;
    srand(test_seed);
    for (int i = 0 ; i < sizeof(wrap_buf); i++) {
        wrap_buf[i] = rand();
    }
    printf("Write %p...\n", (void *)offs);
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, wrap_buf, offs + 3, sizeof(wrap_buf)) );

    bzero(wrap_buf, sizeof(wrap_buf));

    printf("Read back...\n");
    spi_flash_wrap_probe();
    spi_flash_wrap_enable(FLASH_WRAP_SIZE_32B);
    esp_flash_read(chip, wrap_buf, offs + 3, sizeof(wrap_buf));
    spi_flash_wrap_disable();

    printf("Buffer starts 0x%02x 0x%02x 0x%02x 0x%02x\n", wrap_buf[0], wrap_buf[1], wrap_buf[2], wrap_buf[3]);

    srand(test_seed);
    for (int i = 0; i < sizeof(wrap_buf) - 3; i++) {
        uint8_t data = rand();
        TEST_ASSERT_EQUAL_HEX8(data, wrap_buf[i]);
    }
    for (int i = sizeof(wrap_buf) - 3; i < sizeof(wrap_buf); i++) {
        TEST_ASSERT_EQUAL_HEX8(0xFF, wrap_buf[i]);
    }
}

TEST_CASE_FLASH("SPI flash wrap test", test_flash_wrap);
#endif

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

static void test_write_large_buffer(const esp_partition_t *part, const uint8_t *source, size_t length);
static void write_large_buffer(const esp_partition_t *part, const uint8_t *source, size_t length);
static void read_and_check(const esp_partition_t *part, const uint8_t *source, size_t length);

// Internal functions for testing, from esp_flash_api.c
#if !CONFIG_ESPTOOLPY_OCT_FLASH
esp_err_t esp_flash_set_io_mode(esp_flash_t* chip, bool qe);
esp_err_t esp_flash_get_io_mode(esp_flash_t* chip, bool* qe);
esp_err_t esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* flash_id);
esp_err_t spi_flash_chip_mxic_probe(esp_flash_t *chip, uint32_t flash_id);

static bool is_winbond_chip(esp_flash_t* chip)
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

static bool is_mxic_chip(esp_flash_t* chip)
{
    uint32_t flash_id;
    esp_err_t ret = esp_flash_read_chip_id(chip, &flash_id);
    TEST_ESP_OK(ret);
    return (spi_flash_chip_mxic_probe(chip, flash_id)==ESP_OK);
}

IRAM_ATTR NOINLINE_ATTR static void test_toggle_qe(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;

    bool qe;
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    }
    esp_flash_io_mode_t io_mode_before = chip->read_mode;
    esp_err_t ret = esp_flash_get_io_mode(chip, &qe);
    TEST_ESP_OK(ret);

    bool allow_failure = is_winbond_chip(chip) || is_mxic_chip(chip);

    for (int i = 0; i < 4; i ++) {
        esp_rom_printf(DRAM_STR("write qe: %" PRIu32 "->%" PRIu32 "\n"), qe, !qe);
        qe = !qe;
        chip->read_mode = qe? SPI_FLASH_QOUT: SPI_FLASH_SLOWRD;
        ret = esp_flash_set_io_mode(chip, qe);
        if (allow_failure && !qe && ret == ESP_ERR_FLASH_NO_RESPONSE) {
            //allows clear qe failure for Winbond chips
            ret = ESP_OK;
        }
        TEST_ESP_OK(ret);

        bool qe_read;
        ret = esp_flash_get_io_mode(chip, &qe_read);
        TEST_ESP_OK(ret);
        ESP_LOGD(TAG, "qe read: %u", qe_read);
        if (!qe && qe_read) {
            if (allow_failure) {
                ESP_LOGW(TAG, "cannot clear QE bit for known permanent QE (Winbond or MXIC) chips.");
            } else {
                ESP_LOGE(TAG, "cannot clear QE bit, please make sure force clearing QE option is enabled in `spi_flash_common_set_io_mode`, and this chip is not a permanent QE one.");
            }
            chip->read_mode = io_mode_before;
            return;
        }
        TEST_ASSERT_EQUAL(qe, qe_read);
    }
    //restore the io_mode after test
    chip->read_mode = io_mode_before;
}

// These tests show whether the QE is permanent or not for the chip tested.
// To test the behaviour of a new SPI flash chip, enable force_check flag in generic driver
// `spi_flash_common_set_io_mode` and then run this test.
TEST_CASE_FLASH_IGNORE("Test esp_flash_write can toggle QE bit", test_toggle_qe);
TEST_CASE_MULTI_FLASH_IGNORE("Test esp_flash_write can toggle QE bit", test_toggle_qe);
#endif //CONFIG_ESPTOOLPY_OCT_FLASH

// This table could be chip specific in the future.
uint8_t flash_frequency_table[6] = {5, 10, 20, 26, 40, 80};
#define TEST_FLASH_SPEED_MIN 5
void test_permutations_part(const flashtest_config_t* config, esp_partition_t* part, void* source_buf, size_t length)
{
    int clock_index = 0;
    if (config->host_id != -1) {
        while (clock_index < sizeof(flash_frequency_table)/sizeof(uint8_t)) {
            uint8_t speed = flash_frequency_table[clock_index];
            //test io_mode in the inner loop to test QE set/clear function, since
            //the io mode will switch frequently.
            esp_flash_io_mode_t io_mode = SPI_FLASH_READ_MODE_MIN;
            while (io_mode != SPI_FLASH_QIO + 1) {
                if (io_mode > SPI_FLASH_FASTRD &&
                    !SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(config->host_id)) {
                    io_mode++;
                    continue;
                }

                esp_flash_t* chip;
                flashtest_config_t temp_config = *config;
                temp_config.io_mode = io_mode;
                temp_config.freq_mhz = flash_frequency_table[clock_index];
                setup_new_chip(&temp_config, &chip);
                ESP_LOGI(TAG, "test flash io mode: %u, speed: %u", io_mode, speed);

                part->flash_chip = chip;
                read_and_check(part, source_buf, length);
                teardown_test_chip(chip);

                io_mode++;
            }
            clock_index++;
        }
    } else {
        //test main flash
        part->flash_chip = NULL;
        read_and_check(part, source_buf, length);
    }
}

void test_permutations_chip(const flashtest_config_t* config)
{
    esp_log_level_set("gpio", ESP_LOG_NONE);
    esp_flash_t* chip;
    flashtest_config_t temp_config = *config;
    // Use the lowest speed to read configs, data and write data to make sure success
    temp_config.io_mode = SPI_FLASH_READ_MODE_MIN;
    temp_config.freq_mhz = TEST_FLASH_SPEED_MIN;
    setup_new_chip(&temp_config, &chip);

    //Get size to determine whether to test one extra partition
    uint32_t size;
    esp_err_t err = esp_flash_get_size(chip, &size);
    TEST_ESP_OK(err);
    ESP_LOGI(TAG, "Flash size: 0x%08lX", size);
    bool addr_32bit = (size > MAX_ADDR_24BIT);

    // Get test partition, and locate temporary partitions according to the default one
    const esp_partition_t* test_part = get_test_data_partition();
    const int length = sizeof(large_const_buffer);
    TEST_ASSERT(test_part->size > length + 2 + SPI_FLASH_SEC_SIZE);

    esp_partition_t part[2] = {};
    part[0] = *test_part;
    part[0].flash_chip = chip;
    // For flash with size over 16MB, add one extra round of test for the 32-bit address area
    if (addr_32bit) {
        part[1] = *test_part;
        part[1].flash_chip = chip;
        part[1].address = 0x1030000;
        part[1].size = 0x0010000;
    } else {
        part[1].size = 0;
    }

    // Prepare test data and write to the specified region
    uint8_t *source_buf = malloc(length);
    TEST_ASSERT_NOT_NULL(source_buf);
    srand(778);
    for (int i = 0; i < length; i++) {
        source_buf[i] = rand();
    }

    for (int i = 0; i < 2; i++) {
        if (part[i].size == 0) continue;
        write_large_buffer(&part[i], source_buf, length);
    }

    teardown_test_chip(chip);

    for (int i = 0; i < 2; i++) {
        if (part[i].size == 0) continue;

        part[i].flash_chip = (esp_flash_t*)-1;
        ESP_LOGI(TAG, "Testing address 0x%08lX...", part[i].address);
        test_permutations_part(config, &part[i], source_buf, length);
    }

    free(source_buf);
}

TEST_CASE("SPI flash test reading with all speed/mode permutations", "[esp_flash]")
{
    test_permutations_chip(&config_list[0]);
}

#ifndef CONFIG_SPIRAM
TEST_CASE("SPI flash test reading with all speed/mode permutations, 3 chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH]")
{
    for (int i = 0; i < TEST_CONFIG_NUM; i++) {
        test_permutations_chip(&config_list[i]);
    }
}
#endif


static void test_write_large_const_buffer(const esp_partition_t* part)
{
    test_write_large_buffer(part, large_const_buffer, sizeof(large_const_buffer));
}

TEST_CASE_FLASH("Test esp_flash_write large const buffer", test_write_large_const_buffer);
TEST_CASE_MULTI_FLASH("Test esp_flash_write large const buffer", test_write_large_const_buffer);

static void test_write_large_ram_buffer(const esp_partition_t* part)
{
    // buffer in RAM
    uint8_t *source_buf = malloc(sizeof(large_const_buffer));
    TEST_ASSERT_NOT_NULL(source_buf);
    memcpy(source_buf, large_const_buffer, sizeof(large_const_buffer));
    test_write_large_buffer(part, source_buf, sizeof(large_const_buffer));
    free(source_buf);
}

TEST_CASE_FLASH("Test esp_flash_write large RAM buffer", test_write_large_ram_buffer);
TEST_CASE_MULTI_FLASH("Test esp_flash_write large RAM buffer", test_write_large_ram_buffer);

static void write_large_buffer(const esp_partition_t *part, const uint8_t *source, size_t length)
{
    esp_flash_t* chip = part->flash_chip;

    printf("Writing chip %p %p, %u bytes from source %p\n", chip, (void*)part->address, length, source);
    ESP_ERROR_CHECK( esp_flash_erase_region(chip, part->address, (length + SPI_FLASH_SEC_SIZE) & ~(SPI_FLASH_SEC_SIZE - 1)) );

    // note writing to unaligned address
    ESP_ERROR_CHECK( esp_flash_write(chip, source, part->address + 1, length) );
}

static void read_and_check(const esp_partition_t *part, const uint8_t *source, size_t length)
{
    esp_flash_t* chip = part->flash_chip;
    printf("Checking chip %p 0x%08lX, %u bytes\n", chip, part->address, length);
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

static void test_write_large_buffer(const esp_partition_t* part, const uint8_t *source, size_t length)
{
    TEST_ASSERT(part->size > length + 2 + SPI_FLASH_SEC_SIZE);

    write_large_buffer(part, source, length);
    read_and_check(part, source, length);
}

#if !CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE

typedef struct {
    uint32_t us_start;
    size_t len;
    const char* name;
} time_meas_ctx_t;
static void time_measure_start(time_meas_ctx_t* ctx)
{
    ctx->us_start = esp_timer_get_time();
    ccomp_timer_start();
}

static uint32_t time_measure_end(time_meas_ctx_t* ctx)
{
    uint32_t c_time_us = ccomp_timer_stop();
    uint32_t time_us = esp_timer_get_time() - ctx->us_start;

    ESP_LOGI(TAG, "%s: compensated: %.2lf kB/s, typical: %.2lf kB/s", ctx->name, ctx->len / (c_time_us / 1000.), ctx->len / (time_us/1000.));
    return ctx->len * 1000 / (c_time_us / 1000);
}

#define TEST_TIMES      20
#define TEST_SECTORS    4

static uint32_t measure_erase(const esp_partition_t* part)
{
    const int total_len = SPI_FLASH_SEC_SIZE * TEST_SECTORS;
    time_meas_ctx_t time_ctx = {.name = "erase", .len = total_len};

    time_measure_start(&time_ctx);
    esp_err_t err = esp_flash_erase_region(part->flash_chip, part->address, total_len);
    TEST_ESP_OK(err);
    return time_measure_end(&time_ctx);
}

// should called after measure_erase
static uint32_t measure_write(const char* name, const esp_partition_t* part, const uint8_t* data_to_write, int seg_len)
{
    const int total_len = SPI_FLASH_SEC_SIZE;
    time_meas_ctx_t time_ctx = {.name = name, .len = total_len * TEST_TIMES};

    time_measure_start(&time_ctx);
    for (int i = 0; i < TEST_TIMES; i ++) {
        // Erase one time, but write 100 times the same data
        size_t len = total_len;
        int offset = 0;

        while (len) {
            int len_write = MIN(seg_len, len);
            esp_err_t err = esp_flash_write(part->flash_chip, data_to_write + offset, part->address + offset, len_write);
            TEST_ESP_OK(err);

            offset += len_write;
            len -= len_write;
        }
    }
    return time_measure_end(&time_ctx);
}

static uint32_t measure_read(const char* name, const esp_partition_t* part, uint8_t* data_read, int seg_len)
{
    const int total_len = SPI_FLASH_SEC_SIZE;
    time_meas_ctx_t time_ctx = {.name = name, .len = total_len * TEST_TIMES};

    time_measure_start(&time_ctx);
    for (int i = 0; i < TEST_TIMES; i ++) {
        size_t len = total_len;
        int offset = 0;

        while (len) {
            int len_read = MIN(seg_len, len);
            esp_err_t err = esp_flash_read(part->flash_chip, data_read + offset, part->address + offset, len_read);
            TEST_ESP_OK(err);

            offset += len_read;
            len -= len_read;
        }
    }
    return time_measure_end(&time_ctx);
}

static const char* get_chip_vendor(uint32_t id)
{
    switch (id)
    {
    case 0x20:
        return "XMC";
        break;
    case 0x68:
        return "BOYA";
        break;
    case 0xC8:
        return "GigaDevice";
        break;
    case 0x9D:
        return "ISSI";
        break;
    case 0xC2:
        return "MXIC";
        break;
    case 0xEF:
        return "Winbond";
        break;
    case 0xA1:
        return "Fudan Micro";
        break;
    default:
        break;
    }
    return "generic";
}

#define MEAS_WRITE(n)   (measure_write("write in "#n"-byte chunks", part, data_to_write, n))
#define MEAS_READ(n)    (measure_read("read in "#n"-byte chunks", part, data_read, n))

static void test_flash_read_write_performance(const esp_partition_t *part)
{
    esp_flash_t* chip = part->flash_chip;
    const int total_len = SPI_FLASH_SEC_SIZE;
    uint8_t *data_to_write = heap_caps_malloc(total_len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *data_read = heap_caps_malloc(total_len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    srand(777);
    for (int i = 0; i < total_len; i++) {
        data_to_write[i] = rand();
    }

    uint32_t erase_1 = measure_erase(part);
    uint32_t speed_WR_4B = MEAS_WRITE(4);
    uint32_t speed_RD_4B = MEAS_READ(4);
    uint32_t erase_2 = measure_erase(part);
    uint32_t speed_WR_2KB = MEAS_WRITE(2048);
    uint32_t speed_RD_2KB = MEAS_READ(2048);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_to_write, data_read, total_len);

    spi_host_device_t host_id;
    int cs_id;
    uint32_t id;
    esp_flash_read_id(chip, &id);
    const char *chip_name = get_chip_vendor(id >> 16);

    get_chip_host(chip, &host_id, &cs_id);
    if (host_id != SPI1_HOST) {
        // Chips on other SPI buses
        LOG_PERFORMANCE(EXT_, chip_name);
    } else if (cs_id == 0) {
        // Main flash
        LOG_PERFORMANCE(,chip_name);
    } else {
        // Other cs pins on SPI1
        LOG_PERFORMANCE(SPI1_, chip_name);
    }
    free(data_to_write);
    free(data_read);
}

#if !BYPASS_MULTIPLE_CHIP
//To make performance data stable, needs to run on special runner
TEST_CASE("Test esp_flash read/write performance", "[esp_flash][test_env=UT_T1_ESP_FLASH]") {flash_test_func(test_flash_read_write_performance, 1);}
#endif

TEST_CASE_MULTI_FLASH("Test esp_flash read/write performance", test_flash_read_write_performance);

#endif

#ifdef CONFIG_SPIRAM_USE_MALLOC

/* Utility: Read into a small internal RAM buffer using esp_flash_read() and compare what
   we read with 'buffer' */
static void s_test_compare_flash_contents_small_reads(esp_flash_t *chip, const uint8_t *buffer, size_t offs, size_t len)
{
    const size_t INTERNAL_BUF_SZ = 1024; // Should fit in internal RAM
    uint8_t *ibuf = heap_caps_malloc(INTERNAL_BUF_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(ibuf);

    for (int i = 0; i < len; i += INTERNAL_BUF_SZ) {
        size_t to_read = MIN(INTERNAL_BUF_SZ, len - i);
        ESP_ERROR_CHECK( esp_flash_read(chip, ibuf, offs + i, to_read) );
        TEST_ASSERT_EQUAL_HEX8_ARRAY(buffer + i, ibuf, to_read);
    }

    free(ibuf);
}

static void test_flash_read_large_psram_buffer(const esp_partition_t *part)
{
    esp_flash_t* chip = part->flash_chip;
    const size_t BUF_SZ = 256 * 1024;    // Too large for internal RAM
    const size_t TEST_OFFS = 0x1000; // Can be any offset, really

    uint8_t *buf = heap_caps_malloc(BUF_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(buf);

    ESP_ERROR_CHECK( esp_flash_read(chip, buf, TEST_OFFS, BUF_SZ) );

    // Read back the same into smaller internal memory buffer and check it all matches
    s_test_compare_flash_contents_small_reads(chip, buf, TEST_OFFS, BUF_SZ);

    free(buf);
}

TEST_CASE_FLASH("esp_flash_read large PSRAM buffer", test_flash_read_large_psram_buffer);


/* similar to above test, but perform it under memory pressure */
static void test_flash_read_large_psram_buffer_low_internal_mem(const esp_partition_t *part)
{
    esp_flash_t* chip = part->flash_chip;
    const size_t BUF_SZ = 256 * 1024;    // Too large for internal RAM
    const size_t REMAINING_INTERNAL = 1024; // Exhaust internal memory until maximum free block is less than this
    const size_t TEST_OFFS = 0x8000;

    /* Exhaust the available free internal memory */
    test_utils_exhaust_memory_rec erec = test_utils_exhaust_memory(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT, REMAINING_INTERNAL);

    uint8_t *buf = heap_caps_malloc(BUF_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(buf);

    /* Calling esp_flash_read() here will need to allocate a small internal buffer,
       so check it works. */
    ESP_ERROR_CHECK( esp_flash_read(chip, buf, TEST_OFFS, BUF_SZ) );

    test_utils_free_exhausted_memory(erec);

    // Read back the same into smaller internal memory buffer and check it all matches
    s_test_compare_flash_contents_small_reads(chip, buf, TEST_OFFS, BUF_SZ);

    free(buf);
}

TEST_CASE_FLASH("esp_flash_read large PSRAM buffer low memory", test_flash_read_large_psram_buffer_low_internal_mem);
#endif


#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
#define TEST_CNT_RW_TIMES   4
#define TEST_CNT_RW_LEN     64
#define TEST_CNT_ERASE_LEN  8192
void test_flash_counter(const esp_partition_t* part)
{
    esp_flash_t* chip = part->flash_chip;
    uint32_t offs = part->address;
    esp_flash_counters_t flash_counter;
    static uint8_t write_buf[TEST_CNT_RW_LEN * TEST_CNT_RW_TIMES];
    static uint8_t read_buf[TEST_CNT_RW_LEN * TEST_CNT_RW_TIMES];

    for(int i = 0;i < TEST_CNT_RW_LEN * TEST_CNT_RW_TIMES; i ++){
        write_buf[i] = i;
    }

    esp_flash_reset_counters();
    flash_counter = *esp_flash_get_counters();
    // check for resset_counter and get_counter API
    TEST_ASSERT_EACH_EQUAL_HEX8(0, &flash_counter, sizeof(esp_flash_counters_t));

    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_erase_region(chip, offs, TEST_CNT_ERASE_LEN));
    flash_counter = *esp_flash_get_counters();
    // check for erase_counter API
    TEST_ASSERT_EQUAL_UINT32(0, flash_counter.read.count);
    TEST_ASSERT_EQUAL_UINT32(0, flash_counter.write.count);
    TEST_ASSERT_EQUAL_UINT32(1, flash_counter.erase.count);
    TEST_ASSERT_EQUAL_UINT32(0 * TEST_CNT_RW_LEN, flash_counter.read.bytes);
    TEST_ASSERT_EQUAL_UINT32(0 * TEST_CNT_RW_LEN, flash_counter.write.bytes);
    TEST_ASSERT_EQUAL_UINT32(TEST_CNT_ERASE_LEN, flash_counter.erase.bytes);

    int count;
    for(count = 0; count < TEST_CNT_RW_TIMES; count ++) {
        // check counter on write option
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write(chip, write_buf + TEST_CNT_RW_LEN * count, offs + TEST_CNT_RW_LEN * count, TEST_CNT_RW_LEN) );
        flash_counter = *esp_flash_get_counters();
        TEST_ASSERT_EQUAL_UINT32((count + 1), flash_counter.write.count);
        TEST_ASSERT_EQUAL_UINT32((count + 1) * TEST_CNT_RW_LEN, flash_counter.write.bytes);

        // check counter on read option
        TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(chip, read_buf + TEST_CNT_RW_LEN * count, offs + TEST_CNT_RW_LEN * count, TEST_CNT_RW_LEN) );
        flash_counter = *esp_flash_get_counters();
        TEST_ASSERT_EQUAL_UINT32((count + 1), flash_counter.read.count);
        TEST_ASSERT_EQUAL_UINT32((count + 1) * TEST_CNT_RW_LEN, flash_counter.read.bytes);

        esp_flash_dump_counters(stdout);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(write_buf, read_buf, TEST_CNT_RW_LEN * TEST_CNT_RW_TIMES);

    // check remain value
    flash_counter = *esp_flash_get_counters();
    TEST_ASSERT_EQUAL_UINT32(1, flash_counter.erase.count);
    TEST_ASSERT_EQUAL_UINT32(TEST_CNT_ERASE_LEN, flash_counter.erase.bytes);

    esp_flash_reset_counters();
    flash_counter = *esp_flash_get_counters();
    // check for resset_counter after used
    TEST_ASSERT_EACH_EQUAL_HEX8(0, &flash_counter, sizeof(esp_flash_counters_t));

    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write_encrypted(chip, offs, write_buf, TEST_CNT_RW_LEN) );
    TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read_encrypted(chip, offs, read_buf, TEST_CNT_RW_LEN) );

    printf("\ntest for encrypted write/read\n");
    esp_flash_dump_counters(stdout);

    flash_counter = *esp_flash_get_counters();
    // As we do one time of encrypted RW, no erase option
    TEST_ASSERT_EQUAL_UINT32(1, flash_counter.read.count);
    TEST_ASSERT_EQUAL_UINT32(1, flash_counter.write.count);
    TEST_ASSERT_EQUAL_UINT32(0, flash_counter.erase.count);
    TEST_ASSERT_EQUAL_UINT32(1 * TEST_CNT_RW_LEN, flash_counter.read.bytes);
    TEST_ASSERT_EQUAL_UINT32(1 * TEST_CNT_RW_LEN, flash_counter.write.bytes);
    TEST_ASSERT_EQUAL_UINT32(0, flash_counter.erase.bytes);
}

TEST_CASE_FLASH("SPI flash counter test", test_flash_counter);
#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS
