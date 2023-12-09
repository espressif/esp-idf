| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |
# OpenThread Sleepy Device Example

The example demonstrates the Thread Sleepy End Device (SED), the device will enter [Light Sleep mode](https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/api-reference/system/sleep_modes.html#sleep-modes) during idle state.
## How to use example

### Hardware Required

* Prepare an 802.15.4 SoC development board as an OpenThread Sleepy End Device (SED).
* Connect the board using a USB cable for power supply and programming.
* Choose another 802.15.4 SoC as the OpenThread Leader.

## Configure the Openthread Dataset

* Run [ot_cli](../../ot_cli/) on another 802.15.4 SoC device to create openthread dataset configuration and start an openthread network as the leader.
* Configure the Openthread dataset using `idf.py menuconfig` in `Component config ---> Openthread ---> Thread Operation Dataset`, ensuring that the openthread sleepy device's dataset matches the dataset of the leader.

### Build and Flash

Build the project and flash it to the board. Use the following command: `idf.py -p <PORT> erase-flash flash monitor`.

### Example Output

As the example runs, you will see the log output indicating the initialization and operation of OpenThread, including the device joining the OpenThread network as a Sleepy End Device (SED) and periodic polling of the leader.

```
I (769) btbb_init: btbb sleep retention initialization
I (769) ieee802154: ieee802154 mac sleep retention initialization
I(769) OPENTHREAD:[I] ChildSupervsn-: Timeout: 0 -> 190
I (699) main_task: Returned from app_main()
I (799) OPENTHREAD: OpenThread attached to netif
I(799) OPENTHREAD:[N] Mle-----------: Mode 0x0f -> 0x04 [rx-on:no ftd:no full-net:no]
I(809) OPENTHREAD:[N] Mle-----------: Role disabled -> detached
I (819) OPENTHREAD: netif up
I(1519) OPENTHREAD:[N] Mle-----------: Attach attempt 1, AnyPartition reattaching with Active Dataset
I(2479) OPENTHREAD:[N] Mle-----------: RLOC16 fffe -> 5023
I(2529) OPENTHREAD:[N] Mle-----------: Role detached -> child
```  