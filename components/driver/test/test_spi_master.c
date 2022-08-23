/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi_master device driver
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/spi_periph.h"
#include "test_utils.h"
#include "test/test_common_spi.h"
#include "soc/gpio_periph.h"
#include "sdkconfig.h"
#include "esp_private/cache_utils.h"
#include "soc/soc_memory_layout.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/esp_clk.h"
#include "test_utils.h"


const static char TAG[] = "test_spi";

// There is no input-only pin on esp32c3 and esp32s3
#define TEST_SOC_HAS_INPUT_ONLY_PINS  (!DISABLED_FOR_TARGETS(ESP32C3, ESP32S3, ESP32C2))

static void check_spi_pre_n_for(int clk, int pre, int n)
{
    spi_device_handle_t handle;

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = clk,
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3
    };
    char sendbuf[16] = "";
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &handle));

    t.length = 16 * 8;
    t.tx_buffer = sendbuf;
    TEST_ESP_OK(spi_device_transmit(handle, &t));

    spi_dev_t *hw = spi_periph_signal[TEST_SPI_HOST].hw;

    printf("Checking clk rate %dHz. expect pre %d n %d, got pre %d n %d\n", clk, pre, n, hw->clock.clkdiv_pre + 1, hw->clock.clkcnt_n + 1);

    TEST_ASSERT(hw->clock.clkcnt_n + 1 == n);
    TEST_ASSERT(hw->clock.clkdiv_pre + 1 == pre);

    TEST_ESP_OK(spi_bus_remove_device(handle));
}

#define TEST_CLK_TIMES              8
/**
 * In this test, SPI Clock Calculation:
 *   Fspi = Fclk_spi_mst / (pre + n)
 *
 * For each item:
 * {freq, pre, n}
 */
#define TEST_CLK_PARAM_APB_80       {{1, SOC_SPI_MAX_PRE_DIVIDER, 64}, {100000, 16, 50}, {333333, 4, 60}, {800000, 2, 50}, {900000, 2, 44}, {8000000, 1, 10}, {20000000, 1, 4}, {26000000, 1, 3} }
#define TEST_CLK_PARAM_APB_40       {{1, SOC_SPI_MAX_PRE_DIVIDER, 64}, {100000, 8, 50}, {333333, 2, 60}, {800000, 1, 50}, {900000, 1, 44}, {8000000, 1, 5}, {10000000, 1, 4}, {20000000, 1, 2} }

TEST_CASE("SPI Master clockdiv calculation routines", "[spi]")
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    uint32_t apb_freq_hz = esp_clk_apb_freq();
    if (apb_freq_hz == (80 * 1000 * 1000)) {
        uint32_t clk_param[TEST_CLK_TIMES][3] = TEST_CLK_PARAM_APB_80;
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param[i][0], clk_param[i][1], clk_param[i][2]);
        }
    } else {
        TEST_ASSERT(apb_freq_hz == (40 * 1000 * 1000));
        uint32_t clk_param[TEST_CLK_TIMES][3] = TEST_CLK_PARAM_APB_40;
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param[i][0], clk_param[i][1], clk_param[i][2]);
        }
    }

    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
}

static spi_device_handle_t setup_spi_bus_loopback(int clkspeed, bool dma)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096 * 3
    };
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = clkspeed,
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
    };
    spi_device_handle_t handle;

    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, dma ? SPI_DMA_CH_AUTO : 0));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &handle));
    //connect MOSI to two devices breaks the output, fix it.
    spitest_gpio_output_sel(PIN_NUM_MOSI, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    printf("Bus/dev inited.\n");
    return handle;
}

static int spi_test(spi_device_handle_t handle, int num_bytes)
{
    esp_err_t ret;
    int x;
    bool success = true;
    srand(num_bytes);
    char *sendbuf = heap_caps_malloc((num_bytes + 3) & (~3), MALLOC_CAP_DMA);
    char *recvbuf = heap_caps_malloc((num_bytes + 3) & (~3), MALLOC_CAP_DMA);
    for (x = 0; x < num_bytes; x++) {
        sendbuf[x] = rand() & 0xff;
        recvbuf[x] = 0x55;
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = num_bytes * 8;
    t.tx_buffer = sendbuf;
    t.rx_buffer = recvbuf;
    t.addr = 0xA00000000000000FL;
    t.cmd = 0x55;

    printf("Transmitting %d bytes...\n", num_bytes);
    ret = spi_device_transmit(handle, &t);
    TEST_ASSERT(ret == ESP_OK);

    srand(num_bytes);
    for (x = 0; x < num_bytes; x++) {
        if (sendbuf[x] != (rand() & 0xff)) {
            printf("Huh? Sendbuf corrupted at byte %d\n", x);
            TEST_ASSERT(0);
        }
        if (sendbuf[x] != recvbuf[x]) {
            break;
        }
    }
    if (x != num_bytes) {
        int from = x - 16;
        if (from < 0) {
            from = 0;
        }
        success = false;
        printf("Error at %d! Sent vs recved: (starting from %d)\n", x, from);
        for (int i = 0; i < 32; i++) {
            if (i + from < num_bytes) {
                printf("%02X ", sendbuf[from + i]);
            }
        }
        printf("\n");
        for (int i = 0; i < 32; i++) {
            if (i + from < num_bytes) {
                printf("%02X ", recvbuf[from + i]);
            }
        }
        printf("\n");
    }

    if (success) {
        printf("Success!\n");
    }
    free(sendbuf);
    free(recvbuf);
    return success;
}

TEST_CASE("SPI Master test", "[spi]")
{
    bool success = true;
    printf("Testing bus at 80KHz\n");
    spi_device_handle_t handle = setup_spi_bus_loopback(80000, true);
    success &= spi_test(handle, 16); //small
    success &= spi_test(handle, 21); //small, unaligned
    success &= spi_test(handle, 36); //aligned
    success &= spi_test(handle, 128); //aligned
    success &= spi_test(handle, 129); //unaligned
    success &= spi_test(handle, 4096 - 2); //multiple descs, edge case 1
    success &= spi_test(handle, 4096 - 1); //multiple descs, edge case 2
    success &= spi_test(handle, 4096 * 3); //multiple descs

    master_free_device_bus(handle);

    printf("Testing bus at 80KHz, non-DMA\n");
    handle = setup_spi_bus_loopback(80000, false);
    success &= spi_test(handle, 4); //aligned
    success &= spi_test(handle, 16); //small
    success &= spi_test(handle, 21); //small, unaligned
    success &= spi_test(handle, 32); //small
    success &= spi_test(handle, 47); //small, unaligned
    success &= spi_test(handle, 63); //small
    success &= spi_test(handle, 64); //small, unaligned

    master_free_device_bus(handle);

    printf("Testing bus at 26MHz\n");
    handle = setup_spi_bus_loopback(20000000, true);

    success &= spi_test(handle, 128); //DMA, aligned
    success &= spi_test(handle, 4096 * 3); //DMA, multiple descs
    master_free_device_bus(handle);

    printf("Testing bus at 900KHz\n");
    handle = setup_spi_bus_loopback(9000000, true);

    success &= spi_test(handle, 128); //DMA, aligned
    success &= spi_test(handle, 4096 * 3); //DMA, multiple descs
    master_free_device_bus(handle);
    TEST_ASSERT(success);
}


TEST_CASE("SPI Master test, interaction of multiple devs", "[spi]")
{
    esp_err_t ret;
    bool success = true;
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 1000000,
        .duty_cycle_pos = 128,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
    };
    spi_device_handle_t handle1 = setup_spi_bus_loopback(80000, true);
    spi_device_handle_t handle2;
    spi_bus_add_device(TEST_SPI_HOST, &devcfg, &handle2);

    printf("Sending to dev 1\n");
    success &= spi_test(handle1, 7);
    printf("Sending to dev 1\n");
    success &= spi_test(handle1, 15);
    printf("Sending to dev 2\n");
    success &= spi_test(handle2, 15);
    printf("Sending to dev 1\n");
    success &= spi_test(handle1, 32);
    printf("Sending to dev 2\n");
    success &= spi_test(handle2, 32);
    printf("Sending to dev 1\n");
    success &= spi_test(handle1, 63);
    printf("Sending to dev 2\n");
    success &= spi_test(handle2, 63);
    printf("Sending to dev 1\n");
    success &= spi_test(handle1, 5000);
    printf("Sending to dev 2\n");
    success &= spi_test(handle2, 5000);


    ret = spi_bus_remove_device(handle2);
    TEST_ASSERT(ret == ESP_OK);
    master_free_device_bus(handle1);
    TEST_ASSERT(success);
}

#if TEST_SOC_HAS_INPUT_ONLY_PINS  //There is no input-only pin, so this test could be ignored.
static esp_err_t test_master_pins(int mosi, int miso, int sclk, int cs)
{
    esp_err_t ret;
    spi_bus_config_t cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    cfg.mosi_io_num = mosi;
    cfg.miso_io_num = miso;
    cfg.sclk_io_num = sclk;

    spi_device_interface_config_t master_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    master_cfg.spics_io_num = cs;

    ret = spi_bus_initialize(TEST_SPI_HOST, &cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        return ret;
    }

    spi_device_handle_t spi;
    ret = spi_bus_add_device(TEST_SPI_HOST, &master_cfg, &spi);
    if (ret != ESP_OK) {
        spi_bus_free(TEST_SPI_HOST);
        return ret;
    }

    master_free_device_bus(spi);
    return ESP_OK;
}

static esp_err_t test_slave_pins(int mosi, int miso, int sclk, int cs)
{
    esp_err_t ret;
    spi_bus_config_t cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    cfg.mosi_io_num = mosi;
    cfg.miso_io_num = miso;
    cfg.sclk_io_num = sclk;

    spi_slave_interface_config_t slave_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slave_cfg.spics_io_num = cs;

    ret = spi_slave_initialize(TEST_SLAVE_HOST, &cfg, &slave_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        return ret;
    }

    spi_slave_free(TEST_SLAVE_HOST);
    return ESP_OK;
}

TEST_CASE("spi placed on input-only pins", "[spi]")
{
    TEST_ESP_OK(test_master_pins(PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS));
    TEST_ASSERT(test_master_pins(INPUT_ONLY_PIN, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS) != ESP_OK);
    TEST_ESP_OK(test_master_pins(PIN_NUM_MOSI, INPUT_ONLY_PIN, PIN_NUM_CLK, PIN_NUM_CS));
    TEST_ASSERT(test_master_pins(PIN_NUM_MOSI, PIN_NUM_MISO, INPUT_ONLY_PIN, PIN_NUM_CS) != ESP_OK);
    TEST_ASSERT(test_master_pins(PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, INPUT_ONLY_PIN) != ESP_OK);
    TEST_ESP_OK(test_slave_pins(PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS));
    TEST_ESP_OK(test_slave_pins(INPUT_ONLY_PIN, PIN_NUM_MISO, PIN_NUM_CLK, PIN_NUM_CS));
    TEST_ASSERT(test_slave_pins(PIN_NUM_MOSI, INPUT_ONLY_PIN, PIN_NUM_CLK, PIN_NUM_CS) != ESP_OK);
    TEST_ESP_OK(test_slave_pins(PIN_NUM_MOSI, PIN_NUM_MISO, INPUT_ONLY_PIN, PIN_NUM_CS));
    TEST_ESP_OK(test_slave_pins(PIN_NUM_MOSI, PIN_NUM_MISO, PIN_NUM_CLK, INPUT_ONLY_PIN));
}

//There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
#endif  //#if TEST_SOC_HAS_INPUT_ONLY_PINS

TEST_CASE("spi bus setting with different pin configs", "[spi]")
{
    spi_bus_config_t cfg;
    uint32_t flags_o;
    uint32_t flags_expected;

    ESP_LOGI(TAG, "test 6 iomux output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_IOMUX_PINS | SPICOMMON_BUSFLAG_QUAD;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 4 iomux output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_IOMUX_PINS | SPICOMMON_BUSFLAG_DUAL;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_QUAD | SPICOMMON_BUSFLAG_GPIO_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO |  SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

#if TEST_SOC_HAS_INPUT_ONLY_PINS  //There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
    ESP_LOGI(TAG, "test master 5 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test slave 5 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test master 3 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_GPIO_PINS;

    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test slave 3 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
//There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
#endif //#if TEST_SOC_HAS_INPUT_ONLY_PINS

    ESP_LOGI(TAG, "check native flag for 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_IOMUX_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check native flag for 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_IOMUX_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

#if TEST_SOC_HAS_INPUT_ONLY_PINS  //There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
    ESP_LOGI(TAG, "check dual flag for master 5 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check dual flag for master 3 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
//There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
#endif //#if TEST_SOC_HAS_INPUT_ONLY_PINS

    ESP_LOGI(TAG, "check sclk flag...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = -1, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check mosi flag...");
    flags_expected = SPICOMMON_BUSFLAG_MOSI;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = -1, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check miso flag...");
    flags_expected = SPICOMMON_BUSFLAG_MISO;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = -1, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check quad flag...");
    flags_expected = SPICOMMON_BUSFLAG_QUAD;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o));
}

TEST_CASE("SPI Master no response when switch from host1 (SPI2) to host2 (SPI3)", "[spi]")
{
    //spi config
    spi_bus_config_t bus_config;
    spi_device_interface_config_t device_config;
    spi_device_handle_t spi;
    spi_host_device_t host;

    memset(&bus_config, 0, sizeof(spi_bus_config_t));
    memset(&device_config, 0, sizeof(spi_device_interface_config_t));

    bus_config.miso_io_num = -1;
    bus_config.mosi_io_num = PIN_NUM_MOSI;
    bus_config.sclk_io_num = PIN_NUM_CLK;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;

    device_config.clock_speed_hz = 50000;
    device_config.mode = 0;
    device_config.spics_io_num = -1;
    device_config.queue_size = 1;
    device_config.flags = SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST;

    struct spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
        .length = 16,
        .rx_buffer = NULL,
        .tx_data = {0x04, 0x00}
    };

    //initialize for first host
    host = TEST_SPI_HOST;

    TEST_ESP_OK(spi_bus_initialize(host, &bus_config, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(host, &device_config, &spi));

    printf("before first xmit\n");
    TEST_ESP_OK(spi_device_transmit(spi, &transaction));
    printf("after first xmit\n");

    TEST_ESP_OK(spi_bus_remove_device(spi));
    TEST_ESP_OK(spi_bus_free(host));

    //for second host and failed before
    host = TEST_SLAVE_HOST;
    TEST_ESP_OK(spi_bus_initialize(host, &bus_config, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(host, &device_config, &spi));

    printf("before second xmit\n");
    // the original version (bit mis-written) stucks here.
    TEST_ESP_OK(spi_device_transmit(spi, &transaction));
    // test case success when see this.
    printf("after second xmit\n");

    TEST_ESP_OK(spi_bus_remove_device(spi));
    TEST_ESP_OK(spi_bus_free(host));
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5146
DRAM_ATTR  static uint32_t data_dram[80] = {0};
//force to place in code area.
static const uint8_t data_drom[320 + 3] = {
    0xD8, 0xD1, 0x0A, 0xB8, 0xCE, 0x67, 0x1B, 0x11, 0x17, 0xA0, 0xDA, 0x89, 0x55, 0xC1, 0x40, 0x0F, 0x55, 0xEB, 0xF7, 0xEC, 0xF0, 0x3C, 0x0F, 0x4D, 0x2B, 0x9E, 0xBF, 0xCD, 0x57, 0x2C, 0x48, 0x1A,
    0x8B, 0x47, 0xC5, 0x01, 0x0C, 0x05, 0x80, 0x30, 0xF4, 0xEA, 0xE5, 0x92, 0x56, 0x97, 0x98, 0x78, 0x21, 0x34, 0xA1, 0xBC, 0xAE, 0x93, 0x7E, 0x96, 0x08, 0xE6, 0x54, 0x6A, 0x6C, 0x67, 0xCF, 0x58,
    0xEE, 0x15, 0xA8, 0xB6, 0x32, 0x8C, 0x85, 0xF7, 0xE9, 0x88, 0x5E, 0xB1, 0x76, 0xE4, 0xB2, 0xC7, 0x0F, 0x57, 0x51, 0x7A, 0x2F, 0xAB, 0x12, 0xC3, 0x37, 0x99, 0x4E, 0x67, 0x75, 0x28, 0xE4, 0x1D,
    0xF8, 0xBA, 0x22, 0xCB, 0xA1, 0x18, 0x4C, 0xAB, 0x5F, 0xC9, 0xF3, 0xA2, 0x39, 0x92, 0x44, 0xE6, 0x7B, 0xE3, 0xD0, 0x16, 0xC5, 0xC2, 0xCB, 0xD9, 0xC0, 0x7F, 0x06, 0xBF, 0x3E, 0xCE, 0xE1, 0x26,
    0xD5, 0x3C, 0xAD, 0x0E, 0xC1, 0xC7, 0x7D, 0x0D, 0x56, 0x85, 0x6F, 0x32, 0xC8, 0x63, 0x8D, 0x12, 0xAB, 0x1E, 0x81, 0x7B, 0xF4, 0xF1, 0xA9, 0xAF, 0xD9, 0x74, 0x60, 0x05, 0x3D, 0xCC, 0x0C, 0x34,
    0x11, 0x44, 0xAE, 0x2A, 0x13, 0x2F, 0x04, 0xC3, 0x59, 0xF0, 0x54, 0x07, 0xBA, 0x26, 0xD9, 0xFB, 0x80, 0x95, 0xC0, 0x14, 0xFA, 0x27, 0xEF, 0xD3, 0x58, 0xB8, 0xE4, 0xA2, 0xE3, 0x5E, 0x94, 0xB3,
    0xCD, 0x2C, 0x4F, 0xAC, 0x3B, 0xD1, 0xCA, 0xBE, 0x61, 0x71, 0x7B, 0x62, 0xEB, 0xF0, 0xFC, 0xEF, 0x22, 0xB7, 0x3F, 0x56, 0x65, 0x19, 0x61, 0x73, 0x1A, 0x4D, 0xE4, 0x23, 0xE5, 0x3A, 0x91, 0x5C,
    0xE6, 0x1B, 0x5F, 0x0E, 0x10, 0x94, 0x7C, 0x9F, 0xCF, 0x75, 0xB3, 0xEB, 0x42, 0x4C, 0xCF, 0xFE, 0xAF, 0x68, 0x62, 0x3F, 0x9A, 0x3C, 0x81, 0x3E, 0x7A, 0x45, 0x92, 0x79, 0x91, 0x4F, 0xFF, 0xDE,
    0x25, 0x18, 0x33, 0xB9, 0xA9, 0x3A, 0x3F, 0x1F, 0x4F, 0x4B, 0x5C, 0x71, 0x82, 0x75, 0xB0, 0x1F, 0xE9, 0x98, 0xA3, 0xE2, 0x65, 0xBB, 0xCA, 0x4F, 0xB7, 0x1D, 0x23, 0x43, 0x16, 0x73, 0xBD, 0x83,
    0x70, 0x22, 0x7D, 0x0A, 0x6D, 0xD3, 0x77, 0x73, 0xD0, 0xF4, 0x06, 0xB2, 0x19, 0x8C, 0xFF, 0x58, 0xE4, 0xDB, 0xE9, 0xEC, 0x89, 0x6A, 0xF4, 0x0E, 0x67, 0x12, 0xEC, 0x11, 0xD2, 0x1F, 0x8D, 0xD7,
};

TEST_CASE("SPI Master DMA test, TX and RX in different regions", "[spi]")
{
#ifdef CONFIG_SPIRAM
    //test psram if enabled
    ESP_LOGI(TAG, "testing PSRAM...");
    uint32_t *data_malloc = (uint32_t *)heap_caps_malloc(324, MALLOC_CAP_SPIRAM);
    TEST_ASSERT(esp_ptr_external_ram(data_malloc));
#else
    uint32_t *data_malloc = (uint32_t *)heap_caps_malloc(324, MALLOC_CAP_DMA);
    TEST_ASSERT(esp_ptr_in_dram(data_malloc));
#endif
    TEST_ASSERT(data_malloc != NULL);
    TEST_ASSERT(esp_ptr_in_dram(data_dram));
    TEST_ASSERT(esp_ptr_in_drom(data_drom));
    ESP_LOGI(TAG, "dram: %p", data_dram);
    ESP_LOGI(TAG, "drom: %p, malloc: %p", data_drom, data_malloc);

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    uint32_t *data_iram = (uint32_t *)heap_caps_malloc(324, MALLOC_CAP_EXEC);
    TEST_ASSERT(data_iram != NULL);
    TEST_ASSERT(esp_ptr_executable(data_iram) || esp_ptr_in_iram(data_iram) || esp_ptr_in_diram_iram(data_iram));
    ESP_LOGI(TAG, "iram: %p", data_iram);
#endif

    srand(52);
    for (int i = 0; i < 320 / 4; i++) {
#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
        data_iram[i] = rand();
#endif
        data_dram[i] = rand();
        data_malloc[i] = rand();
    }

    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.miso_io_num = PIN_NUM_MOSI;
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();

    //Initialize the SPI bus
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    //Attach the LCD to the SPI bus
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &spi));
    //connect MOSI to two devices breaks the output, fix it.
    spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);

#define TEST_REGION_SIZE 5
    static spi_transaction_t trans[TEST_REGION_SIZE];
    int x;
    memset(trans, 0, sizeof(trans));

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    trans[0].length = 320 * 8,
            trans[0].tx_buffer = data_iram;
    trans[0].rx_buffer = data_malloc + 1;

    trans[1].length = 320 * 8,
            trans[1].tx_buffer = data_dram;
    trans[1].rx_buffer = data_iram;

    trans[2].length = 320 * 8,
            trans[2].tx_buffer = data_drom;
    trans[2].rx_buffer = data_iram;
#endif

    trans[3].length = 320 * 8,
            trans[3].tx_buffer = data_malloc + 2;
    trans[3].rx_buffer = data_dram;

    trans[4].length = 4 * 8,
            trans[4].flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    uint32_t *ptr = (uint32_t *)trans[4].rx_data;
    *ptr = 0x54545454;
    ptr = (uint32_t *)trans[4].tx_data;
    *ptr = 0xbc124960;

    //Queue all transactions.
#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    for (x = 0; x < TEST_REGION_SIZE; x++) {
#else
    for (x = 3; x < TEST_REGION_SIZE; x++) {
#endif
        ESP_LOGI(TAG, "transmitting %d...", x);
        ret = spi_device_transmit(spi, &trans[x]);
        TEST_ASSERT(ret == ESP_OK);
        if (trans[x].flags & SPI_TRANS_USE_RXDATA) {
            TEST_ASSERT_EQUAL_HEX8_ARRAY(trans[x].tx_data, trans[x].rx_data, 4);
        } else {
            TEST_ASSERT_EQUAL_HEX32_ARRAY(trans[x].tx_buffer, trans[x].rx_buffer, trans[x].length / 8 / 4);
        }
    }
    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
    free(data_malloc);
#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    free(data_iram);
#endif
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

//this part tests 3 DMA issues in master mode, full-duplex in IDF2.1
// 1. RX buffer not aligned (start and end)
// 2. not setting rx_buffer
// 3. setting rx_length != length
TEST_CASE("SPI Master DMA test: length, start, not aligned", "[spi]")
{
    uint8_t tx_buf[320] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43};
    uint8_t rx_buf[320];

    spi_device_handle_t spi;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MOSI,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,         //Clock out at 10 MHz
        .mode = 0,                              //SPI mode 0
        .spics_io_num = PIN_NUM_CS,             //CS pin
        .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
        .pre_cb = NULL,
    };
    //Initialize the SPI bus
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    //Attach the LCD to the SPI bus
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &spi));

    //connect MOSI to two devices breaks the output, fix it.
    spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);

    memset(rx_buf, 0x66, 320);

    for ( int i = 0; i < 8; i ++ ) {
        memset( rx_buf, 0x66, sizeof(rx_buf));

        spi_transaction_t t = {};
        t.length = 8 * (i + 1);
        t.rxlength = 0;
        t.tx_buffer = tx_buf + 2 * i;
        t.rx_buffer = rx_buf + i;

        if ( i == 1 ) {
            //test set no start
            t.rx_buffer = NULL;
        } else if ( i == 2 ) {
            //test rx length != tx_length
            t.rxlength = t.length - 8;
        }
        spi_device_transmit( spi, &t );

        for ( int i = 0; i < 16; i ++ ) {
            printf("%02X ", rx_buf[i]);
        }
        printf("\n");

        if ( i == 1 ) {
            // no rx, skip check
        } else if ( i == 2 ) {
            //test rx length = tx length-1
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length / 8 - 1 );
        } else {
            //normal check
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length / 8 );
        }
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}


#if (TEST_SPI_PERIPH_NUM >= 2)
//These will only be enabled on chips with 2 or more SPI peripherals

static uint8_t bitswap(uint8_t in)
{
    uint8_t out = 0;
    for (int i = 0; i < 8; i++) {
        out = out >> 1;
        if (in & 0x80) {
            out |= 0x80;
        }
        in = in << 1;
    }
    return out;
}

void test_cmd_addr(spi_slave_task_context_t *slave_context, bool lsb_first)
{
    spi_device_handle_t spi;

    ESP_LOGI(MASTER_TAG, ">>>>>>>>> TEST %s FIRST <<<<<<<<<<<", lsb_first ? "LSB" : "MSB");

    //initial master, mode 0, 1MHz
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.quadhd_io_num = UNCONNECTED_PIN;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.clock_speed_hz = 1 * 1000 * 1000;
    if (lsb_first) {
        devcfg.flags |= SPI_DEVICE_BIT_LSBFIRST;
    }
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &spi));

    //connecting pins to two peripherals breaks the output, fix it.
    spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
    spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
    spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spiclk_out);

    for (int i = 0; i < 8; i++) {
        //prepare slave tx data
        slave_txdata_t slave_txdata = (slave_txdata_t) {
            .start = spitest_slave_send + 4 * (i % 3),
            .len = 256,
        };
        xQueueSend(slave_context->data_to_send, &slave_txdata, portMAX_DELAY);

        vTaskDelay(50);
        //prepare master tx data
        int cmd_bits = (i + 1) * 2;
        int addr_bits =
#ifdef CONFIG_IDF_TARGET_ESP32
            56 - 8 * i;
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
            //ESP32S2 only supportes up to 32 bits address
            28 - 4 * i;
#endif
        int round_up = (cmd_bits + addr_bits + 7) / 8 * 8;
        addr_bits = round_up - cmd_bits;

        spi_transaction_ext_t trans = (spi_transaction_ext_t) {
            .base = {
                .flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR,
                .addr = 0x456789abcdef0123,
                .cmd = 0x9876,
            },
            .command_bits = cmd_bits,
            .address_bits = addr_bits,
        };

        ESP_LOGI( MASTER_TAG, "===== test%d =====", i );
        ESP_LOGI(MASTER_TAG, "cmd_bits: %d, addr_bits: %d", cmd_bits, addr_bits);
        TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t *)&trans));
        //wait for both master and slave end

        size_t rcv_len;
        slave_rxdata_t *rcv_data = xRingbufferReceive(slave_context->data_received, &rcv_len, portMAX_DELAY);
        rcv_len -= 8;
        uint8_t *buffer = rcv_data->data;

        ESP_LOGI(SLAVE_TAG, "trans_len: %d", rcv_len);
        TEST_ASSERT_EQUAL(rcv_len, (rcv_data->len + 7) / 8);
        TEST_ASSERT_EQUAL(rcv_data->len, cmd_bits + addr_bits);
        ESP_LOG_BUFFER_HEX("slave rx", buffer, rcv_len);

        uint16_t cmd_expected = trans.base.cmd & (BIT(cmd_bits) - 1);
        uint64_t addr_expected = trans.base.addr & ((1ULL << addr_bits) - 1);

        uint8_t *data_ptr = buffer;
        uint16_t cmd_got = *(uint16_t *)data_ptr;
        data_ptr += cmd_bits / 8;
        cmd_got = __builtin_bswap16(cmd_got);
        cmd_got = cmd_got >> (16 - cmd_bits);
        int remain_bits = cmd_bits % 8;

        uint64_t addr_got = *(uint64_t *)data_ptr;
        data_ptr += 8;
        addr_got = __builtin_bswap64(addr_got);
        addr_got = (addr_got << remain_bits);
        addr_got |= (*data_ptr >> (8 - remain_bits));
        addr_got = addr_got >> (64 - addr_bits);

        if (lsb_first) {
            cmd_got = __builtin_bswap16(cmd_got);
            addr_got = __builtin_bswap64(addr_got);

            uint8_t *swap_ptr = (uint8_t *)&cmd_got;
            swap_ptr[0] = bitswap(swap_ptr[0]);
            swap_ptr[1] = bitswap(swap_ptr[1]);
            cmd_got = cmd_got >> (16 - cmd_bits);

            swap_ptr = (uint8_t *)&addr_got;
            for (int j = 0; j < 8; j++) {
                swap_ptr[j] = bitswap(swap_ptr[j]);
            }
            addr_got = addr_got >> (64 - addr_bits);
        }

        ESP_LOGI(SLAVE_TAG, "cmd_got: %04X, addr_got: %08X%08X", cmd_got, (uint32_t)(addr_got >> 32), (uint32_t)addr_got);

        TEST_ASSERT_EQUAL_HEX16(cmd_expected, cmd_got);
        if (addr_bits > 0) {
            TEST_ASSERT_EQUAL_HEX32(addr_expected, addr_got);
            TEST_ASSERT_EQUAL_HEX32(addr_expected >> 8, addr_got >> 8);
        }

        //clean
        vRingbufferReturnItem(slave_context->data_received, rcv_data);
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}

TEST_CASE("SPI master variable cmd & addr test", "[spi]")
{
    spi_slave_task_context_t slave_context = {};
    esp_err_t err = init_slave_context( &slave_context );
    TEST_ASSERT( err == ESP_OK );
    TaskHandle_t handle_slave;
    xTaskCreate( spitest_slave_task, "spi_slave", 4096, &slave_context, 0, &handle_slave);

    //initial slave, mode 0, no dma
    int dma_chan = 0;
    int slave_mode = 0;
    spi_bus_config_t slv_buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.mode = slave_mode;
    //Initialize SPI slave interface
    TEST_ESP_OK( spi_slave_initialize(TEST_SLAVE_HOST, &slv_buscfg, &slvcfg, dma_chan) );

    test_cmd_addr(&slave_context, false);
    test_cmd_addr(&slave_context, true);

    vTaskDelete( handle_slave );
    handle_slave = 0;

    deinit_slave_context(&slave_context);

    TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);

    ESP_LOGI(MASTER_TAG, "test passed.");
}

void test_dummy(spi_device_handle_t spi, int dummy_n, uint8_t *data_to_send, int len)
{
    ESP_LOGI(TAG, "testing dummy n=%d", dummy_n);
    WORD_ALIGNED_ATTR uint8_t slave_buffer[len + (dummy_n + 7) / 8];
    spi_slave_transaction_t slave_t = {
        .tx_buffer = slave_buffer,
        .rx_buffer = slave_buffer,
        .length = len * 8 + ((dummy_n + 7) & (~8)) + 32, //receive more bytes to avoid slave discarding data
    };
    TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

    vTaskDelay(50);

    spi_transaction_ext_t t = {
        .base = {
            .tx_buffer = data_to_send,
            .length = (len + 1) * 8, //send one more byte force slave receive all data
            .flags = SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = dummy_n,
    };
    TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t *)&t));

    spi_slave_transaction_t *ret_slave;
    TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_slave, portMAX_DELAY));
    TEST_ASSERT(ret_slave == &slave_t);

    ESP_LOG_BUFFER_HEXDUMP("rcv", slave_buffer, len + 4, ESP_LOG_INFO);
    int skip_cnt = dummy_n / 8;
    int dummy_remain = dummy_n % 8;
    uint8_t *slave_ptr = slave_buffer;
    if (dummy_remain > 0) {
        for (int i = 0; i < len; i++) {
            slave_ptr[0] = (slave_ptr[skip_cnt] << dummy_remain) | (slave_ptr[skip_cnt + 1] >> (8 - dummy_remain));
            slave_ptr++;
        }
    } else {
        for (int i = 0; i < len; i++) {
            slave_ptr[0] = slave_ptr[skip_cnt];
            slave_ptr++;
        }
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_to_send, slave_buffer, len);
}

TEST_CASE("SPI master variable dummy test", "[spi]")
{
    spi_device_handle_t spi;
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX;

    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, 0));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    spi_slave_interface_config_t slave_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slave_cfg, 0));

    spitest_gpio_output_sel(bus_cfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    spitest_gpio_output_sel(bus_cfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
    spitest_gpio_output_sel(dev_cfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
    spitest_gpio_output_sel(bus_cfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spiclk_out);

    uint8_t data_to_send[] = {0x12, 0x34, 0x56, 0x78};

    test_dummy(spi, 0, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 1, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 2, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 3, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 4, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 8, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 12, data_to_send, sizeof(data_to_send));
    test_dummy(spi, 16, data_to_send, sizeof(data_to_send));

    spi_slave_free(TEST_SLAVE_HOST);
    master_free_device_bus(spi);
}

/**
 * This test is to check when the first transaction of the HD master is to send data without receiving data via DMA,
 * then if the master could receive data correctly.
 *
 * Because an old version ESP32 silicon issue, there is a workaround to enable and start the RX DMA in FD/HD mode in
 * this condition (TX without RX). And if RX DMA is enabled and started in HD mode, because there is no correctly
 * linked RX DMA descriptor, there will be an inlink_dscr_error interrupt emerging, which will influence the following
 * RX transactions.
 *
 * This bug is fixed by triggering this workaround only in FD mode.
 *
 */
TEST_CASE("SPI master hd dma TX without RX test", "[spi]")
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t spi;
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX;
    dev_cfg.clock_speed_hz = 1 * 1000 * 1000;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    spi_slave_interface_config_t slave_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO));

    same_pin_func_sel(bus_cfg, dev_cfg, 0);

    uint32_t buf_size = 32;
    uint8_t *mst_send_buf = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    uint8_t *mst_recv_buf = heap_caps_calloc(buf_size, 1, MALLOC_CAP_DMA);
    uint8_t *slv_send_buf = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    uint8_t *slv_recv_buf = heap_caps_calloc(buf_size, 1, MALLOC_CAP_DMA);

    srand(199);
    for (int i = 0; i < buf_size; i++) {
        mst_send_buf[i] = rand();
    }

    //1. Master sends without receiving, no rx_buffer is set
    spi_slave_transaction_t slave_trans = {
        .rx_buffer = slv_recv_buf,
        .length = buf_size * 8,
    };
    TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_trans, portMAX_DELAY));

    spi_transaction_t master_trans = {
        .tx_buffer = mst_send_buf,
        .length = buf_size * 8,
    };
    TEST_ESP_OK(spi_device_transmit(spi, &master_trans));

    spi_slave_transaction_t *ret_slave;
    TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_slave, portMAX_DELAY));

    spitest_cmp_or_dump(mst_send_buf, slv_recv_buf, buf_size);

    //2. Master receives data
    for (int i = 100; i < 110; i++) {

        srand(i);
        for (int j = 0; j < buf_size; j++) {
            slv_send_buf[j] = rand();
        }
        slave_trans = (spi_slave_transaction_t) {};
        slave_trans.tx_buffer = slv_send_buf;
        slave_trans.length = buf_size * 8;
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_trans, portMAX_DELAY));

        vTaskDelay(50);

        master_trans = (spi_transaction_t) {};
        master_trans.rx_buffer = mst_recv_buf;
        master_trans.rxlength = buf_size * 8;
        TEST_ESP_OK(spi_device_transmit(spi, &master_trans));

        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_slave, portMAX_DELAY));

        spitest_cmp_or_dump(slv_send_buf, mst_recv_buf, buf_size);
    }

    free(mst_send_buf);
    free(mst_recv_buf);
    free(slv_send_buf);
    free(slv_recv_buf);
    spi_slave_free(TEST_SLAVE_HOST);
    master_free_device_bus(spi);
}
#endif  //#if (TEST_SPI_PERIPH_NUM >= 2)

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32)    //TODO: IDF-3494
#define FD_TEST_BUF_SIZE    32
#define TEST_NUM            4
#define FD_SEED1            199
#define FD_SEED2            29
#define FD_SEED3            48
#define FD_SEED4            327

static void master_only_tx_trans(spi_device_handle_t spi, uint8_t *mst_send_buf, uint32_t length)
{
    ESP_LOGI(MASTER_TAG, "FD DMA, Only TX:");
    spi_transaction_t trans = {0};
    trans.tx_buffer = mst_send_buf;
    trans.length = length * 8;
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, &trans));
    ESP_LOG_BUFFER_HEX("MASTER TX:", mst_send_buf, length);
}

static void master_only_rx_trans(spi_device_handle_t spi, uint8_t *mst_recv_buf, uint8_t *slv_send_buf, uint32_t length)
{
    ESP_LOGI(MASTER_TAG, "FD DMA, Only RX:");
    spi_transaction_t trans = {0};
    trans.tx_buffer = NULL;
    trans.rx_buffer = mst_recv_buf;
    trans.length = length * 8;
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, &trans));
    ESP_LOG_BUFFER_HEX("MASTER RX:", mst_recv_buf, length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_send_buf, mst_recv_buf, length);
}

static void master_both_trans(spi_device_handle_t spi, uint8_t *mst_send_buf, uint8_t *mst_recv_buf, uint8_t *slv_send_buf, uint32_t length)
{
    ESP_LOGI(MASTER_TAG, "FD DMA, Both TX and RX:");
    spi_transaction_t trans = {0};
    trans.tx_buffer = mst_send_buf;
    trans.rx_buffer = mst_recv_buf;
    trans.length = length * 8;
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, &trans));
    ESP_LOG_BUFFER_HEX("MASTER TX:", mst_send_buf, length);
    ESP_LOG_BUFFER_HEX("MASTER RX:", mst_recv_buf, length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_send_buf, mst_recv_buf, length);
}

static void fd_master(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t spi;
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    unity_send_signal("Master ready");

    uint8_t *mst_send_buf = heap_caps_malloc(FD_TEST_BUF_SIZE, MALLOC_CAP_DMA);
    uint8_t *mst_recv_buf = heap_caps_calloc(FD_TEST_BUF_SIZE, 1, MALLOC_CAP_DMA);
    uint8_t *slv_send_buf = heap_caps_malloc(FD_TEST_BUF_SIZE, MALLOC_CAP_DMA);

    //Master FD DMA, RX without TX Test
    for (int i = 0; i < TEST_NUM; i++) {
        // 1. Master FD DMA, only receive, with NULL tx_buffer
        get_tx_buffer(FD_SEED1+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(mst_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        master_only_rx_trans(spi, mst_recv_buf, slv_send_buf, FD_TEST_BUF_SIZE);

        //2. Master FD DMA with TX and RX
        get_tx_buffer(FD_SEED2+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(mst_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        master_both_trans(spi, mst_send_buf, mst_recv_buf, slv_send_buf, FD_TEST_BUF_SIZE);
    }

    //Master FD DMA, TX without RX Test
    for (int i = 0; i < TEST_NUM; i++) {
        // 1. Master FD DMA, only send, with NULL rx_buffer
        get_tx_buffer(FD_SEED3+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        master_only_tx_trans(spi, mst_send_buf, FD_TEST_BUF_SIZE);

        //2. Master FD DMA with TX and RX
        get_tx_buffer(FD_SEED4+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(mst_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        master_both_trans(spi, mst_send_buf, mst_recv_buf, slv_send_buf, FD_TEST_BUF_SIZE);
    }

    free(mst_send_buf);
    free(mst_recv_buf);
    free(slv_send_buf);
    master_free_device_bus(spi);
}

static void slave_only_tx_trans(uint8_t *slv_send_buf, uint32_t length)
{
    ESP_LOGI(SLAVE_TAG, "FD DMA, Only TX");
    spi_slave_transaction_t trans = {0};
    trans.tx_buffer = slv_send_buf;
    trans.length = length * 8;
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_transmit(SPI2_HOST, &trans, portMAX_DELAY));
    ESP_LOG_BUFFER_HEX("SLAVE TX:", slv_send_buf, length);
}

static void slave_only_rx_trans(uint8_t *slv_recv_buf, uint8_t *mst_send_buf, uint32_t length)
{
    ESP_LOGI(SLAVE_TAG, "FD DMA, Only RX");
    spi_slave_transaction_t trans = {};
    trans.tx_buffer = NULL;
    trans.rx_buffer = slv_recv_buf;
    trans.length = length * 8;
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_transmit(SPI2_HOST, &trans, portMAX_DELAY));
    ESP_LOG_BUFFER_HEX("SLAVE RX:", slv_recv_buf, length);
    TEST_ASSERT_EQUAL(length * 8, trans.trans_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mst_send_buf, slv_recv_buf, length);
}

static void slave_both_trans(uint8_t *slv_send_buf, uint8_t *slv_recv_buf, uint8_t *mst_send_buf, uint32_t length)
{
    ESP_LOGI(SLAVE_TAG, "FD DMA, Both TX and RX:");
    spi_slave_transaction_t trans = {0};
    trans.tx_buffer = slv_send_buf;
    trans.rx_buffer = slv_recv_buf;
    trans.length = length * 8;
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_transmit(SPI2_HOST, &trans, portMAX_DELAY));
    ESP_LOG_BUFFER_HEX("SLAVE TX:", slv_send_buf, length);
    ESP_LOG_BUFFER_HEX("SLAVE RX:", slv_recv_buf, length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mst_send_buf, slv_recv_buf, length);
}

static void fd_slave(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();

    TEST_ESP_OK(spi_slave_initialize(SPI2_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));

    unity_wait_for_signal("Master ready");

    uint8_t *slv_send_buf = heap_caps_malloc(FD_TEST_BUF_SIZE, MALLOC_CAP_DMA);
    uint8_t *slv_recv_buf = heap_caps_calloc(FD_TEST_BUF_SIZE, 1, MALLOC_CAP_DMA);
    uint8_t *mst_send_buf = heap_caps_malloc(FD_TEST_BUF_SIZE, MALLOC_CAP_DMA);

    for (int i = 0; i < TEST_NUM; i++) {
        //1. Slave TX without RX (rx_buffer == NULL)
        get_tx_buffer(FD_SEED1+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        slave_only_tx_trans(slv_send_buf, FD_TEST_BUF_SIZE);

        //2. Slave both TX and RX
        get_tx_buffer(FD_SEED2+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_both_trans(slv_send_buf, slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);
    }

    for (int i = 0; i < TEST_NUM; i++) {
        // 1. Slave RX without TX (tx_buffer == NULL)
        get_tx_buffer(FD_SEED3+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_only_rx_trans(slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);

        //2. Slave both TX and RX
        get_tx_buffer(FD_SEED4+i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_both_trans(slv_send_buf, slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);
    }

    free(slv_send_buf);
    free(slv_recv_buf);
    free(mst_send_buf);
    TEST_ASSERT(spi_slave_free(SPI2_HOST) == ESP_OK);
}

TEST_CASE_MULTIPLE_DEVICES("SPI Master: FD, DMA, Master Single Direction Test", "[spi_ms][test_env=Example_SPI_Multi_device]", fd_master, fd_slave);
#endif  //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32)    //TODO: IDF-3494
#endif  //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)    //TODO: IDF-3494

//NOTE: Explained in IDF-1445 | MR !14996
#if !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)
/********************************************************************************
 *      Test SPI transaction interval
 ********************************************************************************/
//Disabled since the check in portENTER_CRITICAL in esp_intr_enable/disable increase the delay
#ifndef CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5146

#define RECORD_TIME_PREPARE() uint32_t __t1, __t2
#define RECORD_TIME_START()   do {__t1 = esp_cpu_get_cycle_count();}while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); *p_time = (__t2-__t1);}while(0)
#define GET_US_BY_CCOUNT(t) ((double)t/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

static void speed_setup(spi_device_handle_t *spi, bool use_dma)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.queue_size = 8;     //We want to be able to queue 7 transactions at a time

    //Initialize the SPI bus and the device to test
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, (use_dma ? SPI_DMA_CH_AUTO : 0)));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, spi));
}

static void sorted_array_insert(uint32_t *array, int *size, uint32_t item)
{
    int pos;
    for (pos = *size; pos > 0; pos--) {
        if (array[pos - 1] < item) {
            break;
        }
        array[pos] = array[pos - 1];
    }
    array[pos] = item;
    (*size)++;
}

#define TEST_TIMES  11

static IRAM_ATTR NOINLINE_ATTR void spi_transmit_measure(spi_device_handle_t spi, spi_transaction_t *trans, uint32_t *t_flight)
{
    RECORD_TIME_PREPARE();
    spi_device_transmit(spi, trans); // prime the flash cache
    RECORD_TIME_START();
    spi_device_transmit(spi, trans);
    RECORD_TIME_END(t_flight);
}

static IRAM_ATTR NOINLINE_ATTR void spi_transmit_polling_measure(spi_device_handle_t spi, spi_transaction_t *trans, uint32_t *t_flight)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();    //this can test the code are all in the IRAM at the same time
    RECORD_TIME_PREPARE();
    spi_device_polling_transmit(spi, trans); // prime the flash cache
    RECORD_TIME_START();
    spi_device_polling_transmit(spi, trans);
    RECORD_TIME_END(t_flight);
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

TEST_CASE("spi_speed", "[spi]")
{
    uint32_t t_flight;
    //to get rid of the influence of randomly interrupts, we measured the performance by median value
    uint32_t t_flight_sorted[TEST_TIMES];
    esp_err_t ret;
    int t_flight_num = 0;

    spi_device_handle_t spi;
    const bool use_dma = true;
    WORD_ALIGNED_ATTR spi_transaction_t trans = {
        .length = 1 * 8,
        .flags = SPI_TRANS_USE_TXDATA,
    };

    //first work with DMA
    speed_setup(&spi, use_dma);

    //record flight time by isr, with DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        spi_transmit_measure(spi, &trans, &t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%.2lf", GET_US_BY_CCOUNT(t_flight_sorted[i]));
    }
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(SPI_PER_TRANS_NO_POLLING, "%d us", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
#endif

    //acquire the bus to send polling transactions faster
    ret = spi_device_acquire_bus(spi, portMAX_DELAY);
    TEST_ESP_OK(ret);

    //record flight time by polling and with DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        spi_transmit_polling_measure(spi, &trans, &t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%.2lf", GET_US_BY_CCOUNT(t_flight_sorted[i]));
    }
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(SPI_PER_TRANS_POLLING, "%d us", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
#endif

    //release the bus
    spi_device_release_bus(spi);

    master_free_device_bus(spi);

    speed_setup(&spi, !use_dma);

    //record flight time by isr, without DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        spi_transmit_measure(spi, &trans, &t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%.2lf", GET_US_BY_CCOUNT(t_flight_sorted[i]));
    }
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(SPI_PER_TRANS_NO_POLLING_NO_DMA, "%d us", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
#endif

    //acquire the bus to send polling transactions faster
    ret = spi_device_acquire_bus(spi, portMAX_DELAY);
    TEST_ESP_OK(ret);
    //record flight time by polling, without DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        spi_transmit_polling_measure(spi, &trans, &t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%.2lf", GET_US_BY_CCOUNT(t_flight_sorted[i]));
    }
#ifndef CONFIG_SPIRAM
    TEST_PERFORMANCE_LESS_THAN(SPI_PER_TRANS_POLLING_NO_DMA, "%d us", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
#endif

    //release the bus
    spi_device_release_bus(spi);
    master_free_device_bus(spi);
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

#endif // CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
#endif // !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)
