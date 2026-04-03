| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Power Management Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates power management (PM) locks with multiple tasks: one task holds `ESP_PM_CPU_FREQ_MAX` during a short critical section, another holds `ESP_PM_NO_LIGHT_SLEEP` during a short awake window, and a third task periodically dumps PM config and lock/mode stats. Locks are used only where needed so the system can lower CPU frequency or enter light sleep the rest of the time.

## How to Use Example

### Hardware Required

Any supported ESP development board.

### Configure the Project

Run `idf.py menuconfig`. Under **Example Configuration** you can set:

- **Max CPU frequency (MHz)** — CPU frequency when ESP_PM_CPU_FREQ_MAX PM lock is held.
- **Min CPU frequency (MHz)** — CPU frequency when no PM lock is held.
- **Enable automatic light sleep when idle** — When enabled (and tickless idle is enabled), the chip may enter light sleep when no PM lock is held. Default is on if tickless idle is enabled.

Enable PM under **Component config → Power Management → Support for power management**. The example uses `sdkconfig.defaults` to set `CONFIG_PM_ENABLE` and `CONFIG_FREERTOS_USE_TICKLESS_IDLE`. PM is not supported with SMP FreeRTOS; the example sets `CONFIG_FREERTOS_SMP=n`.

### Build and Flash

Build the project, flash it to the board, and run the serial monitor:

```
idf.py -p PORT flash monitor
```

(Replace PORT with your serial port. Exit monitor with `Ctrl-]`.)

## Example Output

At startup the example prints the PM config. The **pm_dump** task then prints PM config and lock/mode stats every 10 seconds; the two demo tasks do not log.

```
=== Power Management Example (multi-task) ===
I (319) pm: Frequency switching config: CPU_MAX: 400, APB_MAX: 100, APB_MIN: 40, Light sleep: ENABLED
PM configured: max=400 min=40 MHz, light_sleep_enable=1
I (330) main_task: Returned from app_main()

--- PM: max_freq=400 min_freq=40 MHz, light_sleep=1 ---
Time since boot up: 10044534 us
Lock stats:
Name            Type            Arg    Active    Total_count    Time(us)        Time(%) 
background      NO_LIGHT_SLEEP  0      0         20             2998011         30 %
critical_work   CPU_FREQ_MAX    0      1         21             1000507         10 %
rtos1           CPU_FREQ_MAX    0      1         4087           141343          2  %
rtos0           CPU_FREQ_MAX    0      0         4076           184970          2  %

Mode stats:
Mode      CPU_freq    Time(us)    Time(%)   
SLEEP     40 M        5958343     59%
APB_MIN   40 M        2848902     28%
APB_MAX   100M        0           0 %
CPU_MAX   400M        1205696     11%

Sleep stats:
light_sleep_counts:40  light_sleep_reject_counts:0
```

The same block repeats every 10 seconds.
