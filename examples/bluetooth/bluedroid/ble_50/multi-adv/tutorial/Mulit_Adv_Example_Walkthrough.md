# Multi Adv Example Walkthrough

## introduction

In this document, we review the Multi Adv example code which implements a Bluetooth Low Energy (BLE5.0) Multi adv profile on the ESP32C3. This example is designed around two Application Profiles and a series of events that are handled in order to execute a sequence of configuration steps, such as defining extended advertising parameters with all phy 1M,2M and coded and Ext adv data.

## Includes

First, let’s take a look at the include

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "sdkconfig.h"
```

These includes are required for the FreeRTOS and underlying system components to run, including the logging functionality and a library to store data in non-volatile flash memory. We are interested in `"esp_bt.h"`, `"esp_bt_main.h"`, `"esp_gap_ble_api.h"` and `"esp_gatts_api.h"`, which expose the BLE APIs required to implement this example.

 * `esp_bt.h`: implements BT controller and VHCI configuration procedures from the host side.
 * `esp_bt_main.h`: implements initialization and enabling of the Bluedroid stack.
 * `esp_gap_ble_api.h`: implements GAP configuration, such as advertising and connection parameters.
 * `esp_gatts_api.h`: implements GATT configuration, such as creating services and characteristics.

## Main Entry Point

The entry point to this example is the app_main() function:

```c
void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(LOG_TAG, "gap register error, error code = %x", ret);
        return;

   }

   vTaskDelay(200 / portTICK_PERIOD_MS);

   test_sem = xSemaphoreCreateBinary();
   // 1M phy extend adv, Connectable advertising
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(0, &ext_adv_params_1M), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(0, addr_1m), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(0, sizeof(raw_adv_data_1m), &raw_adv_data_1m[0]), test_sem);

   // 2M phy extend adv, Scannable advertising
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(1, &ext_adv_params_2M), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(1, addr_2m), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_scan_rsp_data_raw(1, sizeof(raw_scan_rsp_data_2m), raw_scan_rsp_data_2m), test_sem);

   // 1M phy legacy adv, ADV_IND
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(2, &legacy_adv_params), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(2, addr_legacy), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(2, sizeof(legacy_adv_data), &legacy_adv_data[0]), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_scan_rsp_data_raw(2, sizeof(legacy_scan_rsp_data), &legacy_scan_rsp_data[0]), test_sem);

   // coded phy extend adv, Connectable advertising
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(3, &ext_adv_params_coded), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(3, addr_coded), test_sem);
   FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_scan_rsp_data_raw(3, sizeof(raw_scan_rsp_data_coded), &raw_scan_rsp_data_coded[0]), test_sem);

   // start all adv
   FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(4, &ext_adv[0]), test_sem);

   return;

}
```
The main function starts by initializing the non-volatile storage library. This library allows tosave key-value pairs in flash memory and is used by some components such as the Wi-Fi library to save the SSID and password:

```c
ret = nvs_flash_init();
```
## BT Controller and Stack Initialization

The main function also initializes the BT controller by first creating a BT controller configuration structure named `esp_bt_controller_config_t` with default settings generated by the `BT_CONTROLLER_INIT_CONFIG_DEFAULT()` macro. The BT controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL) and the Physical Layer (PHY).The BT Controller code is exposed as a library that interacts with underlying Bluetooth stack. The controller configuration includes setting the BT controller stack size, priority and HCI baud rate. With the settings created, the BT controller is initialized and enabled with the `esp_bt_controller_init()` function:

```c
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
ret = esp_bt_controller_init(&bt_cfg);
```

Next, the controller is enabled in BLE Mode.

```c
ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
```
> The controller should be enabled in `ESP_BT_MODE_BTDM`, if you want to use the dual mode (BLE + BT).

There are four Bluetooth modes supported:

1. `ESP_BT_MODE_IDLE`: Bluetooth not running
2. `ESP_BT_MODE_BLE`: BLE mode
3. `ESP_BT_MODE_CLASSIC_BT`: BT Classic mode
4. `ESP_BT_MODE_BTDM`: Dual mode (BLE + BT Classic)

After the initialization of the BT controller, the Bluedroid stack, which includes the common definitions and APIs for both BT Classic and BLE, is initialized and enabled by using:

```c
ret = esp_bluedroid_init();
ret = esp_bluedroid_enable();
```
The Bluetooth stack is up and running at this point in the program flow, however the functionality of the application has not been defined yet. The functionality is defined by reacting to events
such as what happens when another device tries to read or write parameters and establish a connection. User need to define GAP and GATT handlers. The application needs
to register a callback function for each event handler in order to let the application know which functions are going to handle the GAP and GATT events:

```c
esp_ble_gap_register_callback(gap_event_handler);
```
The functions  `gap_event_handler()` handle all the events that are pu
shed to the application from the BLE stack.

## Setting GAP Parameters

The register application event is the first one that is triggered during the lifetime of the program, this example uses the Profile A GATT event handle to configure the advertising parameters upon registration. This example has the option to use both standard Bluetooth Core Specification advertising parameters or a customized raw buffer. The option can be selected with the `CONFIG_SET_RAW_ADV_DATA` define. The raw advertising data can be used to implement iBeacons, Eddystone or other proprietary, and custom frame types such as the ones used for Indoor Location Services that are different from the standard specifications.

The function is used to configure different types of extended advertisement types and legacy adv with 1M,2M and coded phy in esp_ble_gap_ext_adv_set_params , esp_ble_gap_ext_adv_set_rand_addr and esp_ble_gap_config_ext_adv_data_raw. Respective structure of each one of them mentioned below with one example:

```c
/**
* @brief ext adv parameters
*/
typedef struct {
    esp_ble_ext_adv_type_mask_t type;   /*!< ext adv type */
    uint32_t interval_min;              /*!< ext adv minimum interval */
    uint32_t interval_max;              /*!< ext adv maximum interval */
    esp_ble_adv_channel_t channel_map;  /*!< ext adv channel map */
    esp_ble_addr_type_t own_addr_type;  /*!< ext adv own address type */
    esp_ble_addr_type_t peer_addr_type; /*!< ext adv peer address type */
    esp_bd_addr_t peer_addr;            /*!< ext adv peer address */
    esp_ble_adv_filter_t filter_policy; /*!< ext adv filter policy */
    int8_t tx_power;                    /*!< ext adv tx power */
    esp_ble_gap_pri_phy_t primary_phy;  /*!< ext adv primary phy */
    uint8_t max_skip;                   /*!< ext adv maximum skip */
    esp_ble_gap_phy_t secondary_phy;    /*!< ext adv secondary phy */
    uint8_t sid;                        /*!< ext adv sid */
    bool scan_req_notif;                /*!< ext adv sacn request event notify */
} esp_ble_gap_ext_adv_params_t;

```
1M phy example->
Ext adv param:

```c
esp_ble_gap_ext_adv_params_t ext_adv_params_1M = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = 0x30,
    .interval_max = 0x30,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_1M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
};

```
Random addr for 1M phy:

```c
uint8_t addr_1m[6] = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x01};

```
Ext adv data for 1M phy:

```c
static uint8_t raw_adv_data_1m[] = {
        0x02, 0x01, 0x06,
        0x02, 0x0a, 0xeb,
        0x11, 0x09, 'E', 'S', 'P', '_', 'M', 'U', 'L', 'T', 'I', '_', 'A',
        'D', 'V', '_', '1', 'M'
};

```

Once we config the all the adv instance we can start all the adv using the func esp_ble_gap_ext_adv_start which can post this to scanner side with respective param.

## GAP Event Handler
Once the Extended advertising data have been set, the GAP event `ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT and ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT ` is triggered.

```c
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status %d", param->ext
_adv_set_rand_addr.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status %d", param->ext_ad
v_set_params.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status %d", param->ext_adv_
data_set.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status %d", param->sca
n_rsp_set.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status %d", param->ext_adv_sta
rt.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT, status %d", param->ext_adv_stop
.status);
        break;
    default:
        break;
    }
}

```
## Default config

This example by default  configured with
1M phy extend adv, Connectable advertising
2M phy extend adv, Scannable advertising
1M phy legacy adv, ADV_IND
coded phy extend adv, Connectable advertising

We can change Ext adv param ( adv type ), Random addr and Raw data using above struct and output will be seen accordingly.


## Conclusion
In this document, we have gone through the Multi adv  example code describing each section. The application is designed around the concept of Application Profiles. In addition, the procedures that this example uses to register event handlers are explained. The events follow a sequence of configuration steps, such as defining Extended advertising parameters, Random address and Raw dat with respective phy.
