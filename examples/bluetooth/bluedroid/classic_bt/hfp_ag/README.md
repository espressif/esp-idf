# Hands-Free Audio Gateway

This example is to show how to use the APIs of Hands-Free (HF) Audio Gateway (AG) Component and the effects of them by providing a set of commands. You can use this example to communicate with a Hands-Free Unit (e.g. a headphone set). This example uses UART for user commands.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate this example, it should be connected to a Hands-Free Unit running on a Headphone/Headset or on another ESP32 development board loaded with Hands Free Unit (hfp_hf) example from ESP-IDF.

### Configure the project

```
idf.py menuconfig
```

ESP32 supports two types of audio data paths: PCM and HCI. Because the default sdkconfig of this example does not configured the data path in a specific way. You should config the data path you want:

- PCM : When using PCM, audio data stream is directed to GPIO pins and you should link these GPIO pins to a speaker via I2S port. You should choose PCM in `menuconfig` path: `Component config --> Bluetooth controller --> BR/EDR Sync(SCO/eSCO) default data path --> PCM`and also `Component config --> Bluetooth --> Bluedroid Options -->Hands Free/Handset Profile --> audio(SCO) data path --> PCM`.
- HCI : When using HCI, audio data stream will be transported to HF unit app via HCI. You should choose HCI in `menuconfig` path: `Component config -->Bluetooth controller -->BR/EDR Sync(SCO/eSCO) default data path --> HCI` and also `Component config --> Bluetooth --> Bluedroid Options -->Hands Free/Handset Profile --> audio(SCO) data path --> HCI`.

**Note: Wide Band Speech is disabled by default, if you want to use it please select it in the menuconfig path: `Component config --> Bluetooth --> Bluedroid Options --> Wide Band Speech`.**

### Build and Flash

Build the project and flash it to the board. Then, run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

When you flash and monitor this example, the commands help table prints the following log at the very begining:

```
########################################################################
HFP AG command usage manual
HFP AG commands begins with "hf" and end with ";"
Supported commands are as follows, arguments are embraced with < and >

hf con;                   -- set up connection with peer device
hf dis;                   -- release connection with peer device
hf cona;                  -- set up audio connection with peer device
hf disa;                  -- release audio connection with peer device
hf vron;                  -- start voice recognition
hf vroff;                 -- stop voice recognition
hf vu <tgt> <vol>;        -- volume update
     tgt: 0-speaker, 1-microphone
     vol: volume gain ranges from 0 to 15
hf ind <call> <ntk> <callsetup> <sig>;       -- unsolicited indication device status to HF Client
     call: call status [0,1]
     callsetup: call setup status [0,3]
     ntk: network status [0,1]
     sig: signal strength value from 0~5
hf ate <rep> <err>;       -- send extended at error code
     rep: response code from 0 to 7
     err: error code from 0 to 32
hf iron;                  -- in-band ring tone provided
hf iroff;                 -- in-band ring tone not provided
hf ac;                    -- Answer Incoming Call from AG
hf rc;                    -- Reject Incoming Call from AG
hf d <num>;               -- Dial Number by AG, e.g. hf d 11223344
hf end;                   -- End a call by AG
hf h;                     -- to see the command for HFP AG
########################################################################
```

**Note:**

- This command help table will print out in monitor whenever you type `hf h;` or if you input a command that is not required by the command parse rule.
- The command you type will not echo in monitor and your command should always start with `hf` and end with `;` or the example will not respond.
- The command you type in will not echo in monitor.

### Service Level Connection and Disconnection

You can type `hf con;` to establish a service level connection with HF Unit device and log prints such as:

```
W (2211) BT_APPL: new conn_srvc id:5, app_id:0
I (2221) BT_APP_HF: APP HFP event: CONNECTION_STATE_EVT
I (2221) BT_APP_HF: --connection state CONNECTED, peer feats 0x0, chld_feats 0x0
I (2291) BT_APP_HF: APP HFP event: CIND_RESPONSE_EVT
I (2291) BT_APP_HF: --CIND Start.
I (2331) BT_APP_HF: APP HFP event: CONNECTION_STATE_EVT
I (2331) BT_APP_HF: --connection state SLC_CONNECTED, peer feats 0xff, chld_feats 0x4010
```

**Note: Only after Hands-free Profile(HFP) service is initialized and a service level connection exists between an HF Unit and an AG device, could other commands be available.**

You can type `hf dis;` to disconnect with the connected HF Unit device, and log prints such as:

```
E (100147) CNSL: Command [hf dis;]
disconnect
W (77321) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 2 closed: Closed (res: 19)
I (77321) BT_APP_HF: APP HFP event: CONNECTION_STATE_EVT
I (77321) BT_APP_HF: --connection state DISCONNECTED, peer feats 0x0, chld_feats 0x0
W (77381) BT_RFCOMM: rfc_find_lcid_mcb LCID reused LCID:0x41 current:0x0
W (77381) BT_RFCOMM: RFCOMM_DisconnectInd LCID:0x41
```

### Audio Connection and Disconnection

You can type `hf cona;` to establish the audio connection between HF Unit and AG device. Also, you can type `hf disa;` to close the audio data stream.

#### Scenarios for Audio Connection

- Answer an incoming call
- Enable voice recognition
- Dial an outgoing call

#### Scenarios for Audio Disconnection

- Reject an incoming call
- Disable the voice recognition

#### Choice of Codec

ESP32 supports both CVSD and mSBC codec. HF Unit and AG device determine which codec to use by exchanging features during service level connection. The choice of codec also depends on the your configuration in `menuconfig`.

Since CVSD is the default codec in HFP, we just show the scenarios using mSBC:

- If you enable `BT_HFP_WBS_ENABLE` in `menuconfig`, mSBC will be available.
- If both HF Unit and AG support mSBC and `BT_HFP_WBS_ENABLE` is enabled, ESP32 chooses mSBC.
- If you use PCM data path, mSBC is not available.

### Answer or Reject an Incoming Call

#### Answer an Incoming Call

You can type `hf ac;` to answer an incoming call and log prints such as:

```
E (1066280) CNSL: Command [hf ac;]
Answer Call from AG.
W (1066280) BT_APPL: BTA_AG_SCO_CODEC_ST: Ignoring event 1
I (1067200) BT_APP_HF: APP HFP event: BCS_EVT
I (1067200) BT_APP_HF: --AG choose codec mode: CVSD Only
E (1067230) BT_BTM: btm_sco_connected, handle 180
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State connected
```

#### Reject an Incoming Call

You can type `hf rc;` to reject an incoming call and log prints such as:

```
E (10040) CNSL: Command [hf rc;]
Reject Call from AG.
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State disconnected
```

#### End a Call

You can type `hf end;` to end the current ongoing call and log prints such as:

```
E (157741) CNSL: Command [hf end;]
End Call from AG.
W (157741) BT_APPL: BTA_AG_SCO_CLOSING_ST: Ignoring event 3
I (159311) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (159311) BT_APP_HF: --Audio State disconnected
I (159311) BT_APP_HF: --ESP AG Audio Connection Disconnected.
```

### Dial Number

You can type `hf d <num>;` to dial `<num>` from AG and log prints such as:

```
E (207351) CNSL: Command [hf d 123456;]
Dial number 123456
I (207361) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (207361) BT_APP_HF: --Audio State connecting
W (207361) BT_APPL: BTA_AG_SCO_OPENING_ST: Ignoring event 1
W (207371) BT_APPL: BTA_AG_SCO_OPENING_ST: Ignoring event 1
E (208801) BT_BTM: btm_sco_connected, handle 181
I (208811) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (208811) BT_APP_HF: --Audio State connected
```

### Volume Control

You can type `hf vu <tgt> <vol>;` to update the volume of a headset or microphone. The parameter should be set as follows:

- `<tgt>` : 0 - headset, 1 - microphone.
- `<vol>` : Integer among 0 - 15.

For example, `hf vu 0 9;` updates the volume of headset and the log on the AG side prints `Volume Update`, while on the HF Unit side the log prints:

```
E (17087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (17087) BT_HF: --volume_target: SPEAKER, volume 9
```

And also, `hf vu 1 9;` updates the volume of a microphone and the log on the HF Unit side prints:

```
E (32087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (32087) BT_HF: --volume_target: MICROPHONE, volume 9
```

#### Voice Recognition

You can type `hf vron;` to start the voice recognition and type `hf vroff;` to terminate this function in the AG device. Both commands will notify the HF Unit the status of voice recognition. For example, type `hf vron;` and the log will print:

```
E (244131) CNSL: Command [hf vron;]
Start Voice Recognition.
I (244141) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (244141) BT_APP_HF: --Audio State connecting
E (245301) BT_BTM: btm_sco_connected, handle 181
I (245311) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (245311) BT_APP_HF: --Audio State connected
```

#### Device Status Indication

You can type `hf ind <call> <ntk> <callsetup> <sig>` to send device status of AG to HF Unit. Log on AG prints such as:  `Device Indicator Changed!`  and on HF Unit side prints such as:

```
E (293641) BT_HF: APP HFP event: CALL_IND_EVT
E (293641) BT_HF: --Call indicator call in progress
E (293641) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (293651) BT_HF: --Call setup indicator INCOMING
E (293651) BT_HF: APP HFP event: SIGNAL_STRENGTH_IND_EVT
E (293661) BT_HF: -- signal strength: 5
```

**Note: The AG device sends only the changed status to the HF Unit.**

#### Send Extended AT Error Code

You can type `hf ate <rep> <err>` to send extended AT error code to HF Unit. The parameter should be set as follows:

- `<rep>` : integer among 0 - 7.
- `<err>` : integer among 0 - 32.

When you type `hf ate 7 7;` the log on the AG side prints `Send CME Error.` while on the HF Unit side prints:

```
E (448146) BT_HF: APP HFP event: AT_RESPONSE
E (448146) BT_HF: --AT response event, code 7, cme 7
```

#### In-Band Ring Tone Setting

You can type `hf iron;` to enable the in-band ring tone and type `hf iroff;` to disable it. The log on the AG side prints such as `Device Indicator Changed!` and on HF Unit side it prints such as:

```
E (19546) BT_HF: APP HFP event: IN-BAND_RING_TONE_EVT
E (19556) BT_HF: --in-band ring state Provided
```

## Troubleshooting

If you encounter any problems, please check if the following rules are followed:

- You should type the command in the terminal according to the format described in the commands help table. 
- Not all commands in the table are supported by the HF Unit.
- If you want to `hf con;` to establish a service level connection with a specific HF Unit, you should add the MAC address of the HF Unit in `app_hf_msg_set.c` for example: `esp_bd_addr_t peer_addr = {0xb4, 0xe6, 0x2d, 0xeb, 0x09, 0x93};`
- Use `esp_hf_client_register_callback()` and  `esp_hf_client_init();` before  establishing a service level connection.

## Example Breakdown

Due to the complexity of the HFP, this example has more source files than other bluetooth examples. To show the functions of HFP in a simple way, we use the Commands and Effects scheme to illustrate APIs of the HFP in ESP-IDF.

- The example will respond to user command through the UART console. Please go to `console_uart.c` for the configuration details.
- For the voice interface, ESP32 has provided PCM input/output signals which can be directed to GPIO pins. So, please go to `gpio_pcm_config.c` for the configuration details.
- If you want to update the command table, please refer to `app_hf_msg_set.c`.
- If you want to update the command parse rules, please refer to `app_hf_msg_prs.c`.
- If you want to update the responses of the AG or want to update the log, please refer to `bt_app_hf.c`.
- The task configuration part is in `bt_app_core.c`.