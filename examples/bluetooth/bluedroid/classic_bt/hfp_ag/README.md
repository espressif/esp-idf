# Hands-Free Audio Gateway

This example is to show how to use the APIs of Hands-Free (HF) Audio Gateway (AG) Component and the effects of them with the help of user commands. You can use this example to communicate with a Hands-Free Unit (e.g. a headphone set). This example uses UART as a transportation of user commands and uses GPIO for PCM audio data stream.

## How to use example

### Hardware Required

If possible, example should be able to run on any commonly available ESP32 development board and is supposed to connect to _Hands Free Unit example (hfp_hf)_ in ESP-IDF.

### Configure the project

```
idf.py menuconfig
```

- Enable `Classic Bluetooth`,  `Hands Free/Handset` and `Aduio Gateway` under `Component config ---> Bluetooth ---> Bluedroid Options`.
- When using PCM as the data path and this example configures PCM audio data to GPIO pins. You can link the GPIO pins to a speaker via I2S port. PCM data path does not support mSBC codec but CVSD codec.
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

When you run this example the command table will prints like:

```
########################################################################
HFP AG command usage manual
HFP AG commands begins with "hf" and end with ";"
Supported commands are as follows, arguments are embraced with < and >

hf con;                   -- setup connection with peer device
hf dis;                   -- release connection with peer device
hf cona;                  -- setup audio connection with peer device
hf disa;                  -- release connection with peer device
hf vron;                  -- start voice recognition
hf vroff;                 -- stop voice recognition
hf vu <tgt> <vol>;        -- volume update
     tgt: 0-speaker, 1-microphone
     vol: volume gain ranges from 0 to 15
hf ind <call> <ntk> <callsetup> <sig>;       -- unsolicited notify device notification to HF Client
     call: call status [0,1]
     callsetup: call setup status [0,3]
     ntk: network status [0,1]
     sig: signal strength value from 0~5
hf ate <rep> <err>;       -- send extended at error code
     rep: response code from 0 to 7
     err: error code from 0 to 32
hf iron;                  -- inband ring tone provided
hf iroff;                 -- inband ring tone not provided
hf ac;                    -- Answer Incoming Call from AG
hf rc;                    -- Reject Incoming Call from AG
hf d <num>;               -- Dial Number by AG, e.g. hf d 11223344
hf end;                   -- End up a call by AG
hf h;                     -- to see the command for HFP AG
########################################################################
```

The commands help table will print out in monitor whenever you type `hf h;` or input a command that is not required by the command parse rule.

### Service Level Connection and Disconnection

You can type `hf con;` to establish a service level connection with HF Unit device and log prints like:

```
E (100147) CNSL: Command [hf dis;]
disconnect
W (100427) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 3 closed: Closed (res: 19)
W (100427) BT_APPL: BTA_HF_CLIENT_SCO_SHUTDOWN_ST: Ignoring event 3
E (100427) BT_HF: APP HFP event: CONNECTION_STATE_EVT
E (100437) BT_HF: --connection state disconnected, peer feats 0x0, chld_feats 0x0
```

**Note: Only after HF service is initialized and a service level connection exists between the HF Unit and AG could other commands be available.**

You can type `hf dis;` to disconnect with the connected HF Unit device, and log prints like:

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

#### Use Scenarios for Audio Connection

- Answer an incoming call
- Enable voice recognition
- Dial an outgoing call

#### Use Scenarios for Audio Disconnection

- Reject an incoming call
- Disable the voice recognition

#### Audio Data path

ESP32 supports two types of audio data path: PCM and HCI.

- PCM : When using PCM, audio data stream is mapped to GPIO pins and you should link these GPIO pins to a speaker via I2S port. 
- HCI : When using HCI, audio data stream acts in "loopback" mode. For example, you can hear your own voice when you place a call to a phone connected with a ESP32 development borad.

#### Codec

ESP32 supports both CVSD and mSBC codec. HF Unit and AG device determine which codec to use by exchanging features in the process of a service level connection. The choice of codec also depends on the user's configuration in `menuconfig`. 

Since CVSD is the default codec in HFP, we just show the scenarios using mSBC :

- If you enable `BT_HFP_WBS_ENABLE` in `menuconfig`, mSBC will be available.
- If both HF Unit and AG support mSBC and also `BT_HFP_WBS_ENABLE` is enabled, ESP32 chooses mSBC.
- If you use PCM data path, mSBC is not available.

### Answer or Reject an Incoming Call

#### Answer an Incoming Call

You can type `hf ac;` to answer an incoming call and log prints like:

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

You can type `hf rc;` to reject an incoming call and log prints like:

```
E (10040) CNSL: Command [hf rc;]
Reject Call from AG.
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State disconnected
```

#### End Up a Call

You can type `hf end;` to end up the current ongoing call and log prints like:

```
E (40390) CNSL: Command [hf end;]
End Call from AG.
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State disconnected
```

### Dial Number

You can type `hf d <num>;` to dial `<num>` from AG and log prints like:

```
E (105600) CNSL: Command [hf d 18629485549;]
Dial number 18629485549
I (105610) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (105610) BT_APP_HF: --Audio State connecting
I (106120) BT_APP_HF: APP HFP event: BCS_EVT
I (106130) BT_APP_HF: --AG choose codec mode: CVSD Only
E (106160) BT_BTM: btm_sco_connected, handle 180
I (106160) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (106160) BT_APP_HF: --Audio State connected

```

### Volume Control

You can type `hf vu <tgt> <vol>;` to sync volume gain of headset or microphone. The parameter set:

- `<tgt>` : 0 - headset, 1 - microphone.
- `<vol>` : Integer among 0 - 15.

For example, `hf vu 0 9;` sync the volume of headset and log on AG side prints `volume update`, on HF Unit side log prints like:

```
E (17087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (17087) BT_HF: --volume_target: SPEAKER, volume 9

```

And also, `hf vu 1 9;` sync the volume gain of microphone and log on HF  Unit side prints like:

```
E (32087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (32087) BT_HF: --volume_target: MICROPHONE, volume 9

```

#### Voice Recognition

You can type `hf vron;` to start the voice recognition of AG and type `hf vroff;` to end the voice recognition. Both command will notify the HF Unit the status of voice recognition. For example, type `hf vron;` and log prints like:

```
E (203128) CNSL: Command [hf vron;]
Start Voice Recognition.
I (203138) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (203138) BT_APP_HF: --Audio State connecting
I (203148) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1014138) BT_APP_HF: --Audio State connected

```

#### Notify Device Notification

You can type `hf ind <call> <ntk> <callsetup> <sig>` to send device status of AG to HF Unit. Log on AG will printout like:  `Device Indicator Changed!`  and on HF Unit side will prints like:

```
E (293641) BT_HF: APP HFP event: CALL_IND_EVT
E (293641) BT_HF: --Call indicator call in progress
E (293641) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
E (293651) BT_HF: --Call setup indicator INCOMING
E (293651) BT_HF: APP HFP event: SIGNAL_STRENGTH_IND_EVT
E (293661) BT_HF: -- signal strength: 5

```

**Note: AG only sends changed status to HF Unit.**

#### Send Extended AT Error Code

You can type `hf ate <rep> <err>` to send extended AT error coed to HF Unit. Parameter set:

- `<rep>` : integer among 0 - 7.
- `<err>` : integer among 0 - 32.

When you type `hf ate 7 7;` log on AG side prints like `Send CME Error.` and on HF Unit side prints like:

```
E (448146) BT_HF: APP HFP event: AT_RESPONSE
E (448146) BT_HF: --AT response event, code 7, cme 7

```

#### Inband Ring Tone Enable and Disable

You can type `hf iron;` to enable inband ring tone and type `hf iroff;` to disable inband ring tone. Log on AG side prints like `Device Indicator Changed!` and on HF Unit side prints like:

```
E (19546) BT_HF: APP HFP event: INBAND_RING_TONE_EVT
E (19556) BT_HF: --inband ring state Provided

```

## Troubleshooting

- You should type the command in the terminal according to the format described in the command help table. 
- Not all commands in the table are supported by HF Unit.
- If you want to use AG to establish a service level connection with HF Unit, you should add the MAC address of HF Unit in `hfp_hf/main/bt_app.c`, for example: `esp_bd_addr_t peer_addr = {0xb4, 0xe6, 0x2d, 0xeb, 0x09, 0x93};`
- Use `esp_hf_client_register_callback()` and  `esp_hf_client_init();` before  establishing a service level connection.

## Example Breakdown

This example has relatively more source files than other bluetooth examples because _Hands Free Profile_ is somehow complex. But we want to show the functions of _Hands Free Profile_ in a simple way, so we use the _Commands and Effects_ scheme to show the usage of APIs of HFP in ESP-IDF.

- The example will respond to user command through UART console. Please go to `hfp_hf/main/console_uart.c`  for the configuration details.
- For voice interface, ESP32 has provided PCM input/output signals which can be mapped to GPIO pins, please go to `hfp_hf/main/gpio_pcm_config.c` for the configuration details. 
- If you want to fix the command table, please refer to `hfp_hf/main/app_hf_msg_set.c`.
- If you want to fix the command parse rules, please refer to `hfp_hf/main/app_hf_msg_prs.c`.
- If you want to fix the responses of AG or want to fix the log, please refer to `hfp_hf/main/bt_app_hf.c`.
- Task configuration part is in `hfp_hf/main/bt_app_core.c`.