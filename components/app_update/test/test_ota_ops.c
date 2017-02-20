#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <esp_ota_ops.h>


/* These OTA tests currently don't assume an OTA partition exists
   on the device, so they're a bit limited
*/

TEST_CASE("esp_ota_begin() verifies arguments", "[ota]")
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_partition_t partition;
    static esp_ota_handle_t handle = 0;

    if (handle != 0) { /* clean up from any previous test */
        esp_ota_end(handle);
        handle = 0;
    }

    /* running partition & configured boot partition are same */
    TEST_ASSERT_NOT_NULL(running);

    /* trying to 'begin' on running partition fails */
    TEST_ASSERT_NOT_EQUAL(ESP_OK, esp_ota_begin(running, OTA_SIZE_UNKNOWN, &handle));
    TEST_ASSERT_EQUAL(0, handle);

    memcpy(&partition, running, sizeof(esp_partition_t));
    partition.address--;

    /* non existent partition fails */
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_NOT_FOUND, esp_ota_begin(&partition, OTA_SIZE_UNKNOWN, &handle));
    TEST_ASSERT_EQUAL(0, handle);
}
