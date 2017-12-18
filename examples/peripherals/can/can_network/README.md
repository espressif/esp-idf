# CAN Network Example

## Overview
The CAN Network Example demonstrates communication between two ESP32 modules (master 
and slave) using the CAN2.0B protocol. CAN is a multi-master protocol, therefore 
the concept of master/slave in this example refers to which node initiates
and stops the transfer of a stream of data messages. The example also includes 
an optional  **Listen Only module** which can passively receive the CAN messages
sent between the master and slave module without participating in any CAN bus activity.

The CAN Network Example will execute the following steps over multiple iterations:

1. Both master and slave go through initialization process
2. The master repeatedly sends **PING** messages until it receives a **PING_RESP**
from the slave. The slave will only send a **PING_RESP** message when it receives
a **PING** message from the master.
3. Once the master has received the **PING_RESP** from the slave, it will send a 
**START_CMD** message to the slave.
4. Upon receiving the **START_CMD** message, the slave will start transmitting 
**DATA** messages until the master sends a **STOP_CMD**. The master will send
the **STOP_CMD** after receiving N **DATA** messages from the slave (N = 50 by 
default).
5. When the slave receives the **STOP_CMD**, it will confirm that it has stopped
by sending a **STOP_RESP** message to the master. 

## External Transceiver and Pin Assignment
The CAN controller in the ESP32 **does not contain an internal transceiver**.
Therefore users are responsible for providing an external transceiver compatible
with the physical layer specifications of their target ISO standard (such as 
SN65HVD23X transceivers for ISO 11898-2 compatibility)

The CAN controller in the ESP32 represents dominant bits to the transceiver as 
logic low, and recessive bits as logic high. The Network Example utilizes the 
following default pin assignments

* TX Pin is routed to GPIO21
* RX Pin is routed to GPIO22

The following diagram illustrates an example network

~~~~
    ----------   ----------   --------------  
   |  Master  | |  Slave   | | Listen Only  | 
   |          | |          | |              | 
   | 21    22 | | 21    22 | | 21    22     | 
    ----------   ----------   --------------  
     |      |     |      |     |      |   
     |      |     |      |     |      |   
    ----------   ----------   ----------  
   | D      R | | D      R | | D      R | 
   |          | |          | |          | 
   |  VP230   | |  VP230   | |  VP230   | 
   |          | |          | |          | 
   | H      L | | H      L | | H      L | 
    ----------   ----------   ----------  
     |      |     |      |     |      |   
     |      |     |      |     |      |   
  |--x------|-----x------|-----x------|--| H
            |            |            |
  |---------x------------x------------x--| L

~~~~

## Note
If there appears to be no activity on the CAN bus when running the example, users
can try running the `can_self_test` example to verify if their transceivers are
wired properly.