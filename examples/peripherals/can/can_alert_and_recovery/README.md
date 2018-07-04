# CAN Alert and Recovery Example

## Overview
The CAN Alert and Recovery Example demonstrates the usage of alerts and bus 
recovery in the CAN driver. This example **requires only a single ESP32 module 
to run**.

The CAN Alert and Recovery Example will do the following...

1. Initialize and start the CAN driver on the ESP32 module
2. Repeatedly transmit messages (no acknowledgement required)
3. Reconfigure alerts to detect bus-off state
4. Purposely trigger errors on transmissions
5. Detect Bus Off condition
6. Initiate bus recovery
7. Deinitialize CAN driver on ESP32 module

## External Transceiver and Pin Assignment
The CAN controller in the ESP32 **does not contain an internal transceiver**.
Therefore users are responsible for providing an external transceiver compatible
with the physical layer specifications of their target ISO standard (such as 
SN65HVD23X transceivers for ISO 11898-2 compatibility)

The CAN controller in the ESP32 represents dominant bits to the transceiver as 
logic low, and recessive bits as logic high. The Alert and Recovery Example
utilizes the following default pin assignments

* TX Pin is routed to GPIO21
* RX Pin is routed to GPIO22
