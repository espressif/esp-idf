| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |
# Example: Application Level Tracing - SystemView Tracing (sysview_tracing)

This test code shows how to perform system-wide behavioral analysis of the program using [SEGGER SystemView tool](https://www.segger.com/products/development-tools/systemview/).

For description of [SystemView tracing](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#system-behaviour-analysis-with-segger-systemview) please refer to **ESP32 Programming Guide**, section **Application Level Tracing library**. The following example provides practical implementation of this functionality.


## Use Case

To find the reason of program's misbehaviour it is often necessary to have the whole picture of interacting components (scheduler, IRQs, tasks, semaphores etc.) in the system. In such cases tools which allow to trace the behaviour of the system as a whole can be very useful.
Consider the following situation. User program have a timer and a task. Upon every tick the timer sends an event to the task. When task receives event it prints a message. Timer should notify the task 10 times.
There can be a problem which causes the task to lose some events.
Below is the timer's ISR code:

```
static void example_timer_isr(void *arg)
{
    example_event_data_t *tim_arg = (example_event_data_t *)arg;

    if (tim_arg->thnd != NULL) {
        if (tim_arg->count++ < 10) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xTaskNotifyFromISR(tim_arg->thnd, tim_arg->count, eSetValueWithOverwrite, &xHigherPriorityTaskWoken) != pdPASS) {
                ESP_EARLY_LOGE(TAG, "Failed to notify task %p", tim_arg->thnd);
            } else {
                if (xHigherPriorityTaskWoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            }
        }
    }
    // re-start timer
    example_timer_rearm(tim_arg->group, tim_arg->timer);
}
```

Below is the task's code:

```
static void example_task(void *p)
{
    example_event_data_t *arg = (example_event_data_t *) p;
    timer_isr_handle_t inth;

    ESP_LOGI(TAG, "%p: run task", xTaskGetCurrentTaskHandle());

    esp_err_t res = timer_isr_register(arg->group, arg->timer, example_timer_isr, arg, 0, &inth);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "%p: failed to register timer ISR", xTaskGetCurrentTaskHandle());
    }
    else {
        res = timer_start(arg->group, arg->timer);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "%p: failed to start timer", xTaskGetCurrentTaskHandle());
        }
    }

    while (1) {
        uint32_t event_val;
        xTaskNotifyWait(0, 0, &event_val, portMAX_DELAY);
        ESP_LOGI(TAG, "Task[%p]: received event %d", xTaskGetCurrentTaskHandle(), event_val);
    }
}
```

Potential problem can arise in such program because task and timer has no any mechanism to acknowledge the events transfer. Task needs some time to process an event before waiting for the next one. In this case call to `ESP_LOGI` will be the most time consuming part of event processing. Therefore when timer's ISR is called at some rate it can happen that `xTaskNotifyFromISR` gets called several times before task calls `xTaskNotifyWait`. In these conditions some events will be lost. Possible solution for this is to increase timer's tick period or to use some events acknowledgement mechanism.

Check the full example code [sysview_tracing](main/sysview_tracing.c) that when compiled in dual core mode reproduces the described problem on both cores. Below is the output of example compiled in single core mode. It shows that task misses several events:

```
I (295) example: 0x3ffb6c10: run task
I (297) example: Task[0x3ffb6c10]: received event 1
I (300) example: Task[0x3ffb6c10]: received event 2
I (306) example: Task[0x3ffb6c10]: received event 5
I (311) example: Task[0x3ffb6c10]: received event 8
I (317) example: Task[0x3ffb6c10]: received event 10
```


## Detecting The Reason Of The Problem

Besides built-in functionality to trace FreeRTOS internal operations SystemView also provides user with means to define its own trace messages and transfer problem-specific data to the host. In this example we extend SystemView with user-defined tracing module in order to
make the root cause of the problem more clear in the gathered trace.

There are two ways to send user-defined info to SystemView:
1. Using built-in SystemView messages. This method uses `SEGGER_SYSVIEW_OnUserStart` and `SEGGER_SYSVIEW_OnUserStop` API to indicate that some user event started or stopped. Disadvantge of this way is that those API do not carry any other data except for user event ID. Advantage is that you do not need to write any additional code to use these functions. In SystemView messages sent by means of that API will be shown as `User Start` or `User Stop` with user event ID as only parameter: 0 - for timer and 1 - for task in this example.
2. Using custom SystemView messages. This is more flexible way. It implies extending SystemView with user module having its own set of events with any number of parameters (data) in each. For more details on extending see [code of this example](main/sysview_tracing.c) and [SystemView User Manual](https://www.segger.com/downloads/jlink/UM08027).

To run the example and find out the reason of the problem:

1.  Connect JTAG interface to ESP32 board, power up both JTAG and ESP32. For details how to setup JTAG interface see [JTAG Debugging](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html).

2.  [Run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd). If you are using the [binary distribution of OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#jtag-debugging-setup-openocd) and one of versions of [ESP-WROVER-KIT](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html#esp-wrover-kit-v3), respective command line will look as follows:

    ```
    cd ~/esp/openocd-esp32
    bin/openocd -s share/openocd/scripts -f board/esp32-wrover-kit-3.3v.cfg
    ```
NOTE: In order to run this example you need OpenOCD version `v0.10.0-esp32-20181105` or later.

3.  Compile and load the example. Note to enable application tracing in menuconfig by going to `Component config > Application Level Tracing` and selecting `(X) Trace memory`. Also in order to generate SystemView trace data you need to enable this in `Component config > Application Level Tracing > FreeRTOS SystemView Tracing` by checking `(X) SystemView Tracing Enable`. Also ensure that all SystemView events are enabled there.

4.  If you are going to use custom events enable them by checking `Example Configuration > Use custom SystemView event IDs`. By default SystemView shows only numeric values of IDs and parameters for custom messages in `Events` view. To make them pretty-looking you need to define for them parsing templates in `SYSVIEW_FreeRTOS.txt` which is resided in SystemView installation directory. For this example you can use `SYSVIEW_FreeRTOS.txt` from the project's root directory.

5.  It is useful to use GDB to start and/or stop tracing automatically. To do this you need to prepare special `gdbinit` file:

    ```
    target remote :3333
    mon reset halt
    b app_main
    commands
    mon esp sysview start file:///tmp/sysview_example.svdat
    # For dual-core mode uncomment the line below and comment the line above
    # mon esp sysview start file:///tmp/sysview_example0.svdat file:///tmp/sysview_example1.svdat
    c
    end
    c
    ```

    Using this file GDB will connect to the target, reset it, and start tracing when it hit breakpoint at `app_main`. Trace data will be saved to `/tmp/sysview_example.svdat`.

6.  Run GDB using the following command from the project root directory:

    ```
    xtensa-esp32-elf-gdb -x gdbinit build/sysview_tracing.elf
    ```

    **Note:** Replace `xtensa-esp32-elf-gdb` with the related gdb tool (e.g. `xtensa-esp32s2-elf-gdb`, `xtensa-esp32s3-elf-gdb` or `riscv32-esp-elf-gdb`) if running the example on different chip.

7.  When program prints the last message, interrupt its execution (e.g. by pressing `CTRL+C`) and type the following command in GDB console to stop tracing:

    ```
    mon esp sysview stop
    ```

    You can also use another breakpoint to stop tracing and add respective lines to `gdbinit`  at step 5.

8.  Open trace data file in SystemView tool.

9.  Right-click on any event in `Events` view and select:

    - `Show User Events only` if you compiled example to use only built-in event.
    - `Show APIs only` if you compiled example to use custom events.

10.  Now you can navigate over user-defined messages and see when timer `TG1_T0_LEVEL` sends events (`SYSVIEW_EXAMPLE_SEND_EVENT_START/STOP` or `User Start/Stop(0)`) and when task `svtrace0` reads them (`SYSVIEW_EXAMPLE_WAIT_EVENT_START/STOP` or `User Start/Stop(1)`).
    If you compiled example to use custom events you will also be able to see the values sent by timer and ones actually received by tasks.
