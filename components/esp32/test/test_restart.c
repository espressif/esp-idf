#include "unity.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


TEST_CASE("restart from PRO CPU", "[restart][ignore]")
{
    esp_restart();
}

static void restart_task(void* arg)
{
    esp_restart();
}

TEST_CASE("restart from APP CPU", "[restart][ignore]")
{
    xTaskCreatePinnedToCore(&restart_task, "restart", 2048, NULL, 5, NULL, 1);

    while(true) {
        ;
    }
}


