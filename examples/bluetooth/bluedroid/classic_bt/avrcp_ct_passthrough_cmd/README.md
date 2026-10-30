| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

AVRCP-PASSTHROUGH-COMMAND EXAMPLE
=================================

This is an example demonstrating how to use AVRCP Controller (CT) passthrough commands, such as PLAY/PAUSE/NEXT.

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)
- [bredr_app_common_utils](../common/bredr_app_common_utils)
- [avrcp_common_utils](../common/avrcp_utils/avrcp_common_utils)
- [avrcp_psth_cmd_utils](../common/avrcp_utils/avrcp_psth_cmd_utils)

Detailed information can be viewed through the [../common/README.md](../common/README.md).

## How to use this example

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output.

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

### Usage notes

- Pair a phone (or any AVRCP Target device) with ESP32/ESP32-S31. Once AVRCP connection is established, the example will automatically send a sequence of passthrough commands.
- Some phones may ignore certain passthrough commands (especially `VOL_UP`/`VOL_DOWN`). For volume control interoperability, refer to the `avrcp_absolute_volume` example.

## Example Output

The output when AVRCP connection is established and commands are sent:

```
I (12466) RC_PSTH_UTILS: AVRC conn_state event: state 1, [1c:8b:ef:c1:3d:89]
I (12466) RC_PSTH_UTILS: start music control simulation
I (12466) RC_PSTH_UTILS: PLAY
I (12486) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x44, key_state 0, rsp_code 9
I (12486) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x44, key_state 1, rsp_code 9
I (13466) RC_PSTH_UTILS: PAUSE
I (13486) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x46, key_state 0, rsp_code 9
I (13486) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x46, key_state 1, rsp_code 9
I (14466) RC_PSTH_UTILS: NEXT TRACK
I (14506) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x4b, key_state 0, rsp_code 9
I (14506) RC_PSTH_UTILS: AVRC passthrough rsp: key_code 0x4b, key_state 1, rsp_code 9
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

