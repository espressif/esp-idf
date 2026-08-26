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
    - Optional main power (ESP_3V3) control via GPIO when in deep sleep on VBAT: turn off main power after switching to VBAT, and turn it on again in advance before wakeup (configurable advance time).

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

#### Main power control when running on VBAT (ESP32-P4)

When the chip is in deep sleep and powered by VBAT, the main power rail (ESP_3V3) may be turned off to reduce power consumption (for example, when it also supplies other peripherals on the board). ESP32-P4 supports controlling the main power enable via an RTC GPIO so that the rail is disabled after switching to VBAT and re-enabled in advance before wakeup, allowing the supply to stabilize before the chip exits deep sleep and switches back to main supply.

Application scenario — circuit connections:

```
  ┌──────────────┐       ┌───────────────────────────────────┐
  │    Button    │ VBAT  │            ESP32-P4               │
  │   Battery    ├──────>│ VBAT                              │
  └──────────────┘       │                                   │
                         │ ESP_3V3                   (RTC IO)│
                         └───────────────────────────────┬───┘
                             ^                           │ Enable (power ctrl)
                 Vout (3.3V) │                           v
                       ┌─────┴───────────────────────────────┐
                       │    Main Power            Enable Pin │
                       └─────────────────────────────────────┘
                                   │ 3.3V
                                   v
                         ┌───────────────────┐
                         │  Other Circuits   │
                         └───────────────────┘
```

- **Button Battery** → ESP32-P4 **VBAT**: backup supply during deep sleep, keeps RTC alive.
- **Power Module** Vout → ESP32-P4 **ESP_3V3** and **Other Circuits**: main 3.3 V rail.
- ESP32-P4 **GPIO (RTC IO 0~3)** → Power Module **Enable**: the chip disables main power after entering deep sleep on VBAT, and re-enables it in advance before wakeup.

Configuration (under `Component config → Hardware Settings → Power Supplier → RTC Backup Battery`):

- Enable the feature: **Control main power (ESP_3V3) enable via GPIO when VBAT powers deep sleep**.
- Select the control pin: **GPIO for main power (ESP_3V3) enable** — RTC GPIO 0~3 (or 2~3 only when the RTC clock source is the external 32 kHz crystal, because GPIO0/1 are then reserved).
- Set output polarity: **Invert main power control pin output** — enable this if the main power is enabled by a low level (active-low).
- Set the wakeup advance time: **Main power enable advance time before wakeup (us)** — the time in microseconds by which the main power enable is asserted before the chip starts the wakeup sequence. This value must be at least the main power supply’s startup/settling time (from enable assertion to stable output); otherwise the chip may brown out or boot unstably after wake.

This feature requires a hardware design in which the main power enable is driven by the chosen RTC GPIO (for example, via an external FET or the enable pin of a regulator).

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
