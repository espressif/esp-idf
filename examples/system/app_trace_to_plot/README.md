| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Application Level Tracing Example (Plotting)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the [Application Level Tracing Library](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#) (henceforth referred to as **App Trace**) to send and plot dummy sensor data to a host via JTAG instead of the normal method of logging via UART.

UART logs are time consuming and can significantly slow down the function that calls it. Therefore, it is generally a bad idea to use UART logs in time-critical functions. Logging to host via JTAG is significantly faster and can be used in time-critical functions. For more details regarding logging to host via JTAG, refer to the [Logging to Host Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#app-trace-logging-to-host).

### Hardware Required

To run this example, you need a supported target dev board connected to a JTAG adapter, which can come in the following forms:

* [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v4-1) which integrates an on-board JTAG adapter. Ensure that the [required jumpers to enable JTAG are connected](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/get-started-wrover-kit.html#setup-options) on the WROVER-KIT.
* ESP32, ESP32-S2 or ESP32-C2 core board (e.g. ESP32-DevKitC, [ESP32-S2-Saola-1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/hw-reference/esp32s2/user-guide-saola-1-v1.2.html)) can also work as long as you connect it to an external JTAG adapter (e.g. FT2232H, J-LINK).
- For ESP32-C3 or ESP32-S3, any board with the built-in USB interface (USB_SERIAL_JTAG).

This example will assume that an ESP-WROVER-KIT is used.

#### Connections:

1. Connect the JTAG interface to the target board. For details about how to set up JTAG interface, please see [JTAG Debugging](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html). Power up both the JTAG debugger and target board.

2. To start the tcp socket server, you need to run `read_trace.py` tool under the `esp-idf/examples/system/app_trace_to_plot` path.

3. After connecting JTAG interface and starting the tcp socket server, you need to [Run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).


### Configure the project

```
idf.py menuconfig
```

* To enable application tracing, select the `(X) Trace memory` option under `Component config > Application Level Tracing`. This option should have been selected by default.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

**Run Plotting Tool** To plot data and open TCP socket, there is a tool named `read_trace.py` under the `examples/system/app_trace_to_plot` path. Run this tool in the terminal session with configured IDF development environment by entering the command below. This command opens a TCP socket and plots the given data when OpenOCD triggered to start App Trace. If you are running tool at first time, you need to install dash with `pip install dash` in the same terminal session after running configuring IDF development environment.

```bash
python read_trace.py --plot-config data.json --source tcp://localhost:53535 --output-file data.log
```

**Start App Trace:** Start OpenOCD and App Trace on the target by entering the command below. This command will start OpenOCD and collect all of the bytes from JTAG log data and send them to the tcp socket `tcp://localhost:53535` (note `tcp://` depends on which IP address and port number opened). Assuming that OpenOCD was started in this example's directory, `data.log` will be saved here as well.

After running the plotting tool and starting apptrace with OpenOCD separately, you need access plotting on related socket address. Default address is `http://127.0.0.1:8055/` and also address can be seen from `read_trace.py` output. You can see the plotting result by accessing the address from browser.


```bash
idf.py openocd --openocd-commands 'reset;esp apptrace start tcp://localhost:53535 0 -1 5'
```

**Note:** data.json file is an example for plot config file. It can be changed or modified.

**Note:** For more details on OpenOCD commands regarding App Trace, refer to the [OpenOCD Application Level Tracing Commands](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#openocd-application-level-tracing-commands)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Configuration file and data format

#### General format of json file is:

```JSON
"Subplot Name": {
    "data_streams"
        "Name of the sensor": {
            "id": int
            "x_axis_data_size": int
            "x_axis_timestamp": bool
            "y_axis_data_size": int
            "data_type": "string"
            ["precision": int]
            ["type": "string"]
            ["mode": "string"]
            ["any_plot_config": Type defined in plotly]
        }
    }
    "xaxis_title": "string",
    "yaxis_title": "string"
}
```

#### Explanations of JSON keys are:

| JSON Keys        | Explanation                                                                                                                                                                       |
|------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| id               | ID value of the incoming packet (e.g. 1, 2, 3)                                                                                                                                    |
| x_axis_data_size | Data size of x axis of the incoming packet in bytes (e.g 2, 4, 8)                                                                                                                 |
| x_axis_timestamp | Will x axis data be timestamp value (e.g. true, false)                                                                                                                            |
| y_axis_data_size | Data size of y axis of the incoming packet in bytes (e.g 2, 4, 8)                                                                                                                 |
| data_type        | Data type of y axis of the incoming packet (e.g. "int", "float", "double")                                                                                                        |
| precision        | Optional. If y axis data of the incoming packet is a floating point number, you need to send it in integer form by multiplying a power of 10 and adjust this field (e.g. 1, 2, 3) |
| type             | Optional. Determines the plotting type of the graph (e.g. "scatter", "bar"). Default is "scatter"                                                                                 |
| mode             | Optional. Determines how values will look like (e.g. "lines", "markers", "lines+markers"). Default is "lines"                                                                     |
| xaxis_title      | Title of x axis in the plot (e.g "time")                                                                                                                                          |
| yaxis_title      | Title of y axis in the plot (e.g "values", "sensor data")                                                                                                                         |

**Note:** Plotting works with plotly subplot feature. `id`, `timestamp_flag`, `data_size` and `data_type` are the mandatory elements. In addition to these elements, you can config your plot with [properties are passed to the constructor of the specified trace type](https://plotly.com/python/reference/scatter/). Data transferring is in {'x': ..., 'y': ..., ...} format. Graph types like Pie or Scatter3D won't work. Scatter, histogram, box, line and similar graph types are working.

#### Data format in microcontroller side is:

```C
struct format {
    char STX[5] = "esp32",
	uint32_t id,
	any_size y_axis_value,
	any_size x_axis_value
    char EXT = 0x03,
}
```
**Note** STX and EXT fields are mandatory and constant to check data validity in visualizer side. You cannot change or remove these fields.

**Note** y_axis_value and x_axis_value could be in any value and different sizes like long, short, and int. The only thing to do is enter the correct data size in the config file.

**Note:** If you want to send a floating point number, send it in integer form by multiplying a power of 10 and filling the precision field in the config file.

## Troubleshooting

### Unable to flash when OpenOCD is connected to the target

On ESP32 boards, one likely cause would be an incorrect SPI flash voltage when starting OpenOCD. Suppose a target board/module with a 3.3 V powered SPI flash is being used, but the configuration file (ex. `board/esp32-wrover.cfg` for ESP32) is selected when starting OpenOCD which can set the SPI flash voltage to 1.8 V. In this situation, the SPI flash will not work after OpenOCD connects to the target as OpenOCD has changed the SPI flash voltage. Therefore, you might not be able to flash to the target when OpenOCD is connected.

To work around this issue, users are suggested to use `board/esp32-wrover.cfg` for ESP32 boards/modules operating with an SPI flash voltage of 1.8 V, and `board/esp-wroom-32.cfg` for 3.3 V. Refer to [ESP32 Modules and Boards](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html) and [Set SPI Flash Voltage](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/tips-and-quirks.html#why-to-set-spi-flash-voltage-in-openocd-configuration) for more details.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
