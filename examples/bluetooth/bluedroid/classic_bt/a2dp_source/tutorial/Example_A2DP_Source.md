# A2DP Source Example Walkthrough

## Introduction

In this tutorial, the A2DP source example code is reviewed. The code implements an A2DP source role, which transmits audio stream to A2DP sink devices. This example performs device discovery to search for a target device and then initiates connection with it. After connection established, the source starts to transmit audio stream to peer device and change the volume locally based on heart beat.

## File Tree

The file tree of this example is shown below. The [main](../main) folder contains the main files of this example including program core files, main function file, etc. The [tutorial](../tutorial) folder contains this guidance document.

```c
└── a2dp_source
    ├── CMakeLists.txt
    ├── main
    │   ├── bt_app_core.c
    │   ├── bt_app_core.h
    │   ├── CMakeLists.txt
    │   └── main.c
    ├── README.md
    ├── sdkconfig.defaults
    └── tutorial
        └── Example_A2DP_Source.md

2 directories, 8 files
```

## Main Entry Point

This example is located in the examples folder of the ESP-IDF under the [bluetooth/bluedroid/classic_bt/a2dp_source](../). The entry point of this program is `app_main()` which contained in [main/main.c](../main/main.c).

### NVS Initialization

The main function starts by initializing the non-volatile storage library. This library allows to save key-value pairs in flash memory and is used by some components such as the Wi-Fi library to save the SSID and password:

```c
/* Initialize NVS — it is used to store PHY calibration  */
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}
ESP_ERROR_CHECK(ret);
```

### Bluetooth Controller and Stack Initialization

The main function also initializes the Bluetooth Controller with default settings. The Bluetooth Controller is invisible to the user applications and deals with the lower layers of the Bluetooth Stack. Next, the controller is enabled in Classic Bluetooth Mode.

```c
/* initialize Bluetooth Controller with default configuration */
esp_bt_controller_config_t bt_cfg = ONTROLLER_INIT_CONFIG_DEFAULT();
if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
    ESP_LOGE(BT_AV_TAG, "%s initialize controller ed", __func__);
    return;
}
/* Enable Bluetooth Controller in Classical Bluetooth mode */
if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != OK) {
    ESP_LOGE(BT_AV_TAG, "%s enable controller failed", __func__);
    return;
}
```

After the initialization of the Bluetooth Controller, the Bluedroid stack, which includes the common definitions and APIs for Classic Bluetooth is initialized and enabled by using:

```c
    /* initialize Bluedroid Host */
    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
#if (CONFIG_EXAMPLE_SSP_ENABLED == false)
    bluedroid_cfg.ssp_en = false;
#endif
    if ((ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s initialize bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    /* Enable Bluedroid Host */
    if (esp_bluedroid_enable() != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s enable bluedroid failed", __func__);
        return;
    }
```

The Classic Bluetooth uses an asynchronous programming paradigm. The entire Bluedroid stack sees the use of events, event handlers, callbacks and state machines. Most APIs are implemented by posting specific type of events to the work queue of Bluedroid Stack. Threads(FreeRTOS) tasks inside Bluedroid then process the events with specific handlers, according to the internal state. When the operations are completed, Bluedroid stack invokes the callback function which is registered by application, to report some other events and information.

For example, after executing `esp_bt_gap_start_discovery()`, an event of `ESP_BT_GAP_DISC_STATE_CHANGED_EVT` occurs to inform that the discovery state has been changed. Application can do some processing at this time.

### Paring Parameter Settings

The main function continues to set up paring parameters including Secure Simple Pairing and Legacy Pairing.

```c
#if (CONFIG_EXAMPLE_SSP_ENABLED == true)
/* set default parameters for Secure Simple Pairing */
esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

/*
 * Set default parameters for Legacy Pairing
 * Use variable pin, input pin code when pairing
 */
esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
esp_bt_pin_code_t pin_code;
esp_bt_gap_set_pin(pin_type, 0, pin_code);
```

### Create Application Task

Finally, the main function starts up the application task. `bt_app_task_start_up()` creates a work queue and a FreeRTOS task to process the events(messages) posted into the work queue.

```c
/* create application task */
bt_app_task_start_up();
```

### Profile Set up

The main function ends up dispatching event "BT_APP_STACK_UP_EVT" to the application task.

```c
/* Bluetooth device name, connection mode and profile set up */
bt_app_work_dispatch(bt_av_hdl_stack_evt,BT_APP_STACK_UP_EVT, NULL, 0, NULL);
```

The call of function `bt_app_work_dispatch` posts the event "BT_APP_STACK_UP_EVT" together with the handler function `bt_av_hdl_stack_evt`, to the work queue of application task. The application task is then triggered to invoke the handler function to process this event.
