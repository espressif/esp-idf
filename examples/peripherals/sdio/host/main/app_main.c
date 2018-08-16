/* SDIO example, host (uses sdmmc host driver)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "soc/sdmmc_periph.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_slave.h"
#include "sdkconfig.h"

/*
 * For SDIO master-slave board, we have 3 pins controlling power of 3 different
 * slaves individially. We only enable one at a time.
 */
#define GPIO_B1     5
#define GPIO_B2     18
#define GPIO_B3     19
#if CONFIG_EXAMPLE_SLAVE_B1
#define SLAVE_PWR_GPIO GPIO_B1
#elif CONFIG_EXAMPLE_SLAVE_B2
#define SLAVE_PWR_GPIO GPIO_B2
#elif CONFIG_EXAMPLE_SLAVE_B3
#define SLAVE_PWR_GPIO GPIO_B3
#endif

/*
   sdio host example.

   This example is supposed to work together with the sdio slave example. It uses the pins as follows:

     *   Host      Slave
     *   IO14      CLK
     *   IO15      CMD
     *   IO2       D0
     *   IO4       D1
     *   IO12      D2
     *   IO13      D3

    This is the only pins that can be used in standard ESP modules. The other set of pins (6, 11, 7, 8, 9, 10)
    are occupied by the spi bus communicating with the flash.

    Protocol Above the ESP slave service:
        - Interrupts:
            0 is used to notify the slave to read the register 0.

        - Registers:
            - 0 is the register to hold tasks. Bits:
                - 0: the slave should reset.
                - 1: the slave should send interrupts.
                - 2: the slave should write the shared registers acoording to the value in register 1.
            - 1 is the register to hold test value.
            - other registers will be written by the slave for testing.

        - FIFO:
            The receving FIFO is size of 256 bytes.
            When the host writes something to slave recv FIFO, the slave should return it as is to the sending FIFO.

    The example works as following process:

        1. reset the slave.
        2. tell the slave to write registers and read them back.
        3. tell the slave to send interrupts to the host.
        4. send data to slave FIFO and read them back.
        5. loop step 4.
   */

#define WRITE_BUFFER_LEN    4096
#define READ_BUFFER_LEN     1024

static const char TAG[] = "example_host";

#define SDIO_INTERRUPT_LINE     GPIO_NUM_4   //DATA1

#define SLAVE_INTR_NOTIFY    0

#define SLAVE_REG_JOB   0
#define SLAVE_REG_VALUE 1

typedef enum {
    JOB_IDLE = 0,
    JOB_RESET = 1,
    JOB_SEND_INT = 2,
    JOB_WRITE_REG = 4,
} example_job_t;

//host use this to inform the slave it should reset its counters
esp_err_t slave_reset(esp_slave_context_t *context)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "send reset to slave...");
    ret = esp_slave_write_reg(context, 0, JOB_RESET, NULL);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = esp_slave_send_slave_intr(context, BIT(SLAVE_INTR_NOTIFY));
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(500 / portTICK_RATE_MS);
    ret = esp_slave_wait_for_ioready(context);
    ESP_LOGI(TAG, "slave io ready");
    return ret;
}

//host use this to initialize the slave card as well as SDIO registers
esp_err_t slave_init(esp_slave_context_t *context)
{
    /* Probe */
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
#ifdef CONFIG_SDIO_EXAMPLE_4BIT
    config.flags = SDMMC_HOST_FLAG_4BIT;
#else
    config.flags = SDMMC_HOST_FLAG_1BIT;
#endif

#ifdef CONFIG_SDIO_EXAMPLE_HIGHSPEED
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
#else
    config.max_freq_khz = SDMMC_FREQ_DEFAULT;
#endif

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    /* Note: For small devkits there may be no pullups on the board.
       This enables the internal pullups to help evaluate the driver quickly.
       However the internal pullups are not sufficient and not reliable,
       please make sure external pullups are connected to the bus in your
       real design.
    */
    //slot_config.flags = SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    sdmmc_host_init();
    sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    sdmmc_card_t *card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (card == NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (;;) {
        if (sdmmc_card_init(&config, card) == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "slave init failed, retry...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    sdmmc_card_print_info(stdout, card);

    gpio_pullup_en(14);
    gpio_pulldown_dis(14);
    gpio_pullup_en(15);
    gpio_pulldown_dis(15);
    gpio_pullup_en(2);
    gpio_pulldown_dis(2);
    gpio_pullup_en(4);
    gpio_pulldown_dis(4);
    gpio_pullup_en(12);
    gpio_pulldown_dis(12);
    gpio_pullup_en(13);
    gpio_pulldown_dis(13);

    *context = ESP_SLAVE_DEFAULT_CONTEXT(card);
    esp_err_t ret = esp_slave_init_io(context);

    return ret;
}

void slave_power_on()
{
#ifdef SLAVE_PWR_GPIO
    gpio_config_t cfg = {
        .pin_bit_mask = BIT(SLAVE_PWR_GPIO),
        .mode = GPIO_MODE_DEF_OUTPUT,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    gpio_set_level(SLAVE_PWR_GPIO, 0);  //low active
    vTaskDelay(100);
#endif
}

DMA_ATTR uint8_t rcv_buffer[READ_BUFFER_LEN];

//try to get an interrupt from the slave and handle it, return if none.
esp_err_t process_event(esp_slave_context_t *context)
{
    esp_err_t ret = esp_slave_wait_int(context, 0);
    if (ret == ESP_ERR_TIMEOUT) {
        return ret;
    }
    ESP_ERROR_CHECK(ret);

    uint32_t intr_raw, intr_st;
    ret = esp_slave_get_intr(context, &intr_raw, &intr_st);
    ESP_ERROR_CHECK(ret);
    ret = esp_slave_clear_intr(context, intr_raw);
    ESP_ERROR_CHECK(ret);
    ESP_LOGD(TAG, "intr: %08X", intr_raw);

    for (int i = 0; i < 8; i++) {
        if (intr_raw & BIT(i)) {
            ESP_LOGI(TAG, "host int: %d", i);
        }
    }

    const int wait_ms = 50;
    if (intr_raw & HOST_SLC0_RX_NEW_PACKET_INT_ST) {
        ESP_LOGD(TAG, "new packet coming");
        while (1) {
            size_t size_read = READ_BUFFER_LEN;
            ret = esp_slave_get_packet(context, rcv_buffer, READ_BUFFER_LEN, &size_read, wait_ms);
            if (ret == ESP_ERR_NOT_FOUND) {
                ESP_LOGE(TAG, "interrupt but no data can be read");
                break;
            } else if (ret != ESP_OK && ret != ESP_ERR_NOT_FINISHED) {
                ESP_LOGE(TAG, "rx packet error: %08X", ret);
                return ret;
            }

            ESP_LOGI(TAG, "receive data, size: %d", size_read);
            ESP_LOG_BUFFER_HEXDUMP(TAG, rcv_buffer, size_read, ESP_LOG_INFO);
            if (ret == ESP_OK) {
                break;
            }
        }
    }
    return ESP_OK;
}

//tell the slave to do a job
static inline esp_err_t slave_inform_job(esp_slave_context_t *context, example_job_t job)
{
    esp_err_t ret;
    ret = esp_slave_write_reg(context, SLAVE_REG_JOB, job, NULL);
    ESP_ERROR_CHECK(ret);
    ret = esp_slave_send_slave_intr(context, BIT(SLAVE_INTR_NOTIFY));
    ESP_ERROR_CHECK(ret);
    return ret;
}

//tell the slave to write registers by write one of them, and read them back
void job_write_reg(esp_slave_context_t *context, int value)
{
    esp_err_t ret;
    uint8_t reg_read[64];
    ESP_LOGI(TAG, "========JOB: write slave reg========");
    ret = esp_slave_write_reg(context, SLAVE_REG_VALUE, value, NULL);
    ESP_ERROR_CHECK(ret);

    ret = slave_inform_job(context, JOB_WRITE_REG);
    ESP_ERROR_CHECK(ret);

    vTaskDelay(10);
    for (int i = 0; i < 64; i++) {
        ESP_LOGD(TAG, "reading register %d", i);
        ret = esp_slave_read_reg(context, i, &reg_read[i]);
        ESP_ERROR_CHECK(ret);
    }

    ESP_LOGI(TAG, "read registers:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, reg_read, 64, ESP_LOG_INFO);
}

//use 1+1+1+1+4+4=12 packets, 513 and 517 not sent
int packet_len[] = {3, 6, 12, 128, 511, 512, 513, 517};
//the sending buffer should be word aligned
DMA_ATTR uint8_t send_buffer[READ_BUFFER_LEN];

//send packets to the slave (they will return and be handled by the interrupt handler)
void job_fifo(esp_slave_context_t *context)
{
    for (int i = 0; i < 1024; i++) {
        send_buffer[i] = 0x46 + i * 5;
    }

    esp_err_t ret;
    int pointer = 0;

    ESP_LOGI(TAG, "========JOB: send fifos========");
    /* CAUTION: This example shows that we can send random length of packet to the slave.
     * However it takes time of two transactions if the length is not multiples of 4 bytes.
     * e.g. sending 6 bytes is done by sending 4 + 2 bytes each transaction.
     * Try to avoid unaligned packets if possible to get higher effeciency.
     */
    for (int i = 0; i < sizeof(packet_len) / sizeof(int); i++) {
        const int wait_ms = 50;
        int length = packet_len[i];
        ret = esp_slave_send_packet(context, send_buffer + pointer, length, wait_ms);
        if (ret == ESP_ERR_TIMEOUT) {
            ESP_LOGD(TAG, "several packets are expected to timeout.");
        } else {
            ESP_ERROR_CHECK(ret);
            ESP_LOGI(TAG, "send packet length: %d", length);
        }
        pointer += (length + 3) & (~3); //the length can be random, but data should start at the 32-bit boundary.
    }
}

//inform the slave to send interrupts to host (the interrupts will be handled in the interrupt handler)
void job_getint(esp_slave_context_t *context)
{
    ESP_LOGI(TAG, "========JOB: get interrupts from slave========");
    slave_inform_job(context, JOB_SEND_INT);
}

void app_main()
{
    esp_slave_context_t context;
    esp_err_t err;

    //enable the power if on espressif SDIO master-slave board
    slave_power_on();

    ESP_LOGI(TAG, "host ready, start initializing slave...");

    err = slave_init(&context);
    ESP_ERROR_CHECK(err);

    err = slave_reset(&context);
    ESP_ERROR_CHECK(err);

    uint32_t start, end;

    job_write_reg(&context, 10);

    int times = 2;

    while (1) {
        job_getint(&context);
        start = xTaskGetTickCount();
        while (1) {
            process_event(&context);
            vTaskDelay(1);
            end = xTaskGetTickCount();
            if ((end - start) * 1000 / CONFIG_FREERTOS_HZ > 5000) {
                break;
            }
        }
        if (--times == 0) {
            break;
        }
    };

    while (1) {
        job_fifo(&context);

        start = xTaskGetTickCount();
        while (1) {
            process_event(&context);
            vTaskDelay(1);
            end = xTaskGetTickCount();
            if ((end - start) * 1000 / CONFIG_FREERTOS_HZ > 2000) {
                break;
            }
        }
    }
}

