/* SPI Slave Halfduplex example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_serial_slave_link/essl.h"
#include "esp_serial_slave_link/essl_spi.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your Hardware spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GPIO_MOSI          11
#define GPIO_MISO          13
#define GPIO_SCLK          12
#define GPIO_CS            10

#define HOST_ID            SPI2_HOST
#define TRANSACTION_LEN    16
//The SPI transaction cycles in this example. You may change the cycle. e.g., use the ``sender`` and change it to a infinite loop
#define EXAMPLE_CYCLES     10

//---------This should be negotiated with the Slave!!!!-------------//
#define SLAVE_READY_FLAG    0x88
#define READY_FLAG_REG      0
#define SYNC_REG_FROM_HOST  (14 * 4)
#define SYNC_REG_TO_HOST    (15 * 4)

static void init_driver(spi_device_handle_t *out_spi, essl_handle_t *out_essl)
{
    spi_device_handle_t spi;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 20000
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1 * 1 * 1000,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .spics_io_num = GPIO_CS,
        .queue_size = 16,
        .command_bits = 8,
        .address_bits = 8,
        .dummy_bits = 8,
        .mode = 0
    };
    ESP_ERROR_CHECK(spi_bus_add_device(HOST_ID, &dev_cfg, &spi));
    *out_spi = spi;

    essl_spi_config_t config = {
        .spi = &spi,
        .tx_buf_size = TRANSACTION_LEN,
        .tx_sync_reg = SYNC_REG_FROM_HOST,
        .rx_sync_reg = SYNC_REG_TO_HOST
    };
    ESP_ERROR_CHECK(essl_spi_init_dev(out_essl, &config));
}

static esp_err_t receiver(essl_handle_t essl)
{
    ESP_LOGI("Master", "Receiver");
    esp_err_t ret;
    uint8_t *recv_buf = heap_caps_calloc(1, TRANSACTION_LEN, MALLOC_CAP_DMA);
    if (!recv_buf) {
        ESP_LOGE("Receiver", "No enough memory");
        return ESP_ERR_NO_MEM;
    }

    int n = EXAMPLE_CYCLES;
    while (n--) {
        size_t actual_rx_length = 0;

        ret = essl_get_packet(essl, recv_buf, TRANSACTION_LEN / 2, &actual_rx_length, portMAX_DELAY);
        if (ret == ESP_OK || ret == ESP_ERR_NOT_FINISHED) {
            ESP_LOGI("Receiver", "%d bytes are actually received:", actual_rx_length);
            ESP_LOG_BUFFER_HEX("Receiver", recv_buf, actual_rx_length);
        } else if (ret == ESP_ERR_NOT_FOUND) {
            /**
             * ``ESP_ERR_NOT_FOUND``: If Slave is not ready to send data until Timeout, you'll get this return error.
             * Here we set Timeout ``portMAX_DELAY``, so you'll never reach this branch.
             * In your own app, when you reach this branch, either retry the ``essl_get_packet``, or handle this situation in your own way.
             */
            ESP_LOGW("Receiver", "Slave has nothing to send now, wait....");
            vTaskDelay(1000);
        } else {
            ESP_LOGE("Sender", "Check arguments / driver initialization, see ``essl.h``");
            return ESP_ERR_INVALID_ARG;
        }
    }
    free(recv_buf);

    return ESP_OK;
}

static esp_err_t sender(essl_handle_t essl)
{
    ESP_LOGI("Master", "Sender");
    esp_err_t ret;
    uint8_t *send_buf = heap_caps_calloc(1, TRANSACTION_LEN, MALLOC_CAP_DMA);
    if (!send_buf) {
        ESP_LOGE("Sender", "No enough memory");
        return ESP_ERR_NO_MEM;
    }
    uint8_t data = 0;

    int n = EXAMPLE_CYCLES;
    while (n--) {
        for (int i = 0; i < TRANSACTION_LEN; i++) {
            send_buf[i] = data + i;
        }

        ret = essl_send_packet(essl, send_buf, TRANSACTION_LEN, portMAX_DELAY);
        if (ret == ESP_OK) {
            data++;
        } else if (ret == ESP_ERR_NOT_FOUND) {
            /**
             * ``ESP_ERR_NOT_FOUND``: If Slave is not ready to receive data until Timeout, you'll get this return error.
             * Here we set Timeout ``portMAX_DELAY``, so you'll never reach this branch.
             * In your own app, when you reach this branch, either retry the ``essl_send_packet``, or handle this situation in your own way.
             */
            ESP_LOGW("Sender", "Slave is not ready to receive data, wait...");
            vTaskDelay(1000);
        } else {
            ESP_LOGE("Sender", "Check arguments / driver initialization, see ``essl.h``");
            return ESP_ERR_INVALID_ARG;
        }
    }
    free(send_buf);

    return ESP_OK;
}

void app_main(void)
{
    spi_device_handle_t spi;
    essl_handle_t essl;
    init_driver(&spi, &essl);

    uint8_t slave_ready_flag = 0;
    do {
        ESP_ERROR_CHECK(essl_read_reg(essl, READY_FLAG_REG, &slave_ready_flag, 0));
        printf("Waiting for Slave to be ready...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while (slave_ready_flag != SLAVE_READY_FLAG);

    ESP_ERROR_CHECK(receiver(essl));
    ESP_ERROR_CHECK(sender(essl));
    ESP_LOGI("Append", "Example done.");

    ESP_ERROR_CHECK(essl_spi_deinit_dev(essl));
    ESP_ERROR_CHECK(spi_bus_remove_device(spi));
    ESP_ERROR_CHECK(spi_bus_free(HOST_ID));
}
