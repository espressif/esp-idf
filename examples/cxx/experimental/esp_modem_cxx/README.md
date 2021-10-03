| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# Simple example of esp_modem component

(See the README.md file in the upper level 'examples' directory for more information about examples.)


## Overview

This example demonstrates the use of the [esp-modem component](https://components.espressif.com/component/espressif/esp_modem) to connect to a network and send some AT commands.
It uses modem CMUX mode so that commands and network could be used at the same time. 

## About the esp_modem

The esp_modem component is deployed automatically when building this example. It will be located inside this example directory, in relative path `managed_components/espressif__esp_modem`. 
You can find a brief introduction in the root directory of the component, in the main `README.md` for  or a full html documentation in the relative path `docs/html/index.html`.

## How to use example

### Hardware Required

To run this example, you need an ESP32 dev board connected to a GSM/4G cellular module, which supports AT commands and PPP connectivity.
The example uses UART to communicate with the modem and it uses the below pin assignment:

| ESP32  | Cellular Modem |
| ------ | -------------- |
| GPIO25 | RX             |
| GPIO26 | TX             |
| GND    | GND            |

### Configure the project

* Choose the modem module in `Choose supported modem device(DCE)` option.
* Set the access point name in `Set Access Point Name(APN)` option, which should depend on the operator of your SIM card.
* Enable `EXAMPLE_NEED_SIM_PIN`: To unlock the SIM card with a PIN code if needed

## Example Output

The example will unlock the SIM card (if enabled) and enter the CMUX mode. 
Then it would initiate the connection to a public MQTT broker and at the same time reading some data using AT commands.

```
I (571) uart: queue free spaces: 30
I (1731) cmux_example: Modem has correctly entered multiplexed command/data mode
I (2241) cmux_example: Operator name: "OperatorName"
I (7781) esp-netif_lwip-ppp: Connected
I (7781) esp-netif_lwip-ppp: Name Server1: xxx.xxx.xxx.xxx
I (7781) esp-netif_lwip-ppp: Name Server2: xxx.xxx.xxx.xxx
I (7781) cmux_example: Got IP address
I (7791) cmux_example: IP          : xxx.xxx.xxx.xxx
I (7791) cmux_example: Netmask     : xxx.xxx.xxx.xxx
I (7801) cmux_example: Gateway     : xxx.xxx.xxx.xxx
I (7801) system_api: Base MAC address is not set
I (7811) system_api: read default base MAC address from EFUSE
I (9061) cmux_example:  TOPIC: /topic/esp-modem
I (9061) cmux_example:  DATA: Hello modem
I (9061) cmux_example: Data received
I (9211) cmux_example: Modem IMSI number: xxxxxxxxxxxxxxxx
I (21221) esp-netif_lwip-ppp: User interrupt
I (21221) esp_modem_netif: PPP state changed event 5
```
