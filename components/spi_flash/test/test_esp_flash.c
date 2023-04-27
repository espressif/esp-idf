#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_flash.h"
#include "esp_private/spi_common_internal.h"
#include "spi_flash_mmap.h"
#include "esp_flash_spi_init.h"
#include "memspi_host_driver.h"
#include <esp_attr.h>
#include "esp_log.h"


#include <test_utils.h>

#include "unity.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "sdkconfig.h"

#include "ccomp_timer.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#endif

#define FUNC_SPI    1

#define MAX_ADDR_24BIT      0x1000000
#define TEST_SPI_SPEED      10
#define TEST_SPI_READ_MODE  SPI_FLASH_FASTRD
// #define FORCE_GPIO_MATRIX

#if CONFIG_IDF_TARGET_ESP32
#define EXTRA_SPI1_CLK_IO   17  //the pin which is usually used by the PSRAM clk
#define SPI1_CS_IO          16  //the pin which is usually used by the PSRAM cs

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
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   35
#define FSPI_PIN_NUM_MISO   37
#define FSPI_PIN_NUM_CLK    36
#define FSPI_PIN_NUM_HD     33
#define FSPI_PIN_NUM_WP     38
#define FSPI_PIN_NUM_CS     34

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32S3
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   11
#define FSPI_PIN_NUM_MISO   13
#define FSPI_PIN_NUM_CLK    12
#define FSPI_PIN_NUM_HD     9
#define FSPI_PIN_NUM_WP     14
#define FSPI_PIN_NUM_CS     10

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   7
#define FSPI_PIN_NUM_MISO   2
#define FSPI_PIN_NUM_CLK    6
#define FSPI_PIN_NUM_HD     4
#define FSPI_PIN_NUM_WP     5
#define FSPI_PIN_NUM_CS     10

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS
#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C6, ESP32H2)
#define TEST_CONFIG_NUM (sizeof(config_list)/sizeof(flashtest_config_t))

typedef void (*flash_test_func_t)(const esp_partition_t *part);

/* Use TEST_CASE_FLASH for SPI flash tests that only use the main SPI flash chip
*/
#define TEST_CASE_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

#define TEST_CASE_FLASH_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash][ignore]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

/* Use TEST_CASE_MULTI_FLASH for tests which also run on external flash, which sits in the place of PSRAM
   (these tests are incompatible with PSRAM)

   These tests run for all the flash chip configs shown in config_list, below (internal and external).
 */


#if defined(CONFIG_SPIRAM)
//SPI1 CS1 occupied by PSRAM
#define BYPASS_MULTIPLE_CHIP    1
#endif

#if CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
//chips without PSRAM
#define TEST_CHIP_NUM   2
#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define TEST_CHIP_NUM   3
#endif

#define _STRINGIFY(s)   #s
#define STRINGIFY(s)   _STRINGIFY(s)
#define TEST_CHIP_NUM_STR   STRINGIFY(TEST_CHIP_NUM)

#if BYPASS_MULTIPLE_CHIP
#define TEST_CASE_MULTI_FLASH   TEST_CASE_MULTI_FLASH_IGNORE
#else
#if CONFIG_FREERTOS_SMP // IDF-5260
#define TEST_CASE_MULTI_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][timeout=60]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}
#else
#define TEST_CASE_MULTI_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][timeout=35]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}
#endif
#endif

#define TEST_CASE_MULTI_FLASH_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][ignore]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}


//currently all the configs are the same with esp_flash_spi_device_config_t, no more information required
typedef esp_flash_spi_device_config_t flashtest_config_t;

static const char TAG[] = "test_esp_flash";

#define FLASHTEST_CONFIG_COMMON     \
    /* 0 always reserved for main flash */ \
    { \
        /* no need to init */ \
        .host_id = -1, \
    } \
    , \
    { \
        .io_mode = TEST_SPI_READ_MODE,\
        .freq_mhz = TEST_SPI_SPEED, \
        .host_id = SPI1_HOST, \
        .cs_id = 1, \
        /* the pin which is usually used by the PSRAM */ \
        .cs_io_num = SPI1_CS_IO, \
        .input_delay_ns = 0, \
    }

#if CONFIG_IDF_TARGET_ESP32
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    /* current runner doesn't have a flash on HSPI */
    // {
    //     .io_mode = TEST_SPI_READ_MODE,
    //     .freq_mhz = TEST_SPI_SPEED,
    //     .host_id = HSPI_HOST,
    //     .cs_id = 0,
    //     // uses GPIO matrix on esp32s2 regardless if FORCE_GPIO_MATRIX
    //     .cs_io_num = HSPI_PIN_NUM_CS,
    //     .input_delay_ns = 20,
    // },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = VSPI_HOST,
        .cs_id = 0,
        .cs_io_num = VSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32S2
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = FSPI_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = HSPI_HOST,
        .cs_id = 0,
        // uses GPIO matrix on esp32s2 regardless of FORCE_GPIO_MATRIX
        .cs_io_num = HSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32S3
flashtest_config_t config_list[] = {
    /* No SPI1 CS1 flash on esp32S3 test */
    {
        /* no need to init */
        .host_id = -1,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = SPI2_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
flashtest_config_t config_list[] = {
    /* No SPI1 CS1 flash on esp32c3 test */
    {
        /* no need to init */
        .host_id = -1,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = SPI2_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#endif

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
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...\n");
        //no need to initialize the bus, however the CLK may need one more output if it's on the usual place of PSRAM
        esp_rom_gpio_connect_out_signal(EXTRA_SPI1_CLK_IO, SPICLK_OUT_IDX, 0, 0);
        //currently the SPI bus for main flash chip is initialized through GPIO matrix
    } else if (host_id == SPI2_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%d (HSPI) CS0...\n", host_id + 1);
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
        ESP_LOGI(TAG, "setup flash on SPI%d (VSPI) CS0...\n", host_id + 1);
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
        ESP_LOGI(TAG, "setup flash on SPI1 CS1...\n");
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
    }
    else if (host_id == SPI2_HOST) {
        ESP_LOGI(TAG, "setup flash on SPI%d (FSPI) CS0...\n", host_id + 1);
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
        ESP_LOGI(TAG, "setup flash on SPI%d (HSPI) CS0...\n", host_id + 1);
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
    ESP_LOGI(TAG, "Flash size: 0x%08X", size);

    const esp_partition_t* test_part = get_test_data_partition();
    TEST_ASSERT_NOT_EQUAL(NULL, test_part->flash_chip);

    esp_partition_t part = *test_part;
    part.flash_chip = chip;

    ESP_LOGI(TAG, "Testing chip %p, address 0x%08X...", part.flash_chip, part.address);
    (*func)(&part);

    // For flash with size over 16MB, add one extra round of test for the 32-bit address area
    if (size > MAX_ADDR_24BIT) {
        part.address = 0x1030000;
        part.size = 0x0010000;
        ESP_LOGI(TAG, "Testing chip %p, address 0x%08X...", part.flash_chip, part.address);
        (*func)(&part);
    }

    teardown_test_chip(chip);
}

static void flash_test_func(flash_test_func_t func, int test_num)
{
    esp_log_level_set("gpio", ESP_LOG_NONE);
    for (int i = 0; i < test_num; i++) {
        ESP_LOGI(TAG, "Testing config %d/%d", i+1, test_num);
        flash_test_core(func, &config_list[i]);
    }
    ESP_LOGI(TAG, "Completed %d configs", test_num);
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
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C6, ESP32H2)
