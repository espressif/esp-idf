#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "I2C_SCANNER";

#define I2C_MASTER_SCL_IO 22       // Define SCL pin
#define I2C_MASTER_SDA_IO 21       // Define SDA pin
#define I2C_MASTER_NUM I2C_NUM_0   // I2C port number
#define I2C_MASTER_FREQ_HZ 100000  // I2C master clock frequency

void i2c_master_init(void) {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void i2c_scanner(void) {
    printf("I2C Scanner\n");
    printf("Scanning...\n");

    for (uint8_t address = 1; address < 127; address++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            printf("Found I2C device at address 0x%02X\n", address);
        } else if (ret == ESP_ERR_TIMEOUT) {
            printf("No device found at address 0x%02X\n", address);
        }
    }
    printf("Scanning done.\n");
}

void app_main(void) {
    i2c_master_init();
    i2c_scanner();
}
