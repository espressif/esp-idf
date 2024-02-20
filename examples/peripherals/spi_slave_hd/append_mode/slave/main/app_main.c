/* SPI Slave Halfduplex example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/spi_slave_hd.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your Hardware spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GPIO_MOSI           11
#define GPIO_MISO           13
#define GPIO_SCLK           12
#define GPIO_CS             10

#define HOST_ID             SPI2_HOST
#define QUEUE_SIZE          6
#define TRANSACTION_LEN     16
#define SYNC_REG_FROM_HOST  (14 * 4)
#define SYNC_REG_TO_HOST    (15 * 4)

//---------This should be negotiated with the Master!!!!-------------//
#define SLAVE_READY_FLAG    0x88
#define READY_FLAG_REG      0

struct trans_link_s {
    spi_slave_hd_data_t trans;
    struct trans_link_s *next;
    bool recycled;      //1: the current transaction descriptor is processed by the HW already, it is available and can be reused for new transaction
};
typedef struct trans_link_s trans_link_t;

/* Pointer to the current transaction */
trans_link_t *tx_curr_trans;
trans_link_t *rx_curr_trans;

static void init_slave_hd(void)
{
    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = GPIO_MOSI;
    bus_cfg.miso_io_num = GPIO_MISO;
    bus_cfg.sclk_io_num = GPIO_SCLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 50000;

    spi_slave_hd_slot_config_t slave_hd_cfg = {};
    slave_hd_cfg.spics_io_num = GPIO_CS;
    slave_hd_cfg.flags |= SPI_SLAVE_HD_APPEND_MODE;
    slave_hd_cfg.mode = 0;
    slave_hd_cfg.command_bits = 8;
    slave_hd_cfg.address_bits = 8;
    slave_hd_cfg.dummy_bits = 8;
    slave_hd_cfg.queue_size = QUEUE_SIZE;
    slave_hd_cfg.dma_chan = SPI_DMA_CH_AUTO;

    ESP_ERROR_CHECK(spi_slave_hd_init(HOST_ID, &bus_cfg, &slave_hd_cfg));
}

//Create a link to the transaction descriptors, malloc the transaction buffers
static esp_err_t create_transaction_pool(uint8_t **data_buf, trans_link_t *trans_link, uint16_t times)
{
    for (int i = 0; i < times; i++) {
        //malloc data buffers for transaction
        data_buf[i] = heap_caps_calloc(1, TRANSACTION_LEN, MALLOC_CAP_DMA);
        if (!data_buf[i]) {
            ESP_LOGI("Create pool:", "No enough memory");
            return ESP_ERR_NO_MEM;
        }

        //attach data buffer and transaction descriptor
        trans_link[i].trans.data = data_buf[i];

        //link the recycling transaction descriptors
        if (i != QUEUE_SIZE - 1) {
            trans_link[i].next = &trans_link[i + 1];
        } else {
            trans_link[i].next = &trans_link[0];
        }

        //init transaction descriptor as available
        trans_link[i].recycled = 1;
    }
    return ESP_OK;
}

//-----------------------------------------------------TX Transaction-----------------------------------------------//
static void prepare_tx_data(trans_link_t *tx_trans)
{
    /**
     * Apply Your Own Data Here
     */
    uint8_t data = rand() % 50;
    tx_trans->trans.len = TRANSACTION_LEN;
    for (int i = 0; i < tx_trans->trans.len; i++) {
        tx_trans->trans.data[i] = data + i;
    }
    tx_trans->recycled = 0;
}

static bool get_tx_transaction_descriptor(trans_link_t **out_trans)
{
    if (tx_curr_trans->recycled == 0) {
        return false;
    }
    *out_trans = tx_curr_trans;
    tx_curr_trans = tx_curr_trans->next;
    return true;
}

void sendTask(void *arg)
{
    uint8_t *tx_buffer[QUEUE_SIZE] = {};
    trans_link_t trans_link[QUEUE_SIZE] = {};
    trans_link_t *trans_to_send;    //The transaction to send data, should get from ``get_tx_transaction_descriptor``
    tx_curr_trans = trans_link;

    ESP_ERROR_CHECK(create_transaction_pool(tx_buffer, trans_link, QUEUE_SIZE));
    //This variable is used to check if you're using transaction descriptors more than you prepared in the pool
    bool get_desc_success = false;

    //This is the total size of the buffers that are loaded by Slave
    uint32_t total_load_buf_size = 0;

    /**
     * Start transactions until internal queue is full (equals QUEUE_SIZE)
     *
     * - The ``spi_slave_hd_append_trans`` API will pre-load Slave's transaction to the hardware.
     * - You don't need a callback to achieve this (comparing to Segment Mode). Therefore, Slave doesn't need to wait until Master finishes its operation.
     *   These transactions would be a queue for Master to read. So the speed will be faster.
     */
    for (int i = 0; i < QUEUE_SIZE; i++) {
        get_desc_success = get_tx_transaction_descriptor(&trans_to_send);
        if (get_desc_success) {
            prepare_tx_data(trans_to_send);
            ESP_ERROR_CHECK(spi_slave_hd_append_trans(HOST_ID, SPI_SLAVE_CHAN_TX, &trans_to_send->trans, portMAX_DELAY));

            //Inform Master the number of bytes that Slave has loaded
            total_load_buf_size += TRANSACTION_LEN;
            spi_slave_hd_write_buffer(HOST_ID, SYNC_REG_TO_HOST, (uint8_t *)&total_load_buf_size, 4);
        }
    }

    //Get one result and load a new transaction
    while (1) {
        spi_slave_hd_data_t *ret_trans;
        trans_link_t *ret_link;
        //Get the transaction descriptor that is already procecssed by the HW and can be recycled
        ESP_ERROR_CHECK(spi_slave_hd_get_append_trans_res(HOST_ID, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
        ret_link = __containerof(ret_trans, trans_link_t, trans);
        ret_link->recycled = 1;

        get_desc_success = get_tx_transaction_descriptor(&trans_to_send);
        if (get_desc_success) {
            prepare_tx_data(trans_to_send);
            ESP_ERROR_CHECK(spi_slave_hd_append_trans(HOST_ID, SPI_SLAVE_CHAN_TX, &trans_to_send->trans, portMAX_DELAY));

            //Inform Master the number of bytes that Slave has loaded
            total_load_buf_size += TRANSACTION_LEN;
            spi_slave_hd_write_buffer(HOST_ID, SYNC_REG_TO_HOST, (uint8_t *)&total_load_buf_size, 4);
        }
    }
}

//-----------------------------------------------------RX Transaction-----------------------------------------------//
static bool get_rx_transaction_descriptor(trans_link_t **out_trans)
{
    if (rx_curr_trans->recycled == 0) {
        return false;
    }
    rx_curr_trans->trans.len = TRANSACTION_LEN;
    *out_trans = rx_curr_trans;
    rx_curr_trans = rx_curr_trans->next;
    return true;
}

void recvTask(void *arg)
{
    uint8_t *rx_buffer[QUEUE_SIZE] = {};
    trans_link_t trans_link[QUEUE_SIZE] = {};
    trans_link_t *trans_for_recv;   //The transaction to receive data, should get from ``get_rx_transaction_descriptor``
    rx_curr_trans = trans_link;

    ESP_ERROR_CHECK(create_transaction_pool(rx_buffer, trans_link, QUEUE_SIZE));
    //This variable is used to check if you're using transaction descriptors more than you prepared in the pool
    bool get_desc_success = false;

    //This is the number of the buffers that are loaded by Slave. The buffer size (TRANSACTION_LEN) should be pre-negotiate with Master.
    uint32_t total_load_buf_num = 0;

    /**
     * Start transactions until internal queue is full (equals QUEUE_SIZE)
     *
     * - The ``spi_slave_hd_append_trans`` API will pre-load Slave's transaction to the hardware.
     * - You don't need a callback to achieve this (comparing to Segment Mode). Therefore, Slave doesn't need to wait until Master finishes its operation.
     *   These transactions would be a queue for Master to send its data. So the speed will be faster.
     */
    for (int i = 0; i < QUEUE_SIZE; i++) {
        get_desc_success = get_rx_transaction_descriptor(&trans_for_recv);
        if (get_desc_success) {
            ESP_ERROR_CHECK(spi_slave_hd_append_trans(HOST_ID, SPI_SLAVE_CHAN_RX, &trans_for_recv->trans, portMAX_DELAY));

            //Inform Master the number of buffer that Slave has loaded
            total_load_buf_num += 1;
            spi_slave_hd_write_buffer(HOST_ID, SYNC_REG_FROM_HOST, (uint8_t *)&total_load_buf_num, 4);
        }
    }

    while (1) {
        spi_slave_hd_data_t *ret_trans;
        trans_link_t *ret_link;
        //Get the transaction descriptor that is already procecssed by the HW and can be recycled
        spi_slave_hd_get_append_trans_res(HOST_ID, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY);
        ret_link = __containerof(ret_trans, trans_link_t, trans);
        ret_link->recycled = 1;
        ESP_LOGI("Receiver", "%d bytes are actually received:", ret_trans->trans_len);
        ESP_LOG_BUFFER_HEX("slave RX:", ret_trans->data, ret_trans->trans_len);

        get_desc_success = get_rx_transaction_descriptor(&trans_for_recv);
        if (get_desc_success) {
            ESP_ERROR_CHECK(spi_slave_hd_append_trans(HOST_ID, SPI_SLAVE_CHAN_RX, &trans_for_recv->trans, portMAX_DELAY));

            //Inform Master the number of buffer that Slave has loaded
            total_load_buf_num += 1;
            spi_slave_hd_write_buffer(HOST_ID, SYNC_REG_FROM_HOST, (uint8_t *)&total_load_buf_num, 4);
        }
    }
}

void app_main(void)
{
    init_slave_hd();

    //Init the shared register
    uint8_t init_value[SOC_SPI_MAXIMUM_BUFFER_SIZE] = {0x0};
    spi_slave_hd_write_buffer(HOST_ID, 0, init_value, SOC_SPI_MAXIMUM_BUFFER_SIZE);

    uint8_t ready_flag = SLAVE_READY_FLAG;
    spi_slave_hd_write_buffer(HOST_ID, READY_FLAG_REG, &ready_flag, 4);

    xTaskCreate(sendTask, "sendTask", 4096, NULL, 1, NULL);
    xTaskCreate(recvTask, "recvTask", 4096, NULL, 1, NULL);
}
