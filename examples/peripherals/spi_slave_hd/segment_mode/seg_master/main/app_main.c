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
#include "driver/spi_master.h"
#include "esp_serial_slave_link/essl_spi.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your Hardware spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GPIO_MOSI    11
#define GPIO_MISO    13
#define GPIO_SCLK    12
#define GPIO_CS      10

#define MASTER_HOST  SPI2_HOST
#define DMA_CHAN     SPI_DMA_CH_AUTO

#define TX_SIZE_MIN  40

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

static const char TAG[] = "SEG_MASTER";

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

static void get_spi_device_default_config(spi_device_interface_config_t *dev_cfg)
{
    memset(dev_cfg, 0x0, sizeof(spi_device_interface_config_t));
    dev_cfg->clock_speed_hz = 10 * 1000 * 1000;
    dev_cfg->mode = 0;
    dev_cfg->spics_io_num = GPIO_CS;
    dev_cfg->cs_ena_pretrans = 0;
    dev_cfg->cs_ena_posttrans = 0;
    dev_cfg->command_bits = 8;
    dev_cfg->address_bits = 8;
    dev_cfg->dummy_bits = 8;
    dev_cfg->queue_size = 16;
    dev_cfg->flags = SPI_DEVICE_HALFDUPLEX;
    dev_cfg->duty_cycle_pos = 0;
    dev_cfg->input_delay_ns = 0;
    dev_cfg->pre_cb = NULL;
    dev_cfg->post_cb = NULL;
}

static void init_master_hd(spi_device_handle_t* out_spi)
{
    //init bus
    spi_bus_config_t bus_cfg = {};
    get_spi_bus_default_config(&bus_cfg);
    ESP_ERROR_CHECK(spi_bus_initialize(MASTER_HOST, &bus_cfg, DMA_CHAN));

    //add device
    spi_device_interface_config_t dev_cfg = {};
    get_spi_device_default_config(&dev_cfg);
    ESP_ERROR_CHECK(spi_bus_add_device(MASTER_HOST, &dev_cfg, out_spi));
}

//-------------------------------Function used for Master-Slave Synchronization---------------------------//
//Wait for Slave to init the shared registers for its configurations, see the Helper Macros above
static esp_err_t wait_for_slave_ready(spi_device_handle_t spi)
{
    esp_err_t ret;
    uint32_t slave_ready_flag;

    while (1) {
        //Master sends CMD2 to get slave configuration
        //The first byte is a flag assigned by slave as a start signal, here it's 0xee
        ret = essl_spi_rdbuf(spi, (uint8_t *)&slave_ready_flag, SLAVE_READY_FLAG_REG, 4, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        if (slave_ready_flag != SLAVE_READY_FLAG) {
            printf("Waiting for Slave to be ready...\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else if (slave_ready_flag == SLAVE_READY_FLAG) {
            return ESP_OK;
        }
    }
}

//Get the MAX length of Slave's TX/RX buffer
static esp_err_t get_slave_max_buf_size(spi_device_handle_t spi, uint32_t *out_send_size, uint32_t *out_recv_size)
{
    esp_err_t ret;

    ret = essl_spi_rdbuf(spi, (uint8_t *)out_send_size, SLAVE_MAX_TX_BUF_LEN_REG, 4, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = essl_spi_rdbuf(spi, (uint8_t *)out_recv_size, SLAVE_MAX_RX_BUF_LEN_REG, 4, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    return ret;
}

/**
 * To get the size of the ready Slave TX buffer
 * This size can be read from the pre-negotiated shared register (here `SLAVE_TX_READY_BUF_SIZE_REG`)
 */
static uint32_t get_slave_tx_buf_size(spi_device_handle_t spi)
{
    uint32_t updated_size;
    uint32_t temp;

    ESP_ERROR_CHECK(essl_spi_rdbuf_polling(spi, (uint8_t *)&temp, SLAVE_TX_READY_BUF_SIZE_REG, 4, 0));
    /**
     * Read until the last 2 reading result are same. Reason:
     * SPI transaction is carried on per 1 Byte. So when Master is reading the shared register, if the
     * value is changed by Slave at this time, Master may get wrong data.
     */
    while (1) {
        ESP_ERROR_CHECK(essl_spi_rdbuf_polling(spi, (uint8_t *)&updated_size, SLAVE_TX_READY_BUF_SIZE_REG, 4, 0));
        if (updated_size == temp) {
            return updated_size;
        }
        temp = updated_size;
    }
}

/**
 * To get the number of the ready Slave RX buffers
 * This number can be read from the pre-negotiated shared register (here `SLAVE_RX_READY_BUF_NUM_REG`)
 */
static uint32_t get_slave_rx_buf_num(spi_device_handle_t spi)
{
    uint32_t updated_num;
    uint32_t temp;

    ESP_ERROR_CHECK(essl_spi_rdbuf_polling(spi, (uint8_t *)&temp, SLAVE_RX_READY_BUF_NUM_REG, 4, 0));
    /**
     * Read until the last 2 reading result are same. Reason:
     * SPI transaction is carried on per 1 Byte. So when Master is reading the shared register, if the
     * value is changed by Slave at this time, Master may get wrong data.
     */
    while (1) {
        ESP_ERROR_CHECK(essl_spi_rdbuf_polling(spi, (uint8_t *)&updated_num, SLAVE_RX_READY_BUF_NUM_REG, 4, 0));
        if (updated_num == temp) {
            return updated_num;
        }
        temp = updated_num;
    }
}

void app_main(void)
{
    spi_device_handle_t spi;
    init_master_hd(&spi);

    ESP_ERROR_CHECK(wait_for_slave_ready(spi));

    /**
     * Here we let the Slave to claim its transaction size. You can modify it in your own way,
     * e.g. let the Senders to claim its MAX length, or let the Master/Slave determine the length themselves, without considering
     * throughputs in the opposite side.
     */
    uint32_t slave_max_tx_buf_size;
    uint32_t slave_max_rx_buf_size;
    ESP_ERROR_CHECK(get_slave_max_buf_size(spi, &slave_max_tx_buf_size, &slave_max_rx_buf_size));
    uint32_t rx_buf_size = slave_max_tx_buf_size;
    printf("\n\n---------SLAVE INFO---------\n\n");
    printf("Slave MAX Send Buffer Size:       %"PRIu32"\n", slave_max_tx_buf_size);
    printf("Slave MAX Receive Buffer Size:    %"PRIu32"\n", slave_max_rx_buf_size);

    uint8_t *recv_buf = heap_caps_calloc(1, rx_buf_size, MALLOC_CAP_DMA);
    if (!recv_buf) {
        ESP_LOGE(TAG, "No enough memory!");
        abort();
    }
    uint8_t *send_buf = heap_caps_calloc(1, slave_max_rx_buf_size, MALLOC_CAP_DMA);
    if (!send_buf) {
        ESP_LOGE(TAG, "No enough memory!");
        abort();
    }

    uint32_t size_has_read = 0;    //Counter of the size that Master has received.
    uint32_t size_to_read = 0;

    uint32_t num_has_sent = 0;     //Counter of the buffer number that Master has sent.
    uint32_t tx_trans_id = 0;
    srand(30);
    while (1) {
        //RECV
        ESP_LOGI(TAG, "RECEIVING......");
        /**
         * This (`size_has_read`) is the counter mentioned in examples/peripherals/spi_slave_hd/segment_mode/seg_slave/app_main.c.
         * See its Note for Function used for Master-Slave Synchronization.
         *
         * Condition when this counter overflows:
         * If the Slave increases its counter with the value smaller than 2^32, then the calculation is still safe. For example:
         * 1. Initially, Slave's counter is (2^32 - 1 - 10), Master's counter is (2^32 - 1 - 20). So the difference would be 10B initially.
         * 2. Slave loads 20 bytes to the DMA, and increase its counter. So the value would be ((2^32 - 1 - 10) + 20) = 9;
         * 3. The difference (`size_can_be_read`) would be (9 - (2^32 - 1 - 20)) = 30;
         *
         * If this is 0, it means Slave didn't load new TX buffer to the bus yet.
         */
        uint32_t size_can_be_read = get_slave_tx_buf_size(spi) - size_has_read;

        if (size_can_be_read > rx_buf_size) {
            ESP_LOGW(TAG, "Slave is going to send buffer(%"PRIu32" Bytes) larger than pre-negotiated MAX size", size_can_be_read);
            /**
             * NOTE:
             * In this condition, Master should still increase its counter (``size_has_read``) by the size that Slave has loaded,
             * because Master RX buffer is not large enough, and it should read as large as it can, then send the CMD8. The extra
             * bits will be missed by Master.
             */
            size_to_read = rx_buf_size;
        } else {
            size_to_read = size_can_be_read;
        }

        if (size_to_read) {
            ESP_ERROR_CHECK(essl_spi_rddma(spi, recv_buf, size_to_read, -1, 0));
            size_has_read += size_can_be_read;  //See NOTE above

            //Process the data. Here we just print it out.
            printf("%s\n", recv_buf);
            memset(recv_buf, 0x0, rx_buf_size);
        }

        //SEND
        ESP_LOGI(TAG, "SENDING......");
        /**
         * Similar logic, see the comment for `size_can_be_read` above.
         * If this is 0, it means Slave didn't load RX buffer to the bus yet.
         */
        uint32_t num_to_send = get_slave_rx_buf_num(spi) - num_has_sent;

        //Prepare your TX transaction in your own way. Here is an example.
        //You can set any size to send (shorter, longer or equal to the Slave Max RX buf size), Slave can get the actual length by ``trans_len`` member of ``spi_slave_hd_data_t``
        uint32_t actual_tx_size = (rand() % (slave_max_rx_buf_size - TX_SIZE_MIN + 1)) + TX_SIZE_MIN;
        snprintf((char *)send_buf, slave_max_rx_buf_size, "this is master's transaction %"PRIu32, tx_trans_id);

        for (int i = 0; i < num_to_send; i++) {
            ESP_ERROR_CHECK(essl_spi_wrdma(spi, send_buf, actual_tx_size, -1, 0));
            num_has_sent++;
            tx_trans_id++;
        }
    }

    free(recv_buf);
    free(send_buf);

    spi_bus_remove_device(spi);
    spi_bus_free(MASTER_HOST);
}
