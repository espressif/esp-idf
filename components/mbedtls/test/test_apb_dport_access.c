/* Implementation of utility functions to verify
   unit tests aren't performing SMP-unsafe DPORT reads.
*/

#include "unity.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/uart_reg.h"
#include "test_apb_dport_access.h"

#ifndef CONFIG_FREERTOS_UNICORE

static void apb_access_loop_task(void *ignore);

static volatile bool apb_access_corrupt;
static TaskHandle_t apb_task_handle;

void start_apb_access_loop()
{
    apb_access_corrupt = false;
    xTaskCreatePinnedToCore(apb_access_loop_task, "accessAPB", 2048, NULL,
                            UNITY_FREERTOS_PRIORITY - 1,
                            &apb_task_handle, !UNITY_FREERTOS_CPU);
}

void verify_apb_access_loop()
{
    vTaskDelete(apb_task_handle);
    apb_task_handle = NULL;
    TEST_ASSERT_FALSE(apb_access_corrupt);
    printf("Verified no APB corruption from operations\n");
}

static void apb_access_loop_task(void *ignore)
{
    uint32_t initial = REG_READ(UART_DATE_REG(0));
    while(1) {
        if (REG_READ(UART_DATE_REG(0)) != initial) {
            apb_access_corrupt = true;
        }
    }
}

#else /*CONFIG_FREERTOS_UNICORE */

void start_apb_access_loop()
{
}

void verify_apb_access_loop()
{
}

#endif
