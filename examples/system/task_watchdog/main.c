
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <esp_task_wdt.h>
#include <stdio.h>
#include <stdint.h>

#define WATCHDOG_TIMEOUT_S 5

void myTask(void *myData)
{

	while (1)
	{
	printf("Registering myTask with the Task Watchdog timer.\n");
	esp_task_wdt_add(NULL);

		for (int i = 0; i < 5; i++)
		{
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			printf("Tick :%d\n", i);
		}

printf("Reset and Delete watchdog registration of this task.\n");		
esp_task_wdt_reset();
		
		esp_task_wdt_delete(NULL);
	}
	vTaskDelete(NULL);
}

void app_main(void)
{

	printf("App starting\n");

	printf("Initializing the task watchdog.\n");
	esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);

	printf("Creatign a new task.\n");
	xTaskCreate(myTask, "My Task", 2048, NULL, 1, NULL );
}
