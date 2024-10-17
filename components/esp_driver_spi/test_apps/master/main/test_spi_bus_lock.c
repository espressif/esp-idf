/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_flash_spi_init.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "unity.h"

#if CONFIG_IDF_TARGET_ESP32
// The VSPI pins on UT_T1_ESP_FLASH are connected to a external flash
#define TEST_BUS_PIN_NUM_MISO   VSPI_IOMUX_PIN_NUM_MISO
#define TEST_BUS_PIN_NUM_MOSI   VSPI_IOMUX_PIN_NUM_MOSI
#define TEST_BUS_PIN_NUM_CLK    VSPI_IOMUX_PIN_NUM_CLK
#define TEST_BUS_PIN_NUM_CS     VSPI_IOMUX_PIN_NUM_CS
#define TEST_BUS_PIN_NUM_WP     VSPI_IOMUX_PIN_NUM_WP
#define TEST_BUS_PIN_NUM_HD     VSPI_IOMUX_PIN_NUM_HD

#else
#define TEST_BUS_PIN_NUM_MISO   SPI2_IOMUX_PIN_NUM_MISO
#define TEST_BUS_PIN_NUM_MOSI   SPI2_IOMUX_PIN_NUM_MOSI
#define TEST_BUS_PIN_NUM_CLK    SPI2_IOMUX_PIN_NUM_CLK
#define TEST_BUS_PIN_NUM_CS     SPI2_IOMUX_PIN_NUM_CS
#define TEST_BUS_PIN_NUM_WP     SPI2_IOMUX_PIN_NUM_WP
#define TEST_BUS_PIN_NUM_HD     SPI2_IOMUX_PIN_NUM_HD

#endif

#define TEST_FLASH_FREQ_MHZ      5

typedef struct {
    union {
        spi_device_handle_t handle;
        esp_flash_t* chip;
    };
    bool finished;
} task_context_t;

#if !(CONFIG_SPIRAM && CONFIG_IDF_TARGET_ESP32)

const static char TAG[] = "test_spi";

void spi_task1(void* arg)
{
    //task1 send 50 polling transactions, acquire the bus and send another 50
    int count = 0;
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = { 0x80, 0x12, 0x34, 0x56 },
        .length = 4 * 8,
    };
    spi_device_handle_t handle = ((task_context_t*)arg)->handle;
    for (int j = 0; j < 50; j ++) {
        TEST_ESP_OK(spi_device_polling_transmit(handle, &t));
        ESP_LOGI(TAG, "task1:%d", count++);
    }
    TEST_ESP_OK(spi_device_acquire_bus(handle, portMAX_DELAY));
    for (int j = 0; j < 50; j ++) {
        TEST_ESP_OK(spi_device_polling_transmit(handle, &t));
        ESP_LOGI(TAG, "task1:%d", count++);
    }
    spi_device_release_bus(handle);
    ESP_LOGI(TAG, "task1 terminates");
    ((task_context_t*)arg)->finished = true;
    vTaskDelete(NULL);
}

void spi_task2(void* arg)
{
    int count = 0;
    //task2 acquire the bus, send 50 polling transactions and then 50 non-polling
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = { 0x80, 0x12, 0x34, 0x56 },
        .length = 4 * 8,
    };
    spi_transaction_t *ret_t;
    spi_device_handle_t handle = ((task_context_t*)arg)->handle;
    TEST_ESP_OK(spi_device_acquire_bus(handle, portMAX_DELAY));

    for (int i = 0; i < 50; i ++) {
        TEST_ESP_OK(spi_device_polling_transmit(handle, &t));
        ESP_LOGI(TAG, "task2: %d", count++);
    }

    for (int j = 0; j < 50; j ++) {
        TEST_ESP_OK(spi_device_queue_trans(handle, &t, portMAX_DELAY));
    }
    for (int j = 0; j < 50; j ++) {
        TEST_ESP_OK(spi_device_get_trans_result(handle, &ret_t, portMAX_DELAY));
        assert(ret_t == &t);
        ESP_LOGI(TAG, "task2: %d", count++);
    }
    spi_device_release_bus(handle);
    vTaskDelay(1);
    ESP_LOGI(TAG, "task2 terminates");
    ((task_context_t*)arg)->finished = true;
    vTaskDelete(NULL);
}

void spi_task3(void* arg)
{
    //task3 send 30 polling transactions, acquire the bus, send 20 polling transactions and then 50 non-polling
    int count = 0;
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = { 0x80, 0x12, 0x34, 0x56 },
        .length = 4 * 8,
    };
    spi_transaction_t *ret_t;
    spi_device_handle_t handle = ((task_context_t*)arg)->handle;

    for (int i = 0; i < 30; i ++) {
        TEST_ESP_OK(spi_device_polling_transmit(handle, &t));
        ESP_LOGI(TAG, "task3: %d", count++);
    }

    TEST_ESP_OK(spi_device_acquire_bus(handle, portMAX_DELAY));
    for (int i = 0; i < 20; i ++) {
        TEST_ESP_OK(spi_device_polling_transmit(handle, &t));
        ESP_LOGI(TAG, "task3: %d", count++);
    }

    for (int j = 0; j < 50; j++) {
        TEST_ESP_OK(spi_device_queue_trans(handle, &t, portMAX_DELAY));
    }
    for (int j = 0; j < 50; j++) {
        TEST_ESP_OK(spi_device_get_trans_result(handle, &ret_t, portMAX_DELAY));
        assert(ret_t == &t);
        ESP_LOGI(TAG, "task3: %d", count++);
    }
    spi_device_release_bus(handle);

    ESP_LOGI(TAG, "task3 terminates");
    ((task_context_t*)arg)->finished = true;
    vTaskDelete(NULL);
}

static void write_large_buffer(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length)
{
    printf("Erasing chip %p, %d bytes\n", chip, length);

    TEST_ESP_OK(esp_flash_erase_region(chip, part->address, (length + part->erase_size) & ~(part->erase_size - 1)));

    printf("Writing chip %p, %d bytes from source %p\n", chip, length, source);
    // note writing to unaligned address
    TEST_ESP_OK(esp_flash_write(chip, source, part->address + 1, length));

    printf("Write done.\n");
}

static void read_and_check(esp_flash_t *chip, const esp_partition_t *part, const uint8_t *source, size_t length)
{
    printf("Checking chip %p, %d bytes\n", chip, length);
    uint8_t *buf = malloc(length);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ESP_OK(esp_flash_read(chip, buf, part->address + 1, length));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(source, buf, length);
    free(buf);

    // check nothing was written at beginning or end
    uint8_t ends[8];

    TEST_ESP_OK(esp_flash_read(chip, ends, part->address, sizeof(ends)));
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[0]);
    TEST_ASSERT_EQUAL_HEX8(source[0], ends[1]);

    TEST_ESP_OK(esp_flash_read(chip, ends, part->address + length, sizeof(ends)));

    TEST_ASSERT_EQUAL_HEX8(source[length - 1], ends[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[2]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[3]);
}

void spi_task4(void* arg)
{
    esp_flash_t *chip = ((task_context_t*)arg)->chip;

    // buffer in RAM
    const int test_len = 16400;
    uint8_t *source_buf = heap_caps_malloc(test_len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(source_buf);

    srand(676);
    for (int i = 0; i < test_len; i++) {
        source_buf[i] = rand();
    }

    ESP_LOGI(TAG, "Testing chip %p...", chip);
    const esp_partition_t *part = get_test_data_partition();
    TEST_ASSERT(part->size > test_len + 2 + part->erase_size);

    write_large_buffer(chip, part, source_buf, test_len);
    read_and_check(chip, part, source_buf, test_len);

    free(source_buf);

    ESP_LOGI(TAG, "task4 terminates");
    ((task_context_t*)arg)->finished = true;
    vTaskDelete(NULL);
}

static void test_bus_lock(bool test_flash)
{
    task_context_t context1 = {};
    task_context_t context2 = {};
    task_context_t context3 = {};
    task_context_t context4 = {};
    TaskHandle_t task1, task2, task3, task4;
    esp_err_t ret;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.miso_io_num = TEST_BUS_PIN_NUM_MISO;
    buscfg.mosi_io_num = TEST_BUS_PIN_NUM_MOSI;
    buscfg.sclk_io_num = TEST_BUS_PIN_NUM_CLK;

    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.queue_size = 100;

    //Initialize the SPI bus and 3 devices
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &context1.handle));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &context2.handle));

    //only have 3 cs pins, leave one for the flash
    devcfg.spics_io_num = -1;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &context3.handle));
    esp_flash_spi_device_config_t flash_cfg = {
        .host_id = TEST_SPI_HOST,
        .cs_id = 2,
        .cs_io_num = TEST_BUS_PIN_NUM_CS,
        .io_mode = SPI_FLASH_DIO,
        .freq_mhz = TEST_FLASH_FREQ_MHZ,
        .input_delay_ns = 0,
    };

    //Clamp the WP and HD pins to VDD to make it work in DIO mode
    gpio_set_direction(TEST_BUS_PIN_NUM_HD, GPIO_MODE_OUTPUT);
    gpio_set_direction(TEST_BUS_PIN_NUM_WP, GPIO_MODE_OUTPUT);
    gpio_set_level(TEST_BUS_PIN_NUM_HD, 1);
    gpio_set_level(TEST_BUS_PIN_NUM_WP, 1);

    esp_flash_t *chip;
    (void) chip;
    if (test_flash) {
        ret = spi_bus_add_flash_device(&chip, &flash_cfg);
        TEST_ESP_OK(ret);
        ret = esp_flash_init(chip);
        TEST_ESP_OK(ret);
        context4.chip = chip;
    }
    ESP_LOGI(TAG, "Start testing...");

    xTaskCreate(spi_task1, "task1", 4096, &context1, 0, &task1);
    xTaskCreate(spi_task2, "task2", 4096, &context2, 0, &task2);
    xTaskCreate(spi_task3, "task3", 4096, &context3, 0, &task3);
    if (test_flash) {
        xTaskCreate(spi_task4, "task4", 2048, &context4, 0, &task4);
    } else {
        context4.finished = true;
    }

    for (;;) {
        vTaskDelay(10);
        if (context1.finished && context2.finished && context3.finished && context4.finished) {
            break;
        }
    }

    TEST_ESP_OK(spi_bus_remove_device(context1.handle));
    TEST_ESP_OK(spi_bus_remove_device(context2.handle));
    TEST_ESP_OK(spi_bus_remove_device(context3.handle));
    if (test_flash) {
        TEST_ESP_OK(spi_bus_remove_flash_device(chip));
    }
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
}

#if CONFIG_IDF_TARGET_ESP32
// no need this case in other target, only esp32 need buslock to split MSPI and GPSPI2 action
TEST_CASE("spi bus lock, with flash", "[spi][test_env=external_flash]")
{
    test_bus_lock(true);
}
#endif //CONFIG_IDF_TARGET_ESP32

TEST_CASE("spi bus lock", "[spi]")
{
    test_bus_lock(false);
}

#if !SOC_MEMSPI_IS_INDEPENDENT
//disable, SPI1 is not available for GPSPI usage on chips later than ESP32
static IRAM_ATTR esp_err_t test_polling_send(spi_device_handle_t handle)
{
    for (int i = 0; i < 10; i++) {
        spi_transaction_t trans = {
            .length = 16,
            .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
        };
        esp_err_t err = spi_device_polling_transmit(handle, &trans);
        if (err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

static IRAM_ATTR NOINLINE_ATTR void test_acquire(spi_device_handle_t handle)
{
    esp_err_t err = spi_device_acquire_bus(handle, portMAX_DELAY);
    if (err == ESP_OK) {
        err = test_polling_send(handle);
        spi_device_release_bus(handle);
    }

    TEST_ESP_OK(err);
}

TEST_CASE("spi master can be used on SPI1", "[spi]")
{
    spi_device_interface_config_t dev_cfg = {
        .mode = 1,
        .clock_speed_hz = 1 * 1000 * 1000,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    spi_device_handle_t handle;
    esp_err_t err;
    err = spi_bus_add_device(SPI1_HOST, &dev_cfg, &handle);
    TEST_ESP_OK(err);

    err = test_polling_send(handle);
    TEST_ESP_OK(err);
    test_acquire(handle);

    err = spi_bus_remove_device(handle);
    TEST_ESP_OK(err);
}
#endif  //disable, SPI1 is not available for GPSPI usage on chips later than ESP32

//TODO: add a case when a non-polling transaction happened in the bus-acquiring time and then release the bus then queue a new trans

#endif //!(CONFIG_SPIRAM && CONFIG_IDF_TARGET_ESP32)
