# Hands-Free Unit

This example is to show how to use the APIs of Hands-Free (HF) Unit Component and the effects of them by providing a set of commands. You can use this example to communicate with an Audio Gateway (AG) device (e.g. a smart phone). This example uses UART for user commands.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate it should be connected to an AG running on a smartphone or on another ESP32 development board loaded with Hands Free Audio Gateway (hfp_ag) example from ESP-IDF.

### Configure the project

```
idf.py menuconfig
```

ESP32 supports two types of audio data path: PCM() and HCI(Host-Controller Interface) but the default sdkconfig of this example does not config the data path in a specific way. You should config the data path you want:

- PCM : When using PCM, audio data stream is mapped to GPIO pins and you should link these GPIO pins to a speaker via I2S port. And you should choose PCM in `menuconfig` path: `Component config --> Bluetooth controller --> BR/EDR Sync(SCO/eSCO) default data path --> PCM`and also `Component config --> Bluetooth --> Bluedroid Options -->Hands Free/Handset Profile --> audio(SCO) data path --> PCM`.
- HCI : When using HCI, audio data stream will be transport to HF unit app via HCI. And you should choose HCI in `menuconfig` path: `Component config -->Bluetooth controller -->BR/EDR Sync(SCO/eSCO) default data path --> HCI` and also `Component config --> Bluetooth --> Bluedroid Options -->Hands Free/Handset Profile --> audio(SCO) data path --> HCI`.

**Note: Wide Band Speech is disabled by default, if you want to use it please select it in menuconfig path: `Component config --> Bluetooth --> Bluedroid Options --> Wide Band Speech`.**

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

When you run this example, the commands help table prints the following at the very begining:

```
########################################################################
HF client command usage manual
HF client commands begin with "hf" and end with ";"
Supported commands are as follows, arguments are embraced with < and >
hf con;   -- setup connection with peer device
hf dis;   -- release connection with peer device
hf cona;  -- setup audio connection with peer device
hf disa;  -- release connection with peer device
hf qop;   -- query current operator name
hf qc;    -- query current call status
hf ac;    -- answer incoming call
hf rc;    -- reject incoming call
hf d <num>;        -- dial <num>, e.g. hf d 11223344
hf rd;             -- redial
hf dm <index>;     -- dial memory
hf vron;           -- start voice recognition
hf vroff;          -- stop voice recognition
hf vu <tgt> <vol>; -- volume update
     tgt: 0-speaker, 1-microphone
     vol: volume gain ranges from 0 to 15
hf rs;             -- retrieve subscriber information
hf rv;             -- retrieve last voice tag number
hf rh <btrh>;      -- response and hold
     btrh:
        0 - put call on hold,
        1 - accept the held call,
        2 -reject the held call
hf k <dtmf>;       -- send dtmf code.
     dtmf: single character in set 0-9, *, #, A-D
hf h;              -- show command manual
########################################################################
```

**Note:**

- This command help table will print out in monitor whenever you type `hf h;` or if you input a command that is not required by the command parse rule.
- The command you type will not echo in monitor and your command should always start with `hf` and end with `;` or the example will not responds.
- The command you typed will not echo in monitor.

### Service Level Connection and Disconnection

You can type `hf con;` to establish a service level connection with AG device and log prints such as:

```
E (78502) CNSL: Command [hf con;]
connect
W (79632) BT_APPL: new conn_srvc id:27, app_id:1
I (79642) BT_HF: APP HFP event: CONNECTION_STATE_EVT
I (79642) BT_HF: --connection state connected, peer feats 0x0, chld_feats 0x0
I (79792) BT_HF: APP HFP event: CALL_IND_EVT
I (79792) BT_HF: --Call indicator NO call in progress
I (79792) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
I (79802) BT_HF: --Call setup indicator NONE
I (79802) BT_HF: APP HFP event: NETWORK_STATE_EVT
I (79812) BT_HF: --NETWORK STATE available
I (79812) BT_HF: APP HFP event: SIGNAL_STRENGTH_IND_EVT
I (79822) BT_HF: -- signal strength: 4
I (79822) BT_HF: APP HFP event: ROAMING_STATUS_IND_EVT
I (79832) BT_HF: --ROAMING: inactive
I (79832) BT_HF: APP HFP event: BATTERY_LEVEL_IND_EVT
I (79842) BT_HF: --battery level 3
I (79842) BT_HF: APP HFP event: CALL_HELD_IND_EVT
I (79852) BT_HF: --Call held indicator NONE held
I (79852) BT_HF: APP HFP event: CONNECTION_STATE_EVT
I (79862) BT_HF: --connection state slc_connected, peer feats 0x16e, chld_feats 0x0
I (79872) BT_HF: APP HFP event: INBAND_RING_TONE_EVT
I (79872) BT_HF: --inband ring state Provided
```

**Note: Only after Hands-Free Profile(HFP) service is initialized and a service level connection exists between an HF Unit and an AG device, could other commands be available.**

You can type `hf dis;` to disconnect with the connected AG device, and log prints such as:

```
E (93382) CNSL: Command [hf dis;]
disconnect
W (93702) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 3 closed: Closed (res: 19)
W (93712) BT_APPL: BTA_HF_CLIENT_SCO_SHUTDOWN_ST: Ignoring event 3
I (93712) BT_HF: APP HFP event: CONNECTION_STATE_EVT
I (93712) BT_HF: --connection state disconnected, peer feats 0x0, chld_feats 0x0
```

### Audio Connection and Disconnection

You can type `hf cona;` to establish the audio connection between HF Unit and AG device. Log prints such as:

```
E (117232) CNSL: Command [hf cona;]
connect audio
I (117232) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (117232) BT_HF: --audio state connecting
E (117262) BT_BTM: btm_sco_connected, handle 181
I (117262) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (117262) BT_HF: --audio state connected
```

Also, you can type `hf disa;` to close the audio data stream. Log prints such as:

```
E (133002) CNSL: Command [hf disa;]
disconnect audio
I (133262) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (133262) BT_HF: --audio state disconnected
```
#### Scenarios for Audio Connection

- Answer an incoming call
- Enable voice recognition
- Dial an outgoing call

#### Scenarios for Audio Disconnection

- Reject an incoming call
- Disable the voice recognition

#### Choise of Codec

ESP32 supports both CVSD and mSBC codec. HF Unit and AG device determine which codec to use by exchanging features during service level connection. The choice of codec also depends on the your configuration in `menuconfig`.

Since CVSD is the default codec in HFP, we just show the scenarios using mSBC:

- If you enable `BT_HFP_WBS_ENABLE` in `menuconfig`, mSBC will be available.
- If both HF Unit and AG support mSBC and `BT_HFP_WBS_ENABLE` is enabled, ESP32 chooses mSBC.
- If you use PCM data path, mSBC is not available.

### Answer or Reject an incoming call

#### Answer an incoming call

You can type `hf ac;` to answer an incoming call and log prints such as:

```
E (196982) CNSL: Command [hf ac;]
Answer call
I (197102) BT_HF: APP HFP event: AT_RESPONSE
I (197102) BT_HF: --AT response event, code 0, cme 0
E (197232) BT_BTM: btm_sco_connected, handle 181
I (197232) BT_HF: APP HFP event: CALL_IND_EVT
I (197232) BT_HF: --Call indicator call in progress
I (197232) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (197242) BT_HF: --audio state connected
```

#### Reject an incoming call

You can type `hf rc;` to reject an incoming call and log prints such as:

```
E (210112) CNSL: Command [hf rc;]
Reject call
I (210822) BT_HF: APP HFP event: AT_RESPONSE
I (210822) BT_HF: --AT response event, code 0, cme 0
I (210842) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (210842) BT_HF: --audio state disconnected
I (210902) BT_HF: APP HFP event: CALL_IND_EVT
I (210902) BT_HF: --Call indicator NO call in progress
```

#### Dial Number

This example supports three dialing commands:

- `hf d <num>;` Dial the specific number.
- `hf rd;` Redial the last number.
- `hf dm <index>` Dial the specific indexed number in the AG memory.

For example, type `hf d 186xxxx5549;` to make an outgoing call to `186xxxx5549` and log prints such as:

```
E (228882) CNSL: Command [hf d 186xxxx5549;]
Dial number 186xxxx5549
E (229702) BT_BTM: btm_sco_connected, handle 181
I (229712) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
I (229712) BT_HF: --Call setup indicator OUTGOING_DIALING
I (229712) BT_HF: APP HFP event: CALL_IND_EVT
I (229712) BT_HF: --Call indicator call in progress
I (229722) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (229722) BT_HF: --audio state connected
I (229732) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
I (229732) BT_HF: --Call setup indicator NONE
```

#### Respond and Hold

You can type `hf rh <btrh>;` to respond or hold the current call. The parameter should be set as follows:

- `<btrh>` : 0 - hold current call, 1 - answer held call, 2 - end held call.

#### Volume Control

You can type `hf vu <tgt> <vol>;` to update volume gain of speaker or microphone. The parameter should be set as follows:

- `<tgt>` : 0 - speaker, 1 - microphone.
- `<vol>` : Integer among 0 - 15.

For example, type `hf vu 0 9;` to update the volume of speaker and log on AG prints:

```
I (43684) BT_APP_HF: APP HFP event: VOLUME_CONTROL_EVT
I (43684) BT_APP_HF: --Volume Target: SPEAKER, Volume 9
```

And also, `hf vu 1 9;` update the volume gain of microphone and log on AG prints:

```
I (177254) BT_APP_HF: APP HFP event: VOLUME_CONTROL_EVT
I (177254) BT_APP_HF: --Volume Target: MICROPHONE, Volume 9
```

#### Voice Recognition

You can type `hf vron;` to start the voice recognition of AG and type `hf vroff;` to terminate this function. For example, type `hf vron;` and log prints such as:

```
E (292432) CNSL: Command [hf vron;]
Start voice recognition
I (293172) BT_HF: APP HFP event: AT_RESPONSE
I (293172) BT_HF: --AT response event, code 0, cme 0
E (293702) BT_BTM: btm_sco_connected, handle 181
I (293702) BT_HF: APP HFP event: AUDIO_STATE_EVT
I (293702) BT_HF: --audio state connecte
```

#### Query Current Operator Name

You can type `hf qop;` to query the current operator name and log prints like:

```
E (338322) CNSL: Command [hf qop;]
Query operator
I (339202) BT_HF: APP HFP event: CURRENT_OPERATOR_EVT
I (339202) BT_HF: --operator name: 中国联通
I (339202) BT_HF: APP HFP event: AT_RESPONSE
I (339202) BT_HF: --AT response event, code 0, cme 0
```

#### Retrieve Subscriber Information

You can type `hf rs;` to retrieve subscriber information and log prints such as:

```
E (352902) CNSL: Command [hf rs;]
Retrieve subscriber information
I (353702) BT_HF: APP HFP event: SUBSCRIBER_INFO_EVT
I (353702) BT_HF: --subscriber type unknown, number 186xxxx5549
I (353702) BT_HF: APP HFP event: AT_RESPONSE
I (353702) BT_HF: --AT response event, code 0, cme 0
```

#### Query Current Call Status

You can type `hf qc;` to query current call status and log prints like:

```
E (354522) CNSL: Command [hf qc;]
Query current call status
I (354582) BT_HF: APP HFP event: CLCC_EVT
I (354582) BT_HF: --Current call: idx 1, dir incoming, state active, mpty single, number 186xxxx5549
I (354582) BT_HF: APP HFP event: AT_RESPONSE
I (354592) BT_HF: --AT response event, code 0, cme 0
```

#### Transport DTMF Code

You can type `hf k <dtmf>;` to transport a DTMF code to AG. Log on HF unit side prints like:`send dtmf code: 9` and log on AG side prints such as:

```
I (196284) BT_APP_HF: APP HFP event: DTMF_RESPONSE_EVT
I (196284) BT_APP_HF: --DTMF code is: 9.
```

## Troubleshooting

If you encounter any problems, please check if the following rules are followed:

- You should type the command in the terminal according to the format described in the commands help table.
- Not all commands in the table are supported by AG device like _Hands Free Audio Gateway (hfp_ag)_ example from ESP-IDF.
- If you want to use `hf con;` to establish a service level connection with specific AG device, you should add the MAC address of the AG device in `bt_app.c`, for example: `esp_bd_addr_t peer_addr = {0xb4, 0xe6, 0x2d, 0xeb, 0x09, 0x93};`
- Use `esp_hf_client_register_callback()` and  `esp_hf_client_init();` before establishing a service level connection.

## Example Breakdown

Due to the complexity of HFP, this example has more source files than other bluetooth examples. To show functions of HFP in a simple way, we use the Commands and Effects scheme to illustrate APIs of HFP in ESP-IDF.

- The example will respond to user command through UART console. Please go to `console_uart.c`  for the configuration details.
- For voice interface, ESP32 has provided PCM input/output signals which can be mapped to GPIO pins. So, please go to `gpio_pcm_config.c` for the configuration details.
- If you want to update the command table, please refer to `app_hf_msg_set.c`.
- If you want to update the command parse rules, please refer to `app_hf_msg_prs.c`.
- If you want to update the responses of HF Unit or want to update the log, please refer to `bt_app_hf.c`.
- Task configuration part is in `bt_app_core.c`.