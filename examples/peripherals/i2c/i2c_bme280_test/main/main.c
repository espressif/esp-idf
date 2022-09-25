#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bme280.h"

void app_main() 
{
  bme280_data_t bme280_data = {};
  bme280_init();
  while(1)
  {
    //Get sensor data and send to serial monitor periodically
    bme280_get_data(&bme280_data);
    ESP_LOGI("TEMP_TAG","temperature = %d\n",bme280_data.temperature);
    ESP_LOGI("HUM_TAG","humidity = %d\n",bme280_data.humidity);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}