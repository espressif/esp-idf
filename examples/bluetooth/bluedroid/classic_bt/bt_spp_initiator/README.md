| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT-SPP-INITATOR demo

This example is to show how to use the APIs of **Serial Port Protocol** (**SPP**) to create an SPP initiator which performs as a client. we aggregate **Secure Simple Pair** (**SSP**) into this demo to show how to use SPP when creating your own APPs. We also provide the demo `bt_spp_acceptor` or the demo `bt_spp_vfs_acceptor` to create an SPP acceptor which performs as a server. In fact, you can create SPP acceptors and SPP initiators on a single device at the same time.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate the example, you should be connect to an SPP acceptor running on a smartphone, a computer or on another ESP32 development board.

### Configure the project
1. Open the project configuration menu:
```
idf.py menuconfig
```

2. Enable the SPP functionality by choosing the path as following:

`Component config --> Bluetooth --> Bluedroid Options --> SPP`.

3. SSP is enabled as default in this example. If you prefer the legacy pairing, you shall disable it in the following path.

`SPP Example Configuration --> Secure Simple Pair`.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Description

After the program starts, the example will initiate a Bluetooth discovery procedure and filter out the peer device by the name in the EIR(Extended Inquiry Response). After discovering the SPP service, it will connect to the SPP acceptor and send data. The example will calculate the data rate or print the sent data after the SPP connection is established.
### Example Output

When you run this example and the IO capability is `ESP_IO_CAP_IO` or `ESP_IO_CAP_IN` , the commands help table prints the following at the very beginning:

```
########################################################################
Supported commands are as follows, arguments are embraced with < and >
spp h;                -- show command manual

Use this cmmand table if the IO Capability of local device set as IO_CAP_IO.
spp ok;               -- manual Numeric Confirmation.

Use this cmmand table if the IO Capability of local device set as IO_CAP_IN.
spp key <auth key>;   -- manual Passkey. (e.g. spp key 136245;)

########################################################################
```

**Note:**

- Only after SPP service is initialized and a service level connection exists between an Initiator and Acceptor device, could other commands be available.
- This command help table will print out in monitor whenever you type `spp h;` or if you input a command that is not required by the command parse rule.

- Commands should always start with `spp` and end with `;` or the example will not responds.

- The command you typed will not echo in monitor.

### Situation under `ESP_IO_CAP_IN`

The log in terminal will indicate you to input the passkey to initiate the connection of SPP.

```
I (2244) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (2244) SPP_INITIATOR_DEMO: ......
I (2394) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (2404) SPP_INITIATOR_DEMO: ......
I (2404) SPP_INITIATOR_DEMO: ESP_SPP_ACCEPTOR
I (2414) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (3274) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=0 scn_num=1
I (3284) SPP_INITIATOR_DEMO: ESP_SPP_CL_INIT_EVT
I (3454) SPP_INITIATOR_DEMO: ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!
W (3454) SPP_INITIATOR_DEMO: To input the key, type `spp key xxxxxx;`
```

### Situation under `ESP_IO_CAP_IO`

The log in terminal will indicate you to confirm the number to initiate the connection of SPP.

```
I (2342) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (2342) SPP_INITIATOR_DEMO: 30 ae a4 80 18 32
I (2342) SPP_INITIATOR_DEMO: ESP_SPP_ACCEPTOR
I (2352) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (3212) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=0 scn_num=1
I (3222) SPP_INITIATOR_DEMO: ESP_SPP_CL_INIT_EVT
I (3392) SPP_INITIATOR_DEMO: ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: 864115
W (3392) SPP_INITIATOR_DEMO: To confirm the value, type `spp ok;`
```

**Note:**

Whether you should passkey or confirm the number also depends on the IO capability of the peer device. And whether the two device are already paired before.

## Troubleshouting

- Set `SPP_SHOW_MODE` as `SPP_SHOW_DATA` or `SPP_SHOW_SPEED` in code (should be same with `bt_spp_acceptor`, if the peer device runs it). When setting `SPP_SHOW_MODE` as `SPP_SHOW_DATA`, if the data rate is too high or the data length is too long, it is strongly recommended to process them in other lower priority application task rather than in this callback directly. Since the printing takes too much time, and it may stuck the Bluetooth stack.

- We haven't do the same update to `bt_acceptor_demo` for the sake of reducing the size of ESP_IDF, but transplanting of input module is supported.

## Example Breakdown

To clearly show how the SSP aggregate with the SPP , we use the Commands and Effects scheme to illustrate the process of secure paring and connection establishment.

- The example will respond to user command through UART console. Please go to `console_uart.c`  for the configuration details.

- If you want to update the command table, please refer to `app_spp_msg_set.c`.

- If you want to update the command parse rules, please refer to `app_spp_msg_prs.c`.

## FAQ
Q: How to change the process of SSP?
A: Users can set the IO Capability and Security Mask for their device (fixed Security Mode, Security Mode 4). In short, the Security Mask sets the security level for authentication stage and the IO Capability determines the way of user interaction during pairing. The default Security Mask of this demo is `ESP_SPP_SEC_AUTHENTICATE` which support MITM (Man In The Middle) protection. For more information about Security Simple Pair on ESP32, please refer to [ESP32_SSP](../bt_spp_acceptor/ESP32_SSP.md).

Q: How can we reach the maximum throughput when using SPP?
A: The default MTU size of classic Bluetooth SPP on ESP32 is 990 bytes, and higher throughput can be achieved in the case that data chunck size is close to the MTU size or multiple of MTU size. For example, sending 100 bytes data per second is much better than sending 10 bytes every 100 milliseconds.

Q: What is the difference between the event `ESP_SPP_CONG_EVT` and the parameter `cong` of the event `ESP_SPP_WRITE_EVT`?
A: The event `ESP_SPP_CONG_EVT` shows the changing status from `congest` to `uncongest`, or form `uncongest` to `congest`. Congestion can have many causes, such as using out of the credit which is sent by peer, reaching the high watermark of the Tx buffer, the congestion at Bluetooth L2CAP layer and so on. The parameter `cong` of the event `ESP_SPP_WRITE_EVT` shows a snapshot of the state of the flow control manager after the write operation is completed. The user needs to carefully consider retransmitting or continuing to write according to these two events. The ESP32 offers an VFS mode of SPP which hides the details of retransmitting, but it will block the caller and is not more efficient than the callback mode.

Q: How many SPP clients does ESP32 support?
A: The ESP32 supports maximum 8 SPP clients, which including virtual SPP connections. Virtual SPP connection means that SPP clients can connect to the different SPP servers running on the same peer device. However the number of SPP clients (excluding virtual connections) shall not exceed the number of Bluetooth ACL connections.
