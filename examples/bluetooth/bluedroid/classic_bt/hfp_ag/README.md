| Supported Targets | ESP32 |
| ----------------- | ----- |

# Hands-Free Audio Gateway (HF-AG)

This example is to show how to use the APIs of Hands-Free Audio Gateway (hf_ag) Component and the effects of them by providing a set of commands. You can use this example to communicate with a device that implements Hands-Free Client Role (e.g. a headphone set).

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate this example, it should be connected to a Hands-Free Client running on a Headphone/Headset or on another ESP32 development board loaded with [hfp_hf](../hfp_hf) example of ESP-IDF.

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

### Special Configurations for HFP

#### Data Path

ESP32 HFP supports two types of audio datapath: PCM and HCI.

The default configuration is `PCM`, if you want to use `vHCI` you should configure the data path before building and downloading the binary.

- PCM: To use PCM, audio stream is directed from Bluetooth controller to the specific GPIO pins you set in the demo, and you should link these GPIO pins to a speaker via I2S port. The audio data will not go through the `Bluedroid`. In menuconfig, you should choose PCM in `menuconfig` path:

    `Component config --> Bluetooth controller --> BR/EDR Sync(SCO/eSCO) default data path --> PCM`

    and also

    `Component config --> Bluetooth --> Bluedroid Options --> Hands Free/Handset Profile --> audio(SCO) data path --> PCM`.

- vHCI: To use vHCI, audio data stream will be directed from Bluetooth Controller through vHCI on ESP32 and go through the Bluedroid to the Application layer. In menuconfig, you should choose vHCI in `menuconfig` path:

    `Component config --> Bluetooth controller --> BR/EDR Sync(SCO/eSCO) default data path --> HCI`

    and also

    `Component config --> Bluetooth --> Bluedroid Options --> Hands Free/Handset Profile --> audio(SCO) data path --> HCI`.

### Codec Choice

ESP32 supports two types of codec for HFP audio data: `CVSD` and `mSBC`.

`CVSD` is the default setting and is also the widely used codec for voice audio. But, `mSBC` is designed to have a better voice quality through `HFP`. To select which one is in use, we provide `Wide Band Speech` item in the `menuconfig`:

`Component config --> Bluetooth --> Bluedroid Options --> Wide Band Speech.`

Switching on the `Wide Band Speech` means that the preferred codec is `mSBC`, but which one is actually being used also depends on the `Data Path` configuration.

- If you choose `PCM` for datapath, you can only use `CVSD` and hardware is responsible for the codec job. In the meanwhile, you cannot use `mSBC` by switching `Wide Band Speech` on, because the `mSBC` is implemented in the Bluedroid (Bluetooth Host Stack) by software.

- If you choose `vHCI` for datapath with `Wide Band Speech` on, codec job is done in the Bluedroid and mSBC is being used.

- If you choose `vHCI` for datapath with `Wide Band Speech` off, hardware is responsible for the codec job and `CVSD` is in use.

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
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.

 ==================================================
 |       Steps to test hfp_ag                     |
 |                                                |
 |  1. Print 'help' to gain overview of commands  |
 |  2. Setup a service level connection           |
 |  3. Run hfp_ag to test                         |
 |                                                |
 =================================================

```

### Service Level Connection and Disconnection

You can type `con` to establish a service level connection with HF Unit device and log prints such as:

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

You can type `dis` to disconnect with the connected HF Unit device, and log prints such as:

```
disconnect
W (77321) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 2 closed: Closed (res: 19)
I (77321) BT_APP_HF: APP HFP event: CONNECTION_STATE_EVT
I (77321) BT_APP_HF: --connection state DISCONNECTED, peer feats 0x0, chld_feats 0x0
W (77381) BT_RFCOMM: rfc_find_lcid_mcb LCID reused LCID:0x41 current:0x0
W (77381) BT_RFCOMM: RFCOMM_DisconnectInd LCID:0x41
```

### Audio Connection and Disconnection

You can type `cona` to establish the audio connection between HF Unit and AG device. Also, you can type `disa` to close the audio data stream.

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

You can type `ac` to answer an incoming call and log prints such as:

```
Answer Call from AG.
W (1066280) BT_APPL: BTA_AG_SCO_CODEC_ST: Ignoring event 1
I (1067200) BT_APP_HF: APP HFP event: BCS_EVT
I (1067200) BT_APP_HF: --AG choose codec mode: CVSD Only
E (1067230) BT_BTM: btm_sco_connected, handle 180
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State connected
```

#### Reject an Incoming Call

You can type `rc` to reject an incoming call and log prints such as:

```
Reject Call from AG.
I (1067240) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (1067240) BT_APP_HF: --Audio State disconnected
```

#### End a Call

You can type `end` to end the current ongoing call and log prints such as:

```
End Call from AG.
W (157741) BT_APPL: BTA_AG_SCO_CLOSING_ST: Ignoring event 3
I (159311) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (159311) BT_APP_HF: --Audio State disconnected
I (159311) BT_APP_HF: --ESP AG Audio Connection Disconnected.
```

### Dial Number

You can type `d <num>` to dial `<num>` from AG and log prints such as:

```
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

You can type `vu <tgt> <vol>` to update the volume of a headset or microphone. The parameter should be set as follows:

- `<tgt>` : 0 - headset, 1 - microphone.
- `<vol>` : Integer among 0 - 15.

For example, `vu 0 9;` updates the volume of headset and the log on the AG side prints `Volume Update`, while on the HF Unit side the log prints:

```
E (17087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (17087) BT_HF: --volume_target: SPEAKER, volume 9
```

And also, `vu 1 9` updates the volume of a microphone and the log on the HF Unit side prints:

```
E (32087) BT_HF: APP HFP event: VOLUME_CONTROL_EVT
E (32087) BT_HF: --volume_target: MICROPHONE, volume 9
```

#### Voice Recognition

You can type `vron` to start the voice recognition and type `vroff` to terminate this function in the AG device. Both commands will notify the HF Unit the status of voice recognition. For example, type `vron` and the log will print:

```
Start Voice Recognition.
I (244141) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (244141) BT_APP_HF: --Audio State connecting
E (245301) BT_BTM: btm_sco_connected, handle 181
I (245311) BT_APP_HF: APP HFP event: AUDIO_STATE_EVT
I (245311) BT_APP_HF: --Audio State connected
```

#### Device Status Indication

You can type `ciev <ind_type> <value>` to send device status of AG to HF Unit. Log on AG prints such as:  `Device Indicator Changed!`  and on HF Unit side prints such as:

```
I (106167) BT_HF: APP HFP event: CALL_SETUP_IND_EVT
I (106167) BT_HF: --Call setup indicator INCOMING
```

**Note: The AG device sends only the changed status to the HF Unit.**

#### Send Extended AT Error Code

You can type `ate <rep> <err>` to send extended AT error code to HF Unit. The parameter should be set as follows:

- `<rep>` : integer among 0 - 7.
- `<err>` : integer among 0 - 32.

When you type `ate 7 7;` the log on the AG side prints `Send CME Error.` while on the HF Unit side prints:

```
E (448146) BT_HF: APP HFP event: AT_RESPONSE
E (448146) BT_HF: --AT response event, code 7, cme 7
```

#### In-Band Ring Tone Setting

You can type `iron` to enable the in-band ring tone and type `iroff` to disable it. The log on the AG side prints such as `Device Indicator Changed!` and on HF Unit side it prints such as:

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
- If you want to update the responses of the AG or want to update the log, please refer to `bt_app_hf.c`.
- The task configuration part is in `bt_app_core.c`.