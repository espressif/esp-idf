| Supported Targets | ESP32 |
| ----------------- | ----- |

# Bluetooth HID Device example

This example aims to show how to implement a Bluetooth HID device using the APIs provided by Classic Bluetooth HID profile.

This example simulates a Bluetooth HID mouse device that periodically sends report to remote Bluetooth HID host after connection. The report indicates a horizontally moving pointer and can be observed on the display on the HID host side. If you want to build an HID device, this can be your first example to look at.

## How to use example

### Hardware Required

* This example is able to run on any commonly available ESP32 development board, e.g. ESP32-DevKitC.

* This example is supposed to connect to a Classic Bluetooth HID Host device, e.g. laptop or tablet.

### Configure the project

```
idf.py menuconfig
```

* Check and enable Classic Bluetooth and Classic BT HID Device under Component config --> Bluetooth --> Bluedroid Options

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The following log will be shown on the IDF monitor console:

```
I (572) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (663) BTDM_INIT: BT controller compile version [5688ed5]
I (663) system_api: Base MAC address is not set
I (663) system_api: read default base MAC address from EFUSE
I (673) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
W (683) phy_init: failed to load RF calibration data (0xffffffff), falling back to full calibration
I (1533) app_main: setting device name
I (1533) esp_bt_gap_cb: event: 10
I (1543) app_main: setting cod major, peripheral
I (3543) app_main: register hid device callback
I (3543) app_main: starting hid device
I (3543) esp_bt_hidd_cb: setting hid parameters
I (3543) esp_bt_gap_cb: event: 10
I (3543) esp_bt_hidd_cb: setting hid parameters success!
I (3553) esp_bt_hidd_cb: setting to connectable, discoverable
I (3563) bt_address: my bluetooth address is 78:E3:6D:CD:02:2A
I (3563) app_main: exiting
```

The messages show the successful initialization of Bluetooth stack and HID application. ESP32 will become discoverable with the Bluetooth device name as "HID Mouse Example", by nearby Bluetooth HID Host device.

Connect to ESP32 on the HID Host side, then finish bonding. After that the HID connection will be established. IDF monitor console will continue to print messages like:

```
I (50663) esp_bt_gap_cb: authentication success: privacy_k65
I (50663) esp_bt_gap_cb: 64 a2 f9 69 57 a4 
W (51443) BT_APPL: new conn_srvc id:20, app_id:1
I (51443) esp_bt_hidd_cb: connected to 64:a2:f9:69:57:a4
I (51443) esp_bt_hidd_cb: making self non-discoverable and non-connectable.
I (51443) mouse_move_task: starting
I (51463) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51493) esp_bt_gap_cb: ESP_BT_GAP_MODE_CHG_EVT mode:2
I (51503) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51553) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51603) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51653) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51703) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51753) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
I (51803) esp_bt_hidd_cb: ESP_HIDD_SEND_REPORT_EVT id:0x00, type:1
```

ESP32 will generate and send HID mouse reports periodically. On the screen of HID Host, the cursor will move horizontally from left to right and then right to left, and so on so forth.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

## Example Breakdown

### Initial settings for Bluetooth HID device profile

Bluetooth HID device requires the specific major and minor device type in the Class of Device (CoD), the following lines of source code performs the configuration of CoD:

```
void app_main(void) {
    ...
    ESP_LOGI(TAG, "setting cod major, peripheral");
    esp_bt_cod_t cod;
    cod.major = ESP_BT_COD_MAJOR_DEV_PERIPHERAL;
    esp_bt_gap_set_cod(cod, ESP_BT_SET_COD_MAJOR_MINOR);
    ...
}
```

Bluetooth HID device profile requires the information of service name, provide, device subclass, report descriptor for SDP server, as well as L2CAP QoS configurations from the application. Following lines in function `app_main` initialize these information fields:

```
void app_main(void) {
    ...
    // Initialize HID SDP information and L2CAP parameters.
    // to be used in the call of `esp_bt_hid_device_register_app` after profile initialization finishes
    do {
        s_local_param.app_param.name = "Mouse";
        s_local_param.app_param.description = "Mouse Example";
        s_local_param.app_param.provider = "ESP32";
        s_local_param.app_param.subclass = ESP_HID_CLASS_MIC;
        s_local_param.app_param.desc_list = hid_mouse_descriptor;
        s_local_param.app_param.desc_list_len = hid_mouse_descriptor_len;

        memset(&s_local_param.both_qos, 0, sizeof(esp_hidd_qos_param_t)); // don't set the qos parameters
    } while (0);

    // Report Protocol Mode is the default mode, according to Bluetooth HID specification
    s_local_param.protocol_mode = ESP_HIDD_REPORT_MODE;

    ESP_LOGI(TAG, "register hid device callback");
    esp_bt_hid_device_register_callback(esp_bt_hidd_cb);

    ESP_LOGI(TAG, "starting hid device");
    esp_bt_hid_device_init();
    ...
}
```

The information is set to global struct `s_local_param` and will be used upon successful profile initialization, i.e. reception of `ESP_HIDD_INIT_EVT` which is generated after the call of `esp_bt_hid_device_init()`:

```
void esp_bt_hidd_cb(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
    ...
    switch (event) {
    case ESP_HIDD_INIT_EVT:
        if (param->init.status == ESP_HIDD_SUCCESS) {
            ESP_LOGI(TAG, "setting hid parameters");
            esp_bt_hid_device_register_app(&s_local_param.app_param, &s_local_param.both_qos, &s_local_param.both_qos);
        } else {
            ESP_LOGE(TAG, "init hidd failed!");
        }
        break;
    ...
    }
    ...
}
```

### Determination of HID Report Mode

There are two HID report modes: Report Protocol Mode and Boot Protocol Mode. The former is the default mode. The two report modes differ in the report contents and format. The example supports both of the two modes.

Report Mode requires report descriptor to describe the usage and format of the reports. For Bluetooth HID device, the report descriptor shall be provided in the SDP server, which can be discovered and used by remote HID Host.

Boot Mode only supports keyboards and mice, with pre-defined report formats. Therefore it does not require a report descriptor parser on the remote HID Host. It is originally used to simplify the design of PC BIOSs.

The following code lines set Report Protocol Mode as the default Report Mode:

```
void app_main(void) {
    ...
    // Report Protocol Mode is the default mode, according to Bluetooth HID specification
    s_local_param.protocol_mode = ESP_HIDD_REPORT_MODE;
    ...
}
```

Report Mode can be choosen by remote HID Host through the SET_PROTOCOL request:

```
void esp_bt_hidd_cb(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
    ...
    switch (event) {
    ...
    case ESP_HIDD_SET_PROTOCOL_EVT:
        ESP_LOGI(TAG, "ESP_HIDD_SET_PROTOCOL_EVT");
        if (param->set_protocol.protocol_mode == ESP_HIDD_BOOT_MODE) {
            ESP_LOGI(TAG, "  - boot protocol");
            xSemaphoreTake(s_local_param.mouse_mutex, portMAX_DELAY);
            s_local_param.x_dir = -1;
            xSemaphoreGive(s_local_param.mouse_mutex);
        } else if (param->set_protocol.protocol_mode == ESP_HIDD_REPORT_MODE) {
            ESP_LOGI(TAG, "  - report protocol");
        }
        xSemaphoreTake(s_local_param.mouse_mutex, portMAX_DELAY);
        s_local_param.protocol_mode = param->set_protocol.protocol_mode;
        xSemaphoreGive(s_local_param.mouse_mutex);
        break;
    ....
    }
    ....
}
```

### Report generation

The example simulates a mouse by creating a FreeRTOS task that periodically generates and sends the HID mouse report:

```
// move the mouse left and right
void mouse_move_task(void* pvParameters)
{
    const char* TAG = "mouse_move_task";

    ESP_LOGI(TAG, "starting");
    for(;;) {
        s_local_param.x_dir = 1;
        int8_t step = 10;
        for (int i = 0; i < 2; i++) {
            xSemaphoreTake(s_local_param.mouse_mutex, portMAX_DELAY);
            s_local_param.x_dir *= -1;
            xSemaphoreGive(s_local_param.mouse_mutex);
            for (int j = 0; j < 100; j++) {
                send_mouse_report(0, s_local_param.x_dir * step, 0, 0);
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

Function `send_mouse_report` is used to pack the information into a mouse HID report and sends it to HID Host, according to the Report Mode applied:

```
// send the buttons, change in x, and change in y
void send_mouse_report(uint8_t buttons, char dx, char dy, char wheel)
{
    uint8_t report_id;
    uint16_t report_size;
    xSemaphoreTake(s_local_param.mouse_mutex, portMAX_DELAY);
    if (s_local_param.protocol_mode == ESP_HIDD_REPORT_MODE) {
        report_id = 0;
        report_size = REPORT_PROTOCOL_MOUSE_REPORT_SIZE;
        s_local_param.buffer[0] = buttons;
        s_local_param.buffer[1] = dx;
        s_local_param.buffer[2] = dy;
        s_local_param.buffer[3] = wheel;
    } else {
        // Boot Mode
        report_id = ESP_HIDD_BOOT_REPORT_ID_MOUSE;
        report_size = ESP_HIDD_BOOT_REPORT_SIZE_MOUSE - 1;
        s_local_param.buffer[0] = buttons;
        s_local_param.buffer[1] = dx;
        s_local_param.buffer[2] = dy;
    }
    esp_bt_hid_device_send_report(ESP_HIDD_REPORT_TYPE_INTRDATA, report_id, report_size, s_local_param.buffer);
    xSemaphoreGive(s_local_param.mouse_mutex);
}
```
