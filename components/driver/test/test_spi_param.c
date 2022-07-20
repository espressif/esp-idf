/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_attr.h"
#include "soc/spi_periph.h"
#include "sdkconfig.h"
#include "test/test_common_spi.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"

#if (TEST_SPI_PERIPH_NUM >= 2)
//These will only be enabled on chips with 2 or more SPI peripherals

#ifndef MIN
#define MIN(a, b)((a) > (b)? (b): (a))
#endif

/********************************************************************************
 *      Test By Internal Connections
 ********************************************************************************/
static void local_test_init(void** context);
static void local_test_deinit(void* context);
static void local_test_loop(const void *test_param, void* context);

static const ptest_func_t local_test_func = {
    .pre_test = local_test_init,
    .post_test = local_test_deinit,
    .loop = local_test_loop,
    .def_param = spitest_def_param,
};

#define TEST_SPI_LOCAL(name, param_set) \
    PARAM_GROUP_DECLARE(name, param_set) \
    TEST_SINGLE_BOARD(SPI_##name, param_set, "[spi][timeout=120]", &local_test_func)

static void local_test_init(void** arg)
{
    esp_log_level_set("gpio", ESP_LOG_WARN);
    TEST_ASSERT(*arg==NULL);
    *arg = malloc(sizeof(spitest_context_t));
    spitest_context_t* context = (spitest_context_t*)*arg;
    TEST_ASSERT(context!=NULL);
    context->slave_context = (spi_slave_task_context_t){};
    esp_err_t err = init_slave_context( &context->slave_context);
    TEST_ASSERT(err == ESP_OK);

    xTaskCreate(spitest_slave_task, "spi_slave", 4096, &context->slave_context, 0, &context->handle_slave);
}

static void local_test_deinit(void* arg)
{
    spitest_context_t* context = arg;
    vTaskDelete(context->handle_slave);
    context->handle_slave = 0;
    deinit_slave_context(&context->slave_context);
}

static void local_test_start(spi_device_handle_t *spi, int freq, const spitest_param_set_t* pset, spitest_context_t* context)
{
    //master config
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    //pin config & initialize
    //we can't have two sets of iomux pins on the same pins
    assert(!pset->master_iomux || !pset->slave_iomux);
    if (pset->slave_iomux) {
        //only in this case, use VSPI iomux pins
        buscfg.miso_io_num =  SLAVE_IOMUX_PIN_MISO;
        buscfg.mosi_io_num =  SLAVE_IOMUX_PIN_MOSI;
        buscfg.sclk_io_num =  SLAVE_IOMUX_PIN_SCLK;
        devcfg.spics_io_num = SLAVE_IOMUX_PIN_CS;
        slvcfg.spics_io_num = SLAVE_IOMUX_PIN_CS;
    } else {
        buscfg.miso_io_num =  MASTER_IOMUX_PIN_MISO;
        buscfg.mosi_io_num =  MASTER_IOMUX_PIN_MOSI;
        buscfg.sclk_io_num =  MASTER_IOMUX_PIN_SCLK;
        devcfg.spics_io_num = MASTER_IOMUX_PIN_CS;
        slvcfg.spics_io_num = MASTER_IOMUX_PIN_CS;
    }
    //this does nothing, but avoid the driver from using iomux pins if required
    buscfg.quadhd_io_num = (!pset->master_iomux && !pset->slave_iomux ? UNCONNECTED_PIN : -1);
    devcfg.mode = pset->mode;
    const int cs_pretrans_max = 15;
    if (pset->dup == HALF_DUPLEX_MISO) {
        devcfg.cs_ena_pretrans = cs_pretrans_max;
        devcfg.flags |= SPI_DEVICE_HALFDUPLEX;
    } else if (pset->dup == HALF_DUPLEX_MOSI) {
        devcfg.cs_ena_pretrans = cs_pretrans_max;
        devcfg.flags |= SPI_DEVICE_NO_DUMMY;
    } else {
        devcfg.cs_ena_pretrans = cs_pretrans_max;
    }
    const int cs_posttrans_max = 15;
    devcfg.cs_ena_posttrans = cs_posttrans_max;
    devcfg.input_delay_ns = pset->slave_tv_ns;
    devcfg.clock_speed_hz = freq;
    if (pset->master_limit != 0 && freq > pset->master_limit) devcfg.flags |= SPI_DEVICE_NO_DUMMY;

    //slave config
    slvcfg.mode = pset->mode;
    slave_pull_up(&buscfg, slvcfg.spics_io_num);

    int dma_chan = (pset->master_dma_chan == 0) ? 0 : SPI_DMA_CH_AUTO;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, dma_chan));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, spi));

    //slave automatically use iomux pins if pins are on VSPI_* pins
    buscfg.quadhd_io_num = -1;
    int slave_dma_chan = (pset->slave_dma_chan == 0) ? 0 : SPI_DMA_CH_AUTO;
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &buscfg, &slvcfg, slave_dma_chan));

    //initialize master and slave on the same pins break some of the output configs, fix them
    if (pset->master_iomux) {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_SPI, spi_periph_signal[TEST_SPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_SPI, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_SPI, spi_periph_signal[TEST_SPI_HOST].spiclk_out);
    } else if (pset->slave_iomux) {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_SPI, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spiclk_out);
    } else {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spiclk_out);
    }

    if (context) {
        //clear master receive buffer
        memset(context->master_rxbuf, 0x66, sizeof(context->master_rxbuf));
    }
}

static void local_test_end(spi_device_handle_t spi)
{
    master_free_device_bus(spi);
    TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);
}

static void local_test_loop(const void* arg1, void* arg2)
{
    const spitest_param_set_t *pset = arg1;
    spitest_context_t *context = arg2;
    spi_device_handle_t spi;
    spitest_init_transactions(pset, context);
    const int *timing_speed_array = pset->freq_list;

    ESP_LOGI(MASTER_TAG, "****************** %s ***************", pset->pset_name);
    for (int i = 0; ; i++) {
        const int freq = timing_speed_array[i];
        if (freq==0) break;
        if (pset->freq_limit && freq > pset->freq_limit) break;

        ESP_LOGI(MASTER_TAG, "==> %dkHz", freq / 1000);

        bool check_master_data = (pset->dup!=HALF_DUPLEX_MOSI &&
                (pset->master_limit==0 || freq <= pset->master_limit));
        if (!check_master_data) ESP_LOGI(MASTER_TAG, "skip master data check");

        bool check_slave_data = (pset->dup!=HALF_DUPLEX_MISO);
        if (!check_slave_data) ESP_LOGI(SLAVE_TAG, "skip slave data check");

        local_test_start(&spi, freq, pset, context);

        for (int k = 0; k < pset->test_size; k++) {
            WORD_ALIGNED_ATTR uint8_t recvbuf[320+8];
            slave_txdata_t *txdata = &context->slave_trans[k];
            spi_slave_transaction_t slave_trans = {
                .tx_buffer = txdata->start,
                .rx_buffer = recvbuf,
                .length = txdata->len,
            };
            esp_err_t err = spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_trans, portMAX_DELAY);
            TEST_ESP_OK(err);

            //wait for both master and slave end
            spi_transaction_t *t = &context->master_trans[k];
            int len = get_trans_len(pset->dup, t);
            ESP_LOGI(MASTER_TAG, "  ==> #%d: len: %d", k, len);
            //send master tx data

            err = spi_device_transmit(spi, t);
            TEST_ESP_OK(err);

            spi_slave_transaction_t *ret_trans;
            err = spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_trans, 5);
            TEST_ESP_OK(err);
            TEST_ASSERT_EQUAL(&slave_trans, ret_trans);

            uint32_t rcv_len = slave_trans.trans_len;
            bool failed = false;

            //check master data
            if (check_master_data && memcmp(slave_trans.tx_buffer, t->rx_buffer, (len + 7) / 8) != 0 ) {
                failed = true;
            }

            //check slave data and length
            //currently the rcv_len can be in range of [t->length-1, t->length+3]
            if ( rcv_len < len - 1 || rcv_len > len + 4) {
                failed = true;
            }
            if (check_slave_data && memcmp(t->tx_buffer, slave_trans.rx_buffer, (len + 7) / 8) != 0 ) {
                failed = true;
            }

            if (failed) {
                ESP_LOGI(SLAVE_TAG, "slave_recv_len: %d", rcv_len);
                spitest_master_print_data(t, len);

                ESP_LOG_BUFFER_HEX("slave tx", slave_trans.tx_buffer, len);
                ESP_LOG_BUFFER_HEX("slave rx", slave_trans.rx_buffer, len);

                //already failed, try to use the TEST_ASSERT to output the reason...
                TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_trans.tx_buffer, t->rx_buffer, (len + 7) / 8);
                TEST_ASSERT_EQUAL_HEX8_ARRAY(t->tx_buffer, slave_trans.rx_buffer, (len + 7) / 8);
                TEST_ASSERT(rcv_len >= len - 1 && rcv_len <= len + 4);
            }
        }
        local_test_end(spi);
    }
}

/************ Timing Test ***********************************************/
//TODO: esp32s2 has better timing performance
static spitest_param_set_t timing_pgroup[] = {
//signals are not fed to peripherals through iomux if the functions are not selected to iomux
#if !DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
    {   .pset_name = "FULL_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .master_iomux = true,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    {   .pset_name = "FULL_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .master_iomux = false,
        .slave_iomux = true,
        .slave_tv_ns = TV_INT_CONNECT,
    },
#endif
    {   .pset_name = "FULL_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .master_limit = SPI_MASTER_FREQ_10M,
        .dup = FULL_DUPLEX,
        .master_iomux = false,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
//signals are not fed to peripherals through iomux if the functions are not selected to iomux
#if !DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
    {   .pset_name = "MISO_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .master_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = true,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    {   .pset_name = "MISO_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        //.freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = false,
        .slave_iomux = true,
        .slave_tv_ns = TV_INT_CONNECT,
    },
#endif
    {   .pset_name = "MISO_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        //.freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = false,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
//signals are not fed to peripherals through iomux if the functions are not selected to iomux
#if !DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
    {   .pset_name = "MOSI_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        //.freq_limit = ESP_SPI_SLAVE_MAX_READ_FREQ, //ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux = true,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    {   .pset_name = "MOSI_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        //.freq_limit = ESP_SPI_SLAVE_MAX_READ_FREQ, //ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux = false,
        .slave_iomux = true,
        .slave_tv_ns = TV_INT_CONNECT,
    },
#endif
    {   .pset_name = "MOSI_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        //.freq_limit = ESP_SPI_SLAVE_MAX_READ_FREQ, //ESP_SPI_SLAVE_MAX_FREQ_SYNC,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux = false,
        .slave_iomux = false,
        .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
};
TEST_SPI_LOCAL(TIMING, timing_pgroup)

/************ Mode Test ***********************************************/
#define FREQ_LIMIT_MODE SPI_MASTER_FREQ_16M
static int test_freq_mode_local[]={
    1*1000*1000,
    SPI_MASTER_FREQ_9M, //maximum freq MISO stable before next latch edge
    SPI_MASTER_FREQ_13M,
    SPI_MASTER_FREQ_16M,
    SPI_MASTER_FREQ_20M,
    SPI_MASTER_FREQ_26M,
    SPI_MASTER_FREQ_40M,
    0,
};

//signals are not fed to peripherals through iomux if the functions are not selected to iomux
#ifdef CONFIG_IDF_TARGET_ESP32
#define LOCAL_MODE_TEST_SLAVE_IOMUX     true

/*
 * When DMA is enabled in mode 0 and 2, an special workaround is used. The MISO (slave's output) is
 * half an SPI clock ahead, but then delay 3 apb clocks.

 * Compared to the normal timing, the MISO is not slower than when the frequency is below 13.3MHz,
 * under which there's no need for the master to compensate the MISO signal. However compensation
 * is required when the frequency is beyond 16MHz, at this time, an extra positive delay is added
 * to the normal delay (3 apb clocks).
 *
 * It's is hard to tell the master driver that kind of delay logic. This magic delay value happens
 * to compensate master timing beyond 16MHz.
 *
 * If the master or slave's timing is changed again, and the test no longer passes, above 16MHz,
 * it's OK to use `master_limit` to disable master data check or skip the test above some
 * frequencies above 10MHz (the design target value).
 */
#define SLAVE_EXTRA_DELAY_DMA           12.5
#else
#define LOCAL_MODE_TEST_SLAVE_IOMUX     false
#define SLAVE_EXTRA_DELAY_DMA           0
#endif


static spitest_param_set_t mode_pgroup[] = {
    {   .pset_name = "Mode 0",
        .freq_list = test_freq_mode_local,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 0,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "Mode 1",
        .freq_list = test_freq_mode_local,
        .freq_limit = SPI_MASTER_FREQ_26M,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 1,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "Mode 2",
        .freq_list = test_freq_mode_local,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 2,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "Mode 3",
        .freq_list = test_freq_mode_local,
        .freq_limit = SPI_MASTER_FREQ_26M,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 3,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "Mode 0, DMA",
        .freq_list = test_freq_mode_local,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 0,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
    {   .pset_name = "Mode 1, DMA",
        .freq_list = test_freq_mode_local,
        .freq_limit = SPI_MASTER_FREQ_26M,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 1,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
    {   .pset_name = "Mode 2, DMA",
        .freq_list = test_freq_mode_local,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 2,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
    {   .pset_name = "Mode 3, DMA",
        .freq_list = test_freq_mode_local,
        .freq_limit = SPI_MASTER_FREQ_26M,
        .master_limit = SPI_MASTER_FREQ_13M,
        .dup = FULL_DUPLEX,
        .mode = 3,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
    /////////////////////////// MISO ////////////////////////////////////
    {   .pset_name = "MISO, Mode 0",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 0,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "MISO, Mode 1",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 1,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "MISO, Mode 2",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 2,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "MISO, Mode 3",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 3,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
    },
    {   .pset_name = "MISO, Mode 0, DMA",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 0,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT+SLAVE_EXTRA_DELAY_DMA,
        .length_aligned = true,
    },
    {   .pset_name = "MISO, Mode 1, DMA",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 1,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
    {   .pset_name = "MISO, Mode 2, DMA",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 2,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT+SLAVE_EXTRA_DELAY_DMA,
        .length_aligned = true,
    },
    {   .pset_name = "MISO, Mode 3, DMA",
        .freq_list = test_freq_mode_local,
        .dup = HALF_DUPLEX_MISO,
        .mode = 3,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .master_iomux = false,
        .slave_iomux = LOCAL_MODE_TEST_SLAVE_IOMUX,
        .slave_tv_ns = TV_INT_CONNECT,
        .length_aligned = true,
    },
};
TEST_SPI_LOCAL(MODE, mode_pgroup)

/**********************SPI master slave transaction length test*************/
/* Test SPI slave can receive different length of data in all 4 modes (permutations of
 * CPOL/CPHA and when DMA is used or not).
 * Length from 1 to 16 bytes are tested.
*/

#define MASTER_DATA_RAND_SEED 123
#define SLAVE_DATA_RAND_SEED 456

TEST_CASE("Slave receive correct data", "[spi]")
{
   // Initialize device handle and spi bus
    unsigned int master_seed_send = MASTER_DATA_RAND_SEED;
    unsigned int slave_seed_send = SLAVE_DATA_RAND_SEED;
    unsigned int master_seed_cmp = slave_seed_send;
    unsigned int slave_seed_cmp = master_seed_send;

    const int buf_size = 20;

    WORD_ALIGNED_ATTR uint8_t slave_sendbuf[buf_size];
    WORD_ALIGNED_ATTR uint8_t slave_recvbuf[buf_size];
    WORD_ALIGNED_ATTR uint8_t master_sendbuf[buf_size];
    WORD_ALIGNED_ATTR uint8_t master_recvbuf[buf_size];

    uint8_t master_cmpbuf[buf_size];
    uint8_t slave_cmpbuf[buf_size];

    for (int spi_mode = 0; spi_mode < 4; spi_mode++) {
        for (int dma_chan = 0; dma_chan < 2; dma_chan++) {
            spi_device_handle_t spi;
            spitest_param_set_t test_param = {
                .dup = FULL_DUPLEX,
                .mode = spi_mode,
                .master_iomux = false,
                .slave_iomux = false,
                .master_dma_chan = 0,
                .slave_dma_chan = (dma_chan ? SPI_DMA_CH_AUTO: 0),
            };
            ESP_LOGI(SLAVE_TAG, "Test slave recv @ mode %d, dma enabled=%d", spi_mode, dma_chan);

            local_test_start(&spi, 1000*1000, &test_param, NULL);

            for (int round = 0; round < 20; round++) {
                // printf("trans %d\n", round);
                int master_trans_len = round + 1;
                const int slave_trans_len = 16;

                memset(master_sendbuf, 0xcc, buf_size);
                memset(slave_sendbuf, 0x55, buf_size);
                memset(master_recvbuf, 0xaa, buf_size);
                memset(slave_recvbuf, 0xbb, buf_size);

                for(int i = 0; i < master_trans_len; i++){
                    master_sendbuf[i] = rand_r(&master_seed_send);
                    slave_sendbuf[i] = rand_r(&slave_seed_send);
                }

                spi_slave_transaction_t slave_trans = {
                    .length = slave_trans_len * 8,
                    .tx_buffer = slave_sendbuf,
                    .rx_buffer = slave_recvbuf
                };
                esp_err_t ret= spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_trans, portMAX_DELAY);
                TEST_ESP_OK(ret);

                spi_transaction_t master_trans = {
                    .length = 8 * master_trans_len,
                    .tx_buffer = master_sendbuf,
                    .rx_buffer = master_recvbuf
                };
                ret = spi_device_transmit(spi, &master_trans);
                TEST_ESP_OK(ret);

                spi_slave_transaction_t *out_trans;
                ret = spi_slave_get_trans_result(TEST_SLAVE_HOST, &out_trans, portMAX_DELAY);
                TEST_ESP_OK(ret);
                TEST_ASSERT_EQUAL_HEX32(&slave_trans, out_trans);

                for(int i = 0; i < master_trans_len; i++){
                    master_cmpbuf[i] = rand_r(&master_seed_cmp);
                    slave_cmpbuf[i] = rand_r(&slave_seed_cmp);
                }

                // esp_log_buffer_hex("master_send", master_sendbuf, buf_size);
                // esp_log_buffer_hex("slave_recv", slave_recvbuf, buf_size);

                // esp_log_buffer_hex("slave_send", slave_sendbuf, buf_size);
                // esp_log_buffer_hex("master_recv", master_recvbuf, buf_size);

                int master_expected_len = MIN(master_trans_len, slave_trans_len);
                TEST_ASSERT_EQUAL_HEX8_ARRAY(master_cmpbuf, master_recvbuf, master_expected_len);

                int slave_expected_len;
                if (dma_chan) {
                    slave_expected_len = (master_expected_len & (~3));
                } else {
                    slave_expected_len = master_expected_len;
                }
                if (slave_expected_len) {
                    TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_cmpbuf, slave_recvbuf, slave_expected_len);
                }
            }
            local_test_end(spi);
        }
    }
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3, ESP32C3, ESP32C2)
//These tests are ESP32 only due to lack of runners
/********************************************************************************
 *      Test By Master & Slave (2 boards)
 *
 *  Wiring:
 * | Master | Slave |
 * | ------ | ----- |
 * | 12     | 19    |
 * | 13     | 23    |
 * | 14     | 18    |
 * | 15     | 5     |
 * | GND    | GND   |
 *
 ********************************************************************************/
static void test_master_init(void** context);
static void test_master_deinit(void* context);
static void test_master_loop(const void *test_cfg, void* context);

static const ptest_func_t master_test_func = {
    .pre_test = test_master_init,
    .post_test = test_master_deinit,
    .loop = test_master_loop,
    .def_param = spitest_def_param,
};

static void test_slave_init(void** context);
static void test_slave_deinit(void* context);
static void test_slave_loop(const void *test_cfg, void* context);

static const ptest_func_t slave_test_func = {
    .pre_test = test_slave_init,
    .post_test = test_slave_deinit,
    .loop = test_slave_loop,
    .def_param = spitest_def_param,
};

#define TEST_SPI_MASTER_SLAVE(name, param_group, extra_tag) \
    PARAM_GROUP_DECLARE(name, param_group) \
    TEST_MASTER_SLAVE(name, param_group, "[spi_ms][test_env=Example_SPI_Multi_device][timeout=120]"extra_tag, &master_test_func, &slave_test_func)

/************ Master Code ***********************************************/
static void test_master_init(void** arg)
{
    TEST_ASSERT(*arg==NULL);
    *arg = malloc(sizeof(spitest_context_t));
    spitest_context_t* context = *arg;
    TEST_ASSERT(context!=NULL);
    context->slave_context = (spi_slave_task_context_t){};
    esp_err_t err = init_slave_context(&context->slave_context);
    TEST_ASSERT(err == ESP_OK);
}

static void test_master_deinit(void* arg)
{
    spitest_context_t* context = (spitest_context_t*)arg;
    deinit_slave_context(&context->slave_context);
}

static void test_master_start(spi_device_handle_t *spi, int freq, const spitest_param_set_t* pset, spitest_context_t* context)
{
    //master config
    spi_bus_config_t buspset=SPI_BUS_TEST_DEFAULT_CONFIG();
    buspset.miso_io_num = MASTER_IOMUX_PIN_MISO;
    buspset.mosi_io_num = MASTER_IOMUX_PIN_MOSI;
    buspset.sclk_io_num = MASTER_IOMUX_PIN_SCLK;
    //this does nothing, but avoid the driver from using native pins
    if (!pset->master_iomux) buspset.quadhd_io_num = UNCONNECTED_PIN;
    spi_device_interface_config_t devpset=SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devpset.spics_io_num = MASTER_IOMUX_PIN_CS;
    devpset.mode = pset->mode;
    const int cs_pretrans_max = 15;
    if (pset->dup==HALF_DUPLEX_MISO) {
        devpset.cs_ena_pretrans = cs_pretrans_max;
        devpset.flags |= SPI_DEVICE_HALFDUPLEX;
    } else if (pset->dup == HALF_DUPLEX_MOSI) {
        devpset.cs_ena_pretrans = cs_pretrans_max;
        devpset.flags |= SPI_DEVICE_NO_DUMMY;
    } else {
        devpset.cs_ena_pretrans = cs_pretrans_max;//20;
    }
    const int cs_posttrans_max = 15;
    devpset.cs_ena_posttrans = cs_posttrans_max;
    devpset.input_delay_ns = pset->slave_tv_ns;
    devpset.clock_speed_hz = freq;
    if (pset->master_limit != 0 && freq > pset->master_limit) devpset.flags |= SPI_DEVICE_NO_DUMMY;

    int dma_chan = (pset->master_dma_chan == 0) ? 0 : SPI_DMA_CH_AUTO;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buspset, dma_chan));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devpset, spi));

    //prepare data for the slave
    for (int i = 0; i < pset->test_size; i ++) {
        /* in the single board, the data is send to the slave task, then to the driver.
        * However, in this test we don't know the data received by the slave.
        * So we send to the return queue of the slave directly.
        */
        //xQueueSend( slave_context.data_to_send, &slave_txdata[i], portMAX_DELAY );

        uint8_t slave_buffer[320+8];
        int length;
        if (pset->dup!=HALF_DUPLEX_MISO) {
            length = context->master_trans[i].length;
        } else {
            length = context->master_trans[i].rxlength;
        }
        uint32_t* ptr = (uint32_t*)slave_buffer;
        ptr[0] = length;
        ptr[1] = (uint32_t)context->slave_trans[i].start;
        if (context->master_trans[i].tx_buffer!=NULL) {
            memcpy(ptr+2, context->master_trans[i].tx_buffer, (context->master_trans[i].length+7)/8);
        }
        //Send to return queue directly
        xRingbufferSend(context->slave_context.data_received, slave_buffer, 8+(length+7)/8, portMAX_DELAY);
    }
    memset(context->master_rxbuf, 0x66, sizeof(context->master_rxbuf));
}

static void test_master_loop(const void *arg1, void* arg2)
{
    const spitest_param_set_t *test_cfg = (spitest_param_set_t*)arg1;
    spitest_context_t* context = (spitest_context_t*)arg2;
    spi_device_handle_t spi;
    spitest_init_transactions(test_cfg, context);
    const int *timing_speed_array = test_cfg->freq_list;

    ESP_LOGI(MASTER_TAG, "****************** %s ***************", test_cfg->pset_name);
    for (int i=0; ; i++ ) {
        const int freq = timing_speed_array[i];
        if (freq==0) break;
        if (test_cfg->freq_limit && freq > test_cfg->freq_limit) break;

        ESP_LOGI(MASTER_TAG, "==============> %dk", freq/1000);
        test_master_start(&spi, freq, test_cfg, context);

        unity_wait_for_signal("slave ready");

        for( int j= 0; j < test_cfg->test_size; j ++ ) {
            //wait for both master and slave end
            ESP_LOGI( MASTER_TAG, "=> test%d", j );
            //send master tx data
            vTaskDelay(20);

            spi_transaction_t *t = &context->master_trans[j];
            TEST_ESP_OK (spi_device_transmit(spi, t) );
            int len = get_trans_len(test_cfg->dup, t);
            spitest_master_print_data(t, len);

            size_t rcv_len;
            slave_rxdata_t *rcv_data = xRingbufferReceive( context->slave_context.data_received, &rcv_len, portMAX_DELAY );
            spitest_slave_print_data(rcv_data, false);

            //check result
            bool check_master_data = (test_cfg->dup != HALF_DUPLEX_MOSI &&
                        (test_cfg->master_limit == 0 || freq <= test_cfg->master_limit));
            const bool check_slave_data = false;
            const bool check_len = false;
            if (!check_master_data) {
                ESP_LOGI(MASTER_TAG, "skip data check due to duplex mode or freq.");
            } else {
                TEST_ESP_OK(spitest_check_data(len, t, rcv_data, check_master_data,
                    check_len, check_slave_data));
            }
            //clean
            vRingbufferReturnItem( context->slave_context.data_received, rcv_data );
        }
        master_free_device_bus(spi);
    }
}

/************ Slave Code ***********************************************/
static void test_slave_init(void** arg)
{
    TEST_ASSERT(*arg==NULL);
    *arg = malloc(sizeof(spitest_context_t));
    spitest_context_t* context = (spitest_context_t*)*arg;
    TEST_ASSERT(context!=NULL);
    context->slave_context = (spi_slave_task_context_t){};
    esp_err_t err = init_slave_context( &context->slave_context );
    TEST_ASSERT( err == ESP_OK );

    xTaskCreate( spitest_slave_task, "spi_slave", 4096, &context->slave_context, 0, &context->handle_slave);
}

static void test_slave_deinit(void* arg)
{
    spitest_context_t* context = (spitest_context_t*)arg;
    vTaskDelete( context->handle_slave );
    context->handle_slave = 0;

    deinit_slave_context(&context->slave_context);
}

static void timing_slave_start(int speed, const spitest_param_set_t* pset, spitest_context_t *context)
{
    //slave config
    spi_bus_config_t slv_buscfg=SPI_BUS_TEST_DEFAULT_CONFIG();
    slv_buscfg.miso_io_num = SLAVE_IOMUX_PIN_MISO;
    slv_buscfg.mosi_io_num = SLAVE_IOMUX_PIN_MOSI;
    slv_buscfg.sclk_io_num = SLAVE_IOMUX_PIN_SCLK;
    //this does nothing, but avoid the driver from using native pins
    if (!pset->slave_iomux) slv_buscfg.quadhd_io_num = UNCONNECTED_PIN;
    spi_slave_interface_config_t slvcfg=SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.spics_io_num = SLAVE_IOMUX_PIN_CS;
    slvcfg.mode = pset->mode;
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    slave_pull_up(&slv_buscfg, slvcfg.spics_io_num);

    int slave_dma_chan = (pset->slave_dma_chan == 0) ? 0 : SPI_DMA_CH_AUTO;
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &slv_buscfg, &slvcfg, slave_dma_chan));

    //prepare data for the master
    for (int i = 0; i < pset->test_size; i++) {
        if (pset->dup==FULL_DUPLEX) {
            memcpy(context->master_trans[i].rx_buffer, context->slave_trans[i].start, (context->master_trans[i].length+7)/8);
        } else if (pset->dup==HALF_DUPLEX_MISO) {
            memcpy(context->master_trans[i].rx_buffer, context->slave_trans[i].start, (context->master_trans[i].rxlength+7)/8);
        }
    }
}

static void test_slave_loop(const void *arg1, void* arg2)
{
    const spitest_param_set_t *pset = (spitest_param_set_t*)arg1;
    spitest_context_t* context = (spitest_context_t*)arg2;
    ESP_LOGI(SLAVE_TAG, "****************** %s ***************", pset->pset_name);
    spitest_init_transactions(pset, context);

    const int *timing_speed_array = pset->freq_list;
    for (int i=0; ; i++ ) {
        const int freq = timing_speed_array[i];
        if (freq==0) break;
        if (pset->freq_limit != 0 && freq > pset->freq_limit) break;

        ESP_LOGI(MASTER_TAG, "==============> %dk", timing_speed_array[i]/1000);
        //Initialize SPI slave interface
        timing_slave_start(freq, pset, context);

        //prepare slave tx data
        for (int i = 0; i < pset->test_size; i ++) {
            xQueueSend( context->slave_context.data_to_send, &context->slave_trans[i], portMAX_DELAY );
            //memcpy(context->master_trans[i].rx_buffer, context->slave_trans[i].start, (context->master_trans[i].length+7)/8);
        }

        vTaskDelay(50/portTICK_PERIOD_MS);
        unity_send_signal("slave ready");

        for( int i= 0; i < pset->test_size; i ++ ) {
            //wait for both master and slave end
            ESP_LOGI( MASTER_TAG, "===== test%d =====", i );
            //send master tx data
            vTaskDelay(20);

            spi_transaction_t *t = &context->master_trans[i];
            int len = get_trans_len(pset->dup, t);
            spitest_master_print_data(t, FULL_DUPLEX);

            size_t rcv_len;
            slave_rxdata_t *rcv_data = xRingbufferReceive( context->slave_context.data_received, &rcv_len, portMAX_DELAY );
            spitest_slave_print_data(rcv_data, true);

            //check result
            const bool check_master_data = false;
            bool check_slave_data = (pset->dup!=HALF_DUPLEX_MISO);
            const bool check_len = true;
            TEST_ESP_OK(spitest_check_data(len, t, rcv_data, check_master_data, check_len, check_slave_data));
            //clean
            vRingbufferReturnItem( context->slave_context.data_received, rcv_data );
        }
        TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);
    }
}

/************ Timing Test ***********************************************/
static spitest_param_set_t timing_conf[] = {
    {   .pset_name = "FULL_DUP, BOTH IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .master_limit = SPI_MASTER_FREQ_16M,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "FULL_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .master_limit = SPI_MASTER_FREQ_11M,
        .dup = FULL_DUPLEX,
        .master_iomux = true,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
    {   .pset_name = "FULL_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .master_limit = SPI_MASTER_FREQ_11M,
        .dup = FULL_DUPLEX,
        .master_iomux = false,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "FULL_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .master_limit = SPI_MASTER_FREQ_9M,
        .dup = FULL_DUPLEX,
        .master_iomux = false,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
    {   .pset_name = "MOSI_DUP, BOTH IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "MOSI_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux= true,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
    {   .pset_name = "MOSI_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux= false,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "MOSI_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MOSI,
        .master_iomux= false,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
    {   .pset_name = "MISO_DUP, BOTH IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "MISO_DUP, MASTER IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = true,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
    {   .pset_name = "MISO_DUP, SLAVE IOMUX",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = false,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
    },
    {   .pset_name = "MISO_DUP, BOTH GPIO",
        .freq_limit = ESP_SPI_SLAVE_MAX_FREQ,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux = false,
        .slave_iomux = false,
        .slave_tv_ns = TV_WITH_ESP_SLAVE_GPIO,
    },
};
TEST_SPI_MASTER_SLAVE(TIMING, timing_conf, "")

/************ Mode Test ***********************************************/
#define FREQ_LIMIT_MODE SPI_MASTER_FREQ_16M
//Set to this input delay so that the master will read with delay until 7M
#define DELAY_HCLK_UNTIL_7M    12.5*3

static int test_freq_mode_ms[]={
    100*1000,
    6*1000*1000,
    7*1000*1000,
    SPI_MASTER_FREQ_8M, //maximum freq MISO stable before next latch edge
    SPI_MASTER_FREQ_9M, //maximum freq MISO stable before next latch edge
    SPI_MASTER_FREQ_10M,
    SPI_MASTER_FREQ_11M,
    SPI_MASTER_FREQ_13M,
    SPI_MASTER_FREQ_16M,
    SPI_MASTER_FREQ_20M,
    0,
};
static int test_freq_20M_only[]={
    SPI_MASTER_FREQ_20M,
    0,
};

spitest_param_set_t mode_conf[] = {
    //non-DMA tests
    {   .pset_name = "mode 0, no DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 0,
    },
    {   .pset_name = "mode 1, no DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 1,
    },
    {   .pset_name = "mode 2, no DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 2,
    },
    {   .pset_name = "mode 3, no DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 3,
    },
    //the master can only read to 16MHz, use half-duplex mode to read at 20.
    {   .pset_name = "mode 0, no DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 0,
    },
    {   .pset_name = "mode 1, no DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 1,
    },
    {   .pset_name = "mode 2, no DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 2,
    },
    {   .pset_name = "mode 3, no DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 3,
    },
    //DMA tests
    {   .pset_name = "mode 0, DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = DELAY_HCLK_UNTIL_7M,
        .mode = 0,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .length_aligned = true,
    },
    {   .pset_name = "mode 1, DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 1,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .length_aligned = true,
    },
    {   .pset_name = "mode 2, DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = DELAY_HCLK_UNTIL_7M,
        .mode = 2,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .length_aligned = true,
    },
    {   .pset_name = "mode 3, DMA",
        .freq_list = test_freq_mode_ms,
        .master_limit = FREQ_LIMIT_MODE,
        .dup = FULL_DUPLEX,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 3,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
        .length_aligned = true,
    },
    //the master can only read to 16MHz, use half-duplex mode to read at 20.
    {   .pset_name = "mode 0, DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 0,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
    },
    {   .pset_name = "mode 1, DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 1,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
    },
    {   .pset_name = "mode 2, DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 2,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
    },
    {   .pset_name = "mode 3, DMA, 20M",
        .freq_list = test_freq_20M_only,
        .dup = HALF_DUPLEX_MISO,
        .master_iomux= true,
        .slave_iomux = true,
        .slave_tv_ns = TV_WITH_ESP_SLAVE,
        .mode = 3,
        .master_dma_chan = SPI_DMA_CH_AUTO,
        .slave_dma_chan = SPI_DMA_CH_AUTO,
    },
};
TEST_SPI_MASTER_SLAVE(MODE, mode_conf, "")

#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3, ESP32C3, ESP32C2)

#endif // #if (TEST_SPI_PERIPH_NUM >= 2)
