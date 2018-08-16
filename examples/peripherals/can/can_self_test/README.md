# CAN Self Test Example

## Overview
The CAN Self Test Example demonstrates the self testing capabilities of the
ESP32 CAN peripheral and **only requires a single ESP32 module to run**.
The Self Test Example can be used to verify that the wiring between the ESP32 
and an external transceiver operates correctly.

The CAN Self Test Example will do the following over multiple iterations:

1. Start the CAN driver
2. Simultaneously transmit and receive messages using the self reception request.
3. Stop the CAN driver

## External Transceiver and Pin Assignment
The CAN controller in the ESP32 **does not contain an internal transceiver**.
Therefore users are responsible for providing an external transceiver compatible
with the physical layer specifications of their target ISO standard (such as 
SN65HVD23X transceivers for ISO 11898-2 compatibility)

The CAN controller in the ESP32 represents dominant bits to the transceiver as 
logic low, and recessive bits as logic high. The Self Test Example utilizes the 
following default pin assignments

* TX Pin is routed to GPIO21
* RX Pin is routed to GPIO22

## Note
If the Self Test Example does not receive any messages, it is likely that the
wiring between the ESP32 and the external transceiver is incorrect. To verify
that the CAN controller in the ESP32 is operating correctly, users can bypass
the external transceiver by connecting the TX Pin directly to the RX Pin when 
running the Self Test Example. 
