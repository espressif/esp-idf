/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi_master device driver
*/

#include "sdkconfig.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "sys/param.h"
#include "driver/gpio.h"
#include "hal/spi_ll.h"     // for SPI_LL_SUPPORT_CLK_SRC_PRE_DIV
#include "soc/spi_periph.h"
#include "soc/soc_memory_layout.h"
#include "esp_private/cache_utils.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/esp_clk.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_heap_caps.h"
#include "esp_clk_tree.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_cache.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "spi_performance.h"

const static char TAG[] = "test_spi";

// There is no input-only pin except on esp32 and esp32s2
#define TEST_SOC_HAS_INPUT_ONLY_PINS  (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

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

    int real_freq_khz;
    spi_device_get_actual_freq(handle, &real_freq_khz);
    printf("Checking clk rate %dHz. expect pre %d n %d, got pre %d n %d, real_freq %d kHZ\n", clk, pre, n, hw->clock.clkdiv_pre + 1, hw->clock.clkcnt_n + 1, real_freq_khz);

    TEST_ASSERT(hw->clock.clkcnt_n + 1 == n);
    TEST_ASSERT(hw->clock.clkdiv_pre + 1 == pre);

    TEST_ESP_OK(spi_bus_remove_device(handle));
}

/**
 * In this test, SPI Clock Calculation:
 *   Fspi = Fclk_spi_mst / (pre + n)
 *
 * For each item:
 * {freq, pre, n}
 *
 * Only test on SPI_CLK_SRC_DEFAULT here
 */
#define TEST_CLK_TIMES     8
uint32_t clk_param_80m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 16, 50}, {333333, 4, 60}, {800000, 2, 50}, {900000, 2, 44}, {8000000, 1, 10}, {20000000, 1, 4}, {26000000, 1, 3} };
uint32_t clk_param_160m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 16, 50}, {333333, 4, 60}, {800000, 2, 50}, {900000, 2, 44}, {8000000, 1, 10}, {20000000, 1, 4}, {26000000, 1, 3} };
#if SPI_LL_SUPPORT_CLK_SRC_PRE_DIV
uint32_t clk_param_40m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 4, 50}, {333333, 1, 60}, {800000, 1, 25}, {2000000, 1, 10}, {5000000, 1,  4}, {12000000, 1, 2}, {18000000, 1, 1} };
uint32_t clk_param_48m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 4, 60}, {333333, 2, 36}, {800000, 1, 30}, {5000000, 1, 5}, {12000000, 1, 2}, {18000000, 1, 2}, {24000000, 1, 1} };
#else
uint32_t clk_param_40m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 8, 50}, {333333, 2, 60}, {800000, 1, 50}, {2000000, 1, 20}, {5000000, 1,  8}, {12000000, 1, 3}, {18000000, 1, 2} };
uint32_t clk_param_48m[TEST_CLK_TIMES][3] = {{1, SPI_LL_MAX_PRE_DIV_NUM, 64}, {100000, 8, 60}, {333333, 3, 48}, {800000, 1, 60}, {5000000, 1, 10}, {12000000, 1, 4}, {18000000, 1, 3}, {26000000, 1, 2} };
#endif

TEST_CASE("SPI Master clockdiv calculation routines", "[spi]")
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    uint32_t clock_source_hz;

    esp_clk_tree_src_get_freq_hz(SPI_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &clock_source_hz);
    printf("\nTest clock source SPI_CLK_SRC_DEFAULT = %ld\n", clock_source_hz);
    if ((160 * 1000 * 1000) == clock_source_hz) {
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param_160m[i][0], clk_param_160m[i][1], clk_param_160m[i][2]);
        }
    } else if ((80 * 1000 * 1000) == clock_source_hz) {
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param_80m[i][0], clk_param_80m[i][1], clk_param_80m[i][2]);
        }
    } else if ((48 * 1000 * 1000) == clock_source_hz) {
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param_48m[i][0], clk_param_48m[i][1], clk_param_48m[i][2]);
        }
    } else if ((40 * 1000 * 1000) == clock_source_hz) {
        for (int i = 0; i < TEST_CLK_TIMES; i++) {
            check_spi_pre_n_for(clk_param_40m[i][0], clk_param_40m[i][1], clk_param_40m[i][2]);
        }
    } else {
        ESP_LOGW(TAG, "Don't find any routing param!!");
    }

    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
}

// Test All clock source
#define TEST_CLK_BYTE_LEN           10000
#define TEST_TRANS_TIME_BIAS_RATIO  (float)8.0/100   // think 8% transfer time bias as acceptable
TEST_CASE("SPI Master clk_source and divider accuracy", "[spi]")
{
    int64_t start = 0, end = 0;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.max_transfer_sz = TEST_CLK_BYTE_LEN;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // prepare trans
    uint8_t *sendbuf = heap_caps_malloc(TEST_CLK_BYTE_LEN, MALLOC_CAP_DMA);
    spi_transaction_t trans = {};
    trans.tx_buffer = sendbuf;
    trans.length = TEST_CLK_BYTE_LEN * 8;

    uint8_t spi_clk_sour[] = SOC_SPI_CLKS;
    uint32_t clock_source_hz;
    for (uint8_t sour_idx = 0; sour_idx < sizeof(spi_clk_sour); sour_idx++) {
        esp_clk_tree_src_get_freq_hz(spi_clk_sour[sour_idx], ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &clock_source_hz);
        printf("\nTesting unknown clock source @%ld Hz\n", clock_source_hz);
#if SPI_LL_SUPPORT_CLK_SRC_PRE_DIV
        clock_source_hz /= 2;  //targets support pre-div will divide clock by 2 before SPI peripheral
#endif
        for (uint8_t test_time = 0; test_time < 8; test_time ++) {
            spi_device_handle_t handle;
            spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
            devcfg.clock_source = spi_clk_sour[sour_idx];
            devcfg.clock_speed_hz = MIN(IDF_TARGET_MAX_SPI_CLK_FREQ, clock_source_hz) >> test_time;
#if CONFIG_IDF_TARGET_ESP32
            devcfg.flags |= SPI_DEVICE_HALFDUPLEX;  //esp32 half duplex to work on high freq
#endif
            if ((soc_module_clk_t)devcfg.clock_source == SOC_MOD_CLK_RC_FAST) {
                devcfg.clock_speed_hz /= 2; //rc_fast have bad accuracy, test at low speed
            }
            TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &handle));
            // one trans first to trigger lazy load
            TEST_ESP_OK(spi_device_polling_transmit(handle, &trans));

            // test single tx/rx under full duplex mode, refer to `TEST_CASE_MULTIPLE_DEVICES("SPI Master: FD, DMA, Master Single Direction Test"...`
            if (!(devcfg.flags && SPI_DEVICE_HALFDUPLEX)) {
                trans.tx_buffer = NULL;
                trans.rxlength = trans.length;
                trans.rx_buffer = sendbuf;
            }

            // calculate theoretical transaction time by actual freq and trans length
            int real_freq_khz;
            spi_device_get_actual_freq(handle, &real_freq_khz);
            // (byte_len * 8 / real_freq_hz) * 1000 000, (unit)us
            int trans_cost_us_predict = (float)TEST_CLK_BYTE_LEN * 8 * 1000 / real_freq_khz;

            // transaction and measure time
            start = esp_timer_get_time();
            TEST_ESP_OK(spi_device_polling_transmit(handle, &trans));
            end = esp_timer_get_time();
            int trans_cost = end - start;
            int time_tolerance = trans_cost_us_predict * TEST_TRANS_TIME_BIAS_RATIO;
#if !SOC_CLK_TREE_SUPPORTED
            time_tolerance *= 2;    //cpu is executing too slow before clock supported
#endif
            printf("exp_freq %dk real_freq %dk predict_cost %d real_cost_us %d diff %d tolerance %d us\n", devcfg.clock_speed_hz / 1000, real_freq_khz, trans_cost_us_predict, trans_cost, (trans_cost - trans_cost_us_predict), time_tolerance);

            TEST_ASSERT_LESS_THAN_UINT32(time_tolerance, abs(trans_cost - trans_cost_us_predict));
            TEST_ESP_OK(spi_bus_remove_device(handle));
        }
    }

    free(sendbuf);
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
}

TEST_CASE("test_device_dynamic_freq_update", "[spi]")
{
    spi_device_handle_t dev0;
    int master_send;

    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.flags |= SPI_DEVICE_HALFDUPLEX;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev0));

    spi_transaction_t trans_cfg = {
        .tx_buffer = &master_send,
        .length = sizeof(master_send) * 8,
    };

    trans_cfg.override_freq_hz = IDF_TARGET_MAX_SPI_CLK_FREQ;
    for (int i = 1; i < 15; i++) {
        TEST_ESP_OK(spi_device_transmit(dev0, &trans_cfg));
        spi_device_get_actual_freq(dev0, &master_send);
        printf("override %5ld k real freq %5d k\n", trans_cfg.override_freq_hz / 1000, master_send);
        if (trans_cfg.override_freq_hz) {
            TEST_ASSERT_EQUAL_UINT32(trans_cfg.override_freq_hz / 1000, master_send);
        }
        if (i > 10) {
            //test without override freq
            trans_cfg.override_freq_hz = 0;
            continue;
        }
        if (!(i % 2)) {
            //test override freq every 2 trans
            trans_cfg.override_freq_hz /= 2;
        }
    }
    TEST_ESP_OK(spi_bus_remove_device(dev0));
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
    TEST_ASSERT(success);

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
    TEST_ASSERT(success);

    printf("Testing bus at %dMHz\n", IDF_TARGET_MAX_SPI_CLK_FREQ / 1000000);
    handle = setup_spi_bus_loopback(IDF_TARGET_MAX_SPI_CLK_FREQ, true);
    success &= spi_test(handle, 128); //DMA, aligned
    success &= spi_test(handle, 4096 * 3); //DMA, multiple descs
    master_free_device_bus(handle);
    TEST_ASSERT(success);

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

//There is no input-only pin except on esp32 and esp32s2, so this test could be ignored.
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
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test 4 iomux output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_IOMUX_PINS | SPICOMMON_BUSFLAG_DUAL;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_QUAD | SPICOMMON_BUSFLAG_GPIO_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO |  SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

#if TEST_SOC_HAS_INPUT_ONLY_PINS  //There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
    ESP_LOGI(TAG, "test master 5 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test slave 5 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test master 3 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_GPIO_PINS;

    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);

    ESP_LOGI(TAG, "test slave 3 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ESP_OK(spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    TEST_ASSERT_EQUAL_HEX32(flags_expected, flags_o);
//There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
#endif //#if TEST_SOC_HAS_INPUT_ONLY_PINS

    ESP_LOGI(TAG, "check native flag for 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_IOMUX_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

    ESP_LOGI(TAG, "check native flag for 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_IOMUX_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

#if TEST_SOC_HAS_INPUT_ONLY_PINS  //There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
    ESP_LOGI(TAG, "check dual flag for master 5 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

    ESP_LOGI(TAG, "check dual flag for master 3 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL | SPICOMMON_BUSFLAG_GPIO_PINS;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = INPUT_ONLY_PIN, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = INPUT_ONLY_PIN, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
//There is no input-only pin on esp32c3 and esp32s3, so this test could be ignored.
#endif //#if TEST_SOC_HAS_INPUT_ONLY_PINS

    ESP_LOGI(TAG, "check sclk flag...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = -1, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

    ESP_LOGI(TAG, "check mosi flag...");
    flags_expected = SPICOMMON_BUSFLAG_MOSI;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = -1, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

    ESP_LOGI(TAG, "check miso flag...");
    flags_expected = SPICOMMON_BUSFLAG_MISO;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = -1, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));

    ESP_LOGI(TAG, "check quad flag...");
    flags_expected = SPICOMMON_BUSFLAG_QUAD;
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = -1, .quadwp_io_num = spi_periph_signal[TEST_SPI_HOST].spiwp_iomux_pin,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
    cfg = (spi_bus_config_t) {
        .mosi_io_num = spi_periph_signal[TEST_SPI_HOST].spid_iomux_pin, .miso_io_num = spi_periph_signal[TEST_SPI_HOST].spiq_iomux_pin, .sclk_io_num = spi_periph_signal[TEST_SPI_HOST].spiclk_iomux_pin, .quadhd_io_num = spi_periph_signal[TEST_SPI_HOST].spihd_iomux_pin, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_MASTER, &flags_o, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(TEST_SPI_HOST, &cfg, flags_expected | SPICOMMON_BUSFLAG_SLAVE, &flags_o, NULL));
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
    // the original version (bit miswritten) stucks here.
    TEST_ESP_OK(spi_device_transmit(spi, &transaction));
    // test case success when see this.
    printf("after second xmit\n");

    TEST_ESP_OK(spi_bus_remove_device(spi));
    TEST_ESP_OK(spi_bus_free(host));
}

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

    srand(52);
    for (int i = 0; i < 320 / 4; i++) {
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

#define TEST_REGION_SIZE 3
    static spi_transaction_t trans[TEST_REGION_SIZE];
    memset(trans, 0, sizeof(trans));

    trans[0].length = 320 * 8;
    trans[0].tx_buffer = data_malloc + 2;
    trans[0].rx_buffer = data_dram;

    trans[1].length = 4 * 8;
    trans[1].flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    uint32_t *ptr = (uint32_t *)trans[1].rx_data;
    *ptr = 0x54545454;
    ptr = (uint32_t *)trans[1].tx_data;
    *ptr = 0xbc124960;

    trans[2].length = 64 * 8;
    trans[2].tx_buffer = data_drom;
    trans[2].rx_buffer = data_malloc;

    //Queue all transactions.
    for (int x = 0; x < TEST_REGION_SIZE; x++) {
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
}

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

    for (int i = 0; i < 8; i ++) {
        memset(rx_buf, 0x66, sizeof(rx_buf));
        spi_transaction_t t = {};
        t.length = 8 * (i + 1);
        t.rxlength = 0;
        t.tx_buffer = tx_buf + 2 * i;
        t.rx_buffer = rx_buf + i;

        if (i == 1) {
            //test set no start
            t.rx_buffer = NULL;
        } else if (i == 2) {
            //test rx length != tx_length
            t.rxlength = t.length - 8;
        }
        spi_device_transmit(spi, &t);

        for (int i = 0; i < 16; i ++) {
            printf("%02X ", rx_buf[i]);
        }
        printf("\n");

        if (i == 1) {
            // no rx, skip check
        } else if (i == 2) {
            //test rx length = tx length-1
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length / 8 - 1);
        } else {
            //normal check
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length / 8);
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
    same_pin_func_sel(buscfg, devcfg.spics_io_num, 0, false);

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
        int addr_bits = 0;
#ifdef CONFIG_IDF_TARGET_ESP32
        addr_bits = 56 - 8 * i;
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        //ESP32S2 only supports up to 32 bits address
        addr_bits = 28 - 4 * i;
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

        ESP_LOGI(MASTER_TAG, "===== test%d =====", i);
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

        ESP_LOGI(SLAVE_TAG, "cmd_got: %" PRIX16 ", addr_got: %" PRIX32 "%" PRIX32, cmd_got, (uint32_t)(addr_got >> 32), (uint32_t)addr_got);

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
    esp_err_t err = init_slave_context(&slave_context, TEST_SLAVE_HOST);
    TEST_ASSERT(err == ESP_OK);
    TaskHandle_t handle_slave;
    xTaskCreate(spitest_slave_task, "spi_slave", 4096, &slave_context, 0, &handle_slave);

    //initial slave, mode 0, no dma
    int dma_chan = 0;
    int slave_mode = 0;
    spi_bus_config_t slv_buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.mode = slave_mode;
    //Initialize SPI slave interface
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &slv_buscfg, &slvcfg, dma_chan));

    test_cmd_addr(&slave_context, false);
    test_cmd_addr(&slave_context, true);

    vTaskDelete(handle_slave);
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
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_DISABLED));

    same_pin_func_sel(bus_cfg, dev_cfg.spics_io_num, 0, false);

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

    same_pin_func_sel(bus_cfg, dev_cfg.spics_io_num, 0, false);

    uint32_t buf_size = 32;
    uint8_t *mst_send_buf = spi_bus_dma_memory_alloc(TEST_SPI_HOST, buf_size, 0);
    uint8_t *mst_recv_buf = spi_bus_dma_memory_alloc(TEST_SPI_HOST, buf_size, 0);
    uint8_t *slv_send_buf = spi_bus_dma_memory_alloc(TEST_SLAVE_HOST, buf_size, 0);
    uint8_t *slv_recv_buf = spi_bus_dma_memory_alloc(TEST_SLAVE_HOST, buf_size, 0);

    srand(199);
    for (int i = 0; i < buf_size; i++) {
        mst_send_buf[i] = rand();
    }

    //1. Master sends without receiving, no rx_buffer is set
    spi_slave_transaction_t slave_trans = {
        .rx_buffer = slv_recv_buf,
        .length = buf_size * 8,
        .flags = SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO,
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
        slave_trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
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
        test_fill_random_to_buffers_dualboard(FD_SEED1 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(mst_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        master_only_rx_trans(spi, mst_recv_buf, slv_send_buf, FD_TEST_BUF_SIZE);

        //2. Master FD DMA with TX and RX
        test_fill_random_to_buffers_dualboard(FD_SEED2 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(mst_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        master_both_trans(spi, mst_send_buf, mst_recv_buf, slv_send_buf, FD_TEST_BUF_SIZE);
    }

    //Master FD DMA, TX without RX Test
    for (int i = 0; i < TEST_NUM; i++) {
        // 1. Master FD DMA, only send, with NULL rx_buffer
        test_fill_random_to_buffers_dualboard(FD_SEED3 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        master_only_tx_trans(spi, mst_send_buf, FD_TEST_BUF_SIZE);

        //2. Master FD DMA with TX and RX
        test_fill_random_to_buffers_dualboard(FD_SEED4 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
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
    trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
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
    trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
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
    trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
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
        test_fill_random_to_buffers_dualboard(FD_SEED1 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        slave_only_tx_trans(slv_send_buf, FD_TEST_BUF_SIZE);

        //2. Slave both TX and RX
        test_fill_random_to_buffers_dualboard(FD_SEED2 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_both_trans(slv_send_buf, slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);
    }

    for (int i = 0; i < TEST_NUM; i++) {
        // 1. Slave RX without TX (tx_buffer == NULL)
        test_fill_random_to_buffers_dualboard(FD_SEED3 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_only_rx_trans(slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);

        //2. Slave both TX and RX
        test_fill_random_to_buffers_dualboard(FD_SEED4 + i, mst_send_buf, slv_send_buf, FD_TEST_BUF_SIZE);
        memset(slv_recv_buf, 0x0, FD_TEST_BUF_SIZE);
        slave_both_trans(slv_send_buf, slv_recv_buf, mst_send_buf, FD_TEST_BUF_SIZE);
    }

    free(slv_send_buf);
    free(slv_recv_buf);
    free(mst_send_buf);
    TEST_ASSERT(spi_slave_free(SPI2_HOST) == ESP_OK);
}

TEST_CASE_MULTIPLE_DEVICES("SPI Master: FD, DMA, Master Single Direction Test", "[spi_ms][test_env=generic_multi_device]", fd_master, fd_slave);
#endif  //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32)    //TODO: IDF-3494

//NOTE: Explained in IDF-1445 | MR !14996
#if !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)
/********************************************************************************
 *      Test SPI transaction interval
 ********************************************************************************/
//Disabled since place code in flash increase the delay
#if CONFIG_SPI_MASTER_ISR_IN_IRAM

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
    printf("[Performance][%s]: %d us\n", "SPI_PER_TRANS_INTR_DMA", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
    TEST_ASSERT_LESS_THAN_INT(IDF_TARGET_MAX_TRANS_TIME_INTR_DMA, (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
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
    printf("[Performance][%s]: %d us\n", "SPI_PER_TRANS_POLL_DMA", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
    TEST_ASSERT_LESS_THAN_INT(IDF_TARGET_MAX_TRANS_TIME_POLL_DMA, (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
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
    printf("[Performance][%s]: %d us\n", "SPI_PER_TRANS_INTR_CPU", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
    TEST_ASSERT_LESS_THAN_INT(IDF_TARGET_MAX_TRANS_TIME_INTR_CPU, (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
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
    printf("[Performance][%s]: %d us\n", "SPI_PER_TRANS_POLL_CPU", (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
    TEST_ASSERT_LESS_THAN_INT(IDF_TARGET_MAX_TRANS_TIME_POLL_CPU, (int)GET_US_BY_CCOUNT(t_flight_sorted[(TEST_TIMES + 1) / 2]));
#endif

    //release the bus
    spi_device_release_bus(spi);
    master_free_device_bus(spi);
}

#endif // CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
#endif // !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)

//****************************************spi master add device test************************************//
#define SPI_MAX_DEVICE_NUM  SOC_SPI_PERIPH_CS_NUM(TEST_SPI_HOST)
//add dummy devices first
#if CONFIG_IDF_TARGET_ESP32
#define DUMMY_CS_PINS() {25, 26, 27}
#elif CONFIG_IDF_TARGET_ESP32H2
#define DUMMY_CS_PINS() {9, 10, 11, 12, 22, 25}
#elif CONFIG_IDF_TARGET_ESP32P4
#define DUMMY_CS_PINS() {20, 21, 22, 23, 24, 25}
#else
#define DUMMY_CS_PINS() {0, 1, 4, 5, 8, 9}
#endif //CONFIG_IDF_TARGET_ESP32

#define CS_REAL_DEV       PIN_NUM_CS
#define TEST_TRANS_LEN    48

void test_add_device_master(void)
{
    spi_device_handle_t devs[SPI_MAX_DEVICE_NUM] = {};
    uint8_t cs_pins[SPI_MAX_DEVICE_NUM] = DUMMY_CS_PINS();

    uint8_t master_sendbuf[TEST_TRANS_LEN] = {0};
    uint8_t master_recvbuf[TEST_TRANS_LEN] = {0};
    uint8_t master_expect[TEST_TRANS_LEN] = {0};

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .queue_size = 3,
    };

    for (uint8_t i = 0; i < SPI_MAX_DEVICE_NUM; i++) {
        dev_cfg.spics_io_num = cs_pins[i];
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &devs[i]));
    }

    spi_transaction_t trans = {};
    trans.length = sizeof(master_sendbuf) * 8;
    trans.tx_buffer = master_sendbuf;
    trans.rx_buffer = master_recvbuf;

    for (uint8_t i = 0; i < SPI_MAX_DEVICE_NUM; i++) {
        //1. add max dummy devices
        //2. replace devs[i] as a real device, than start a transaction
        //3. free devs[i] after transaction to release the real CS pin for using again by another dev,
        //So it will loop to check every gpio_signal one by one use one physical pin
        spi_bus_remove_device(devs[i]);
        dev_cfg.spics_io_num = CS_REAL_DEV;
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &devs[i]));

        memset(master_recvbuf, 0, sizeof(master_recvbuf));
        test_fill_random_to_buffers_dualboard(21, master_sendbuf, master_expect, TEST_TRANS_LEN);

        unity_send_signal("Master ready");
        unity_wait_for_signal("Slave ready");
        spi_device_transmit(devs[i], &trans);

        ESP_LOGI("Master", "dev %d communication:", i);
        ESP_LOG_BUFFER_HEX("Tx", master_sendbuf, sizeof(master_sendbuf));
        spitest_cmp_or_dump(master_expect, master_recvbuf, TEST_TRANS_LEN);

        //swap self as a dummy device to free real cs line
        spi_bus_remove_device(devs[i]);
        dev_cfg.spics_io_num = cs_pins[i];
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &devs[i]));
    }

    for (uint8_t i = 0; i < SPI_MAX_DEVICE_NUM; i++) {
        spi_bus_remove_device(devs[i]);
    }
    spi_bus_free(TEST_SPI_HOST);
}

void test_add_device_slave(void)
{
    uint8_t slave_sendbuf[TEST_TRANS_LEN] = {0};
    uint8_t slave_recvbuf[TEST_TRANS_LEN] = {0};
    uint8_t slave_expect[TEST_TRANS_LEN] = {0};

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = CS_REAL_DEV,
        .queue_size = 3,
    };
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &bus_cfg, &slvcfg, SPI_DMA_CH_AUTO));

    spi_slave_transaction_t slave_trans = {};
    slave_trans.length = sizeof(slave_sendbuf) * 8;
    slave_trans.tx_buffer = slave_sendbuf;
    slave_trans.rx_buffer = slave_recvbuf;
    slave_trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;

    for (uint8_t i = 0; i < SPI_MAX_DEVICE_NUM; i++) {
        memset(slave_recvbuf, 0, sizeof(slave_recvbuf));
        test_fill_random_to_buffers_dualboard(21, slave_expect, slave_sendbuf, TEST_TRANS_LEN);

        unity_wait_for_signal("Master ready");
        unity_send_signal("Slave ready");
        spi_slave_transmit(TEST_SPI_HOST, &slave_trans, portMAX_DELAY);

        ESP_LOGI("Slave", "dev %d communication:", i);
        ESP_LOG_BUFFER_HEX("Tx", slave_sendbuf, sizeof(slave_sendbuf));
        spitest_cmp_or_dump(slave_expect, slave_recvbuf, TEST_TRANS_LEN);
    }

    spi_slave_free(TEST_SPI_HOST);
    spi_bus_free(TEST_SPI_HOST);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Master:Test multiple devices", "[spi_ms]", test_add_device_master, test_add_device_slave);

#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)

#define TEST_ISR_CNT    100
static void test_master_isr_core_post_trans_cbk(spi_transaction_t *curr_trans)
{
    *((int *)curr_trans->user) += esp_cpu_get_core_id();
}

TEST_CASE("test_master_isr_pin_to_core", "[spi]")
{
    spi_device_handle_t dev0;
    uint32_t master_send;
    uint32_t master_receive;
    uint32_t master_expect;

    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.post_cb = test_master_isr_core_post_trans_cbk;

    spi_transaction_t trans_cfg = {
        .tx_buffer = &master_send,
        .rx_buffer = &master_receive,
        .user = & master_expect,
        .length = sizeof(uint32_t) * 8,
    };

    master_expect = 0;
    for (int i = 0; i < TEST_ISR_CNT; i++) {
        TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev0));

        TEST_ESP_OK(spi_device_transmit(dev0, &trans_cfg));

        TEST_ESP_OK(spi_bus_remove_device(dev0));
        TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
    }
    printf("Test Master ISR Not Assign: %d : %ld\n", TEST_ISR_CNT, master_expect);
    // by default the esp_intr_alloc is called on ESP_MAIN_TASK_AFFINITY_CPU0 now
    TEST_ASSERT_EQUAL_UINT32(0, master_expect);

    //-------------------------------------CPU1---------------------------------------
    buscfg.isr_cpu_id = ESP_INTR_CPU_AFFINITY_1;

    master_expect = 0;
    for (int i = 0; i < TEST_ISR_CNT; i++) {
        TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev0));

        TEST_ESP_OK(spi_device_transmit(dev0, &trans_cfg));

        TEST_ESP_OK(spi_bus_remove_device(dev0));
        TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
    }
    printf("Test Master ISR Assign CPU1: %d : %ld\n", TEST_ISR_CNT, master_expect);
    TEST_ASSERT_EQUAL_UINT32(TEST_ISR_CNT, master_expect);
}
#endif

#if CONFIG_SPI_MASTER_IN_IRAM
#define TEST_MASTER_IRAM_TRANS_LEN  120
static IRAM_ATTR void test_master_iram_post_trans_cbk(spi_transaction_t *trans)
{
    *((bool *)trans->user) = true;
}

static IRAM_ATTR void test_master_iram(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.intr_flags = ESP_INTR_FLAG_IRAM;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t dev_handle = {0};
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.cs_ena_pretrans = 1;
    devcfg.post_cb = test_master_iram_post_trans_cbk;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev_handle));

    bool flag_trans_done;
    uint8_t *master_send = heap_caps_malloc(TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *master_recv = heap_caps_calloc(1, TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *master_exp  = heap_caps_malloc(TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DEFAULT);
    test_fill_random_to_buffers_dualboard(211, master_send, master_exp, TEST_MASTER_IRAM_TRANS_LEN);
    spi_transaction_t trans_cfg = {
        .tx_buffer = master_send,
        .rx_buffer = master_recv,
        .user = &flag_trans_done,
        .length = TEST_MASTER_IRAM_TRANS_LEN * 8,
    }, *ret_trans;

    // Test interrupt trans api once -----------------------------
    unity_send_signal("Master ready");
    unity_wait_for_signal("Slave ready");

    spi_flash_disable_interrupts_caches_and_other_cpu();
    flag_trans_done = false;
    spi_device_queue_trans(dev_handle, &trans_cfg, portMAX_DELAY);
    while (!flag_trans_done) {
        // waiting for transaction done and return from ISR
    }
    spi_device_get_trans_result(dev_handle, &ret_trans, portMAX_DELAY);
    spi_flash_enable_interrupts_caches_and_other_cpu();

    ESP_LOG_BUFFER_HEX("master tx", ret_trans->tx_buffer, TEST_MASTER_IRAM_TRANS_LEN);
    spitest_cmp_or_dump(master_exp, trans_cfg.rx_buffer, TEST_MASTER_IRAM_TRANS_LEN);

    // Test polling trans api once -------------------------------
    unity_wait_for_signal("Slave ready");
    test_fill_random_to_buffers_dualboard(119, master_send, master_exp, TEST_MASTER_IRAM_TRANS_LEN);

    spi_flash_disable_interrupts_caches_and_other_cpu();
    spi_device_polling_transmit(dev_handle, &trans_cfg);
    spi_flash_enable_interrupts_caches_and_other_cpu();

    ESP_LOG_BUFFER_HEX("master tx", ret_trans->tx_buffer, TEST_MASTER_IRAM_TRANS_LEN);
    spitest_cmp_or_dump(master_exp, trans_cfg.rx_buffer, TEST_MASTER_IRAM_TRANS_LEN);

    free(master_send);
    free(master_recv);
    free(master_exp);
    TEST_ESP_OK(spi_bus_remove_device(dev_handle));
    spi_bus_free(TEST_SPI_HOST);
}

static void test_iram_slave_normal(void)
{
    uint8_t *slave_sendbuf = heap_caps_malloc(TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *slave_recvbuf = heap_caps_calloc(1, TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *slave_expect = heap_caps_malloc(TEST_MASTER_IRAM_TRANS_LEN, MALLOC_CAP_DEFAULT);

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &bus_cfg, &slvcfg, SPI_DMA_CH_AUTO));

    spi_slave_transaction_t slave_trans = {};
    slave_trans.length = TEST_MASTER_IRAM_TRANS_LEN * 8;
    slave_trans.tx_buffer = slave_sendbuf;
    slave_trans.rx_buffer = slave_recvbuf;
    slave_trans.flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
    test_fill_random_to_buffers_dualboard(211, slave_expect, slave_sendbuf, TEST_MASTER_IRAM_TRANS_LEN);

    unity_wait_for_signal("Master ready");
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_transmit(TEST_SPI_HOST, &slave_trans, portMAX_DELAY));
    ESP_LOG_BUFFER_HEX("slave tx", slave_sendbuf, TEST_MASTER_IRAM_TRANS_LEN);
    spitest_cmp_or_dump(slave_expect, slave_recvbuf, TEST_MASTER_IRAM_TRANS_LEN);

    unity_send_signal("Slave ready");
    test_fill_random_to_buffers_dualboard(119, slave_expect, slave_sendbuf, TEST_MASTER_IRAM_TRANS_LEN);
    TEST_ESP_OK(spi_slave_transmit(TEST_SPI_HOST, &slave_trans, portMAX_DELAY));
    ESP_LOG_BUFFER_HEX("slave tx", slave_sendbuf, TEST_MASTER_IRAM_TRANS_LEN);
    spitest_cmp_or_dump(slave_expect, slave_recvbuf, TEST_MASTER_IRAM_TRANS_LEN);

    free(slave_sendbuf);
    free(slave_recvbuf);
    free(slave_expect);
    spi_slave_free(TEST_SPI_HOST);
    spi_bus_free(TEST_SPI_HOST);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Master:IRAM_safe", "[spi_ms]", test_master_iram, test_iram_slave_normal);
#endif

TEST_CASE("test_bus_free_safty_to_remain_devices", "[spi]")
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t dev0, dev1;
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev0));
    devcfg.spics_io_num = PIN_NUM_MISO;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev1));

    int master_send;
    spi_transaction_t trans_cfg = {
        .tx_buffer = &master_send,
        .length = sizeof(uint32_t) * 8,
    };

    TEST_ESP_OK(spi_bus_remove_device(dev0));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, spi_bus_free(TEST_SPI_HOST));

    //transaction should OK after a failed call to bus_free
    TEST_ESP_OK(spi_device_transmit(dev1, (spi_transaction_t *)&trans_cfg));

    TEST_ESP_OK(spi_bus_remove_device(dev1));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
}

#if SOC_LIGHT_SLEEP_SUPPORTED
TEST_CASE("test_spi_master_sleep_retention", "[spi]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    spi_device_handle_t dev_handle;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    buscfg.flags |= SPICOMMON_BUSFLAG_GPIO_PINS;
    buscfg.flags |= SPICOMMON_BUSFLAG_SLP_ALLOW_PD;
    buscfg.miso_io_num = buscfg.mosi_io_num;    // set spi "self-loop"
    uint8_t send[16] = "hello spi x\n";
    uint8_t recv[16];
    spi_transaction_t trans_cfg = {
        .length = 8 * sizeof(send),
        .tx_buffer = send,
        .rx_buffer = recv,
    };

    for (int periph = SPI2_HOST; periph < SPI_HOST_MAX; periph ++) {
        for (int test_dma = 0; test_dma <= 1; test_dma ++) {
            int use_dma = SPI_DMA_DISABLED;
#if SOC_GDMA_SUPPORT_SLEEP_RETENTION    // TODO: IDF-11317 test dma on esp32 and s2
            use_dma = test_dma ? SPI_DMA_CH_AUTO : SPI_DMA_DISABLED;
#endif
            printf("Retention on GPSPI%d with dma: %d\n", periph + 1, use_dma);
            TEST_ESP_OK(spi_bus_initialize(periph, &buscfg, use_dma));
            TEST_ESP_OK(spi_bus_add_device(periph, &devcfg, &dev_handle));

            for (uint8_t cnt = 0; cnt < 3; cnt ++) {
                printf("Going into sleep...\n");
                TEST_ESP_OK(esp_light_sleep_start());
                printf("Waked up!\n");

                // check if the sleep happened as expected
                TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
                // check if the power domain also is powered down
                TEST_ASSERT_EQUAL((buscfg.flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
                memset(recv, 0, sizeof(recv));
                send[10] = cnt + 'A';
                TEST_ESP_OK(spi_device_transmit(dev_handle, &trans_cfg));
                printf("%s", recv);
                spitest_cmp_or_dump(trans_cfg.tx_buffer, trans_cfg.rx_buffer, sizeof(send));
            }

            TEST_ESP_OK(spi_bus_remove_device(dev_handle));
            TEST_ESP_OK(spi_bus_free(periph));
        }
    }

    esp_sleep_set_sleep_context(NULL);
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
}

#if CONFIG_PM_ENABLE
TEST_CASE("test_spi_master_auto_sleep_retention", "[spi]")
{
    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    uint32_t xtal_hz = 0;
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &xtal_hz);
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_hz / 1000000,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true,
#endif
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    for (uint8_t allow_pd = 0; allow_pd < 2; allow_pd ++) {
        spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
        buscfg.flags = (allow_pd) ? SPICOMMON_BUSFLAG_SLP_ALLOW_PD : 0;
        buscfg.flags |= SPICOMMON_BUSFLAG_GPIO_PINS;
        buscfg.miso_io_num = buscfg.mosi_io_num;    // set spi "self-loop"
        TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_DISABLED));

        spi_device_handle_t dev_handle;
        spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
        TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev_handle));

        uint8_t send[13] = "hello spi 0\n";
        uint8_t recv[13];
        spi_transaction_t trans_cfg = {
            .length = 8 * sizeof(send),
            .tx_buffer = send,
            .rx_buffer = recv,
        };

        for (uint8_t cnt = 0; cnt < 3; cnt ++) {
            printf("Going into Auto sleep with power %s ...\n", (buscfg.flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) ? "down" : "hold");
            vTaskDelay(1000);   //auto light sleep here
            printf("Waked up!\n");

            // check if the sleep happened as expected
            TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
            // check if the power domain also is powered down
            TEST_ASSERT_EQUAL((buscfg.flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
            memset(recv, 0, sizeof(recv));
            send[10] = cnt + '0';
            TEST_ESP_OK(spi_device_polling_transmit(dev_handle, &trans_cfg));
            printf("%s", recv);
            spitest_cmp_or_dump(trans_cfg.tx_buffer, trans_cfg.rx_buffer, sizeof(send));
        }

        TEST_ESP_OK(spi_bus_remove_device(dev_handle));
        TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
    }
    esp_sleep_set_sleep_context(NULL);
    pm_config.light_sleep_enable = false;
    TEST_ESP_OK(esp_pm_configure(&pm_config));
}
#endif  //CONFIG_PM_ENABLE
#endif  //SOC_LIGHT_SLEEP_SUPPORTED

#if CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE
#define TEST_EDMA_PSRAM_TRANS_NUM    5
#define TEST_EDMA_TRANS_LEN          20000
#define TEST_EDMA_BUFFER_SZ          (TEST_EDMA_PSRAM_TRANS_NUM * TEST_EDMA_TRANS_LEN)

void test_spi_psram_trans(spi_device_handle_t dev_handle, void *tx, void *rx)
{
    spi_transaction_t trans_cfg = {
        .tx_buffer = tx,
        .rx_buffer = rx,
    };

    int trans_len = TEST_EDMA_TRANS_LEN - TEST_EDMA_PSRAM_TRANS_NUM / 2;
    for (uint8_t cnt = 0; cnt < TEST_EDMA_PSRAM_TRANS_NUM; cnt ++) {
        trans_cfg.length = trans_len * 8;
        trans_cfg.rxlength = trans_len * 8;
        trans_cfg.flags = (cnt % 2) ? 0 : SPI_TRANS_DMA_USE_PSRAM;

        // To use psram, hardware will pass data through MSPI and GDMA to GPSPI, which need some time
        // GPSPI bandwidth(speed * line_num) should always no more than PSRAM bandwidth
        trans_cfg.override_freq_hz = (CONFIG_SPIRAM_SPEED / 4) * 1000 * 1000;
        printf("%d TX %p RX %p len %d @%ld kHz\n", cnt, trans_cfg.tx_buffer, trans_cfg.rx_buffer, trans_len, trans_cfg.override_freq_hz / 1000);
        TEST_ESP_OK(spi_device_transmit(dev_handle, &trans_cfg));
        TEST_ASSERT(!(trans_cfg.flags & (SPI_TRANS_DMA_RX_FAIL | SPI_TRANS_DMA_TX_FAIL)));
        spitest_cmp_or_dump(trans_cfg.tx_buffer, trans_cfg.rx_buffer, trans_len);
        trans_cfg.tx_buffer += trans_len;
        trans_cfg.rx_buffer += trans_len;
        trans_len ++;
    }
}

TEST_CASE("SPI_Master: PSRAM buffer transaction via EDMA", "[spi]")
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.miso_io_num = buscfg.mosi_io_num;    // set spi "self-loopback"
    buscfg.max_transfer_sz = TEST_EDMA_BUFFER_SZ;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t dev_handle = NULL;
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.clock_speed_hz = 80 * 1000 * 1000;   // Test error case on highest freq first
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev_handle));
    int real_freq_khz;
    spi_device_get_actual_freq(dev_handle, &real_freq_khz);

    uint8_t *internal_1 = heap_caps_calloc(1, TEST_EDMA_BUFFER_SZ, MALLOC_CAP_INTERNAL);
    uint8_t *external_1 = heap_caps_calloc(1, TEST_EDMA_BUFFER_SZ, MALLOC_CAP_SPIRAM);
    uint8_t *external_2 = heap_caps_calloc(1, TEST_EDMA_BUFFER_SZ, MALLOC_CAP_SPIRAM);
    test_fill_random_to_buffers_dualboard(1001, internal_1, external_2, TEST_EDMA_BUFFER_SZ);

    printf("Test error case: High freq @%d kHz\n", real_freq_khz);
    spi_transaction_t trans_cfg = {
        .length = TEST_EDMA_TRANS_LEN * 8,
        .tx_buffer = external_2,
        .rx_buffer = external_1,
    };

    // also test on polling API, and automalloc mechanism
    for (uint8_t i = 0; i < 2; i++) {
        printf("\n==== %s ====\n", i ? "EDMA" : "Auto Malloc");
        trans_cfg.flags = i ? SPI_TRANS_DMA_USE_PSRAM : 0;
        uint32_t before = esp_get_free_heap_size();
        spi_device_polling_start(dev_handle, &trans_cfg, portMAX_DELAY);
        uint32_t after = esp_get_free_heap_size();
        printf("mem_diff: %ld, trans_len: %d\n", after - before, TEST_EDMA_TRANS_LEN);
        // rx buffer still potential re-malloc from psram even if SPI_TRANS_DMA_USE_PSRAM is set
        TEST_ASSERT(i ? (before - after) < 2 * TEST_EDMA_TRANS_LEN : (before - after) > 2 * TEST_EDMA_TRANS_LEN);
        spi_device_polling_end(dev_handle, portMAX_DELAY);
        printf("TX fail: %d, RX fail: %d\n", !!(trans_cfg.flags & SPI_TRANS_DMA_TX_FAIL), !!(trans_cfg.flags & SPI_TRANS_DMA_RX_FAIL));
        TEST_ASSERT((!!i) == !!(trans_cfg.flags & (SPI_TRANS_DMA_TX_FAIL | SPI_TRANS_DMA_RX_FAIL)));
        if (!i) { // data should be correct if using auto malloc
            spitest_cmp_or_dump(trans_cfg.tx_buffer, trans_cfg.rx_buffer, TEST_EDMA_TRANS_LEN);
        }
    }

    printf("\nTest trans: internal -> psram\n");
    memset(external_1, 0, TEST_EDMA_BUFFER_SZ);
    TEST_ESP_OK(esp_cache_msync((void *)external_1, TEST_EDMA_BUFFER_SZ, (ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED)));
    test_spi_psram_trans(dev_handle, internal_1, external_1);

    printf("\nTest trans: psram    -> psram\n");
    memset(external_2, 0, TEST_EDMA_BUFFER_SZ);
    TEST_ESP_OK(esp_cache_msync((void *)external_2, TEST_EDMA_BUFFER_SZ, (ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED)));
    test_spi_psram_trans(dev_handle, external_1, external_2);

    printf("\nTest trans: psram    -> internal\n");
    memset(internal_1, 0, TEST_EDMA_BUFFER_SZ);
    test_spi_psram_trans(dev_handle, external_2, internal_1);

    free(internal_1);
    free(external_1);
    free(external_2);
    spi_bus_remove_device(dev_handle);
    spi_bus_free(TEST_SPI_HOST);
}
#endif
