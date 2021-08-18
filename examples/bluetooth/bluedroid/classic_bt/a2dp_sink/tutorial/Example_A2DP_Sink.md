# A2DP Sink Example Walkthrough

## Introduction

In this tutorial, the A2DP sink example code is reviewed. The code implements an A2DP sink role, which receives audio stream from A2DP source devices.

## File Tree

The file tree of this example is shown below. The [main](../main) folder contains the main files of this example including audio and video related files, program core files, main function file, etc. The [tutorial](../tutorial) folder contains this guidance document.

```c
└── a2dp_sink
    ├── CMakeLists.txt
    ├── main
    │   ├── bt_app_av.c
    │   ├── bt_app_av.h
    │   ├── bt_app_core.c
    │   ├── bt_app_core.h
    │   ├── CMakeLists.txt
    │   ├── Kconfig.projbuild
    │   └── main.c
    ├── README.md
    ├── sdkconfig.defaults
    └── tutorial
        └── Example_A2DP_Sink.md

2 directories, 11 files
```

## Main Entry Point

This example is located in the examples folder of the ESP-IDF under the [bluetooth/bluedroid/classic_bt/a2dp_sink](../). The entry point of this program is `app_main()` which contained in [main/main.c](../main/main.c).

### NVS Initialization

The main function starts by initializing the non-volatile storage library. This library allows to save key-value pairs in flash memory and is used by some components such as the Wi-Fi library to save the SSID and password:

```c
/* initialize NVS — it is used to store PHY calibration  */
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
    ESP_LOGE(BT_AV_TAG, "%s initialize controller ed\n", __func__);
    return;
}
/* enable Bluetooth Controller in Classic Bluetooth mode */
if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != OK) {
    ESP_LOGE(BT_AV_TAG, "%s enable controller failed\n", __func__);
    return;
}
```

After the initialization of the Bluetooth Controller, the Bluedroid Stack, which includes the common definitions and APIs for Classic Bluetooth is initialized and enabled by using:

```c
/* initialize Bluedroid Host */
if (esp_bluedroid_init() != ESP_OK) {
    ESP_LOGE(BT_AV_TAG, "%s initialize bluedroid failed\n", __func__);
    return;
}
/* enable Bluedroid Host */
if (esp_bluedroid_enable() != ESP_OK) {
    ESP_LOGE(BT_AV_TAG, "%s enable bluedroid failed\n", __func__);
    return;
}
```

The Classic Bluetooth uses an asynchronous programming paradigm. The entire Bluedroid stack sees the use of events, event handlers, callbacks and state machines. Most APIs are implemented by posting specific type of events to the work queue of Bluedroid Stack. Threads(FreeRTOS) tasks inside Bluedroid then process the events with specific handlers, according to the internal state. When the operations are completed, Bluedroid stack invokes the callback function which is registered by application, to report some other events and information.

For example, after executing `esp_bt_gap_start_discovery()`, an event of `ESP_BT_GAP_DISC_STATE_CHANGED_EVT` occurs to inform that the discovery state has been changed. Application can do some processing at this time.

### I2S Installation

The main function installs I2S to play the audio. A loudspeaker, additional ADC or hardware requirements and possibly an external I2S codec may be needed.

```c
    /* I2S configuration parameters */
    #ifdef CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_INTERNAL_DAC
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_ADC_DAC, NULL);
        chan_cfg.id = I2S_NUM_0;
        i2s_dac_slot_config_t slot_cfg = I2S_DAC_SLOT_CONFIG(I2S_DAC_CHAN_BOTH);
        i2s_adc_dac_clk_config_t clk_cfg = I2S_ADC_DAC_CLK_CONFIG(44100);
    #else
        i2s_gpio_config_t i2s_pin = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = CONFIG_EXAMPLE_I2S_BCK_PIN,
            .ws = CONFIG_EXAMPLE_I2S_LRCK_PIN,
            .dout = CONFIG_EXAMPLE_I2S_DATA_PIN,
            .din = I2S_GPIO_UNUSED
        };
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
        chan_cfg.id = I2S_NUM_0;
        i2s_std_slot_config_t slot_cfg = I2S_STD_MSB_SLOT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        slot_cfg.auto_clear = true;
        i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(44100);
    #endif
        /* enable I2S */
        i2s_new_channel(&chan_cfg, &tx_chan, NULL);
        i2s_init_channel(tx_chan, &clk_cfg, &slot_cfg);
        i2s_start_channel(tx_chan);
```

### Paring Parameter Settings

The main function continues to set up paring parameters including Secure Simple Pairing and Legacy Pairing.

```c
#if (CONFIG_BT_SSP_ENABLED == true)
    /* set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /* set default parameters for Legacy Pairing (Use fixed pin code) */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;
    pin_code[0] = '1';
    pin_code[1] = '2';
    pin_code[2] = '3';
    pin_code[3] = '4';
    esp_bt_gap_set_pin(pin_type, 4, pin_code);
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
bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_STACK_UP_EVT, NULL, 0, NULL);
```

The call of function `bt_app_work_dispatch` posts the event "BT_APP_STACK_UP_EVT" together with the handler function `bt_av_hdl_stack_evt`, to the work queue of application task. The application task is then triggered to invoke the handler function to process this event.