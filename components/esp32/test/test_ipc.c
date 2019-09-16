#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_ipc.h"
#include "sdkconfig.h"

#if !CONFIG_FREERTOS_UNICORE
static void test_func_ipc_cb(void *arg)
{
    vTaskDelay(50);
    int *val = (int *)arg;
    *val = 0xa5a5;
}

TEST_CASE("Test blocking IPC function call", "[ipc]")
{
    int val = 0x5a5a;
    esp_ipc_call_blocking(!xPortGetCoreID(), test_func_ipc_cb, &val);
    TEST_ASSERT_EQUAL_HEX(val, 0xa5a5);
}
#endif /* !CONFIG_FREERTOS_UNICORE */
