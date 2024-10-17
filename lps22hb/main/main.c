#include <stdio.h>
#include "lps22hb.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"

#define LPS22HB_ADDRESS 0x5C

#define I2C_MASTER_SDA_IO 17      // Define your SDA pin
#define I2C_MASTER_SCL_IO 18      // Define your SCL pin
#define I2C_MASTER_NUM I2C_NUM_0  // I2C port number
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

// threshold macros
#define THRESHOLD_PRESSURE 1030.00
#define THRESHOLD_TEMPERATURE 33.00

volatile uint8_t interrupt_counter = 0;

float pressure_hPa;
float temperature_C;

void i2c_master_init();
void i2c_scanner();

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);                
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0); 
}

void i2c_scanner()
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
}

esp_err_t lps22hb_init();

void IRAM_ATTR ontime()
{
    if (lps22hb_read_pressure(&pressure_hPa) == ESP_OK &&
        lps22hb_read_temperature(&temperature_C) == ESP_OK) {
        interrupt_counter = 1;
    } else {
        ESP_LOGE("Sensor", "Failed to read from LPS22HB");
    }
}

void app_main(void)
{
    i2c_master_init();
    i2c_scanner();

    ESP_ERROR_CHECK(lps22hb_init());
    ESP_ERROR_CHECK(lps22hb_who_am_i(LPS22HB_ADDRESS));

    esp_timer_handle_t timer_handle;
    esp_timer_create_args_t timer_conf = {
        .callback = ontime,
        .arg = NULL,
        .name = "ontime",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_conf, &timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000 * 1000));

    while (1)
    {
        if (interrupt_counter)
        {
            printf("Pressure: %.2f hPa\n", pressure_hPa);
            if (pressure_hPa > THRESHOLD_PRESSURE)
            {
                printf("WARNING: Pressure too high...\n");
            }

            printf("Temperature: %.2f C\n", temperature_C);
            if (temperature_C > THRESHOLD_TEMPERATURE)
            {
                printf("WARNING: Temperature too high...\n");
            }
            interrupt_counter = 0; // Reset counter after handling
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
