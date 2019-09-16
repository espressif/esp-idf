# Deep Sleep Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The [deep sleep mode](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#sleep-modes) of the ESP32 is a power saving mode that causes the CPU, majority of RAM, and digital peripherals that are clocked from APB_CLK to be powered off. Deep sleep mode can be exited using one of multiple [wake up sources](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#wakeup-sources). This example demonstrates how to use the [`esp_sleep.h`](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#api-reference) API to enter deep sleep mode, then wake up form different sources.

The following wake up sources are demonstrated in this example (refer to the [Wakeup Sources documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#wakeup-sources) for more details regarding wake up sources):

1. **Timer:** An RTC timer that can be programmed to trigger a wake up after a preset time. This example will trigger a wake up every 20 seconds.
2. **EXT1:** External wake up 1 which is tied to multiple RTC GPIOs. This example use GPIO25 and GPIO26 to trigger a wake up with any one of the two pins are HIGH.
3. **Touch:** Touch pad sensor interrupt. This example uses touch pads connected to GPIO32 and GPIO33 to trigger a wake up when any of the pads are pressed.
4. **ULP:** Ultra Low Power Coprocessor which can continue to run during deep sleep. This example utilizes the ULP and constantly sample the chip's temperature and trigger a wake up  if the chips temperature exceeds ~5 degrees Celsius.

Note: Some wake up sources can be disabled via configuration (see section on [project configuration](#Configure-the-project))

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board without any extra hardware if only **Timer** and **ULP** wake up sources are used. However, the following extra connections will be required for the remaining wake up sources.

- **EXT1:** GPIO25 and GPIO26 should be connected to LOW to avoid floating pins. When triggering a wake up, connect one or both of then pins to HIGH. Note that floating pins may trigger an wake up.

- **Touch:** GPIO32 and GPIO33 should be connected to touch sensors (see [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_en.md)).

### Configure the project

```
idf.py menuconfig
```

* **Touch wake up** can be enabled/disabled via `Example configuration > Enable touch wake up`
* **ULT wake up** can be enabled/disabled via `Example configuration > Enable temperature monitoring by ULP`

Wake up sources that are unused or unconnected should be disabled in configuration to prevent inadvertent triggering of wake up as a result of floating pins.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

On initial startup, this example will detect that this is the first boot and output the following low:

```
...
I (304) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Not a deep sleep reset
Enabling timer wakeup, 20s
Enabling EXT1 wakeup on pins GPIO25, GPIO26
Touch pad #8 average: 2148, wakeup threshold set to 2048.
Touch pad #9 average: 2148, wakeup threshold set to 2048.
Enabling touch pad wakeup
Enabling ULP wakeup
Entering deep sleep
```

The ESP32 will then enter deep sleep. When a wake up occurs, the ESP32 must undergo the entire boot process again. However the example will detect that this boot is due to a wake up and indicate the wake up source in the output log such as the following:

```
...
I (304) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Wake up from timer. Time spent in deep sleep: 20313ms
ULP did 110 temperature measurements in 20313 ms
Initial T=87, latest T=87
Enabling timer wakeup, 20s
Enabling EXT1 wakeup on pins GPIO25, GPIO26
Touch pad #8 average: 2149, wakeup threshold set to 2049.
Touch pad #9 average: 2146, wakeup threshold set to 2046.
Enabling touch pad wakeup
Enabling ULP wakeup
Entering deep sleep
```
