| Supported Targets | ESP32-H2 | ESP32-P4 |
| ----------------- | -------- | -------- |

# VBAT Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

When the application is idle and the chip enters deepsleep, you may want to turn off the main power to save power (because the main power usually also drives other power-consuming devices on the board), but you also want the ESP chip to maintain the RTC timing and wake-up functions. Or you want to enter deepsleep mode that only maintains RTC timing but with another power supply when the main power is about to undervoltage. In these cases, you can use this feature, which allows the chip to switch to backup battery power when entering deepsleep.

This example demonstrates the ESP backup battery power supply solution, which has the following features:

    - Supports using backup power (VBAT) during deepsleep, and RTC Timer can keep timing after the main power is lost.
    - Supports automatic power switching by PMU during sleep/wake-up, automatically switches to backup power supply when entering sleep mode and switches to main power supply when waking up.
    - Support battery voltage detection, wake up the chip and switch to the main power supply when undervoltage occurs.
    - Support battery charging, automatically stop charging when threshold voltage is reached, and charging current can be configured.
    - Supports selection of chip status when charging the battery, options include keep active, entering lightsleep or entering deepsleep.

## How to use example

### Hardware Required

This example should be run on a development board with a backup battery. Currently the following development boards are supported:
    - [ESP32-P4-Function-EV-Board Rev](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/index.html#esp32-p4-function-ev-board)

```
                    0(%1)NC   ┌──────────┐
       BAT ─────^^^^─────┐    │          │
                         │    │          │  
                         │    │ ESP32-P4 │  
                         │    │          │  
   ESP_3V3 ─────^^^^─────┴────┤VBAT      │
                    0(%1)     └──────────┘
```

By default, the ESP_VBAT power supply on this development board is shorted to ESP_3V3 through a 0 Ω resistor.

**You need to re-solder the resistor 0 Ω on ESP_3V3 to the empty pad on BAT. (You can find the resistor position on the schematic doc of the development board). Then connect `RTC_Battery +` to the positive terminal of the battery and `RTC_Battery -` to the negative terminal of the battery.**

### Configure the project

```
idf.py menuconfig
```

- If you are using a non-rechargeable battery, VBAT brownout wakeup can be enabled via `Component config → Hardware Settings → Power Supplier → RTC Backup Battery`.
- If you are using a rechargeable battery, the automatic wake-up charging feature can be enabled via `Component config → Hardware Settings → Power Supplier → RTC Backup Battery -> The battery for RTC battery is a rechargeable battery`.
- The charging current limiting resistor can be configured via `Component config → Hardware Settings → Power Supplier → RTC Backup Battery -> vbat charger circuit resistor value (ohms)`.
- The chip state while waiting for battery charging the battery can be selected by `Example Configuration → Configure the chip state while waiting for battery charging`.
- The period to check whether the battery has been charged done can be selected by `Battery charging done check period (in seconds)`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

On initial startup, this example will detect that this is the first boot and output the following log:

```
...
I (271) main_task: Started on CPU0
I (281) main_task: Calling app_main()
Not a deep sleep reset
Current RTC Time: 110106 (us)
Entering deep sleep
```

Then the chip will then enter deep sleep, and switch to VBAT power supply.

If non-rechargeable battery is used, nothing will happens when VBAT is undervoltage by default, but if `CONFIG_ESP_VBAT_WAKEUP_CHIP_ON_VBAT_BROWNOUT` is enabled, when the battery voltage drops to the configured brownout threshold `ESP_VBAT_BROWNOUT_DET_LVL_SEL`, the chip will wake up and go to sleep again, but will not switch to VBAT power during deepsleep. 

```
W VBAT: RTC battery voltage low, please change battery...
Battery is low, VBAT power will not be used during deep sleep!
Current RTC Time: 18493666 (us)
Entering deep sleep
```

If rechargeable battery is used, when the battery voltage drops to the configured charging threshold (`CONFIG_ESP_VBAT_DET_LVL_LOW_SEL`), the chip will wake up and start charge the battery, when the battery voltage rises to the configured stop charging threshold (`CONFIG_ESP_VBAT_DET_LVL_HIGH_SEL`), the chip will stop charging the battery and re-enter deepsleep, and so on. The following log will be output:

```
...
W VBAT: RTC battery voltage low, start charging...
Wake up from Low power VBAT 
Battery is low, waiting for charging to complete before going to sleep!
W VBAT: RTC battery voltage reaches high limit , stop charging...
Current RTC Time: 20753113 (us)
Entering deep sleep
...
```
