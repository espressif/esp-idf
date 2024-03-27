/* SPI Slave Halfduplex example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_slave_hd.h"

#define TIME_IS_OUT(start, end, timeout)     (timeout) > ((end)-(start)) ? 0 : 1

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your Hardware spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GPIO_MOSI 11
#define GPIO_MISO 13
#define GPIO_SCLK 12
#define GPIO_CS   10

#define SLAVE_HOST SPI2_HOST
#define DMA_CHAN   SPI_DMA_CH_AUTO
#define QUEUE_SIZE 4

/**
 * Helper Macros for Master-Slave synchronization, each setting is 4-byte-width
 *
 * The address and value should be negotiated with Master beforehand
 */
//----------------------General Settings---------------------//
//Indicate Slave General Settings are ready
#define SLAVE_READY_FLAG_REG            0
#define SLAVE_READY_FLAG                0xEE
//Value in these 4 registers (Byte 4, 5, 6, 7) indicates the MAX Slave TX buffer length
#define SLAVE_MAX_TX_BUF_LEN_REG        4
//Value in these 4 registers indicates the MAX Slave RX buffer length
#define SLAVE_MAX_RX_BUF_LEN_REG        8

//----------------------Updating Info------------------------//
//Value in these 4 registers indicates size of the TX buffer that Slave has loaded to the DMA
#define SLAVE_TX_READY_BUF_SIZE_REG     12
//Value in these 4 registers indicates number of the RX buffer that Slave has loaded to the DMA
#define SLAVE_RX_READY_BUF_NUM_REG      16

static const char TAG[] = "SEG_SLAVE";

/* Used for Master-Slave synchronization */
static uint32_t s_tx_ready_buf_size;  //See ``cb_set_tx_ready_buf_size()``
static uint32_t s_rx_ready_buf_num;   //See ``cb_set_rx_ready_buf_num()``

static uint32_t s_tx_data_id;

//-------------------------------Function used for Master-Slave Synchronization---------------------------//
/**
 * NOTE: Only if all the counters are in same size (here uint32_t), the calculation is safe (when the shared register overflows).
 * NOTE: If Slave resets the counters, Master needs to reset its counter accordingly.
 * NOTE: You can also implement your own synchronization method, BUT DO MIND the parallelled issue.
 */

/**
 * To write the size of the TX buffer that is loaded to the hardware (DMA)
 *
 * This callback function will be called when each TX buffer is loaded to the DMA.
 * - For Slave, ``s_tx_ready_buf_size`` is a counter of the loaded buffer size and will be written to the pre-negotiated shared register.
 * - For Master, it also needs a counter for the size of buffers that have been received.
 * - The difference between these 2 counters will be the number that Master can read, without concern of reading meaningless data.
 *
 * There are 3 conditions:
 * 1. Master reads same length as Slave loaded size.
 * 2. If Master reads longer than the Slave loaded size, the extra bits that Master reads from the bus would be meaningless.
 * 3. If Master reads shorter than the Slave loaded size, Master should still increase its counter by the Slave loaded size (here ``event->trans->len``),
 *    because the extra bits that Slave sends to the bus would be missed by the Master.
 */
static bool cb_set_tx_ready_buf_size(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    s_tx_ready_buf_size += event->trans->len;
    spi_slave_hd_write_buffer(SLAVE_HOST, SLAVE_TX_READY_BUF_SIZE_REG, (uint8_t *)&s_tx_ready_buf_size, 4);
    return true;
}

/**
 * To write the number of the RX buffers that are loaded to the hardware (DMA)
 *
 * This callback function will be called when each RX buffer is loaded to the DMA.
 * - For Slave, ``s_rx_ready_buf_num`` is a counter for the loaded buffer and will be written to the pre-negotiated shared register
 * - For Master, it also needs a counter for the number of buffers that have been sent out.
 * - The difference between these 2 counters will be the number that Master can send.
 */
static bool cb_set_rx_ready_buf_num(void *arg, spi_slave_hd_event_t *event, BaseType_t *awoken)
{
    s_rx_ready_buf_num ++;
    spi_slave_hd_write_buffer(SLAVE_HOST, SLAVE_RX_READY_BUF_NUM_REG, (uint8_t *)&s_rx_ready_buf_num, 4);
    return true;
}

static void get_spi_bus_default_config(spi_bus_config_t *bus_cfg)
{
    memset(bus_cfg, 0x0, sizeof(spi_bus_config_t));
    bus_cfg->mosi_io_num = GPIO_MOSI;
    bus_cfg->miso_io_num = GPIO_MISO;
    bus_cfg->sclk_io_num = GPIO_SCLK;
    bus_cfg->quadwp_io_num = -1;
    bus_cfg->quadhd_io_num = -1;
    bus_cfg->max_transfer_sz = 14000;
    bus_cfg->flags = 0;
    bus_cfg->intr_flags = 0;
}

static void get_spi_slot_default_config(spi_slave_hd_slot_config_t *slave_hd_cfg)
{
    memset(slave_hd_cfg, 0x0, sizeof(spi_slave_hd_slot_config_t));
    slave_hd_cfg->spics_io_num = GPIO_CS;
    slave_hd_cfg->flags = 0;
    slave_hd_cfg->mode = 0;
    slave_hd_cfg->command_bits = 8;
    slave_hd_cfg->address_bits = 8;
    slave_hd_cfg->dummy_bits = 8;
    slave_hd_cfg->queue_size = QUEUE_SIZE;
    slave_hd_cfg->dma_chan = DMA_CHAN;
    slave_hd_cfg->cb_config = (spi_slave_hd_callback_config_t) {
        .cb_send_dma_ready = cb_set_tx_ready_buf_size,
        .cb_recv_dma_ready = cb_set_rx_ready_buf_num
    };
}

static void init_slave_hd(void)
{
    spi_bus_config_t bus_cfg = {};
    get_spi_bus_default_config(&bus_cfg);

    spi_slave_hd_slot_config_t slave_hd_cfg = {};
    get_spi_slot_default_config(&slave_hd_cfg);

    ESP_ERROR_CHECK(spi_slave_hd_init(SLAVE_HOST, &bus_cfg, &slave_hd_cfg));
}

//Example code for data to send. Use your own code for TX data here.
static bool get_tx_data(uint8_t *data, uint32_t max_len, uint32_t *out_len)
{
    uint32_t min_len = 0;
    *out_len = (rand() % (max_len - min_len + 1)) + min_len;
    if (*out_len < (max_len - min_len) / 2) {
        *out_len = 0;
        return false;
    }

    snprintf((char *)data, *out_len, "Transaction No.%"PRIu32" from slave, length: %"PRIu32, s_tx_data_id, *out_len);
    s_tx_data_id++;
    return true;
}

void sender(void *arg)
{
    esp_err_t err = ESP_OK;
    uint32_t send_buf_size = *(uint32_t *)arg;      //Tx buffer max size
    uint8_t *send_buf[QUEUE_SIZE];                  //TX buffer
    spi_slave_hd_data_t slave_trans[QUEUE_SIZE];    //Transaction descriptor
    spi_slave_hd_data_t *ret_trans;                 //Pointer to the descriptor of return result

    for (int i = 0; i < QUEUE_SIZE; i++) {
        // slave_trans
        send_buf[i] = heap_caps_calloc(1, send_buf_size, MALLOC_CAP_DMA);
        if (!send_buf[i]) {
            ESP_LOGE(TAG, "No enough memory!");
            abort();
        }
    }

    /**
     * These 2 counters are used to maintain the driver internal queue.
     * internal queue item number = queue_sent_cnt - queue_recv_cnt
     */
    uint32_t queue_sent_cnt = 0;
    uint32_t queue_recv_cnt = 0;

    uint32_t descriptor_id = 0;
    uint32_t ready_data_size = 0;
    bool data_ready = false;

    while (1) {
        /**
         * Start TX transaction
         * If the internal queue number is equal to the QUEUE_SIZE you set when initialisation,
         * - you should not call ``spi_slave_hd_queue_trans``.
         * - you should call ``spi_slave_hd_get_trans_res`` instead.
         */
        if (queue_sent_cnt - queue_recv_cnt < QUEUE_SIZE) {
            /**
             * Here we simply get some random data.
             * In your own code, you could prepare some buffers, and create a FreeRTOS task to generate/get data, and give a
             * Semaphore to unblock this `sender()` Task.
             */
            data_ready = get_tx_data(send_buf[descriptor_id], send_buf_size, &ready_data_size);
            if (data_ready) {
                slave_trans[descriptor_id].data = send_buf[descriptor_id];
                slave_trans[descriptor_id].len = ready_data_size;
                //Due to the `queue_sent_cnt` and `queue_recv_cnt` logic above, we are sure there is space to send data, this will return ESP_OK immediately
                ESP_ERROR_CHECK(spi_slave_hd_queue_trans(SLAVE_HOST, SPI_SLAVE_CHAN_TX, &slave_trans[descriptor_id], portMAX_DELAY));
                descriptor_id = (descriptor_id + 1) % QUEUE_SIZE;   //descriptor_id will be: 0, 1, 2, ..., QUEUE_SIZE, 0, 1, ....
                queue_sent_cnt++;
            }
        }
        //Normally the task would be blocked when there is no data to send, here we use a delay to yield to other tasks
        vTaskDelay(1);

        //Recycle the transaction
        while (1) {
            /**
             * Get the TX transaction result
             *
             * The ``ret_trans`` will exactly point to the transaction descriptor passed to the driver before (here ``slave_trans``).
             * For TX, the ``ret_trans->trans_len`` is meaningless. But you do need this API to maintain the internal queue.
             */
            err = spi_slave_hd_get_trans_res(SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, 0);
            if (err != ESP_OK) {
                assert(err == ESP_ERR_TIMEOUT);
                //Recycle all the used descriptors until there is no in-the-flight descriptor
                break;
            }
            queue_recv_cnt++;
        }
    }
}

void receiver(void *arg)
{
    spi_slave_hd_data_t *ret_trans;
    uint32_t recv_buf_size = *(uint32_t *)arg;
    uint8_t *recv_buf[QUEUE_SIZE];
    spi_slave_hd_data_t slave_trans[QUEUE_SIZE];
    for (int i = 0; i < QUEUE_SIZE; i++) {
        recv_buf[i] = heap_caps_calloc(1, recv_buf_size, MALLOC_CAP_DMA);
        if (!recv_buf[i]) {
            ESP_LOGE(TAG, "No enough memory!");
            abort();
        }
    }

    /**
     * - For SPI Slave, you can use this way (making use of the internal queue) to pre-load transactions to driver. Thus if
     *   Master's speed is slower than Slave, Slave won't need to wait until Master finishes.
     */
    uint32_t descriptor_id = 0;
    for (int i = 0; i < QUEUE_SIZE; i++) {
        slave_trans[descriptor_id].data = recv_buf[descriptor_id];
        slave_trans[descriptor_id].len = recv_buf_size;
        ESP_ERROR_CHECK(spi_slave_hd_queue_trans(SLAVE_HOST, SPI_SLAVE_CHAN_RX, &slave_trans[descriptor_id], portMAX_DELAY));
        descriptor_id = (descriptor_id + 1) % QUEUE_SIZE;   //descriptor_id will be: 0, 1, 2, ..., QUEUE_SIZE, 0, 1, ....
    }

    while (1) {
        /**
         * Get the RX transaction result
         *
         * The actual used (by master) buffer size could be derived by ``ret_trans->trans_len``:
         * For example, when Master sends 4bytes, whereas slave prepares 512 bytes buffer. When master finish sending its
         * 4 bytes, it will send CMD7, which will force stopping the transaction. Slave will get the actual received length
         * from `ret_trans->trans_len`` member (here 4 bytes). The ``ret_trans`` will exactly point to the transaction descriptor
         * passed to the driver before (here ``slave_trans``). If the master sends longer than slave recv buffer,
         * slave will lose the extra bits.
         */
        ESP_ERROR_CHECK(spi_slave_hd_get_trans_res(SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
        //Process the received data in your own code. Here we just print it out.
        printf("%d bytes are received: \n%s\n", ret_trans->trans_len, ret_trans->data);
        memset(ret_trans->data, 0x0, recv_buf_size);

        /**
         * Prepared data for new transaction
         */
        slave_trans[descriptor_id].data = recv_buf[descriptor_id];
        slave_trans[descriptor_id].len = recv_buf_size;
        //Start new transaction
        ESP_ERROR_CHECK(spi_slave_hd_queue_trans(SLAVE_HOST, SPI_SLAVE_CHAN_RX, &slave_trans[descriptor_id], portMAX_DELAY));
        descriptor_id = (descriptor_id + 1) % QUEUE_SIZE;   //descriptor_id will be: 0, 1, 2, ..., QUEUE_SIZE, 0, 1, ....
    }
}

void app_main(void)
{
    init_slave_hd();

    //Reset the shared register to 0
    uint8_t init_value[SOC_SPI_MAXIMUM_BUFFER_SIZE] = {0x0};
    spi_slave_hd_write_buffer(SLAVE_HOST, 0, init_value, SOC_SPI_MAXIMUM_BUFFER_SIZE);

    static uint32_t send_buf_size = 5000;
    spi_slave_hd_write_buffer(SLAVE_HOST, SLAVE_MAX_TX_BUF_LEN_REG, (uint8_t *)&send_buf_size, sizeof(send_buf_size));

    static uint32_t recv_buf_size = 120;
    spi_slave_hd_write_buffer(SLAVE_HOST, SLAVE_MAX_RX_BUF_LEN_REG, (uint8_t *)&recv_buf_size, sizeof(recv_buf_size));

    uint32_t slave_ready_flag = SLAVE_READY_FLAG;
    spi_slave_hd_write_buffer(SLAVE_HOST, SLAVE_READY_FLAG_REG, (uint8_t *)&slave_ready_flag, sizeof(slave_ready_flag));

    xTaskCreate(sender, "sendTask", 4096, &send_buf_size, 1, NULL);
    xTaskCreate(receiver, "recvTask", 4096, &recv_buf_size, 1, NULL);
}
