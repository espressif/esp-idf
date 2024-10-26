BMX280 for ESP-IDF
==================
BMX280 is a basic I2C based driver for ESP32 devices licensed mostly under MIT.
(See caption "License" for details.)

Usage
-----
Clone this repository or add it as a submodule into your components directory.
Add the module as a requirement to your main module, or other modules.

Example Code
------------
```c
#include "esp_log.h"
#include "bmx280.h"

void app_main(void)
{
    // Entry Point
    //ESP_ERROR_CHECK(nvs_flash_init());
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_17,
        .scl_io_num = GPIO_NUM_16,
        .sda_pullup_en = false,
        .scl_pullup_en = false,

        .master = {
            .clk_speed = 100000
        }
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    bmx280_t* bmx280 = bmx280_create(I2C_NUM_0);

    if (!bmx280) { 
        ESP_LOGE("test", "Could not create bmx280 driver.");
        return;
    }

    ESP_ERROR_CHECK(bmx280_init(bmx280));

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bmx280_configure(bmx280, &bmx_cfg));

    while (1)
    {
        ESP_ERROR_CHECK(bmx280_setMode(bmx280, BMX280_MODE_FORCE));
        do {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while(bmx280_isSampling(bmx280));

        float temp = 0, pres = 0, hum = 0;
        ESP_ERROR_CHECK(bmx280_readoutFloat(bmx280, &temp, &pres, &hum));

        ESP_LOGI("test", "Read Values: temp = %f, pres = %f, hum = %f", temp, pres, hum);
    }
}
```

License
-------
This repository contains a lot of code I have written which is licensed under
MIT, however there are sections modified from the BME280 datasheet which is
unclearly licensed.

The unclearly licensed section is clearly marked with two comments. Any code
between `// HERE BE DRAGONS` and `// END OF DRAGONS` contains modified versions
of the Bosch Sensortec code.

Please take note of this in production.