| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# OpenThread Power Save Example

The example demonstrates the OpenThread Power Save based on light sleep, which reduces power consumption by turning off RF, PHY, BB, and IEEE802154 MAC during the sleep state.  
## How to use example

### Hardware Required

* Prepare an ESP32-C6 development board as an OpenThread Sleepy End Device (SED).   
* Connect the board using a USB cable for power supply and programming.  
* Choose another ESP32-C6 as the OpenThread Leader.  


## Configure the Openthread Dataset

* Run [ot_cli](../ot_cli/) on another ESP32-C6 device to create openthread dataset configuration and start an openthread network as the leader.  
* Configure the Openthread dataset (`CONFIG_OPENTHREAD_NETWORK_DATASET`) in [ot_power_save](../ot_power_save/main/esp_ot_power_save_config.h), ensuring that the device's dataset matches the dataset of the leader.  

## Erase the ot_storage

If desired, erase the ot_storage by running `idf.py -p <PORT> erase-flash` before flashing the board to remove previous examples or other project data.

### Build and Flash

 Build the project and flash it to the board. Use the following command: `idf.py -p <PORT> build flash monitor`.  


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