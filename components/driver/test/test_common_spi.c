#include "test/test_common_spi.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "driver/gpio.h"

int test_freq_default[]=TEST_FREQ_DEFAULT();

const char MASTER_TAG[] = "test_master";
const char SLAVE_TAG[] = "test_slave";

DRAM_ATTR uint8_t spitest_master_send[] = {
    0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43,
    0x74,
    0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43,
    0x74,
    0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43,
    0x74,
    };
DRAM_ATTR uint8_t spitest_slave_send[] = {
    0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0,
    0xda,
    0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0,
    0xda,
    0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0,
    0xda,
    };

void spitest_def_param(void* arg)
{
    spitest_param_set_t *param_set=(spitest_param_set_t*)arg;
    param_set->test_size = 8;
    if (param_set->freq_list==NULL) param_set->freq_list = test_freq_default;
}

/**********************************************************************************
 * functions for slave task
 *********************************************************************************/
esp_err_t init_slave_context(spi_slave_task_context_t *context)
{
    context->data_to_send = xQueueCreate( 16, sizeof( slave_txdata_t ));
    if ( context->data_to_send == NULL ) {
        return ESP_ERR_NO_MEM;
    }
    context->data_received = xRingbufferCreate( 1024, RINGBUF_TYPE_NOSPLIT );
    if ( context->data_received == NULL ) {
        return ESP_ERR_NO_MEM;
    }
    context->spi=TEST_SLAVE_HOST;
    return ESP_OK;
}

void deinit_slave_context(spi_slave_task_context_t *context)
{
    TEST_ASSERT( context->data_to_send != NULL );
    vQueueDelete( context->data_to_send );
    context->data_to_send = NULL;
    TEST_ASSERT( context->data_received != NULL );
    vRingbufferDelete( context->data_received );
    context->data_received = NULL;
}

/* The task requires a queue and a ringbuf, which should be initialized before task starts.
   Send ``slave_txdata_t`` to the queue to make the task send data;
   the task returns data got to the ringbuf, which should have sufficient size.
*/
void spitest_slave_task(void* arg)
{
    spi_slave_task_context_t* context = (spi_slave_task_context_t*) arg;
    QueueHandle_t queue = context->data_to_send;
    RingbufHandle_t ringbuf = context->data_received;
    uint8_t recvbuf[320+8];
    slave_txdata_t txdata;

    ESP_LOGI( SLAVE_TAG, "slave up" );
    //never quit, but blocked by the queue, waiting to be killed, when no more send from main task.
    while( 1 ) {
        BaseType_t ret = xQueueReceive( queue, &txdata, portMAX_DELAY );
        assert(ret);

        spi_slave_transaction_t t = {};
        t.length = txdata.len;
        t.tx_buffer = txdata.start;
        t.rx_buffer = recvbuf+8;
        //loop until trans_len != 0 to skip glitches
        do {
            TEST_ESP_OK( spi_slave_transmit( context->spi, &t, portMAX_DELAY ) );
        } while ( t.trans_len <= 2 );
        memcpy(recvbuf, &t.trans_len, sizeof(uint32_t));
        *(uint8_t**)(recvbuf+4) = (uint8_t*)txdata.start;
        ESP_LOGI( SLAVE_TAG, "received: %d", t.trans_len );
        xRingbufferSend( ringbuf, recvbuf, 8+(t.trans_len+7)/8, portMAX_DELAY );
    }
}

void slave_pull_up(const spi_bus_config_t* cfg, int spics_io_num)
{
    gpio_set_pull_mode(cfg->mosi_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(cfg->sclk_io_num, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(spics_io_num, GPIO_PULLUP_ONLY);
}

/**********************************************************************************
 * functions for slave task
 *********************************************************************************/

static int test_len[] = {1, 3, 5, 7, 9, 11, 33, 64};

void spitest_init_transactions(const spitest_param_set_t *cfg, spitest_context_t* context)
{
    spi_transaction_t* trans = context->master_trans;
    uint8_t *rx_buf_ptr = context->master_rxbuf;
    const spi_dup_t dup = cfg->dup;

    for (int i = 0; i < cfg->test_size; i++) {
        const void* tx_buffer = spitest_master_send + i%8;
        int length = 8*test_len[i];
        if (cfg->length_aligned) length = (length+31)&(~31);

        if (dup == HALF_DUPLEX_MISO) {
            trans[i] = (spi_transaction_t) {
                .rx_buffer = rx_buf_ptr,
                .rxlength = length,
            };
        } else if (dup == HALF_DUPLEX_MOSI) {
            trans[i] = (spi_transaction_t) {
                .tx_buffer = tx_buffer,
                .length = length,
            };
        } else {
            trans[i] = (spi_transaction_t) {
                .tx_buffer = tx_buffer,
                .length = length,
                .rx_buffer = rx_buf_ptr,
            };
        }
        rx_buf_ptr = (uint8_t*)( (uint32_t)(rx_buf_ptr + (length+7)/8 + 3) & (~3));

        const void* slave_tx = spitest_slave_send + (cfg->slave_unaligned_addr? i%3: (i%3)*4);
        //prepare slave tx data
        context->slave_trans[i] = (slave_txdata_t) {
            .start = slave_tx,
            .len = 512,
        };
        if (cfg->slave_dma_chan != 0) context->slave_trans[i].len = 1024;
    }
}

void spitest_master_print_data(spi_transaction_t *t, int rxlength)
{
    if (t->tx_buffer) ESP_LOG_BUFFER_HEX( "master tx", t->tx_buffer, t->length/8 );
    if (t->rx_buffer) ESP_LOG_BUFFER_HEX( "master rx", t->rx_buffer, rxlength/8 );
}

void spitest_slave_print_data(slave_rxdata_t *t, bool print_rxdata)
{
    int rcv_len = (t->len+7)/8;
    ESP_LOGI(SLAVE_TAG, "trans_len: %d", t->len);
    ESP_LOG_BUFFER_HEX("slave tx", t->tx_start, rcv_len);
    if (print_rxdata) ESP_LOG_BUFFER_HEX("slave rx", t->data, rcv_len);
}

esp_err_t spitest_check_data(int len, spi_transaction_t *master_t, slave_rxdata_t *slave_t, bool check_master_data, bool check_slave_len, bool check_slave_data)
{
    //currently the rcv_len can be in range of [t->length-1, t->length+3]
    if (check_slave_len) {
        uint32_t rcv_len = slave_t->len;
        TEST_ASSERT(rcv_len >= len - 1 && rcv_len <= len + 4);
    }

    if (check_master_data) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_t->tx_start, master_t->rx_buffer, (len + 7) / 8);
    }

    if (check_slave_data) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(master_t->tx_buffer, slave_t->data, (len + 7) / 8);
    }
    return ESP_OK;
}


void master_free_device_bus(spi_device_handle_t spi)
{
    TEST_ESP_OK( spi_bus_remove_device(spi) );
    TEST_ESP_OK( spi_bus_free(TEST_SPI_HOST) );
}

void spitest_gpio_output_sel(uint32_t gpio_num, int func, uint32_t signal_idx)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], func);
    GPIO.func_out_sel_cfg[gpio_num].func_sel=signal_idx;
}