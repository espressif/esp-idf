/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Extra check to ensure our custom chip driver file is correctly linked
#if !CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST
#error Custom flash chip driver not used!
#endif

void app_main(void)
{
    printf("Hello world!\n");

    while(1) {
        vTaskDelay(1);
    }
}
