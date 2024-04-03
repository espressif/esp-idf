| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

Bluetooth Power Save Example
=================================

This example is based on the [bleprph](../bleprph) example to show how to use the bluetooth power save mode.

If the modem sleep mode is enabled, bluetooth will switch periodically between active and sleep.
In sleep state, RF, PHY and BB are turned off in order to reduce power consumption.
For more information about sleep modes, please refer to [Sleep Modes](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html).

This example contains some build configurations. For each configuration, a few configuration options are set:
- `sdkconfig.defaults.esp32`: ESP32 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.defaults.esp32c3`: ESP32C3 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.40m.esp32c3`: ESP32C3 uses main XTAL as low power clock in light sleep enabled.
- `sdkconfig.defaults.esp32c6`: ESP32C6 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.40m.esp32c6`: ESP32C6 uses main XTAL as low power clock in light sleep enabled.
- `sdkconfig.defaults.esp32s3`: ESP32S3 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.40m.esp32s3`: ESP32S3 uses main XTAL as low power clock in light sleep enabled.
- `sdkconfig.defaults.esp32h2`: ESP32H2 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.32m.esp32h2`: ESP32H2 uses main XTAL as low power clock in light sleep enabled.
- `sdkconfig.defaults.esp32c2`: ESP32C2 uses 32kHz XTAL as low power clock in light sleep enabled.
- `sdkconfig.26m.esp32c2`: ESP32C2 uses main XTAL as low power clock in light sleep enabled.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32/ESP32-C3/ESP32-S3/ESP32-H2/ESP32-C6 development board.

### Configure the project

```
idf.py menuconfig
```

1. Configure RTC clock source:
   - `Component config > Hardware Settings > RTC Clock Config > RTC clock source`
2. Enable power management:
   - `Component config > Power Management > [*] Support for power management`
3. Configure FreeRTOS:
   - `Component config > FreeRTOS > Kernel`
     -  `(1000) configTICK_RATE_HZ`
     -  `[*] configUSE_TICKLESS_IDLE`
     -  `(3)     configEXPECTED_IDLE_TIME_BEFORE_SLEEP`

#### For Chip ESP32/ESP32-C3/ESP32-S3

4. Enable power down MAC and baseband:
   - `Component config > PHY > [*] Power down MAC and baseband of Wi-Fi and Bluetooth when PHY is disabled`
5. Enable bluetooth modem sleep:
   - `Component config > Bluetooth > Controller Options > MODEM SLEEP Options`
     - `[*] Bluetooth modem sleep`
     - `[*]     Bluetooth Modem sleep Mode 1`
6. Configure bluetooth low power clock:
   - `Component config > Bluetooth > Controller Options > MODEM SLEEP Options > Bluetooth modem sleep > Bluetooth Modem sleep Mode 1 > Bluetooth low power clock`
7. Enable power up main XTAL during light sleep:
   - `Component config > Bluetooth > Controller Options > MODEM SLEEP Options > [*] power up main XTAL during light sleep`

#### For Chip ESP32-C6/ESP32-H2/ESP32-C2

4. Enable bluetooth modem sleep:
   - `Component config > Bluetooth > Controller Options`
     - `[*] Enable BLE sleep`
5. Configure bluetooth low power clock:
   - `Component config > Bluetooth > Controller Options > BLE low power clock source`
   - Use RTC clock source as low power clock source during light sleep:
     - `(X) Use system RTC slow clock source`
6. Power down flash during light sleep:
   - `Component config > Hardware Settings > Sleep Config`
     - `[*] Power down flash in light sleep when there is no SPIRAM`

### Build and Flash

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.40m.esp32c3" set-target ESP32C3 build
```

* `-D SDKCONFIG_DEFAULTS` select configuration file to be used for creating app sdkconfig.
* `set-target ` Set the chip target to build.


To flash the project and see the output, run:


```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

When you run this example, the prints the following at the very beginning:

```
I (333) cpu_start: Starting scheduler.
I (347) pm: Frequency switching config: CPU_MAX: 160, APB_MAX: 80, APB_MIN: 40, Light sleep: ENABLED
I (351) sleep: Enable automatic switching of GPIO sleep configuration
I (358) BTDM_INIT: BT controller compile version [f2e5d81]
I (365) BTDM_INIT: Bluetooth will use main XTAL as Bluetooth sleep clock.
I (372) phy_init: phy_version 912,d001756,Jun  2 2022,16:28:07
I (411) system_api: Base MAC address is not set
I (412) system_api: read default base MAC address from EFUSE
I (412) BTDM_INIT: Bluetooth MAC: 7c:df:a1:61:b6:f6

I (419) NimBLE_BLE_PRPH: BLE Host Task Started
I (424) uart: queue free spaces: 8
I (432) NimBLE: GAP procedure initiated: stop advertising.

I (435) NimBLE: Device Address:
I (437) NimBLE: 7c:df:a1:61:b6:f6
I (441) NimBLE:

I (446) NimBLE: GAP procedure initiated: advertise;
I (450) NimBLE: disc_mode=2
I (453) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (463) NimBLE:
```

## Typical current consumption with management enabled

|         | max current | modem sleep | light sleep (main XTAL) | light sleep (32KHz XTAL) |
| ------- | ----------- | ----------- | ----------------------- | ------------------------ |
| ESP32   | 231 mA      | 14.1 mA     | X                       | 1.9 mA                   |
| ESP32C3 | 262 mA      | 12 mA       | 2.3 mA                  | 140 uA                   |
| ESP32S3 | 240 mA      | 17.9 mA     | 3.3 mA                  | 230 uA                   |
| ESP32C6 | 240 mA      | 22 mA       | 3.3 mA                  | 34  uA                   |
| ESP32H2 | 82 mA       | 16.0 mA     | 4.0 mA                  | 24 uA                    |
| ESP32C2 | 130 mA      | 18.0 mA     | 2.5 mA                  | 169 uA                   |

X: This feature is currently not supported.

## Example Breakdown

- ESP32 does not support the use of main XTAL in light sleep mode, so an external 32kHz crystal is required.
- ESP32C2 support XTAL frequency of 26MHz and 40MHz, the XTAL frequency is set to 26MHz in default.
- ESP32C2 support external 32kHz crystal by connecting the crystal to the chip through pin0