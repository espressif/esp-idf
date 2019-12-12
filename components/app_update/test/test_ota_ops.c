#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <test_utils.h>
#include <esp_ota_ops.h>
#include "bootloader_common.h"

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

TEST_CASE("esp_ota_get_next_update_partition logic", "[ota]")
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    const esp_partition_t *factory = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                                                              ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    const esp_partition_t *ota_0 = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                                                            ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    const esp_partition_t *ota_1 = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                                                            ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    const esp_partition_t *ota_2 = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
                                                            ESP_PARTITION_SUBTYPE_APP_OTA_2, NULL);

    TEST_ASSERT_NOT_NULL(running);
    TEST_ASSERT_NOT_NULL(factory);
    TEST_ASSERT_NOT_NULL(ota_0);
    TEST_ASSERT_NOT_NULL(ota_1);
    TEST_ASSERT_NULL(ota_2); /* this partition shouldn't exist in test partition table */

    TEST_ASSERT_EQUAL_PTR(factory, running); /* this may not be true if/when we get OTA tests that do OTA updates */

    /* (The test steps verify subtypes before verifying pointer equality, because the failure messages are more readable
       this way.)
     */

    /* Factory app OTA updates OTA 0 slot */
    const esp_partition_t *p = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_EQUAL_HEX8(ESP_PARTITION_SUBTYPE_APP_OTA_0, p->subtype);
    TEST_ASSERT_EQUAL_PTR(ota_0, p);

    p = esp_ota_get_next_update_partition(factory);
    TEST_ASSERT_EQUAL_HEX8(ESP_PARTITION_SUBTYPE_APP_OTA_0, p->subtype);
    TEST_ASSERT_EQUAL_PTR(ota_0, p);


    /* OTA slot 0 updates OTA slot 1 */
    p = esp_ota_get_next_update_partition(ota_0);
    TEST_ASSERT_EQUAL_HEX8(ESP_PARTITION_SUBTYPE_APP_OTA_1, p->subtype);
    TEST_ASSERT_EQUAL_PTR(ota_1, p);
    /* OTA slot 1 updates OTA slot 0 */
    p = esp_ota_get_next_update_partition(ota_1);
    TEST_ASSERT_EQUAL_HEX8(ESP_PARTITION_SUBTYPE_APP_OTA_0, p->subtype);;
    TEST_ASSERT_EQUAL_PTR(ota_0, p);
}

TEST_CASE("esp_ota_get_partition_description ", "[ota]")
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    TEST_ASSERT_NOT_NULL(running);
    esp_app_desc_t app_desc1, app_desc2;
    TEST_ESP_OK(esp_ota_get_partition_description(running, &app_desc1));
    const esp_partition_pos_t running_pos = {
            .offset = running->address,
            .size = running->size
    };
    TEST_ESP_OK(bootloader_common_get_partition_description(&running_pos, &app_desc2));

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE((uint8_t *)&app_desc1, (uint8_t *)&app_desc2, sizeof(app_desc1), "must be the same");

    const esp_partition_t *not_app = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    TEST_ASSERT_NOT_NULL(not_app);
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_ota_get_partition_description(not_app, &app_desc1));
    const esp_partition_pos_t not_app_pos = {
            .offset = not_app->address,
            .size = not_app->size
    };
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, bootloader_common_get_partition_description(&not_app_pos, &app_desc1));
}
