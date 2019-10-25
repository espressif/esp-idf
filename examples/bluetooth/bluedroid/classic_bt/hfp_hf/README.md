# Hands-Free Unit

This example is to show how to use the APIs of Hands-Free (HF) Unit Component and the effects of them with the help of structured commands. You can use this example to communicate with an Audio Gateway (AG) device (e.g. a smart phone). This example use the UART as a transportation of commands and reserve the GPIO for PCM audio path.

## How to use example

### Hardware Required

If possible, example should be able to run on any commonly available ESP32 development board. And is supposed to connect to _Hands Free Audio Gateway (hfp_ag)_ example in ESP-IDF.

### Configure the project

```
idf.py menuconfig
```

- Enable `Classic Bluetooth`,  `Hands Free/Handset` and `Hands Free Unit` under `Component config ---> Bluetooth ---> Bluedroid Options`
- When using PCM as the data path and this example configures PCM audio data to GPIO pins. You can link the GPIO pins to a speaker via i2s port. PCM data path does not support mSBC codec but CVSD codec.
- When using HCI data path, ESP32 support both CVSD and mSBC codec.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

When you run this example, the command help table will print out in the monitor:

```
########################################################################
HF client command usage manual
HF client commands begin with "hf" and ends with ";"
Supported commands are as follows, arguments are embraced with < and >
hf con;   -- setup connection with peer device
hf dis;   -- release connection with peer device
hf cona;  -- setup audio connection with peer device
hf disa;  -- release connection with peer device
hf qop;   -- query current operator name
hf qc;    -- query current call status
hf ac;    -- answer incoming call
hf rc;    -- reject incoming call
hf d <num>;                -- dial <num>, e.g. hf d 11223344
hf rd;                     -- redial
hf dm <index>;             -- dial memory
hf vron;                   -- start voice recognition
hf vroff;                  -- stop voice recognition
hf vu <tgt> <vol>;         -- volume update
		tgt: 0-speaker, 1-microphone
     	vol: volume gain ranges from 0 to 15
hf rs;                     -- retrieve subscriber information
hf rv;                     -- retrieve last voice tag number
hf rh <btrh>;              -- response and hold
		btrh:
			0 - put call on hold,
     		1 - accept the held call, 
     		2 -reject the held call
hf k <dtmf>;               -- send dtmf code. 
		dtmf: single character in set 0-9, *, #, A-D
hf h;                      -- show command manual
########################################################################
```

The commands help table will print out in monitor whenever you type `hf h;` or input a command that is not required by the command parse rule.

### Service Level Connection and Disconnection

In this example, type `hf con;` will trigger the HF Unit to establish a service level connection with AG device and log prints like:

```
E (100147) CNSL: Command [hf dis;]
disconnect
W (100427) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 3 closed: Closed (res: 19)
W (100427) BT_APPL: BTA_HF_CLIENT_SCO_SHUTDOWN_ST: Ignoring event 3
E (100427) BT_HF: APP HFP event: CONNECTION_STATE_EVT
E (100437) BT_HF: --connection state disconnected, peer feats 0x0, chld_feats 0x0
```

**Note: Only after HFP service are initiated and a service level connection exists between the HF Unit and AG device that other commands are available.**

And you can type `hf dis;` to disconnect with the connected AG device, and log prints like:

```
E (100147) CNSL: Command [hf dis;]
disconnect
W (100427) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 3 closed: Closed (res: 19)
W (100427) BT_APPL: BTA_HF_CLIENT_SCO_SHUTDOWN_ST: Ignoring event 3
E (100427) BT_HF: APP HFP event: CONNECTION_STATE_EVT
E (100437) BT_HF: --connection state disconnected, peer feats 0x0, chld_feats 0x0
```

### Audio Connection and Disconnection

You can type `hf cona;` to establish the audio connection between HF Unit and AG device. Also, you can type `hf disa;` to close the audio data stream.

#### Situations for Audio Connection

- Answer an incoming call
- Enable voice recognition
- Dial a outgoing call

#### Situations for Audio Disconnection

- Reject an incoming call
- Disable the voice recognition

#### Audio Data path

ESP32 supports two type of audio data pth: PCM and HCI.

- PCM : When using PCM audio data stream is "matrixed" to GPIO pins and you should link these GPIO pins to a speaker via i2s port. 
- HCI : When using HCI audio data stream will act in "loopback" mode. For example, you can hear your own voice when you place a call to a phone connected with a ESP32 development borad.

#### Codec

ESP32 support both CVSD and mSBC codec. HF Unit and AG devices determine which encoding to use when establishing a service level connection. The choice of codec method also depends on the user's configuration options for this example. Since CVSD is the default codec method in HFP, the situation using mSBC is showed here:

- If you enable `BT_HFP_WBS_ENABLE` in `menuconfig`, mSBC will be available.
- If both HF Unit and AG support mSBC and `BT_HFP_WBS_ENABLE` is enabled, ESP32 chooses mSBC.
- If you use PCM data path, mSBC is not available.

### Answer or Reject an incoming call

#### Answer an incoming call

You can type `hf ac;` to answer an incoming call and log prints like:

```
E (11278) CNSL: Command [hf ac;]
Answer call
E (754946) BT_HF: APP HFP event: AT_RESPONSE
E (754946) BT_HF: --AT response event, code 0, cme 0
E (755326) BT_HF: APP HFP event: CALL_IND_EVT
E (755326) BT_HF: --Call indicator call in progress
E (755336) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (755336) BT_HF: --Call setup indicator NONE
E (755966) BT_HF: APP HFP event: SIGNAL_STRENGTH_IND_EVT
E (755966) BT_HF: -- signal strength: 5 (option)
E (756206) BT_APPL: bta_dm_pm_btm_status hci_status=32
```

#### Reject an incoming call

You can type `hf rc;` to reject an incoming call and log prints like:

```
E (717326) CNSL: Command [hf rc;]
Reject call
E (717356) BT_HF: APP HFP event: AT_RESPONSE
E (717356) BT_HF: --AT response event, code 0, cme 0
E (717666) BT_HF: APP HFP event: AUDIO_STATE_EVT
E (717666) BT_HF: --audio state disconnected
E (717676) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (717676) BT_HF: --Call setup indicator NONE
```

#### Dial Number

This example supports three dialing commands, you can choose one from  them:

- `hf d <num>;` Dial the specific number.
- `hf rd;` Redial the last number.
- `hf dm <index>` Dial the specific indexed number in the AG memory.

When you type `hf d 186xxxx5549;` your phone will put an outgoing call to 186xxxx5549 and log prints like:

```
E (176927) CNSL: Command [hf d 186xxxx5549;]
Dial number 186xxxx5549
E (15246) BT_HF: APP HFP event: AT_RESPONSE
E (15246) BT_HF: --AT response event, code 0, cme 0
E (15246) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (15246) BT_HF: --Call setup indicator OUTGOING_DIALING
E (15736) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (15736) BT_HF: --Call setup indicator OUTGOING_ALERTING
E (15816) BT_BTM: btm_sco_connected, handle 180
E (15816) BT_HF: APP HFP event: AUDIO_STATE_EVT
E (15816) BT_HF: --audio state connected_msbc
```

#### Respond and Hold

You can type `hf rh <btrh>;` to respond or hold the current call. The parameter set:

- `<btrh>` : 0 - hold current call, 1 - answer held call, 2 - end held call.

#### Volume Control

You can type `hf vu <tgt> <vol>;` to sync volume gain of headset or microphone. The parameter set:

- `<tgt>` : 0 - headset, 1 - microphone.
- `<vol>` : Integer among 0 - 15.

For example, `hf vu 0 9;` sync the volume of headset and log prints like:

```
E (34827) CNSL: Command [hf vu 0 9;]
volume update
E (35467) BT_HF: APP HFP event: AT_RESPONSE
E (35467) BT_HF: --AT response event, code 0, cme 0

```

And also,  `hf vu 1 9;`  sync the volume gain of microphone and log prints like:

```
E (39247) CNSL: Command [hf vu 1 9;]
volume updateE (39257) 
BT_HF: APP HFP event: AT_RESPONSEE (39257) 
BT_HF: --AT response event, code 0, cme 0

```

#### Voice Recognition

You can type `hf vron;` to start the voice recognition of AG and type `hf vroff;` to end the voice recognition. For example, type `hf vron;` and log prints like:

```
E (45002) CNSL: Command [hf vron;]
Start voice recognition
E (25162) BT_HF: APP HFP event: AT_RESPONSE
E (25162) BT_HF: --AT response event, code 0, cme 0
E (25182) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (25182) BT_HF: --volume_target: SPEAKER, volume 7
E (191202) BT_HF: --VR state enabled

```

#### Query Current Operator Name

You can type `hf qop;` to query the current operator name and log prints like:

```
E (45002) CNSL: Command [hf qop;]
Query operator
E (393672) BT_HF: APP HFP event: CURRENT_OPERATOR_EVT
E (393672) BT_HF: --operator name: 中国联通
E (393672) BT_HF: APP HFP event: AT_RESPONSE
E (393682) BT_HF: --AT response event, code 0, cme 0

```

#### Retrieve Subscriber Information

You can type `hf rs;` to retrieve subscriber information and log prints like:

```
E (231427) CNSL: Command [hf rs;]
Retrieve subscriber information
E (231447) BT_HF: APP HFP event: SUBSCRIBER_INFO_EVT
E (231447) BT_HF: --subscriber type voice, number +86156xxxx1969
E (231447) BT_HF: APP HFP event: AT_RESPONSE
E (231447) BT_HF: --AT response event, code 0, cme 

```

#### Query Current Call Status

You can type `hf qc;` to query current call status and log prints like:

```
E (102837) CNSL: Command [hf qc;]
Query current call status
E (29228) BT_HF: APP HFP event: CLCC_EVT
E (29228) BT_HF: --Current call: idx 1, dir incoming, state active, mpty single, number 186xxxx5549
E (29238) BT_HF: APP HFP event: AT_RESPONSE
E (29238) BT_HF: --AT response event, code 0, cme 0

```

## Troubleshooting

- You should type the command in the terminal according to the format described in the command help table. 
- Not all commands in the table are supported by AG device.
- If you want to use HF Unit to establish a service level connection with AG, you should add the MAC address of AG in `hfp_hf/main/bt_app.c`, for example:

```
esp_bd_addr_t peer_addr = {0xb4, 0xe6, 0x2d, 0xeb, 0x09, 0x93};

```

- Use `esp_hf_client_register_callback()` and  `esp_hf_client_init();` before  establishing a service level connection.

## Example Breakdown

This example has relatively more source files than other bluetooth examples because _Hands Free Profile_ is somehow complex. But we want to show the functions of _Hands Free Profile_ in a simple way, so we use the _Commands and Effects_ scheme to show the usage of APIs of HFP in ESP-IDF.

- The example will respond to user command through UART console. Please go to `hfp_hf/main/console_uart.c`  for the configuration details.
- For voice interface, ESP32 has provided PCM input/output signals which can be **matrixed** to GPIOs, please go to `hfp_hf/main/gpio_pcm_config.c` for the configuration details. 
- If you want to fix the command table, please refer to `hfp_hf/main/app_hf_msg_set.c`.
- If you want to fix the command parse rules, please refer to `hfp_hf/main/app_hf_msg_prs.c`.
- If you want to fix the responses of HF Unit or want to fix the log, please refer to `hfp_hf/main/bt_app_hf.c`.
- Task configuration part is in `hfp_hf/main/bt_app_core.c`.