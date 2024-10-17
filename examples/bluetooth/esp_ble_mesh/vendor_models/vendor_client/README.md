| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

ESP BLE Mesh Vendor Client Example
==================================

This example demonstrates how to create a vendor client model in Provisioner, and the [vendor server example](../vendor_server) demonstrates how to create a vendor server model in an unprovisioned device.

### 1. Procedures
1. Compile the vendor client and vendor server examples and flash;
2. When Provisioner starts, it will register callbacks, initialize mesh stack, initialize the vendor client model, set matching device uuid, enable its functionality, add local application key and bind the application key with the vendor client model;
3. When the unprovisioned device starts, it will initialize mesh stack and enable its functionality;
4. When the Provisioner and unprovisioned device are ready, the Provisioner will start to provision the unprovisioned device;
5. After the device is provisioned successfully, Provisioner will configure the node, i.e. add application key, bind application key with the vendor server model of the node;
6. After the configuration is completed, users can press the "Boot" button on the development board to send a vendor client message to the node;
7. When the node receives the vendor client message, it will send a vendor server message as a response;
8. After the Provisioner receives the response, it will calculate the latency (in microseconds).

### 2. Results
The following is a sample test result, the distance between the Provisioner and the node is about 1 meter.

#### 2.1 Provisioner - Vendor Client
I (6684) Client: Send 0xc002e5
I (6834) Client: Recv 0xc102e5, tid 0x0001, time 156585us
I (7934) Client: Send 0xc002e5
I (8024) Client: Recv 0xc102e5, tid 0x0002, time 83348us
I (9044) Client: Send 0xc002e5
I (9094) Client: Recv 0xc102e5, tid 0x0003, time 51202us
I (10244) Client: Send 0xc002e5
I (10294) Client: Recv 0xc102e5, tid 0x0004, time 45290us
I (11544) Client: Send 0xc002e5
I (11564) Client: Recv 0xc102e5, tid 0x0005, time 22973us
I (12734) Client: Send 0xc002e5
I (12754) Client: Recv 0xc102e5, tid 0x0006, time 22369us
I (14114) Client: Send 0xc002e5
I (14134) Client: Recv 0xc102e5, tid 0x0007, time 22113us
I (15544) Client: Send 0xc002e5
I (15614) Client: Recv 0xc102e5, tid 0x0008, time 68969us
I (16694) Client: Send 0xc002e5
I (16714) Client: Recv 0xc102e5, tid 0x0009, time 21782us
I (17784) Client: Send 0xc002e5
I (17814) Client: Recv 0xc102e5, tid 0x000a, time 25010us

#### 2.2 Node - Vendor Server
I (6409) Server: Recv 0xc002e5, tid 0x0001
I (6429) Server: Send 0xc102e5
I (7609) Server: Recv 0xc002e5, tid 0x0002
I (7619) Server: Send 0xc102e5
I (8719) Server: Recv 0xc002e5, tid 0x0003
I (8729) Server: Send 0xc102e5
I (9899) Server: Recv 0xc002e5, tid 0x0004
I (9909) Server: Send 0xc102e5
I (11189) Server: Recv 0xc002e5, tid 0x0005
I (11199) Server: Send 0xc102e5
I (12379) Server: Recv 0xc002e5, tid 0x0006
I (12389) Server: Send 0xc102e5
I (13759) Server: Recv 0xc002e5, tid 0x0007
I (13769) Server: Send 0xc102e5
I (15189) Server: Recv 0xc002e5, tid 0x0008
I (15209) Server: Send 0xc102e5
I (16339) Server: Recv 0xc002e5, tid 0x0009
I (16349) Server: Send 0xc102e5
I (17439) Server: Recv 0xc002e5, tid 0x000a
I (17449) Server: Send 0xc102e5