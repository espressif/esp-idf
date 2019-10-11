# High Resolution Timer Example (`esp_timer`) 

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The [High Resolution Timer (`esp_timer`)](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html) APIs allow an application to create multiple timers using a single hardware timer, and hides complexity associated with managing multiple timers, invoking callbacks, accounting for APB frequency changes (if dynamic frequency scaling is enabled), and maintaining correct time after light sleep.

This example illustrates the usage of the [`esp_timer` API](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html#api-reference) to create one-shot and periodic software timers.

The `esp_timer` API also provides the `esp_timer_get_time()` function which returns the time since boot in microseconds. This can be useful for fine-grained timing in tasks and ISRs thus is also demonstrated in this example.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

Under `Component config > Common ESP-related` are the following `esp_timer` related configurations

* `High-resolution timer task stack size` can be increased if timer callbacks require a larger stack
* `Enable esp_timer profiling features` will cause `esp_timer_dump()` to include more information.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example should have the following log output:

```
...
I (294) example: Started timers, time since boot: 9662 us
periodic            500000        509644          1          0             0
one-shot                 0       5009654          1          0             0
I (794) example: Periodic timer called, time since boot: 509694 us
I (1294) example: Periodic timer called, time since boot: 1009671 us
I (1794) example: Periodic timer called, time since boot: 1509671 us
I (2294) example: Periodic timer called, time since boot: 2009671 us
periodic            500000       2509644          1          4           542
one-shot                 0       5009654          1          0             0
I (2794) example: Periodic timer called, time since boot: 2509671 us
I (3294) example: Periodic timer called, time since boot: 3009671 us
I (3794) example: Periodic timer called, time since boot: 3509671 us
I (4294) example: Periodic timer called, time since boot: 4009671 us
periodic            500000       4509644          1          8          1026
one-shot                 0       5009654          1          0             0
I (4794) example: Periodic timer called, time since boot: 4509671 us
I (5294) example: Periodic timer called, time since boot: 5009669 us
I (5294) example: One-shot timer called, time since boot: 5009788 us
I (5294) example: Restarted periodic timer with 1s period, time since boot: 5012675 us
I (6294) example: Periodic timer called, time since boot: 6012692 us
periodic           1000000       7012666          2         11          1391
one-shot                 0             0          1          1         11472
I (7294) example: Periodic timer called, time since boot: 7012692 us
I (8294) example: Periodic timer called, time since boot: 8012692 us
periodic           1000000       9012666          2         13          1639
one-shot                 0             0          1          1         11472
I (9294) example: Periodic timer called, time since boot: 9012692 us
I (10294) example: Periodic timer called, time since boot: 10012692 us
I (10314) example: Entering light sleep for 0.5s, time since boot: 10024351 us
I (10314) example: Woke up from light sleep, time since boot: 10525143 us
...
```

## Example Breakdown

### 1. Creating and starting timers

The example starts by creating a periodic and a one shot timer using the `esp_timer_create()` function. Once created, the two timers are started using the `esp_timer_start_periodic()` and `esp_timer_start_once()` functions.

```
I (265) example: Starting timers, time since boot: 2479 us
```

### 2. Getting initial timer dump

These two repeating lines are the output of `esp_timer_dump()` function. There is one line for each of the timers created. This function can be useful for debugging purposes. Note that such debugging information is available because the example sets `CONFIG_ESP_TIMER_PROFILING` option in sdkconfig. Without this option, less information will be available. See documentation of `esp_timer_dump()` in ESP-IDF programming guide for more details.

```
timer               period     next time      times        times         callback
name                            to fire      started       fired       run time (us)
------------------------------------------------------------------------------------

periodic            500000        502455          1          0             0
one-shot                 0       5002469          1          0             0
```

### 3. Periodic timer keeps running at 500ms period:

```
I (765) example: Periodic timer called, time since boot: 502506 us
I (1265) example: Periodic timer called, time since boot: 1002478 us
I (1765) example: Periodic timer called, time since boot: 1502478 us
I (2265) example: Periodic timer called, time since boot: 2002478 us
periodic            500000       2502455          1          4           511
one-shot                 0       5002469          1          0             0
I (2765) example: Periodic timer called, time since boot: 2502478 us
I (3265) example: Periodic timer called, time since boot: 3002478 us
I (3765) example: Periodic timer called, time since boot: 3502478 us
I (4265) example: Periodic timer called, time since boot: 4002478 us
periodic            500000       4502455          1          8           971
one-shot                 0       5002469          1          0             0
I (4765) example: Periodic timer called, time since boot: 4502478 us
I (5265) example: Periodic timer called, time since boot: 5002476 us
```

### 4. One-shot timer runs

The one-shot timer runs and changes the period of the periodic timer. Now the periodic timer runs with a period of 1 second:

```    
I (5265) example: One-shot timer called, time since boot: 5002586 us
I (5265) example: Restarted periodic timer with 1s period, time since boot: 5005475 us
I (6265) example: Periodic timer called, time since boot: 6005492 us
periodic           1000000       7005469          2         11          1316
one-shot                 0             0          1          1         11474
I (7265) example: Periodic timer called, time since boot: 7005492 us
I (8265) example: Periodic timer called, time since boot: 8005492 us
periodic           1000000       9005469          2         13          1550
one-shot                 0             0          1          1         11474
I (9265) example: Periodic timer called, time since boot: 9005492 us
I (10265) example: Periodic timer called, time since boot: 10005492 us
```

### 5. Continuation through light sleep

To illustrate that timekeeping continues correctly after light sleep, the example enters light sleep for 0.5 seconds. This sleep does not impact timer period, and the timer is executed 1 second after the previous iteration. Note that the timers can not execute during light sleep, since the CPU is not running at that time. Such timers would execute immediately after light sleep, and then continue running with their normal period.
    
```
I (10275) example: Entering light sleep for 0.5s, time since boot: 10011559 us
I (10275) example: Woke up from light sleep, time since boot: 10512007 us
I (10765) example: Periodic timer called, time since boot: 11005492 us
I (11765) example: Periodic timer called, time since boot: 12005492 us
```

### 6. Finally, timers are deleted.

```
I (12275) example: Stopped and deleted timers
```