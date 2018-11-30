# High resolution timer (`esp_timer`) example

This example illustrates usage of `esp_timer` APIs to create one-shot and periodic software timers.

`esp_timer` APIs allow application to create multiple timers using single hardware timer, and hides complexity associated with managing multiple timers, invoking callbacks, accounting for APB frequency changes (if dynamic frequency scaling is enabled), and maintaining correct time after light sleep.

`esp_timer` API also provides `esp_timer_get_time` function which returns time since boot, in microseconds. This can be useful for fine-grained timing in tasks and ISRs.


## Example flow explained

1. Example starts and timers are created.

    ```
    I (265) example: Starting timers, time since boot: 2479 us
    ```

2. These two repeating lines are the output of `esp_timer_dump` function. There is one line for each of the timers created. This function can be useful for debugging purposes. Note that such debugging information is available because the example sets `CONFIG_ESP_TIMER_PROFILING` option in sdkconfig. Without this option, less information will be available. See documentation of `esp_timer_dump` in ESP-IDF programming guide for more details.

    ```
    timer               period     next time      times        times         callback
    name                            to fire      started       fired       run time (us)
    ------------------------------------------------------------------------------------
    
    periodic            500000        502455          1          0             0
    one-shot                 0       5002469          1          0             0
    ```

3. Periodic timer keeps running at 500ms period:

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
4. One-shot timer runs, and changes the period of the periodic timer. Now the periodic timer runs with 1 second period:

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
5. To illustrate that timekeeping continues correctly after light sleep, the example enters light sleep for 0.5s. This sleep does not impact timer period, and the timer is executed 1 second after the previous iteration. Note that the timers can not execute during light sleep, since the CPU is not running at that time. Such timers would execute immediately after light sleep, and then continue running with their normal period.
    
    ```
    I (10275) example: Entering light sleep for 0.5s, time since boot: 10011559 us
    I (10275) example: Woke up from light sleep, time since boot: 10512007 us
    I (10765) example: Periodic timer called, time since boot: 11005492 us
    I (11765) example: Periodic timer called, time since boot: 12005492 us
    ```

6. Finally, timers are deleted.

    ```
    I (12275) example: Stopped and deleted timers
    ```

---

See the README.md file in the upper level 'examples' directory for more information about examples.
