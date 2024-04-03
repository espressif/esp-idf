# Introduction

This demo demonstrates the Wi-Fi and Bluetooth (BLE/BR/EDR) coexistence feature of ESP32. Simply put, users can use the Wi-Fi function while operating Bluetooth. In this demo,

* The Wi-Fi function is demonstrated by measuring its transfer rate, using the `iperf` protocol;
* The Bluetooth function is demonstrated by the fast provisioning function. Details can be seen in `fast_prov_server`.


> Note: In this demo, you call wifi API and bluetooth API to achieve the functions you want. such as `wifi_get_local_ip` API and `esp_ble_mesh_provisioner_add_unprov_dev` API.

# What You Need

Download and flash the `wifi_coexist` project to your ESP32 development board and then use the following commands to get started with this demo.

1. Connect your development board to the Wi-Fi network by entering the `sta ssid password` command in your serial port tool.
	- For example, you should enter `sta test_wifi 12345678` if you want to connect your board to a network with a SSID of `test_wifi` and a password of `12345678`.

2. Start a TCP server by entering the `iperf -s -i 3 -t 1000` command in your serial port tool, which prints the current transfer rate of the Wi-Fi network the board connects to.

3. Start a TCP client by entering the `iperf -c board_IP_address -i 3 -t 60` command in your PC terminal.
	- For example, you should enter `iperf -c 192.168.10.42 -i 3 -t 60`, if the IP address of your board is 192.168.10.42.


Meanwhile, you can use the Bluetooth function during the whole process, for example, controlling the LED indicator on your board.

> Note:
>
> 1. Please use the correct serial port number for connecting your board when entering commands in your serial port tool;
> 2. Your PC and board should connect to the same Wi-Fi network;


# Project Directory

The `wifi_coexist` demo contains the following files and subfolders:

```
$ tree examples/bluetooth/esp_ble_mesh/wifi_coexist
├── main        /* Stores the `.c` and `.h` application code files for this demo */
├── components  /* Stores the `.c` and `.h` iperf code files for this demo */
├── Makefile    /* Compiling parameters for the demo */
├── README.md   /* Quick start guide */
├── build
├── sdkconfig      /* Current parameters of `idf.py menuconfig` */
├── sdkconfig.defaults   /* Default parameters of `idf.py menuconfig` */
├── sdkconfig.old      /* Previously saved parameters of `idf.py menuconfig` */
└── tutorial         /* More in-depth information about the demo */
```

The `main` folder mainly implements the BLE Mesh feature.  Details can be seen in `fast_prov_server`.

The `components` folder mainly implements the Wi-Fi feature, which allows some basic commands and `iperf-related` test commands.

> Note:
>
> [Iperf](https://iperf.fr) is a tool for active measurements of the maximum achievable bandwidth on IP networks. It supports tuning of various parameters related to timing, buffers and protocols (TCP, UDP, SCTP with IPv4 and IPv6).

# Example Walkthrough

## Main Entry Point

The program’s entry point is the `app_main()` function.

## Initialization

The code block below first initialize the board, then its bluetooth-related functions (including the Bluetooth and BLE Mesh) and the Wi-Fi console.

```c
void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    err = board_init();
    if (err) {
        ESP_LOGE(TAG, "board_init failed (err %d)", err);
        return;
    }

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
        return;
    }

    wifi_console_init();
}
```

### Initializing the Board

This demo calls the `board_init` function to:

1. First initialize three pins for the LED indicator, and set its initial status to OFF (i.e. initialize the `led_state[i].previous` variable to `LED_OFF`).

```c
for (int i = 0; i < 3; i++) {
    esp_rom_gpio_pad_select_gpio(led_state[i].pin);
    gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
    gpio_set_level(led_state[i].pin, LED_OFF);
    led_state[i].previous = LED_OFF;
}
```

2. Create a task named `led_action_thread` for controlling the status of the light, and then a queue named `led_action_queue` for storing status data, whose format is defined in `struct _led_state`.

```c
led_action_queue = xQueueCreate(60, sizeof(struct _led_state));
ret = xTaskCreate(led_action_thread, "led_action_thread", 4096, NULL, 5, NULL);
```

3. The `led_action_thread` task continuously sets the status of the LED indicator by calling the `gpio_set_level` function, using status data obtains data from the `led_action_queue` queue.

```c
static void led_action_thread(void *arg)
{
    struct _led_state led = {0};

    while (1) {
        if (xQueueReceive(led_action_queue, &led, (TickType_t)portMAX_DELAY)) {
            ESP_LOGI(TAG, "%s: pin 0x%04x onoff 0x%02x", __func__, led.pin, led.current);
            /* If the node is controlled by phone, add a delay when turn on/off led */
            if (fast_prov_server.primary_role == true) {
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }
            gpio_set_level(led.pin, led.current);
        }
    }
}
```

### Initializing the Bluetooth

This demo calls the `bluetooth_init` function to:

1. First initialize the non-volatile storage library, which allows saving key-value pairs in flash memory and is used by some components. You can save the node's keys and configuration information at `menuconfig` --> `Bluetooth Mesh support` --> `Store Bluetooth Mesh key and configuration persistently`:

	 ```c
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
	 ```

2. Initializes the BT controller by first creating a BT controller configuration structure named `esp_bt_controller_config_t` with default settings generated by the `BT_CONTROLLER_INIT_CONFIG_DEFAULT()` macro. The BT controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL) and the Physical Layer (PHY). The BT Controller is invisible to the user applications and deals with the lower layers of the BLE stack. The controller configuration includes setting the BT controller stack size, priority and HCI baud rate. With the settings created, the BT controller is initialized and enabled with the `esp_bt_controller_init()` function:

	```c
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	```

	Next, the controller is enabled in BLE Mode.

	```c
	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	```
	The controller should be enabled in `ESP_BT_MODE_BTDM`, if you want to use the dual mode (BLE + BT). There are four Bluetooth modes supported:

	* `ESP_BT_MODE_IDLE`: Bluetooth not running
	* `ESP_BT_MODE_BLE`: BLE mode
	* `ESP_BT_MODE_CLASSIC_BT`: BT Classic mode
	* `ESP_BT_MODE_BTDM`: Dual mode (BLE + BT Classic)

	After the initialization of the BT controller, the Bluedroid stack, which includes the common definitions and APIs for both BT Classic and BLE, is initialized and enabled by using:

	```
	ret = esp_bluedroid_init();
	ret = esp_bluedroid_enable();
	```

### Initializing the BLE Mesh

This demo calls the `ble_mesh_init` function to:

1. Initialize the board's uuid by setting the `dev_uuid` variable, which is used to distinguish devices when provisioning.

```c
static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    /* First two bytes of device uuid is compared with match value by Provisioner */
    memcpy(dev_uuid + 2, esp_bt_dev_get_address(), 6);

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb);
    esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb);
    esp_ble_mesh_register_config_server_callback(example_ble_mesh_config_server_cb);

    err = esp_ble_mesh_init(&prov, &comp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize BLE Mesh", __func__);
        return err;
    }
    ...
    return ESP_OK;
}
```
2. Register the provisioning callback function in the BLE Mesh stack with `esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb)`, among which `esp_ble_mesh_prov_cb` is used to handle events thrown by protocol stations. This requires the user to implement it himself, and also needs to know the meaning of the event and how to trigger it. For example: The `ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT` event is triggered when the provisioner starts provisioning unprovisioned device, and is handled in the `example_ble_mesh_provisioning_cb` function. Note that you need to register this function with the BLE Mesh protocol stack by calling the `esp_ble_mesh_register_prov_callback` API.

	```c
	ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT,                /*!< Provisioner establish a BLE Mesh link event */

	static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
	{
   	     esp_err_t err;

         switch (event) {
    	  case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        	ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT");
       	 provisioner_prov_link_open(param->provisioner_prov_link_open.bearer);
        	break;
   	 }
	}

	esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
	```
3. Register BLE Mesh callback for user-defined Models' operations with `esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb)`.

4. Register BLE Mesh Config Client Model callback with `esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb)`.
5. Register BLE Mesh Config Server Model callback with `esp_ble_mesh_register_config_server_callback(example_ble_mesh_config_server_cb)`.

6. Initialize the BLE Mesh module by calling the `esp_ble_mesh_init(&prov, &comp)` API, which initializes the provisioning capabilities and composition data information. Registered information is stored in the `prov` struct, which is essentially a composition of one or more Models.


### Initializing the Wi-Fi Console

This demo calls the `wifi_console_init` function:

```c
    initialise_wifi();

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    register_wifi();

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
```

1. Initialize the basic Wi-Fi function by calling `initialise_wifi`, which sets

	* the Current Wi-Fi power save type to `WIFI_PS_MIN_MODEM`, which indicates the station wakes up to receive beacon every DTIM period
	* the Wi-Fi API configuration storage type to `WIFI_STORAGE_RAM`, which indicates all configuration will only be stored in the embedded memory
	* the Wi-Fi operating mode to `WIFI_MODE_STA`, which allows the board to work in Station mode.


2. Create a UART based console REPL by calling the `esp_console_new_repl_uart` function.
3. Register the commands by calling the `register_wifi` function.
	* An example of registering a `restart` command with a `restart()` function to handle this command can be seen below. After the initialization, you can enter the `restart` command in your serial port tool to call the `restart()` function.
        ```c
            static int restart(int argc, char **argv)
            {
                ESP_LOGI(TAG, "Restarting");
                esp_restart();
            }
            const esp_console_cmd_t restart_cmd = {
                .command = "restart",
                .help = "Restart the program",
                .hint = NULL,
                .func = &restart,
            };
        ```

    Note that the `sta`,`scan`,`ap`,`query`,`iperf`,`restart` and `heap` commands are supported in this demo.

4. Start the REPL by calling `esp_console_start_repl`. The repl task internally will read data from the command line and then invoke the handler previously registered.
