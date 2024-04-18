# Periodic Adv Example Walkthrough

## introduction

In this document, We review the Periodic  Adv example code which implements a Bluetooth Low Energy (BLE5.0) Multi adv profile on the ESP32C3. This example is designed the periodic advertisement which allow the scanner to sync with the advertiser so that scanner and advertiser wake up same time.

##include
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
#include "freertos/semphr.h"
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
    // 2M phy extend adv, Connectable advertising
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params_2M), test_s
em);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(EXT_ADV_HANDLE, addr_2m), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE, sizeof(raw_ext_adv_dat
a_2m), &raw_ext_adv_data_2m[0]), test_sem);

    // start all adv
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, &ext_adv[0]), test_sem);
    
    // set periodic adv param
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_set_params(EXT_ADV_HANDLE, &periodic_adv_params), test_sem);

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
    // set periodic adv raw data
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, sizeof(periodic_adv_raw_data), &periodic_adv_raw_data[0], false), test_sem);
    // start periodic adv, include the ADI field in AUX_SYNC_IND PDUs
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE, true), test_sem);
    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        // just update the Advertising DID of the periodic advertising, unchanged data
        FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, 0, NULL, true), test_sem);
    }
#else
    // set periodic adv raw data
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, sizeof(periodic_adv_raw_data), &periodic_adv_raw_data[0]), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE), test_sem);
#endif

    return;
}

```
The main function starts by initializing the non-volatile storage library. This library allows to save key-value pairs in flash memory and is used by some components such as the Wi-Fi library to
save the SSID and password:

```c
ret = nvs_flash_init();
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
The Bluetooth stack is up and running at this point in the program flow, however the functionality of the application has not been defined yet. The functionality is defined by reacting to events such as what happens when another device tries to read or write parameters and establish a connection. The two main managers of events are the GAP and GATT event handlers. The application needs to register a callback function for each event handler in order to let the application know which functions are going to handle the GAP and GATT events:

```c
esp_ble_gap_register_callback(gap_event_handler);
```
The functions  `gap_event_handler()` handle all the events that are pushed to the application from the BLE stack.

## Setting GAP Parameters

The register application event is the first one that is triggered during the lifetime of the program, this example uses the Profile A GATT event handle to configure the advertising parameters upon registration. This example has the option to use both standard Bluetooth Core Specification advertising parameters or a customized raw buffer. The option can be selected with the `CONFIG_SET_RAW_ADV_DATA` define. The raw advertising data can be used to implement iBeacons, Eddystone or other proprietaries, and custom frame types such as the ones used for Indoor Location Services that are different from the standard specifications.

The function is used to configure different types of extended advertisement types and legacy adv with 1M,2M and coded phy is esp_ble_gap_ext_adv_set_params , esp_ble_gap_ext_adv_set_rand_addr and esp_ble_gap_config_ext_adv_data_raw. Respective structure of each one of them mentioned below with one example:

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
2M phy example->
Ext adv param:
```c
esp_ble_gap_ext_adv_params_t ext_adv_params_2M = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
    .interval_min = 0x30,
    .interval_max = 0x30,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
};
```

Ext adv Random addr:
```c
uint8_t addr_2m[6] = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x02};
```
Ext adv raw data:

```c
static uint8_t raw_ext_adv_data_2m[] = {
        0x02, 0x01, 0x06,
        0x02, 0x0a, 0xeb,
        0x13, 0x09, 'E', 'S', 'P', '_', 'M', 'U', 'L', 'T', 'I', '_', 'A',
        'D', 'V', '_', '8', '0', 'M', 'S'
};
```

Similar thing for the periodic param :
The function is used for the esp_ble_gap_periodic_adv_set_params, esp_ble_gap_config_periodic_adv_data_raw and esp_ble_gap_periodic_adv_start. Respective structure of each one of them mentioned below with one example:

```c
static esp_ble_gap_periodic_adv_params_t periodic_adv_params = {
    .interval_min = 0x40, // 80 ms interval
    .interval_max = 0x40,
    .properties = 0, // Do not include TX power
};
```

```c
static uint8_t periodic_adv_raw_data[] = {
        0x02, 0x01, 0x06,
        0x02, 0x0a, 0xeb,
        0x03, 0x03, 0xab, 0xcd,
        0x11, 0x09, 'E', 'S', 'P', '_', 'P', 'E', 'R', 'I', 'O', 'D', 'I',
        'C', '_', 'A', 'D', 'V'
};
```

```c
static esp_ble_gap_ext_adv_t ext_adv[1] = {
    // instance, duration, period
    [0] = {EXT_ADV_HANDLE, 0, 0},
};
```
Once we config the all the adv instances, We can start advertising using the function esp_ble_gap_ext_adv_start  a which can post this to scanner side with respective param.

## GAP Event Handler
Once the Extended advertising data have been set, the GAP event `ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT,ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT and ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT` is triggered.

```c

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
     switch (event) {
     case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status %d", param->ext _adv_set_rand_addr.status);
         break;
     case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status %d", param->ext_adv_set_params.status);
         break;
     case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status %d", param->ext_adv_data_set.status);
         break;
     case ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status %d", param->scan_rsp_set.status);
         break;
     case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status %d", param->ext_adv_start.status);
         break;
     case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
         ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT, status %d", param->ext_adv_stop.status);
         break;
     case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
         xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, status %d", param->p
eroid_adv_set_params.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT, status %d", param->per
iod_adv_data_set.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT, status %d", param->period
_adv_start.status);
        break;
    default:
        break;
    }
}
```
## Default config
2M phy with connectable mode of periodic adv.


## Conclusion
In this document, we have gone through the Periodic adv example code describing each section. The application is designed around the concept of Application Profiles. In addition, the procedures that this example uses to register event handlers are explained. The events follow a sequence of configuration steps, such as defining Extended advertising parameters, Random address, Raw data, Periodic param, periodic data and start of Periodic adv in connectable mode.


