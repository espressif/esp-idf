# Low Power Management Example => Automatic light and deep sleep implementation

This example illustrates usage of automatic light and deep sleep mode.

It uses the automatic light and deep sleep functions implemented in pm_esp32.c file. Depending on the type of lock acquired or released, it switches between different modes: Active mode, Light sleep and Deep sleep. The light sleep and deep sleep functions are called in the Idle task.

The wakeup source is the timer defined in the pm_esp32 file. The example also prints time spent in light or deep sleep mode to illustrate that timekeeping continues while the chip is in light sleep.

## How to use example

### Hardware Required

This example can run on any commonly available ESP32 development board.

### Configure the project

Run `make menuconfig` and set serial port under Serial Flasher Options.

It's important to note that CONFIG_PM_ENABLE and CONFIG_FREERTOS_USE_TICKLESS_IDLE should be set to 1 in order to enable automatic light and deep sleep.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (314) POWER_SAVE: Entering different power saving modes

I (324) POWER_SAVE: Wake up from deep sleep due to timer. Time spent in deep sleep: 5022ms

I (334) POWER_SAVE: Entering ACTIVE MODE : all the features of the chip are active.

I (5344) POWER_SAVE: Entering LIGHT SLEEP
I (10354) POWER_SAVE: Returned from light sleep due to timer. Time spent in deep sleep 5000 ms

I (10354) POWER_SAVE: Entering DEEP SLEEP

```
