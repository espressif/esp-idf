#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


TEST_CASE("restart from PRO CPU", "[restart][reset=SW_CPU_RESET]")
{
    esp_restart();
}

static void restart_task(void *arg)
{
    esp_restart();
}

#ifndef CONFIG_FREERTOS_UNICORE
TEST_CASE("restart from APP CPU", "[restart][reset=SW_CPU_RESET]")
{
    xTaskCreatePinnedToCore(&restart_task, "restart", 2048, NULL, 5, NULL, 1);
    while (true) {
        ;
    }
}
#endif
