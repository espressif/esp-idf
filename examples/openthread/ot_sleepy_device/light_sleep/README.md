| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |
# OpenThread Sleepy Device Example

The example demonstrates the Thread Sleepy End Device (SED), the device will enter [Light Sleep mode](https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/api-reference/system/sleep_modes.html#sleep-modes) during idle state.
## How to use example

### Hardware Required
* One 802.15.4 SoC (for example ESP32-H2) runs [ot_cli](../../ot_cli/) example, and forms a Thread network.
* A second 802.15.4 SoC runs this example.

### Configure the project

Set the chip target: `idf.py set-target <chip_name>`, then configure the project via `idf.py menuconfig`.

There are two options to configure Openthread Dataset:

* Auto start mode: Enable `OPENTHREAD_NETWORK_AUTO_START` under `OpenThread Sleepy Example---> Enable the automatic start mode`, and configure the dataset under `Component config ---> Openthread ---> Thread Operation Dataset`.
* Manual mode: Disable `OPENTHREAD_NETWORK_AUTO_START`, use the CLI command to configure the dataset and start network.

### Build and Flash

Build the project and flash it to the board. Use the following command: `idf.py -p <PORT> erase-flash flash monitor`.

### Configure the Openthread sleepy device
```
esp32h2> ot mode -
esp32h2> ot pollperiod 3000
esp32h2> ot dataset set active <the same as dataset of the leader>
esp32h2> ot ifconfig up
esp32h2> ot thread start
```

### Example Output

As the example runs, you will see the log output indicating the initialization and operation of OpenThread, including the device joining the OpenThread network as a Sleepy End Device (SED) and periodic polling of the leader.

```
I (486) app_init: ESP-IDF:          v5.3-dev-2053-g4d7e86eeb6-dirty
I (493) app_init: Min chip rev:     v0.0
I (497) app_init: Max chip rev:     v0.99
I (502) app_init: Chip rev:         v0.1
I (507) sleep: Enable automatic switching of GPIO sleep configuration
I (514) sleep_clock: System Power, Clock and Reset sleep retention initialization
I (522) sleep_clock: Modem Power, Clock and Reset sleep retention initialization
I (530) sleep_sys_periph: Interrupt Matrix sleep retention initialization
I (538) sleep_sys_periph: HP System sleep retention initialization
I (545) sleep_sys_periph: TEE/APM sleep retention initialization
I (551) sleep_sys_periph: UART sleep retention initialization
I (558) sleep_sys_periph: Timer Group sleep retention initialization
I (565) sleep_sys_periph: IO Matrix sleep retention initialization
I (572) sleep_sys_periph: SPI Mem sleep retention initialization
I (579) sleep_sys_periph: SysTimer sleep retention initialization
I (597) main_task: Started on CPU0
I (597) main_task: Calling app_main()
I (608) pm: Frequency switching config: CPU_MAX: 96, APB_MAX: 96, APB_MIN: 96, Light sleep: ENABLED
I (609) ot_esp_power_save: Create ot cI (631) phy: phy_version: 230,2, 9aae6ea, Jan 15 2024, 11:17:12
I (633) phy: libbtbb version: 944f18e, Jan 15 2024, 11:17:25
I (634) btbb_init: btbb sleep retention initialization
I (646) ieee802154: ieee802154 mac sleep retention initialization
I (652) gdma: GDMA pair (0, 0) retention initialization
I(660) OPENTHREAD:[I] ChildSupervsn-: Timeout: 0 -> 190
> I (664) OPENTHREAD: OpenThread attached to netif
I (635) main_task: Returned from app_main()
esp32h2> ot mode -

I(2250683) OPENTHREAD:[N] Mle-----------: Mode 0x0f -> 0x04 [rx-on:no ftd:no full-net:no]
Done
esp32h2> ot pollperiod 3000

Done

esp32h2> ot dataset set active 0e080000000000010000000300001a35060004001fffe00208dead00beef00cafe0708fd000db800a00000051000112233445566778899aabbccdd0000030e4f70656e5468726561642d455350010212340410104810e2315100afd6bc9215a6bfac530c0402a0f7f8

Done

esp32h2> ot ifconfig up

Done
I (2274801) OT_STATE: netif up
esp32h2> ot thread start

I(2279917) OPENTHREAD:[N] Mle-----------: Role disabled -> detached
Done
> I(2280368) OPENTHREAD:[N] Mle-----------: Attach attempt 1, AnyPartition reattaching with Active Dataset
I(2281262) OPENTHREAD:[N] Mle-----------: RLOC16 fffe -> 5019
I(2281264) OPENTHREAD:[N] Mle-----------: Role detached -> child

```

When the device is running in auto start mode, the running log is as follows:

```
I(662) OPENTHREAD:[I] ChildSupervsn-: Timeout: 0 -> 190
> I (666) OPENTHREAD: OpenThread attached to netif
I(668) OPENTHREAD:[N] Mle-----------: Mode 0x0f -> 0x04 [rx-on:no ftd:no full-net:no]
I (637) main_task: Returned from app_main()
I(693) OPENTHREAD:[N] Mle-----------: Role disabled -> detached
I (705) OT_STATE: netif up
I(867) OPENTHREAD:[N] Mle-----------: Attach attempt 1, AnyPartition reattaching with Active Dataset
I(1819) OPENTHREAD:[N] Mle-----------: RLOC16 fffe -> 500b
I(1821) OPENTHREAD:[N] Mle-----------: Role detached -> child
```
### Note
Currently, UART wakeup is not enabled. Once the device joins the network as a child and enters sleep mode, the OT CLI will become inaccessible.
