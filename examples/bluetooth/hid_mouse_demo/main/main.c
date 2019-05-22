
#include "esp_log.h"
#include "esp_hidd_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_gap_bt_api.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore;
bool connected = false;

// send the buttons, change in x, and change in y
void send_mouse(uint8_t buttons, char dx, char dy) {
    uint8_t buf[3];
    int len = 3;
    bool c;

    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    c = connected;
    xSemaphoreGive(xSemaphore);
    if(c == false) { // if we're not connected, don't send
        return;
    }

    buf[0] = buttons;
    buf[1] = dx;
    buf[2] = dy;

    esp_hid_device_send_report(ESP_HIDD_REPORT_TYPE_INTRDATA, 0xa1, len, buf);
}

// move the mouse left and right
void mouse_move_task(void* pvParameters) {
    const char* TAG = "mouse_move_task";

    ESP_LOGI(TAG, "starting");
    for(;;) {
        int dx = 1;
        for(int i=0; i<2; i++) {
            dx *= -1;
            for(int j=0; j<100; j++) {
                send_mouse(0, dx, 0);
                vTaskDelay(15 / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// callback for notifying when hidd application is registered or not registered
void application_cb(esp_bd_addr_t bd_addr, esp_hidd_application_state_t state) {
    const char* TAG = "application_cb";

    switch(state) {
        case ESP_HIDD_APP_STATE_NOT_REGISTERED:
            ESP_LOGI(TAG, "app not registered");
            break;
        case ESP_HIDD_APP_STATE_REGISTERED:
            ESP_LOGI(TAG, "app is now registered!");
            if(bd_addr == NULL) {
                ESP_LOGI(TAG, "bd_addr is null...");
                break;
            }
            break;
        default:
            ESP_LOGW(TAG, "unknown app state %i", state);
            break;
    }
}

// callback for hidd connection changes
void connection_cb(esp_bd_addr_t bd_addr, esp_hidd_connection_state_t state) {
    const char* TAG = "connection_cb";
    
    switch(state) {
        case ESP_HIDD_CONN_STATE_CONNECTED:
            ESP_LOGI(TAG, "connected to %02x:%02x:%02x:%02x:%02x:%02x",
                bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
            ESP_LOGI(TAG, "closing bluetooth to future connections");
            esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            connected = true;
            xSemaphoreGive(xSemaphore);
            break;
        case ESP_HIDD_CONN_STATE_CONNECTING:
            ESP_LOGI(TAG, "connecting");
            break;
        case ESP_HIDD_CONN_STATE_DISCONNECTED:
            ESP_LOGI(TAG, "disconnected from %02x:%02x:%02x:%02x:%02x:%02x",
                bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
            ESP_LOGI(TAG, "making self discoverable");
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            connected = false;
            xSemaphoreGive(xSemaphore);
            break;
        case ESP_HIDD_CONN_STATE_DISCONNECTING:
            ESP_LOGI(TAG, "disconnecting");
            break;
        default:
            ESP_LOGI(TAG, "unknown connection status");
            break;
    }
}

// callback for when hid host requests a report
void get_report_cb(uint8_t type, uint8_t id, uint16_t buffer_size) {
    const char* TAG = "get_report_cb";
    ESP_LOGI(TAG, "got a get_report request from host");
}

// callback for when hid host sends a report
void set_report_cb(uint8_t type, uint8_t id, uint16_t len, uint8_t* p_data) {
    const char* TAG = "set_report_cb";
    ESP_LOGI(TAG, "got a report from host");
}

// callback for when hid host requests a protocol change
void set_protocol_cb(uint8_t protocol) {
    const char* TAG = "set_protocol_cb";
    ESP_LOGI(TAG, "got a set_protocol request from host");
}

// callback for when hid host sends interrupt data
void intr_data_cb(uint8_t report_id, uint16_t len, uint8_t* p_data) {
    const char* TAG = "intr_data_cb";
    ESP_LOGI(TAG, "got an interrupt report from host");
}

// callback for when hid host does a virtual cable unplug
void vc_unplug_cb(void) {
    const char* TAG = "vc_unplug_cb";
    ESP_LOGI(TAG, "host did a virtual cable unplug");
}

void print_bt_address() {
    const char* TAG = "bt_address";
    const uint8_t* bd_addr;

    bd_addr = esp_bt_dev_get_address();
    ESP_LOGI(TAG, "my bluetooth address is %02X:%02X:%02X:%02X:%02X:%02X",
        bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4], bd_addr[5]);
}

// a generic mouse descriptor
uint8_t hid_descriptor_mouse_boot_mode[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)

    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)

    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)

    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};
int hid_descriptor_mouse_boot_mode_len = sizeof(hid_descriptor_mouse_boot_mode);

void app_main() {
    const char* TAG = "app_main";
	esp_err_t ret;
    static esp_hidd_callbacks_t callbacks;
    static esp_hidd_app_param_t app_param;
    static esp_hidd_qos_param_t both_qos;

    // start the task to move the mouse back and forth
    xSemaphore = xSemaphoreCreateMutex();
    xTaskCreate(mouse_move_task, "mouse_move_task", 2*1024, NULL, 5, NULL);
    
    app_param.name = "Mouse";
    app_param.description = "Mouse Example";
    app_param.provider = "ESP32";
    app_param.subclass = 0x8;
    app_param.desc_list = hid_descriptor_mouse_boot_mode;
    app_param.desc_list_len = hid_descriptor_mouse_boot_mode_len;
    memset(&both_qos, 0, sizeof(esp_hidd_qos_param_t)); // don't set the qos parameters

    callbacks.application_state_cb = application_cb;
    callbacks.connection_state_cb = connection_cb;
    callbacks.get_report_cb = get_report_cb;
    callbacks.set_report_cb = set_report_cb;
    callbacks.set_protocol_cb = set_protocol_cb;
    callbacks.intr_data_cb = intr_data_cb;
    callbacks.vc_unplug_cb = vc_unplug_cb;

	ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(TAG, "initialize controller failed: %s\n",  esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(TAG, "enable controller failed: %s\n",  esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(TAG, "initialize bluedroid failed: %s\n",  esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(TAG, "enable bluedroid failed: %s\n",  esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "setting hid parameters");
    esp_hid_device_register_app(&app_param, &both_qos, &both_qos);

	ESP_LOGI(TAG, "starting hid device");
	esp_hid_device_init(&callbacks);

    ESP_LOGI(TAG, "setting device name");
    esp_bt_dev_set_device_name("HID Mouse Example");

    ESP_LOGI(TAG, "setting to connectable, discoverable");
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    print_bt_address();
	ESP_LOGI(TAG, "exiting");
}
