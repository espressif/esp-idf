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

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

#include "hal/spi_flash_hal.h"
#include "ccomp_timer.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#endif

#define FUNC_SPI    1

static uint8_t sector_buf[4096];

#define TEST_SPI_SPEED      ESP_FLASH_10MHZ
#define TEST_SPI_READ_MODE  SPI_FLASH_FASTRD
// #define FORCE_GPIO_MATRIX

#define EXTRA_SPI1_CLK_IO   17  //the pin which is usually used by the PSRAM clk

#if CONFIG_IDF_TARGET_ESP32
#define HSPI_PIN_NUM_MOSI   HSPI_IOMUX_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   HSPI_IOMUX_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    HSPI_IOMUX_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     HSPI_IOMUX_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     HSPI_IOMUX_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     HSPI_IOMUX_PIN_NUM_CS

#define VSPI_PIN_NUM_MOSI   VSPI_IOMUX_PIN_NUM_MOSI
#define VSPI_PIN_NUM_MISO   VSPI_IOMUX_PIN_NUM_MISO
#define VSPI_PIN_NUM_CLK    VSPI_IOMUX_PIN_NUM_CLK
#define VSPI_PIN_NUM_HD     VSPI_IOMUX_PIN_NUM_HD
#define VSPI_PIN_NUM_WP     VSPI_IOMUX_PIN_NUM_WP
#define VSPI_PIN_NUM_CS     VSPI_IOMUX_PIN_NUM_CS
#elif CONFIG_IDF_TARGET_ESP32S2
#define FSPI_PIN_NUM_MOSI   FSPI_IOMUX_PIN_NUM_MOSI
#define FSPI_PIN_NUM_MISO   FSPI_IOMUX_PIN_NUM_MISO
#define FSPI_PIN_NUM_CLK    FSPI_IOMUX_PIN_NUM_CLK
#define FSPI_PIN_NUM_HD     FSPI_IOMUX_PIN_NUM_HD
#define FSPI_PIN_NUM_WP     FSPI_IOMUX_PIN_NUM_WP
#define FSPI_PIN_NUM_CS     FSPI_IOMUX_PIN_NUM_CS

// Just use the same pins for HSPI and VSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#define VSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define VSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define VSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define VSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define VSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define VSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS
#endif

#define TEST_CONFIG_NUM (sizeof(config_list)/sizeof(flashtest_config_t))

typedef void (*flash_test_func_t)(esp_flash_t* chip);

/* Use FLASH_TEST_CASE for SPI flash tests that only use the main SPI flash chip
*/
#define FLASH_TEST_CASE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

#define FLASH_TEST_CASE_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash][ignore]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

/* Use FLASH_TEST_CASE_3 for tests which also run on external flash, which sits in the place of PSRAM
   (these tests are incompatible with PSRAM)

   These tests run for all the flash chip configs shown in config_list, below (internal and external).
 */
#if defined(CONFIG_SPIRAM_SUPPORT) || TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2)
#define FLASH_TEST_CASE_3(STR, FUNCT_TO_RUN)
#define FLASH_TEST_CASE_3_IGNORE(STR, FUNCT_TO_RUN)
#else
// Disabled for ESP32-S2 due to lack of runners
#define FLASH_TEST_CASE_3(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", 3 chips", "[esp_flash][test_env=UT_T1_ESP_FLASH]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}

#define FLASH_TEST_CASE_3_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", 3 chips", "[esp_flash][test_env=UT_T1_ESP_FLASH][ignore]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}
#endif

//currently all the configs are the same with esp_flash_spi_device_config_t, no more information required
typedef esp_flash_spi_device_config_t flashtest_config_t;

static const char TAG[] = "test_esp_flash";

#define FLASHTEST_CONFIG_COMMON     \
    /* 0 always reserved for main flash */ \
    { \
        /* no need to init */ \
        .host_id = -1, \
    }, \
    { \
        .io_mode = TEST_SPI_READ_MODE,\
        .speed = TEST_SPI_SPEED, \
        .host_id = SPI_HOST, \
        .cs_id = 1, \
        /* the pin which is usually used by the PSRAM */ \
        .cs_io_num = 16, \
        .input_delay_ns = 0, \
    }

#if CONFIG_IDF_TARGET_ESP32
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    /* current runner doesn't have a flash on HSPI */
    // {
    //     .io_mode = TEST_SPI_READ_MODE,
    //     .speed = TEST_SPI_SPEED,
    //     .host_id = HSPI_HOST,
    //     .cs_id = 0,
    //     // uses GPIO matrix on esp32s2 regardles if FORCE_GPIO_MATRIX
    //     .cs_io_num = HSPI_PIN_NUM_CS,
    //     .input_delay_ns = 20,
    // },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .speed = TEST_SPI_SPEED,
        .host_id = VSPI_HOST,
        .cs_id = 0,
        .cs_io_num = VSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32S2
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    /* No runners for esp32s2 for these config yet */
    // {
    //     .io_mode = TEST_SPI_READ_MODE,
    //     .speed = TEST_SPI_SPEED,
    //     .host_id = FSPI_HOST,
    //     .cs_id = 0,
    //     .cs_io_num = FSPI_PIN_NUM_CS,
    //     .input_delay_ns = 0,
    // },
    // /* current runner doesn't have a flash on HSPI */
    // {
    //     .io_mode = TEST_SPI_READ_MODE,
    //     .speed = TEST_SPI_SPEED,
    //     .host_id = HSPI_HOST,
    //     .cs_id = 0,
    //     // uses GPIO matrix on esp32s2 regardles if FORCE_GPIO_MATRIX
    //     .cs_io_num = HSPI_PIN_NUM_CS,
    //     .input_delay_ns = 20,
    // },
};
#endif

static void get_chip_host(esp_flash_t* chip, spi_host_device_t* out_host_id, int* out_cs_id)
{
    spi_host_device_t host_id;
    int cs_id;
    if (chip == NULL) {
        host_id = SPI_HOST;
        cs_id = 0;
    } else {
        spi_flash_memspi_data_t* driver_data = (spi_flash_memspi_data_t*)chip->host->driver_data;
        host_id = spi_flash_ll_hw_get_id(driver_data->spi);
        cs_id = driver_data->cs_num;
    }
    if (out_host_id) {
        *out_host_id = host_id;
    }
    if (out_cs_id) {
        *out_cs_id = cs_id;
    }
}

static void setup_bus(spi_host_device_t host_id)
{
    if (host_id == SPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...\n");
        //no need to initialize the bus, however the CLK may need one more output if it's on the usual place of PSRAM
#ifdef EXTRA_SPI1_CLK_IO
        gpio_matrix_out(EXTRA_SPI1_CLK_IO, SPICLK_OUT_IDX, 0, 0);
#endif
#if !DISABLED_FOR_TARGETS(ESP32)
    } else if (host_id == FSPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%d (FSPI) CS0...\n", host_id + 1);
        spi_bus_config_t fspi_bus_cfg = {
            .mosi_io_num = FSPI_PIN_NUM_MOSI,
            .miso_io_num = FSPI_PIN_NUM_MISO,
            .sclk_io_num = FSPI_PIN_NUM_CLK,
            .quadhd_io_num = FSPI_PIN_NUM_HD,
            .quadwp_io_num = FSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
#ifdef FORCE_GPIO_MATRIX
        fspi_bus_cfg.quadhd_io_num = 5;
#endif
        esp_err_t ret = spi_bus_initialize(host_id, &fspi_bus_cfg, 0);
        TEST_ESP_OK(ret);
#endif
        //currently the SPI bus for main flash chip is initialized through GPIO matrix
    } else if (host_id == HSPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%d (HSPI) CS0...\n", host_id + 1);
        spi_bus_config_t hspi_bus_cfg = {
            .mosi_io_num = HSPI_PIN_NUM_MOSI,
            .miso_io_num = HSPI_PIN_NUM_MISO,
            .sclk_io_num = HSPI_PIN_NUM_CLK,
            .quadhd_io_num = HSPI_PIN_NUM_HD,
            .quadwp_io_num = HSPI_PIN_NUM_WP,
            .max_transfer_sz = 64,
        };
#if !DISABLED_FOR_TARGETS(ESP32S2)
#ifdef FORCE_GPIO_MATRIX
        hspi_bus_cfg.quadhd_io_num = 23;
#endif
#endif
        esp_err_t ret = spi_bus_initialize(host_id, &hspi_bus_cfg, 0);
        TEST_ESP_OK(ret);

#if !DISABLED_FOR_TARGETS(ESP32)
        // HSPI have no multiline mode, use GPIO to pull those pins up
        gpio_set_direction(HSPI_PIN_NUM_HD, GPIO_MODE_OUTPUT);
        gpio_set_level(HSPI_PIN_NUM_HD, 1);

        gpio_set_direction(HSPI_PIN_NUM_WP, GPIO_MODE_OUTPUT);
        gpio_set_level(HSPI_PIN_NUM_WP, 1);
#endif
    }
#if !DISABLED_FOR_TARGETS(ESP32S2)
    else if (host_id == VSPI_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%d (VSPI) CS0...\n", host_id + 1);
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
    }
#endif // disabled for esp32s2
    else {
        ESP_LOGE(TAG, "invalid bus");
    }
}

static void release_bus(int host_id)
{
    //SPI1 bus can't be deinitialized
    if (host_id == SPI2_HOST || host_id == SPI3_HOST) {
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
        ESP_LOGI(TAG, "Testing config %d/%d", i, test_num);
        flashtest_config_t* config = &config_list[i];
        esp_flash_t* chip;
        setup_new_chip(config, &chip);
        (*func)(chip);
        teardown_test_chip(chip, config->host_id);
    }
    ESP_LOGI(TAG, "Completed %d configs", test_num);
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

IRAM_ATTR NOINLINE_ATTR static void test_toggle_qe(esp_flash_t* chip)
{
    bool qe;
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    }
    esp_flash_io_mode_t io_mode_before = chip->read_mode;
    esp_err_t ret = esp_flash_get_io_mode(chip, &qe);
    TEST_ESP_OK(ret);

    bool allow_failure = is_winbond_chip(chip) || is_mxic_chip(chip);

    for (int i = 0; i < 4; i ++) {
        ets_printf(DRAM_STR("write qe: %d->%d\n"), qe, !qe);
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
        ESP_LOGD(TAG, "qe read: %d", qe_read);
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
FLASH_TEST_CASE_IGNORE("Test esp_flash_write can toggle QE bit", test_toggle_qe);
FLASH_TEST_CASE_3_IGNORE("Test esp_flash_write can toggle QE bit", test_toggle_qe);


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
                cfg->io_mode = io_mode;
                cfg->speed = speed;
                setup_new_chip(cfg, &chip);

                spi_host_device_t host_id;
                get_chip_host(chip, &host_id, NULL);

                if (io_mode > SPI_FLASH_FASTRD
                    && !SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)) {
                    continue;
                }

                ESP_LOGI(TAG, "test flash io mode: %d, speed: %d", io_mode, speed);

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

#ifndef CONFIG_SPIRAM_SUPPORT
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2)
// No runners
TEST_CASE("SPI flash test reading with all speed/mode permutations, 3 chips", "[esp_flash][test_env=UT_T1_ESP_FLASH]")
{
    for (int i = 0; i < TEST_CONFIG_NUM; i++) {
        test_permutations(&config_list[i]);
    }
}
#endif
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

#define MEAS_WRITE(n)   (measure_write("write in "#n"-byte chunks", &test_part, data_to_write, n))
#define MEAS_READ(n)    (measure_read("read in "#n"-byte chunks", &test_part, data_read, n))

static void test_flash_read_write_performance(esp_flash_t* chip)
{
    const esp_partition_t *part = get_test_data_partition();
    // Copy to new partition variable and replace the chip member
    // Actually there's no "partition" in the external flash on runners. We just don't bother creating a new partition variable.
    esp_partition_t test_part;
    memcpy(&test_part, part, sizeof(esp_partition_t));
    test_part.flash_chip = chip;

    const int total_len = SPI_FLASH_SEC_SIZE;
    uint8_t *data_to_write = heap_caps_malloc(total_len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *data_read = heap_caps_malloc(total_len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    srand(777);
    for (int i = 0; i < total_len; i++) {
        data_to_write[i] = rand();
    }

    uint32_t erase_1 = measure_erase(&test_part);
    uint32_t speed_WR_4B = MEAS_WRITE(4);
    uint32_t speed_RD_4B = MEAS_READ(4);
    uint32_t erase_2 = measure_erase(&test_part);
    uint32_t speed_WR_2KB = MEAS_WRITE(2048);
    uint32_t speed_RD_2KB = MEAS_READ(2048);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_to_write, data_read, total_len);

#define LOG_DATA(bus, suffix, chip) IDF_LOG_PERFORMANCE("FLASH_SPEED_BYTE_PER_SEC_"#bus#suffix, "%d, flash_chip: %s", speed_##suffix, chip)
#define LOG_ERASE(bus, var, chip) IDF_LOG_PERFORMANCE("FLASH_SPEED_BYTE_PER_SEC_"#bus"ERASE", "%d, flash_chip: %s", var, chip)

// Erase time may vary a lot, can increase threshold if this fails with a reasonable speed
#define LOG_PERFORMANCE(bus, chip) do {\
            LOG_DATA(bus, WR_4B, chip); \
            LOG_DATA(bus, RD_4B, chip); \
            LOG_DATA(bus, WR_2KB, chip); \
            LOG_DATA(bus, RD_2KB, chip); \
            LOG_ERASE(bus, erase_1, chip); \
            LOG_ERASE(bus, erase_2, chip); \
        } while (0)

    spi_host_device_t host_id;
    int cs_id;
    uint32_t id;
    esp_flash_read_id(chip, &id);
    const char *chip_name = get_chip_vendor(id >> 16);

    get_chip_host(chip, &host_id, &cs_id);
    if (host_id != SPI_HOST) {
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

FLASH_TEST_CASE("Test esp_flash read/write performance", test_flash_read_write_performance);
FLASH_TEST_CASE_3("Test esp_flash read/write performance", test_flash_read_write_performance);


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

static void test_flash_read_large_psram_buffer(esp_flash_t *chip)
{
    const size_t BUF_SZ = 256 * 1024;    // Too large for internal RAM
    const size_t TEST_OFFS = 0x1000; // Can be any offset, really

    uint8_t *buf = heap_caps_malloc(BUF_SZ, MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(buf);

    ESP_ERROR_CHECK( esp_flash_read(chip, buf, TEST_OFFS, BUF_SZ) );

    // Read back the same into smaller internal memory buffer and check it all matches
    s_test_compare_flash_contents_small_reads(chip, buf, TEST_OFFS, BUF_SZ);

    free(buf);
}

FLASH_TEST_CASE("esp_flash_read large PSRAM buffer", test_flash_read_large_psram_buffer);


/* similar to above test, but perform it under memory pressure */
static void test_flash_read_large_psram_buffer_low_internal_mem(esp_flash_t *chip)
{
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

FLASH_TEST_CASE("esp_flash_read large PSRAM buffer low memory", test_flash_read_large_psram_buffer_low_internal_mem);


#endif
