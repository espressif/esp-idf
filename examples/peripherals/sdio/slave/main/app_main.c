/* SDIO example, slave (uses sdio slave driver)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   */
#include "driver/sdio_slave.h"
#include "esp_log.h"
#include "sys/queue.h"
#include "soc/soc.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "sdkconfig.h"

/*
   sdio slave example.

   This example is supposed to work together with the sdio host example. It uses the pins as follows:

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

    The host works as following process:

        1. reset the slave.
        2. tell the slave to write registers and read them back.
        3. tell the slave to send interrupts to the host.
        4. send data to slave FIFO and read them back.
        5. loop step 4.
   */

#define SDIO_SLAVE_QUEUE_SIZE 11

#define BUFFER_SIZE     128
#define BUFFER_NUM      16

#define EV_STR(s) "================ "s" ================"

typedef enum {
    JOB_IDLE = 0,
    JOB_RESET = 1,
    JOB_SEND_INT = 2,
    JOB_WRITE_REG = 4,
} example_job_t;

static const char TAG[] = "example_slave";
static int s_job = JOB_IDLE;

DMA_ATTR uint8_t data_to_send[BUFFER_SIZE] = {0x97, 0x84, 0x43, 0x67, 0xc1, 0xdd, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x56, 0x55, 0x44, 0x33 ,0x22, 0x11, 0x00 };
DMA_ATTR uint8_t data_to_recv[BUFFER_SIZE] = {0x97, 0x84, 0x43, 0x67, 0xc1, 0xdd, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x56, 0x55, 0x44, 0x33 ,0x22, 0x11, 0x00 };

static const char job_desc[][32] = {
    "JOB_IDLE",
    "JOB_RESET",
    "JOB_SEND_INT",
    "JOB_WRITE_REG",
};


//reset counters of the slave hardware, and clean the receive buffer (normally they should be sent back to the host)
static esp_err_t slave_reset(void)
{
    esp_err_t ret;
    sdio_slave_stop();
    ret = sdio_slave_reset();
    if (ret != ESP_OK) return ret;
    ret = sdio_slave_start();
    if (ret != ESP_OK) return ret;

    //Since the buffer will not be sent any more, we return them back to receving driver
    while(1) {
        sdio_slave_buf_handle_t handle;
        ret = sdio_slave_send_get_finished(&handle, 0);
        if (ret != ESP_OK) break;
        ret = sdio_slave_recv_load_buf(handle);
        ESP_ERROR_CHECK(ret);
    }
    return ESP_OK;
}

//sent interrupts to the host in turns
static esp_err_t task_hostint(void)
{
    for(int i = 0; i < 8; i++) {
        ESP_LOGV(TAG, "send intr: %d", i);
        sdio_slave_send_host_int(i);
        //check reset for quick response to RESET signal
        if (s_job & JOB_RESET) break;
        vTaskDelay(500/portTICK_RATE_MS);
    }
    return ESP_OK;
}

//read the value in a specified register set by the host, and set other register according to this.
//the host will read these registers later
static esp_err_t task_write_reg(void)
{
    //the host write REG1, the slave should write its registers according to value of REG1
    uint8_t read = sdio_slave_read_reg(1);
    for (int i = 0; i < 64; i++) {
        //skip interrupt regs.
        if (i >= 28 && i <= 31) continue;
        sdio_slave_write_reg(i, read+3*i);
    }
    uint8_t reg[64] = {0};
    for (int i = 0; i < 64; i++) {
        //skip interrupt regs.
        if (i >= 28 && i <= 31) continue;
        reg[i] = sdio_slave_read_reg(i);
    }
    ESP_LOGI(TAG, "write regs:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, reg, 64, ESP_LOG_INFO);
    return ESP_OK;
}

//we use the event callback (in ISR) in this example to get higer responding speed
//note you can't do delay in the ISR
//``sdio_slave_wait_int`` is another way to handle interrupts
static void event_cb(uint8_t pos)
{
    ESP_EARLY_LOGD(TAG, "event: %d", pos);
    switch(pos) {
        case 0:
            s_job = sdio_slave_read_reg(0);
            sdio_slave_write_reg(0, JOB_IDLE);
            break;
    }
}

DMA_ATTR uint8_t buffer[BUFFER_NUM][BUFFER_SIZE] = {};

//Main application
void app_main(void)
{
    esp_err_t ret;

    sdio_slave_config_t config = {
        .sending_mode       = SDIO_SLAVE_SEND_PACKET,
        .send_queue_size    = SDIO_SLAVE_QUEUE_SIZE,
        .recv_buffer_size   = BUFFER_SIZE,
        .event_cb           = event_cb,
        /* Note: For small devkits there may be no pullups on the board.
           This enables the internal pullups to help evaluate the driver
           quickly. However the internal pullups are not sufficient and not
           reliable, please make sure external pullups are connected to the
           bus in your real design.
        */
        //.flags              = SDIO_SLAVE_FLAG_INTERNAL_PULLUP,
    };
#ifdef CONFIG_SDIO_DAT2_DISABLED
    /* For slave chips with 3.3V flash, DAT2 pullup conflicts with the pulldown
       required by strapping pin (MTDI). We can either burn the EFUSE for the
       strapping or just disable the DAT2 and work in 1-bit mode.
     */
    config.flags |= SDIO_SLAVE_FLAG_DAT2_DISABLED;
#endif

    ret = sdio_slave_initialize(&config);
    ESP_ERROR_CHECK(ret);

    sdio_slave_write_reg(0, JOB_IDLE);

    sdio_slave_buf_handle_t handle;
    for(int i = 0; i < BUFFER_NUM; i++) {
        handle = sdio_slave_recv_register_buf(buffer[i]);
        assert(handle != NULL);

        ret = sdio_slave_recv_load_buf(handle);
        ESP_ERROR_CHECK(ret);
    }

    sdio_slave_set_host_intena(SDIO_SLAVE_HOSTINT_SEND_NEW_PACKET |
            SDIO_SLAVE_HOSTINT_BIT0 |
            SDIO_SLAVE_HOSTINT_BIT1 |
            SDIO_SLAVE_HOSTINT_BIT2 |
            SDIO_SLAVE_HOSTINT_BIT3 |
            SDIO_SLAVE_HOSTINT_BIT4 |
            SDIO_SLAVE_HOSTINT_BIT5 |
            SDIO_SLAVE_HOSTINT_BIT6 |
            SDIO_SLAVE_HOSTINT_BIT7
      );

    sdio_slave_start();

    ESP_LOGI(TAG, EV_STR("slave ready"));

    for(;;) {
        //receive data and send back to host.
        size_t length;
        uint8_t *ptr;

        const TickType_t non_blocking = 0;
        ret = sdio_slave_recv(&handle, &ptr, &length, non_blocking);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "handle: %p, recv len: %d, data:", handle, length);
            ESP_LOG_BUFFER_HEXDUMP(TAG, ptr, length, ESP_LOG_INFO);
            /* If buffer is no longer used, call sdio_slave_recv_load_buf to return it here.  Since we wants to show how
             * to share large buffers between drivers here (we share between sending and receiving), keep the buffer
             * until the buffer is sent by sending driver.
             */

            //send the received buffer to host, with the handle as the argument
            ret = sdio_slave_send_queue(ptr, length, handle, non_blocking);
            if (ret == ESP_ERR_TIMEOUT) {
                // send failed, direct return the buffer to rx
                ESP_LOGE(TAG, "send_queue full, discard received.");
                ret = sdio_slave_recv_load_buf(handle);
            }
            ESP_ERROR_CHECK(ret);
        }

        // if there's finished sending desc, return the buffer to receiving driver
        for(;;){
            sdio_slave_buf_handle_t handle;
            ret = sdio_slave_send_get_finished(&handle, 0);
            if (ret == ESP_ERR_TIMEOUT) break;
            ESP_ERROR_CHECK(ret);
            ret = sdio_slave_recv_load_buf(handle);
            ESP_ERROR_CHECK(ret);
        }

        if (s_job != 0) {
            for(int i = 0; i < 8; i++) {
                if (s_job & BIT(i)) {
                    ESP_LOGI(TAG, EV_STR("%s"), job_desc[i+1]);
                    s_job &= ~BIT(i);

                    switch(BIT(i)) {
                    case JOB_SEND_INT:
                        ret = task_hostint();
                        ESP_ERROR_CHECK(ret);
                        break;
                    case JOB_RESET:
                        ret = slave_reset();
                        ESP_ERROR_CHECK(ret);
                        break;
                    case JOB_WRITE_REG:
                        ret = task_write_reg();
                        ESP_ERROR_CHECK(ret);
                        break;
                    }
                }
            }
        }
        vTaskDelay(1);
    }
}


