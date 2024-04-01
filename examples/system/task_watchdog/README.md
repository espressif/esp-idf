| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Task Watchdog Example

The following example demonstrates how to use the following features of the task watchdog timer (TWDT):

- How to initialize and deinitialize the TWDT
- How to subscribe and unsubscribe tasks to the TWDT
- How to subscribe and unsubscribe users to the TWDT
- How to tasks and users can reset (i.e., feed) the TWDT

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32-S or ESP32-C series chip
* A USB cable for Power supply and programming

### Configure the project

Program should run correctly without needing any special configuration. However, users can disable `CONFIG_ESP_TASK_WDT_INIT` which will prevent the TWDT from being automatically initialized on startup. If disabled, the example will manually initialize the TWDT.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) for all the steps to configure and use the ESP-IDF to build projects.

## Example Output

When the example runs normally, the following output will be observed:

```
I (316) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
TWDT initialized
Subscribed to TWDT
Delay for 10 seconds
Unsubscribed from TWDT
TWDT deinitialized
Example complete
```

Users can comment out any of the `esp_task_wdt_reset()` or `esp_task_wdt_reset_user()` calls to trigger the TWDT, which in turn will result in the following output:

```
I (316) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
TWDT initialized
Subscribed to TWDT
Delay for 10 seconds
E (6326) task_wdt: Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:
E (6326) task_wdt:  - task (CPU 0)
E (6326) task_wdt: Tasks currently running:
E (6326) task_wdt: CPU 0: IDLE
E (6326) task_wdt: CPU 1: IDLE
E (6326) task_wdt: Print CPU 0 (current core) backtrace
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
