#include "test/test_common_spi.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "soc/spi_periph.h"
#include "test/test_common_spi.h"

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
    TEST_LOCAL(name, param_set, "[spi][timeout=120]", &local_test_func)

static void local_test_init(void** arg)
{
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
        buscfg.miso_io_num = spi_periph_signal[VSPI_HOST].spiq_iomux_pin;
        buscfg.mosi_io_num = spi_periph_signal[VSPI_HOST].spid_iomux_pin;
        buscfg.sclk_io_num = spi_periph_signal[VSPI_HOST].spiclk_iomux_pin;
        devcfg.spics_io_num = spi_periph_signal[VSPI_HOST].spics0_iomux_pin;
        slvcfg.spics_io_num = spi_periph_signal[VSPI_HOST].spics0_iomux_pin;
    } else {
        buscfg.miso_io_num = spi_periph_signal[HSPI_HOST].spiq_iomux_pin;
        buscfg.mosi_io_num = spi_periph_signal[HSPI_HOST].spid_iomux_pin;
        buscfg.sclk_io_num = spi_periph_signal[HSPI_HOST].spiclk_iomux_pin;
        devcfg.spics_io_num = spi_periph_signal[HSPI_HOST].spics0_iomux_pin;
        slvcfg.spics_io_num = spi_periph_signal[HSPI_HOST].spics0_iomux_pin;
    }
    //this does nothing, but avoid the driver from using iomux pins if required
    buscfg.quadhd_io_num = (!pset->master_iomux && !pset->slave_iomux ? spi_periph_signal[VSPI_HOST].spiq_iomux_pin : -1);
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

    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, pset->master_dma_chan));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, spi));

    //slave automatically use iomux pins if pins are on VSPI_* pins
    buscfg.quadhd_io_num = -1;
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &buscfg, &slvcfg, pset->slave_dma_chan));

    //initialize master and slave on the same pins break some of the output configs, fix them
    if (pset->master_iomux) {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_SPI, spi_periph_signal[HSPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[VSPI_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_SPI, spi_periph_signal[HSPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_SPI, spi_periph_signal[HSPI_HOST].spiclk_out);
    } else if (pset->slave_iomux) {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_SPI, spi_periph_signal[VSPI_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spiclk_out);
    } else {
        spitest_gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spid_out);
        spitest_gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[VSPI_HOST].spiq_out);
        spitest_gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spics_out[0]);
        spitest_gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[HSPI_HOST].spiclk_out);
    }

    //prepare slave tx data
    for (int k = 0; k < pset->test_size; k++)
        xQueueSend(context->slave_context.data_to_send, &context->slave_trans[k], portMAX_DELAY);

    //clear master receive buffer
    memset(context->master_rxbuf, 0x66, sizeof(context->master_rxbuf));

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

        ESP_LOGI(MASTER_TAG, "======> %dk", freq / 1000);
        local_test_start(&spi, freq, pset, context);

        for (int k = 0; k < pset->test_size; k++) {
            //wait for both master and slave end
            ESP_LOGI(MASTER_TAG, "=> test%d", k);
            //send master tx data
            vTaskDelay(9);

            spi_transaction_t *t = &context->master_trans[k];
            TEST_ESP_OK(spi_device_transmit(spi, t));
            int len = get_trans_len(pset->dup, t);
            spitest_master_print_data(t, len);

            size_t rcv_len;
            slave_rxdata_t *rcv_data = xRingbufferReceive(context->slave_context.data_received, &rcv_len, portMAX_DELAY);
            spitest_slave_print_data(rcv_data, true);

            //check result
            bool check_master_data = (pset->dup!=HALF_DUPLEX_MOSI &&
                    (pset->master_limit==0 || freq <= pset->master_limit));
            bool check_slave_data = (pset->dup!=HALF_DUPLEX_MISO);
            const bool check_len = true;
            if (!check_master_data) ESP_LOGI(MASTER_TAG, "skip master data check");
            if (!check_slave_data) ESP_LOGI(SLAVE_TAG, "skip slave data check");

            TEST_ESP_OK(spitest_check_data(len, t, rcv_data, check_master_data, check_len, check_slave_data));
            //clean
            vRingbufferReturnItem(context->slave_context.data_received, rcv_data);
        }
        master_free_device_bus(spi);
        TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);
    }
}

/************ Timing Test ***********************************************/
static spitest_param_set_t timing_pgroup[] = {
    { .pset_name = "FULL_DUP, MASTER IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .pset_name = "FULL_DUP, SLAVE IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "FULL_DUP, BOTH GPIO",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .master_limit = SPI_MASTER_FREQ_10M,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .pset_name = "MISO_DUP, MASTER IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .master_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO+12.5,
                    //for freq lower than 20M, the delay is 60(62.5)ns, however, the delay becomes 75ns over 26M
    },
    { .pset_name = "MISO_DUP, SLAVE IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      //.freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT+12.5,
                    //for freq lower than 20M, the delay is 60(62.5)ns, however, the delay becomes 75ns over 26M

    },
    { .pset_name = "MISO_DUP, BOTH GPIO",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      //.freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO+12.5,
                    //for freq lower than 20M, the delay is 60(62.5)ns, however, the delay becomes 75ns over 26M

    },
    { .pset_name = "MOSI_DUP, MASTER IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      //.freq_limit = ESP_SPI_SLAVE_MAX_READ_FREQ, //ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MOSI,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .pset_name = "MOSI_DUP, SLAVE IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      //.freq_limit = ESP_SPI_SLAVE_MAX_READ_FREQ, //ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MOSI,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "MOSI_DUP, BOTH GPIO",
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

static spitest_param_set_t mode_pgroup[] = {
    { .pset_name = "Mode 0",
      .freq_list = test_freq_mode_local,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 0,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "Mode 1",
      .freq_list = test_freq_mode_local,
      .freq_limit = SPI_MASTER_FREQ_26M,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 1,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "Mode 2",
      .freq_list = test_freq_mode_local,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "Mode 3",
      .freq_list = test_freq_mode_local,
      .freq_limit = SPI_MASTER_FREQ_26M,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 3,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "Mode 0, DMA",
      .freq_list = test_freq_mode_local,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 0,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT, //at 16M, the MISO delay (-0.5T+(3+2)apb) equals to non-DMA mode delay (3apb).
      .length_aligned = true,
    },
    { .pset_name = "Mode 1, DMA",
      .freq_list = test_freq_mode_local,
      .freq_limit = SPI_MASTER_FREQ_26M,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 1,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
      .length_aligned = true,
    },
    { .pset_name = "Mode 2, DMA",
      .freq_list = test_freq_mode_local,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 2,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT, //at 16M, the MISO delay (-0.5T+(3+2)apb) equals to non-DMA mode delay (3apb).
      .length_aligned = true,
    },
    { .pset_name = "Mode 3, DMA",
      .freq_list = test_freq_mode_local,
      .freq_limit = SPI_MASTER_FREQ_26M,
      .master_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .mode = 3,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
      .length_aligned = true,
    },
    // MISO ////////////////////////////////////
    { .pset_name = "MISO, Mode 0",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 0,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "MISO, Mode 1",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 1,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "MISO, Mode 2",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "MISO, Mode 3",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 3,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .pset_name = "MISO, Mode 0, DMA",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 0,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT+12.5, //at 16M, the MISO delay (-0.5T+(3+2)apb) equals to non-DMA mode delay (3apb).
      .length_aligned = true,
    },
    { .pset_name = "MISO, Mode 1, DMA",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 1,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
      .length_aligned = true,
    },
    { .pset_name = "MISO, Mode 2, DMA",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 2,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT+12.5, //at 16M, the MISO delay (-0.5T+(3+2)apb) equals to non-DMA mode delay (3apb).
      .length_aligned = true,
    },
    { .pset_name = "MISO, Mode 3, DMA",
      .freq_list = test_freq_mode_local,
      .dup = HALF_DUPLEX_MISO,
      .mode = 3,
      .slave_dma_chan = 2,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
      .length_aligned = true,
    },
};
TEST_SPI_LOCAL(MODE, mode_pgroup)
