#Periodic Sync Example Walkthrough

## Introduction
In this tutorial, the Periodic sync example code for the ESP32C3 is reviewed. The code implement Bluetooth Low Energy (BLE5.0) periodic sync , which scans for nearby peripheral which can support legacy, extended and periodic advertisement. Periodic Sync allow the advertiser to sync with scanner so that scanner and advertiser wake up at same time.

* ADV_EXT_IND is over primary advertising channels
* AUX_ADV_IND and AUX_SYNC_IND are over secondary advertising channels

Little info about the EXT_ADV_IND , AUX_ADV_IND and AUX_SYNC_IND with scanner support of periodic sync.

ADV_EXT_IND is over primary advertising channels and is used to indicate that an advertisement will be sent on a secondary advertisement channel. The information in ADV_EXT_IND will inform the scanner:

* Which secondary advertising channel will be used by AUX_ADV_IND
* Which PHY will be used by AUX_ADV_IND, 1M PHY, 2M PHY, or 1M Coded PHY
* When AUX_ADV_IND will be presented on that specified secondary advertising channel

If the scanner is capable of periodic advertising, it will enable its receiver at a specific channel and PHY at a specific time slot.

AUX_ADV_IND is over the secondary advertising channels and is used to indicate a periodic advertising event. The information in AUX_ADV_IND, which points to the first AUX_SYNC_IND, are:

* The offset time of the next periodic advertising packet
* The interval of periodic advertising
* The secondary channel map used through this periodic advertising lifetime
Access address, etc.

With this information, the scanner can synchronize with the advertiser and they will dance together.

*AUX_SYNC_IND includes the AdvData which needs to be periodically advertised.

## Includes

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h
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
#include "freertos/semphr.h"
```

These `includes` are required for the FreeRTOS and underlying system components to run, including the logging functionality and a library to store data in non-volatile flash memory. We are interested in `“bt.h”`, `“esp_bt_main.h”`, `"esp_gap_ble_api.h"` and `“esp_gattc_api.h”`, which expose the BLE APIs required to implement this example.

* `esp_bt.h`: configures the BT controller and VHCI from the host side.
* `esp_bt_main.h`: initializes and enables the Bluedroid stack.
* `esp_gap_ble_api.h`: implements the GAP configuration, such as advertising and connection parameters.
* `esp_gattc_api.h`: implements the GATT Client configuration, such as connecting to peripherals and searching for services.

## Main Entry Point

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
        ESP_LOGE(LOG_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret)
);
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

    FUNC_SEND_WAIT_SEM(esp_ble_gap_set_ext_scan_params(&ext_scan_params), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD), test_sem);


    return;
}
```
The main function starts by initializing the non-volatile storage library. This library allows to save key-value pairs in flash memory and is used by some components such as the Wi-Fi library to save the SSID and password:

```c
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}
ESP_ERROR_CHECK( ret );
```

## BT Controller and Stack Initialization

The main function also initializes the BT controller by first creating a BT controller configuration structure named `esp_bt_controller_config_t` with default settings generated by the `BT_CONTROLLER_INIT_CONFIG_DEFAULT()` macro. The BT controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL) and the Physical Layer (PHY). The BT Controller is invisible to the user applications and deals with the lower layers of the BLE stack. The controller configuration includes setting the BT controller stack size, priority and HCI baud rate. With the settings created, the BT controller is initialized and enabled with the `esp_bt_controller_init()` function:

```c
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
ret = esp_bt_controller_init(&bt_cfg);
```

Next, the controller is enabled in BLE Mode.

```c
ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
```
> The controller should be enabled in `ESP_BT_MODE_BTDM`, if you want to use the dual mode (BLE +
BT).

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
The main function ends by registering the GAP and GATT event handlers, as well as the Application Profile and set the maximum supported MTU size.

```c
    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
```

The GAP and GATT event handlers are the functions used to catch the events generated by the BLE stack and execute functions to configure parameters of the application. Moreover, the event handlers are also used to handle read and write events coming from the central. The GAP event handler takes care of scanning and connecting to servers and the GATT handler manages events that happen after the client has connected to a server, such as searching for services and writing and reading data. The GAP and GATT event handlers are registered by using:

```c
esp_ble_gap_register_callback();
esp_ble_gattc_register_callback();
```
The functions `esp_gap_cb()` and `esp_gattc_cb()` handle all the events generated by the BLE stack.

## Setting Scan Parameters
if interested. However, in order to perform the scanning, first the configuration parameters needto be set.
The func will be called in the context of bin semaphore esp_ble_gap_set_ext_scan_params,which takes the param as struct esp_ble_ext_scan_params_t

```c
/**
* @brief ext scan parameters
*/
typedef struct {
    esp_ble_addr_type_t own_addr_type;        /*!< ext scan own address type */
    esp_ble_scan_filter_t filter_policy;      /*!< ext scan filter policy */
    esp_ble_scan_duplicate_t  scan_duplicate; /*!< ext scan duplicate scan */
    esp_ble_ext_scan_cfg_mask_t cfg_mask;     /*!< ext scan config mask */
    esp_ble_ext_scan_cfg_t uncoded_cfg;       /*!< ext scan uncoded config parameters */
    esp_ble_ext_scan_cfg_t coded_cfg;         /*!< ext scan coded config parameters */
} esp_ble_ext_scan_params_t;

 /**
 * @brief ext scan config
 */
 typedef struct {
     esp_ble_scan_type_t scan_type; /*!< ext scan type */
     uint16_t scan_interval;        /*!< ext scan interval. This is defined as the ti
me interval from when the Controller started its last LE scan until it begins the subsequent LE s
can.*/
     //Range: 0x0004 to 0x4000
     //Default: 0x0010 (10 ms)
     //Time = N * 0.625 msec
     //Time Range: 2.5 msec to 10.24     seconds

     uint16_t scan_window;          /*!< ext scan window. The duration of the LE scan
. LE_Scan_Window shall be less than or equal to LE_Scan_Interval*/
    //Range: 0x0004 to 0x4000                                                            //Default
t: 0x0010 (10 ms)
    //Time = N * 0.625 msec
    //Time Range: 2.5 msec to 10240 msec

 } esp_ble_ext_scan_cfg_t;

```
An it is initialized as :

```c
static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK | ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
    .coded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
};
```

same for the periodic sync:

```c
/**
* @brief periodic adv sync parameters
*/
typedef struct {
    esp_ble_gap_sync_t filter_policy;       /*!< Configures the filter policy for periodic advertising sync:
                                                 0: Use Advertising SID, Advertiser Address Type, and Advertiser Address parameters to determine the advertiser to listen to.
                                                 1: Use the Periodic Advertiser List to determine the advertiser to listen to. */
    #if (BLE_FEAT_CREATE_SYNC_ENH)
    esp_ble_gap_sync_t reports_disabled;    /*!< Supported only by esp32c2, esp32c6, and esp32h2; can be set by menuconfig:
                                                 0: Reporting initially enabled.
                                                 1: Reporting initially disabled. */
    esp_ble_gap_sync_t filter_duplicates;   /*!< Supported only by esp32c2, esp32c6, and esp32h2; can be set by menuconfig:
                                                 0: Duplicate filtering initially disabled.
                                                 1: Duplicate filtering initially enabled. */
    #endif
    uint8_t sid;                            /*!< SID of the periodic advertising */
    esp_ble_addr_type_t addr_type;          /*!< Address type of the periodic advertising */
    esp_bd_addr_t addr;                     /*!< Address of the periodic advertising */
    uint16_t skip;                          /*!< Maximum number of periodic advertising events that can be skipped */
    uint16_t sync_timeout;                  /*!< Synchronization timeout */
} esp_ble_gap_periodic_adv_sync_params_t;

```
An it is initialized as:
```c
static esp_ble_gap_periodic_adv_sync_params_t periodic_adv_sync_params = {
     .filter_policy = 0,
     .sid = 0,
     .addr_type = BLE_ADDR_TYPE_RANDOM,
     .skip = 10,
     .sync_timeout = 1000,
 };
```
 The BLE scan parameters are configured so that the type of scanning is active (includes reading he scanning response), it is of public type, allows any advertising device to be read and has a scanning interval of 80 ms (1.25 ms * 0x40) and a scanning window of 80 ms (1.25 ms * 0x40).


## Start Scanning
This func invoke to start the scan to get near by discover devices esp_ble_gap_start_ext_scan.

## Getting Scan Results
The Below call back fun used for the event from start of scan to the end of getting adv report extanded as well as periodic report.
We can differer extended and periodic adv using this event ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT and ESP_GAP_BLE_EXT_ADV_REPORT_EVT.

```c
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT, status %d", param->set_e
xt_scan_params.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT, status %d", param->ext_scan_s
tart.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT, status %d", param->period_adv_
stop.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, status %d", param->
period_adv_create_sync.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT, status %d", param->
period_adv_sync_cancel.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT, status %d", para
m->period_adv_sync_term.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT, sync handle %d", param->period
ic_adv_sync_lost.sync_handle);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT:
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT, status %d", param->periodic_a
dv_sync_estab.status);
        esp_log_buffer_hex("sync addr", param->periodic_adv_sync_estab.adv_addr, 6);
        ESP_LOGI(LOG_TAG, "sync handle %d sid %d perioic adv interval %d adv phy %d", param->periodic_adv_sync_estab.sync_handle,
                                                                                      param->periodic_adv_sync_estab.sid,
                                                                                      param->periodic_adv_sync_estab.period_adv_interval,
                                                                                      param->periodic_adv_sync_estab.adv_phy);
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
            adv_name = esp_ble_resolve_adv_data(param->ext_adv_report.params.adv_data, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            if ((adv_name != NULL) && (memcmp(adv_name, "ESP_MULTI_ADV_80MS", adv_name_len) == 0)
 && !periodic_sync) {
            periodic_sync = true;
                char adv_temp_name[30] = {'0'};
                memcpy(adv_temp_name, adv_name, adv_name_len);
                ESP_LOGI(LOG_TAG, "Start create sync with the peer device %s", adv_temp_name);
            periodic_adv_sync_params.sid = param->ext_adv_report.params.sid;
                periodic_adv_sync_params.addr_type = param->ext_adv_report.params.addr_type;
                memcpy(periodic_adv_sync_params.addr, param->ext_adv_report.params.addr, sizeof(esp_bd_addr_t));
            esp_ble_gap_periodic_adv_create_sync(&periodic_adv_sync_params);
            }
    }
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT:
        ESP_LOGI(LOG_TAG, "periodic adv report, sync handle %d data status %d data len %d rssi %d
", param->period_adv_report.params.sync_handle,
   param->period_adv_report.params.data_status,

   param->period_adv_report.params.data_length,

   param->period_adv_report.params.rssi);
        break;

    default:
        break;
    }
}
```
## Default Config
Here we are doing scan as Active with the fixed duration of 80msec.


## Conclusion

We have reviewed the Periodic Sync example code for the ESP32C3. This example scans for nearby devices which support extended adv and periodic adv.
