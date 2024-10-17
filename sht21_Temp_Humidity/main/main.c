#include <stdio.h>
#include "sht21.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "I2C_SCANNER";

#define I2C_MASTER_SDA_IO 17      // Define your SDA pin
#define I2C_MASTER_SCL_IO 18      // Define your SCL pin
#define I2C_MASTER_NUM I2C_NUM_0  // I2C port number
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

#define threshold_temp 33.00
#define threshold_humidity 70.00

float temperature = 0;
float humidity = 0;
volatile int interruptCounter = 0; // For counting interrupts
void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);                // Config hardware to above structure
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0); // I2C_NUM, mode, slv_rx_buf_len, slv_tx_buf_len, intr_alloc_flags
}

void i2c_scanner(void)
{
    printf("I2C Scanner\n");
    printf("Scanning...\n");

    for (uint8_t address = 1; address < 127; address++)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK)
        {
            printf("Found I2C device at address 0x%02X\n", address);
        }
    }
    printf("Scanning done.\n");
}

static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer() // timer interrrutp function
{
    portENTER_CRITICAL_ISR(&timerMux);
    interruptCounter++;
    sht21_get_temperature(&temperature);
    sht21_get_humidity(&humidity);
    portEXIT_CRITICAL_ISR(&timerMux);
}

void app_main(void)
{
    i2c_master_init();
    i2c_scanner();
    sht21_init(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO,
               I2C_MASTER_FREQ_HZ);

    const esp_timer_create_args_t timer_args = {
        .callback = &onTimer,
        .name = "timer"};

    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, 2000000);
    while (1)
    {

        // sht21_get_temperature(&temperature);

        if (interruptCounter)
        {
            printf("Temperature: %.2f C\n", temperature);
            if (temperature > threshold_temp)
            {
                printf("WARNING:\n");
                printf("Temperature is too high\n");
            }

            // sht21_get_humidity(&humidity);
            printf("Humidity: %.2f %%RH\n", humidity);
            if (humidity > threshold_humidity)
            {
                printf("WARNING:\n");
                printf("Humidity is too high\n");
            }
            // vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        portENTER_CRITICAL(&timerMux);
        interruptCounter = 0;
        portEXIT_CRITICAL(&timerMux);
    }
}