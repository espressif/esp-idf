| Supported Targets | ESP32 |
| ----------------- | ----- |

AVRCP-CT-METADATA EXAMPLE
======================

This is the example of API implementing Audio/Video Remote Control Profile to get metadata.

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)
- [bredr_app_common_utils](../common/bredr_app_common_utils)
- [a2dp_sink_common_utils](../common/a2dp_utils/a2dp_sink_common_utils)
- [a2dp_sink_int_codec_utils](../common/a2dp_utils/a2dp_sink_int_codec_utils)
- [a2dp_sink_ext_codec_utils](../common/a2dp_utils/a2dp_sink_ext_codec_utils)
- [avrcp_common_utils](../common/avrcp_utils/avrcp_common_utils)
- [avrcp_metadata_utils](../common/avrcp_utils/avrcp_metadata_utils)

```
+---------------------------------------------------+---------------------+
|                avrcp_metadata_utils               |                     |
+---------------------------------------------------+                     |
|                 avrcp_common_utils                |                     |
+-------------------------+-------------------------+                     |
|a2dp_sink_int_codec_utils|a2dp_sink_ext_codec_utils|  bt_app_core_utils  |
+-------------------------+-------------------------+                     |
|               a2dp_sink_common_utils              |                     |
+---------------------------------------------------+                     |
|               bredr_app_common_utils              |                     |
+---------------------------------------------------+---------------------+
```

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

## Example Output

The output when receiving metadata:

```
I (81700) RC_CT: AVRC event notification: 2
I (81740) RC_CT: AVRC metadata rsp: attribute id 0x1, 
I (81740) RC_CT: AVRC metadata rsp: attribute id 0x2, 
I (81740) RC_CT: AVRC metadata rsp: attribute id 0x4, 
I (81750) RC_CT: AVRC metadata rsp: attribute id 0x20,
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
