# Task Watchdog Example

This test code shows how to initialize the task watchdog, add tasks to the
watchdog task list, feeding the tasks, deleting tasks from the watchdog task
list, and deinitializing the task watchdog.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32-S or ESP32-C series chip
* A USB cable for Power supply and programming

### Configure the project

Program should run without error. Comment out `esp_task_wdt_reset()` to observe a watchdog timeout.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) for all the steps to configure and use the ESP-IDF to build projects.

## Example Output

As you run the example, you will see the following log:

With `esp_task_wdt_reset()`:

```
I (0) cpu_start: Starting scheduler on APP CPU.
Initialize TWDT
Delay for 10 seconds
Unsubscribing and deleting tasks
Complete
```

Without `esp_task_wdt_reset()`:
```
I (0) cpu_start: Starting scheduler on APP CPU.
Initialize TWDT
Delay for 10 seconds
E (6316) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (6316) task_wdt:  - reset task (CPU 0)
E (6316) task_wdt:  - reset task (CPU 1)
E (6316) task_wdt: Tasks currently running:
E (6316) task_wdt: CPU 0: IDLE
E (6316) task_wdt: CPU 1: IDLE
E (6316) task_wdt: Print CPU 0 (current core) backtrace

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
