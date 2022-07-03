/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi slave hd mode
*/

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"

#include "soc/spi_periph.h"
#include "driver/spi_master.h"
#include "esp_serial_slave_link/essl_spi.h"


#if (TEST_SPI_PERIPH_NUM >= 2)
//These will be only enabled on chips with 2 or more SPI peripherals

#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
#include "driver/spi_slave_hd.h"
#include "esp_rom_gpio.h"
#include "unity.h"
#include "test/test_common_spi.h"

#define TEST_DMA_MAX_SIZE    4092
#define TEST_BUFFER_SIZE 256     ///< buffer size of each wrdma buffer in fifo mode
#define TEST_SEG_SIZE   25

//ESP32-S2 cannot do single board test over IOMUX+GPIO matrix
#define TEST_MASTER_GPIO_MATRIX     1

#define SPI_SLOT_TEST_DEFAULT_CONFIG() {\
    .spics_io_num = PIN_NUM_CS, \
    .flags = 0, \
    .mode = 0, \
    .command_bits = 8,\
    .address_bits = 8,\
    .dummy_bits = 8,\
    .queue_size = 10,\
}

//context definition for the tcf framework
typedef struct {
    WORD_ALIGNED_ATTR uint8_t master_wrdma_buf[TEST_DMA_MAX_SIZE];
    WORD_ALIGNED_ATTR uint8_t master_rddma_buf[TEST_DMA_MAX_SIZE];
    WORD_ALIGNED_ATTR uint8_t slave_wrdma_buf[TEST_DMA_MAX_SIZE];
    WORD_ALIGNED_ATTR uint8_t slave_rddma_buf[TEST_DMA_MAX_SIZE];
    SemaphoreHandle_t ev_rdbuf;
    SemaphoreHandle_t ev_wrbuf;

    spi_slave_hd_data_t tx_data;
    spi_slave_hd_data_t rx_data;
} testhd_context_t;


static uint32_t get_hd_flags(void)
{
#if !defined(SLAVE_SUPPORT_QIO)
    return 0;
#endif
    int flag_id = rand() % 5;
    ESP_LOGI("io mode", "%d", flag_id);

    switch (flag_id) {
        case 1:
            return SPI_TRANS_MODE_DIO;
        case 2:
            return SPI_TRANS_MODE_DIO | SPI_TRANS_MODE_DIOQIO_ADDR;
        case 3:
            return SPI_TRANS_MODE_QIO;
        case 4:
            return SPI_TRANS_MODE_QIO | SPI_TRANS_MODE_DIOQIO_ADDR;
        default:
            return 0;
    }
}

void config_single_board_test_pin(void)
{
    esp_rom_gpio_connect_out_signal(PIN_NUM_MOSI, spi_periph_signal[TEST_SPI_HOST].spid_out, 0, 0);
    esp_rom_gpio_connect_in_signal(PIN_NUM_MOSI, spi_periph_signal[TEST_SLAVE_HOST].spid_in, 0);

    esp_rom_gpio_connect_out_signal(PIN_NUM_MISO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out, 0, 0);
    esp_rom_gpio_connect_in_signal(PIN_NUM_MISO, spi_periph_signal[TEST_SPI_HOST].spiq_in, 0);

    esp_rom_gpio_connect_out_signal(PIN_NUM_CS, spi_periph_signal[TEST_SPI_HOST].spics_out[0], 0, 0);
    esp_rom_gpio_connect_in_signal(PIN_NUM_CS, spi_periph_signal[TEST_SLAVE_HOST].spics_in, 0);

    esp_rom_gpio_connect_out_signal(PIN_NUM_CLK, spi_periph_signal[TEST_SPI_HOST].spiclk_out, 0, 0);
    esp_rom_gpio_connect_in_signal(PIN_NUM_CLK, spi_periph_signal[TEST_SLAVE_HOST].spiclk_in, 0);
}

static void init_master_hd(spi_device_handle_t* spi, const spitest_param_set_t* config, int freq)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.max_transfer_sz = TEST_DMA_MAX_SIZE*30;
    bus_cfg.quadhd_io_num = PIN_NUM_HD;
    bus_cfg.quadwp_io_num = PIN_NUM_WP;
#if defined(TEST_MASTER_GPIO_MATRIX) && CONFIG_IDF_TARGET_ESP32S2
    bus_cfg.flags |= SPICOMMON_BUSFLAG_GPIO_PINS;
#endif

    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX;
    dev_cfg.command_bits = 8;
    dev_cfg.address_bits = 8;
    dev_cfg.dummy_bits = 8;
    dev_cfg.clock_speed_hz = freq;
    dev_cfg.mode = config->mode;
    dev_cfg.input_delay_ns = config->slave_tv_ns;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, spi));
}

static void init_slave_hd(int mode, bool append_mode, const spi_slave_hd_callback_config_t* callback)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.max_transfer_sz = TEST_DMA_MAX_SIZE*30;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
#ifdef TEST_SLAVE_GPIO_MATRIX
    bus_cfg.flags |= SPICOMMON_BUSFLAG_FORCE_GPIO;
#endif
    spi_slave_hd_slot_config_t slave_hd_cfg = SPI_SLOT_TEST_DEFAULT_CONFIG();
    slave_hd_cfg.mode = mode;
    slave_hd_cfg.dma_chan = SPI_DMA_CH_AUTO;
    if (append_mode) {
        slave_hd_cfg.flags |= SPI_SLAVE_HD_APPEND_MODE;
    }
    if (callback) {
        slave_hd_cfg.cb_config = *callback;
    } else {
        slave_hd_cfg.cb_config = (spi_slave_hd_callback_config_t){};
    }
    TEST_ESP_OK(spi_slave_hd_init(TEST_SLAVE_HOST, &bus_cfg, &slave_hd_cfg));
}

static void test_hd_init(void** arg)
{
    TEST_ASSERT(*arg==NULL);
    *arg = heap_caps_malloc(sizeof(testhd_context_t), MALLOC_CAP_DMA);
    assert(((int)arg%4)==0);
    testhd_context_t* context = (testhd_context_t*)*arg;
    TEST_ASSERT(context!=NULL);

    context->ev_rdbuf = xSemaphoreCreateBinary();
    context->ev_wrbuf = xSemaphoreCreateBinary();
}

static void test_hd_deinit(void* arg)
{
    testhd_context_t *context = arg;
    vSemaphoreDelete(context->ev_rdbuf);
    vSemaphoreDelete(context->ev_wrbuf);
}

esp_err_t wait_wrbuf_sig(testhd_context_t* context, TickType_t wait)
{
    BaseType_t r = xSemaphoreTake(context->ev_wrbuf, wait);
    if (r==pdTRUE) {
        return ESP_OK;
    } else {
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t wait_rdbuf_sig(testhd_context_t* context, TickType_t wait)
{
    BaseType_t r = xSemaphoreTake(context->ev_rdbuf, wait);
    if (r==pdTRUE) {
        return ESP_OK;
    } else {
        return ESP_ERR_TIMEOUT;
    }
}

static void check_no_rx(testhd_context_t* context)
{
    spi_slave_hd_data_t* ret_trans;
    esp_err_t ret = spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, 0);
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, ret);
}

static void check_no_tx(testhd_context_t* context)
{
    spi_slave_hd_data_t* ret_trans;
    esp_err_t ret = spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, 0);
    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, ret);
}

bool wrbuf_cb(void* arg, spi_slave_hd_event_t* ev, BaseType_t* awoken)
{
    TEST_ASSERT_EQUAL(SPI_EV_BUF_RX, ev->event);
    testhd_context_t* ctx = (testhd_context_t*)arg;
    BaseType_t r = xSemaphoreGiveFromISR(ctx->ev_wrbuf, awoken);
    TEST_ASSERT_TRUE(r);
    return true;
}

bool rdbuf_cb(void* arg, spi_slave_hd_event_t* ev, BaseType_t* awoken)
{
    TEST_ASSERT_EQUAL(SPI_EV_BUF_TX, ev->event);
    testhd_context_t* ctx = (testhd_context_t*)arg;
    BaseType_t r = xSemaphoreGiveFromISR(ctx->ev_rdbuf, awoken);
    TEST_ASSERT_TRUE(r);
    return true;
}

static void test_hd_start(spi_device_handle_t *spi, int freq, const spitest_param_set_t* cfg, testhd_context_t* ctx)
{
    init_master_hd(spi, cfg, freq);

    spi_slave_hd_callback_config_t callback = {
        .cb_buffer_rx = wrbuf_cb,
        .cb_buffer_tx = rdbuf_cb,
        .arg = ctx,
    };
    init_slave_hd(cfg->mode, 0, &callback);

    //when test with single board via same set of mosi, miso, clk and cs pins.
    config_single_board_test_pin();

    wait_wrbuf_sig(ctx, 0);
    wait_rdbuf_sig(ctx, 0);
    check_no_rx(ctx);
    check_no_tx(ctx);


    srand(9322);
    for (int i = 0; i < TEST_DMA_MAX_SIZE; i++) ctx->slave_rddma_buf[i] = rand();
    for (int i = 0; i < TEST_DMA_MAX_SIZE; i++) ctx->master_wrdma_buf[i] = rand();

    int pos  = rand() % TEST_DMA_MAX_SIZE;
    int len = rand() % TEST_DMA_MAX_SIZE + 1;
    if (pos + len > TEST_DMA_MAX_SIZE) len = TEST_DMA_MAX_SIZE - pos;

    ESP_LOGI("rddma_load_len", "%d", len);
    ctx->tx_data = (spi_slave_hd_data_t) {
        .data = &ctx->slave_rddma_buf[pos],
        .len = len,
    };
    esp_err_t err = spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ctx->tx_data, portMAX_DELAY);
    TEST_ESP_OK(err);

    ctx->rx_data = (spi_slave_hd_data_t) {
        .data = ctx->slave_wrdma_buf,
        .len = TEST_DMA_MAX_SIZE,
    };
    err = spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ctx->rx_data, portMAX_DELAY);
    TEST_ESP_OK(err);
}


#define REG_REGION_SIZE SOC_SPI_MAXIMUM_BUFFER_SIZE

void check_no_signal(testhd_context_t* context)
{
    vTaskDelay(1);
    TEST_ASSERT(wait_wrbuf_sig(context, 0) == ESP_ERR_TIMEOUT);
    TEST_ASSERT(wait_rdbuf_sig(context, 0) == ESP_ERR_TIMEOUT);
    check_no_rx(context);
    check_no_tx(context);
}

void test_wrdma(testhd_context_t* ctx, const spitest_param_set_t *cfg, spi_device_handle_t spi)
{
    int pos = rand() % TEST_DMA_MAX_SIZE;
    int len = rand() % TEST_DMA_MAX_SIZE+1;
    if (pos+len > TEST_DMA_MAX_SIZE) len = TEST_DMA_MAX_SIZE - pos;

    int test_seg_size = len;//TEST_SEG_SIZE;
    ESP_LOGW("test_wrdma", "len: %d, seg_size: %d\n", len, test_seg_size);
    TEST_ESP_OK(essl_spi_wrdma(spi, &ctx->master_wrdma_buf[pos], len, test_seg_size, get_hd_flags()));

    spi_slave_hd_data_t* ret_trans;
    esp_err_t ret = spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY);
    TEST_ESP_OK(ret);
    TEST_ASSERT_EQUAL(&ctx->rx_data, ret_trans);
    TEST_ASSERT_EQUAL(len, ret_trans->trans_len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(&ctx->master_wrdma_buf[pos], ctx->slave_wrdma_buf, len);

    ctx->rx_data = (spi_slave_hd_data_t) {
        .data = ctx->slave_wrdma_buf,
        .len = TEST_DMA_MAX_SIZE,
    };
    esp_err_t err = spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ctx->rx_data, portMAX_DELAY);
    TEST_ESP_OK(err);
}

void test_rddma(testhd_context_t* ctx, const spitest_param_set_t* cfg, spi_device_handle_t spi)
{
    uint8_t* data_expected = ctx->tx_data.data;
    int len;
    int test_seg_size;


    len = ctx->tx_data.len;
    test_seg_size = TEST_SEG_SIZE;

    ESP_LOGW("test_rddma", "pos: %d, len: %d, slave_tx: %d, seg_size: %d\n", data_expected - ctx->slave_rddma_buf, len, ctx->tx_data.len, test_seg_size);

    TEST_ESP_OK(essl_spi_rddma(spi, ctx->master_rddma_buf, len, test_seg_size, get_hd_flags()));

    spi_slave_hd_data_t* ret_trans;
    esp_err_t ret = spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY);
    TEST_ESP_OK(ret);
    TEST_ASSERT_EQUAL(&ctx->tx_data, ret_trans);

    spitest_cmp_or_dump(data_expected, ctx->master_rddma_buf, len);

    int pos = rand() % TEST_DMA_MAX_SIZE;
    len = rand() % TEST_DMA_MAX_SIZE+1;
    if (pos + len > TEST_DMA_MAX_SIZE) len = TEST_DMA_MAX_SIZE - pos;

    ctx->tx_data = (spi_slave_hd_data_t) {
        .data = &ctx->slave_rddma_buf[pos],
        .len = len,
    };
    esp_err_t err = spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ctx->tx_data, portMAX_DELAY);
    TEST_ESP_OK(err);
}

static void test_hd_loop(const void* arg1, void* arg2)
{
    const spitest_param_set_t *test_cfg = arg1;
    testhd_context_t *context = arg2;
    const int *timing_speed_array = test_cfg->freq_list;

    ESP_LOGI(MASTER_TAG, "****************** %s ***************", test_cfg->pset_name);
    for (int j = 0; ; j++) {
        spi_device_handle_t spi;
        const int freq = timing_speed_array[j];
        if (freq==0) break;
        if (test_cfg->freq_limit && freq > test_cfg->freq_limit) break;

        ESP_LOGI(MASTER_TAG, "======> %dk", freq / 1000);

        test_hd_start(&spi, freq, test_cfg, context);

        uint8_t* mem_ptr;
        uint8_t slave_mem[REG_REGION_SIZE];
        uint8_t recv_buffer[REG_REGION_SIZE];

        srand(123);
        uint32_t mem[(REG_REGION_SIZE/4)];
        for (int i = 0; i < (REG_REGION_SIZE/4); i++) {
            mem[i] = rand();
        }
        mem_ptr = (uint8_t*)mem;

        check_no_signal(context);

        spi_slave_hd_write_buffer(TEST_SLAVE_HOST, 0, (uint8_t *) mem, SOC_SPI_MAXIMUM_BUFFER_SIZE);

        srand(123);
        for (int i = 0; i < (REG_REGION_SIZE/4); i++) {
            TEST_ASSERT(mem[i] == rand());
        }
        check_no_signal(context);
        test_rddma(context, test_cfg, spi);

        for (int i = 0; i < 128; i ++) {
            int pos = rand()%REG_REGION_SIZE;
            int len = rand()%REG_REGION_SIZE+1;
            if (len+pos>REG_REGION_SIZE) len = REG_REGION_SIZE-pos;

            memset(recv_buffer, 0xcc, sizeof(recv_buffer));

            check_no_signal(context);
            test_wrdma(context, test_cfg, spi);
            check_no_signal(context);
            test_rddma(context, test_cfg, spi);

            check_no_signal(context);
            TEST_ESP_OK(essl_spi_rdbuf(spi, recv_buffer, pos, len, get_hd_flags()));
            wait_rdbuf_sig(context, portMAX_DELAY);

            ESP_LOGI("mem", "pos: %d, len: %d", pos, len);
            // ESP_LOG_BUFFER_HEX("recv_buffer", recv_buffer, len);
            // ESP_LOG_BUFFER_HEX("mem", &mem_ptr[pos], len);
            TEST_ASSERT_EQUAL_HEX8_ARRAY(&mem_ptr[pos], recv_buffer, len);
        }

        check_no_signal(context);

        //clear slave buffer
        memset(mem, 0xcc, REG_REGION_SIZE);
        memcpy(slave_mem, mem, REG_REGION_SIZE);

        TEST_ESP_OK(essl_spi_wrbuf(spi, mem_ptr, 0, REG_REGION_SIZE, get_hd_flags()));
        wait_wrbuf_sig(context, portMAX_DELAY);

        TEST_ESP_OK(essl_spi_rdbuf(spi, recv_buffer, 0, REG_REGION_SIZE, get_hd_flags()));
        wait_rdbuf_sig(context, portMAX_DELAY);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_mem, recv_buffer, REG_REGION_SIZE);

        srand(466);
        for (int i = 0; i < 64; i ++) {
            ESP_LOGI("temp_i", "^^^^^^^^^^^^^^^^ %d ^^^^^^^^^^", i);
            for (int j = 0; j < (REG_REGION_SIZE/4); j++) {
                mem[j] = rand();
            }
            for (int k = 0; k < 2; k++) {
                int pos = rand() % REG_REGION_SIZE;
                int len = rand() % REG_REGION_SIZE + 1;
                if (len + pos > REG_REGION_SIZE) len = REG_REGION_SIZE - pos;

                printf("pos: %d, len: %d\n", pos, len);

                TEST_ESP_OK(essl_spi_wrbuf(spi, &mem_ptr[pos], pos, len, get_hd_flags()));
                wait_wrbuf_sig(context, portMAX_DELAY);
                memcpy(&slave_mem[pos], &mem_ptr[pos], len);
            }

            check_no_signal(context);
            test_rddma(context, test_cfg, spi);

            check_no_signal(context);
            test_wrdma(context, test_cfg, spi);

            TEST_ESP_OK(essl_spi_rdbuf(spi, recv_buffer, 0, REG_REGION_SIZE, get_hd_flags()));

            wait_rdbuf_sig(context, portMAX_DELAY);

            check_no_signal(context);

            TEST_ASSERT_EQUAL_HEX8_ARRAY(&slave_mem, recv_buffer, REG_REGION_SIZE);
        }

        master_free_device_bus(spi);
        spi_slave_hd_deinit(TEST_SLAVE_HOST);
    }
}

static const ptest_func_t hd_test_func = {
    .pre_test = test_hd_init,
    .post_test = test_hd_deinit,
    .loop = test_hd_loop,
    .def_param = spitest_def_param,
};

#define TEST_SPI_HD(name, test_set) \
    PARAM_GROUP_DECLARE(name, test_set) \
    TEST_SINGLE_BOARD(name, test_set, "[spi][timeout=120]", &hd_test_func)

static int test_freq_hd[] = {
    // 100*1000,
    // SPI_MASTER_FREQ_10M, //maximum freq MISO stable before next latch edge
    // SPI_MASTER_FREQ_20M, //maximum freq MISO stable before next latch edge
    SPI_MASTER_FREQ_40M, //maximum freq MISO stable before next latch edge
    0,
};

#define TEST_HD_IN_CONTINUOUS_MODE  true

static spitest_param_set_t hd_conf[] = {
    { .pset_name = "MODE0",
        .freq_list = test_freq_hd,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_WITH_ESP_SLAVE,
      .mode = 0,
    },
    { .pset_name = "MODE1",
        .freq_list = test_freq_hd,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_WITH_ESP_SLAVE,
      .mode = 1,
    },
    { .pset_name = "MODE2",
        .freq_list = test_freq_hd,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_WITH_ESP_SLAVE,
      .mode = 2,
    },
    { .pset_name = "MODE3",
        .freq_list = test_freq_hd,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_WITH_ESP_SLAVE,
      .mode = 3,
    },
};
TEST_SPI_HD(HD, hd_conf);

/*
 * When the previous transaction of master exceeds the length of slave prepared too long, the
 * interrupt of slave will be triggered in side that transaction. In the ISR slave has to prepare
 * for the next transaction, while the master is still sending the previous one.
 *
 * This test checks that the previous trans will not influence the data slave prepared for the next transaction.
 */
TEST_CASE("test spi slave hd segment mode, master too long", "[spi][spi_slv_hd]")
{
    spi_device_handle_t spi;
    spitest_param_set_t *cfg = &hd_conf[0];
    int freq = 100*1000; // the frequency should be small enough for the slave to prepare new trans

    init_master_hd(&spi, cfg, freq);

    //no callback needed
    init_slave_hd(cfg->mode, 0, NULL);

    //Use GPIO matrix to connect signal of master and slave via same set of pins on one board.
    config_single_board_test_pin();

    const int send_buf_size = 1024;

    WORD_ALIGNED_ATTR uint8_t* slave_send_buf = malloc(send_buf_size * 2);
    WORD_ALIGNED_ATTR uint8_t* master_send_buf = malloc(send_buf_size * 2);
    WORD_ALIGNED_ATTR uint8_t* slave_recv_buf = malloc(send_buf_size * 2);
    WORD_ALIGNED_ATTR uint8_t* master_recv_buf = malloc(send_buf_size * 2);

    memset(slave_recv_buf, 0xcc, send_buf_size * 2);
    memset(master_recv_buf, 0xcc, send_buf_size * 2);
    srand (939);
    for (int i = 0; i< send_buf_size * 2; i++) {
        master_send_buf[i] = rand();
        slave_send_buf[i] = rand();
    }

    //make the first transaction shorter than the actual trans length of the master, so that the second one will be loaded while the master is still doing the first transaction.
    int trans_len[] = {5, send_buf_size};
    spi_slave_hd_data_t slave_trans[4] = {
        //recv, the buffer size should be aligned to 4
        {
            .data = slave_recv_buf,
            .len = (trans_len[0] + 3) & (~3),
        },
        {
            .data = slave_recv_buf + send_buf_size,
            .len = (trans_len[1] + 3) & (~3),
        },
        //send
        {
            .data = slave_send_buf,
            .len = trans_len[0],
        },
        {
            .data = slave_send_buf + send_buf_size,
            .len = trans_len[1],
        },
    };

    for (int i = 0; i < 2; i ++) {
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &slave_trans[i], portMAX_DELAY));
    }
    for (int i = 2; i < 4; i ++) {
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &slave_trans[i], portMAX_DELAY));
    }

    essl_spi_wrdma(spi, master_send_buf, send_buf_size, -1, 0);
    essl_spi_wrdma(spi, master_send_buf + send_buf_size, send_buf_size, 5, 0);

    essl_spi_rddma(spi, master_recv_buf, send_buf_size, -1, 0);
    essl_spi_rddma(spi, master_recv_buf + send_buf_size, send_buf_size, 5, 0);

    for (int i = 0; i < 2; i ++) {
        spi_slave_hd_data_t *ret_trans;
        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT(ret_trans == &slave_trans[i]);
        TEST_ASSERT_EQUAL(slave_trans[i].len, ret_trans->trans_len);
    }

    for (int i = 2; i < 4; i ++) {
        spi_slave_hd_data_t *ret_trans;
        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT(ret_trans == &slave_trans[i]);
    }

    spitest_cmp_or_dump(slave_send_buf, master_recv_buf, trans_len[0]);
    spitest_cmp_or_dump(slave_send_buf + send_buf_size, master_recv_buf + send_buf_size, trans_len[1]);

    spitest_cmp_or_dump(master_send_buf, slave_recv_buf, trans_len[0]);
    spitest_cmp_or_dump(master_send_buf + send_buf_size, slave_recv_buf + send_buf_size, trans_len[1]);

    free(master_recv_buf);
    free(slave_recv_buf);
    free(master_send_buf);
    free(slave_send_buf);
    spi_slave_hd_deinit(TEST_SLAVE_HOST);
    master_free_device_bus(spi);
}

#endif //SOC_SPI_SUPPORT_SLAVE_HD_VER2
#endif //#if (TEST_SPI_PERIPH_NUM >= 2)


#if (TEST_SPI_PERIPH_NUM == 1)
#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//These tests are for chips which only have 1 SPI controller
/********************************************************************************
 *      Test By Master & Slave (2 boards)
 *
 * Master (C3, C2, H2) && Slave (C3, C2, H2):
 *      PIN | Master     | Slave      |
 *      ----| ---------  | ---------  |
 *      CS  | 10         | 10         |
 *      CLK | 6          | 6          |
 *      MOSI| 7          | 7          |
 *      MISO| 2          | 2          |
 *      GND | GND        | GND        |
 *
 ********************************************************************************/
#include "driver/spi_slave_hd.h"
#include "unity.h"
#include "test/test_common_spi.h"

static void hd_master(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t spi;
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX;
    dev_cfg.command_bits = 8;
    dev_cfg.address_bits = 8;
    dev_cfg.dummy_bits = 8;
    dev_cfg.clock_speed_hz = 100 * 1000;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    const int send_buf_size = 1024;

    WORD_ALIGNED_ATTR uint8_t *master_send_buf = malloc(send_buf_size * 2);
    WORD_ALIGNED_ATTR uint8_t *master_recv_buf = calloc(1, send_buf_size * 2);
    //This buffer is used for 2-board test and should be assigned totally the same as the ``hd_slave`` does.
    WORD_ALIGNED_ATTR uint8_t *slave_send_buf = malloc(send_buf_size * 2);
    get_tx_buffer(199, master_send_buf, slave_send_buf, send_buf_size);

    //This is the same as the ``hd_slave`` sets.
    int trans_len[] = {5, send_buf_size};

    unity_wait_for_signal("slave ready");
    essl_spi_wrdma(spi, master_send_buf, send_buf_size, -1, 0);

    unity_wait_for_signal("slave ready");
    essl_spi_wrdma(spi, master_send_buf + send_buf_size, send_buf_size, 5, 0);


    unity_wait_for_signal("slave ready");
    essl_spi_rddma(spi, master_recv_buf, send_buf_size, -1, 0);
    spitest_cmp_or_dump(slave_send_buf, master_recv_buf, trans_len[0]);

    unity_wait_for_signal("slave ready");
    essl_spi_rddma(spi, master_recv_buf + send_buf_size, send_buf_size, 5, 0);
    spitest_cmp_or_dump(slave_send_buf + send_buf_size, master_recv_buf + send_buf_size, trans_len[1]);


    free(master_recv_buf);
    free(master_send_buf);
    free(slave_send_buf);

    master_free_device_bus(spi);
}

static void hd_slave(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.max_transfer_sz = 14000 * 30;

    spi_slave_hd_slot_config_t slave_hd_cfg = {
        .spics_io_num = PIN_NUM_CS,
        .dma_chan = SPI_DMA_CH_AUTO,
        .flags = 0,
        .mode = 0,
        .command_bits = 8,
        .address_bits = 8,
        .dummy_bits = 8,
        .queue_size = 10,
    };
    TEST_ESP_OK(spi_slave_hd_init(TEST_SLAVE_HOST, &bus_cfg, &slave_hd_cfg));

    const int send_buf_size = 1024;

    WORD_ALIGNED_ATTR uint8_t *slave_send_buf = malloc(send_buf_size * 2);
    WORD_ALIGNED_ATTR uint8_t *slave_recv_buf = calloc(1, send_buf_size * 2);
    //This buffer is used for 2-board test and should be assigned totally the same as the ``hd_master`` does.
    WORD_ALIGNED_ATTR uint8_t *master_send_buf = malloc(send_buf_size * 2);
    get_tx_buffer(199, master_send_buf, slave_send_buf, send_buf_size);

    //make the first transaction shorter than the actual trans length of the master, so that the second one will be loaded while the master is still doing the first transaction.
    int trans_len[] = {5, send_buf_size};
    spi_slave_hd_data_t slave_trans[4] = {
        //recv, the buffer size should be aligned to 4
        {
            .data = slave_recv_buf,
            .len = (trans_len[0] + 3) & (~3),
        },
        {
            .data = slave_recv_buf + send_buf_size,
            .len = (trans_len[1] + 3) & (~3),
        },
        //send
        {
            .data = slave_send_buf,
            .len = trans_len[0],
        },
        {
            .data = slave_send_buf + send_buf_size,
            .len = trans_len[1],
        },
    };

    for (int i = 0; i < 2; i ++) {
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &slave_trans[i], portMAX_DELAY));
        unity_send_signal("slave ready");
    }
    for (int i = 2; i < 4; i ++) {
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &slave_trans[i], portMAX_DELAY));
        unity_send_signal("slave ready");
    }

    for (int i = 0; i < 2; i ++) {
        spi_slave_hd_data_t *ret_trans;
        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT(ret_trans == &slave_trans[i]);
        TEST_ASSERT_EQUAL(slave_trans[i].len, ret_trans->trans_len);
    }

    for (int i = 2; i < 4; i ++) {
        spi_slave_hd_data_t *ret_trans;
        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT(ret_trans == &slave_trans[i]);
    }

    spitest_cmp_or_dump(master_send_buf, slave_recv_buf, trans_len[0]);
    spitest_cmp_or_dump(master_send_buf + send_buf_size, slave_recv_buf + send_buf_size, trans_len[1]);

    free(slave_recv_buf);
    free(slave_send_buf);
    free(master_send_buf);

    spi_slave_hd_deinit(TEST_SLAVE_HOST);
}

TEST_CASE_MULTIPLE_DEVICES("SPI Slave HD: segment mode, master sends too long", "[spi_ms][test_env=Example_SPI_Multi_device]", hd_master, hd_slave);
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(...)
#endif  //#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
#endif  //#if (TEST_SPI_PERIPH_NUM == 1)
