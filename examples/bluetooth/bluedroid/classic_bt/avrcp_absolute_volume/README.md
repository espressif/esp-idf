| Supported Targets | ESP32 |
| ----------------- | ----- |

AVRCP-ABSOLUTE-VOLUME EXAMPLE
======================

This is the example of API implementing Audio/Video Remote Control Profile to control absolute volume.

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)
- [bredr_app_common_utils](../common/bredr_app_common_utils)
- [a2dp_sink_common_utils](../common/a2dp_utils/a2dp_sink_common_utils)
- [a2dp_sink_int_codec_utils](../common/a2dp_utils/a2dp_sink_int_codec_utils)
- [a2dp_sink_ext_codec_utils](../common/a2dp_utils/a2dp_sink_ext_codec_utils)
- [avrcp_common_utils](../common/avrcp_utils/avrcp_common_utils)
- [avrcp_abs_vol_utils](../common/avrcp_utils/avrcp_abs_vol_utils)

```
+---------------------------------------------------+---------------------+
|                 avrcp_abs_vol_utils               |                     |
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

The output when the connection is established:

```
I (56320) RC_VC_SRV: Volume is set locally to: 3%
I (57160) RC_TG: AVRC register event notification: 13, param: 0x0
I (66320) RC_VC_SRV: Volume is set locally to: 7%
I (67160) RC_TG: AVRC register event notification: 13, param: 0x0
I (76320) RC_VC_SRV: Volume is set locally to: 11%
I (77160) RC_TG: AVRC register event notification: 13, param: 0x0
I (86320) RC_VC_SRV: Volume is set locally to: 15%
I (87160) RC_TG: AVRC register event notification: 13, param: 0x0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
