COMMON CODE
======================

This folder provides the universal code for classic Bluetooth applications, and corresponding functions can be achieved by combining different parts.

## File Tree

The file tree of this folder is shown below.
- The [a2dp_utils](./a2dp_utils) folder contains the files of A2DP including `a2dp_sink_common_utils`, `a2dp_sink_ext_codec_utils` and `a2dp_sink_int_codec_utils` related files. The [a2dp_sink_common_utils](./a2dp_utils/a2dp_sink_common_utils) folder is about common handle function for A2DP sink event. The [a2dp_sink_ext_codec_utils](./a2dp_utils/a2dp_sink_ext_codec_utils) folder is about external codec of audio data. The [a2dp_sink_int_codec_utils](./a2dp_utils/a2dp_sink_int_codec_utils) folder is about internal codec of audio data.
- The [avrcp_utils](./avrcp_utils) folder contains the files of AVRCP including `avrcp_abs_vol_utils`, `avrcp_common_utils`, `avrcp_cover_art_utils` and `avrcp_metadata_utils` related files. The [avrcp_abs_vol_utils](./avrcp_utils/avrcp_abs_vol_utils) folder is about AVRCP absolute volume control event. The [avrcp_common_utils](./avrcp_utils/avrcp_common_utils) folder is about common handle function for AVRCP event. The [avrcp_cover_art_utils](./avrcp_utils/avrcp_cover_art_utils) folder is about AVRCP cover art event. The [avrcp_metadata_utils](./avrcp_utils/avrcp_metadata_utils) folder is about AVRCP metadata event.
- The [bredr_app_common_utils](./bredr_app_common_utils) folder contains the files of Classic Bluetooth common code including Classic Bluetooth common initialization and handle function about GAP and device.
- The [bt_app_core_utils](./bt_app_core_utils) folder contains the program core files.

```
└── common
    ├── a2dp_utils
    │   ├── a2dp_sink_common_utils
    │   ├── a2dp_sink_ext_codec_utils
    │   └── a2dp_sink_int_codec_utils
    ├── avrcp_utils
    │   ├── avrcp_abs_vol_utils
    │   ├── avrcp_common_utils
    │   ├── avrcp_cover_art_utils
    │   └── avrcp_metadata_utils
    ├── bredr_app_common_utils
    ├── bt_app_core_utils
    └── README.md
```

## How to use

The functional framework diagram is shown below. The `bt_app_core_utils` is responsible for all event scheduling. Specific functions can be implemented by general code combined with unique code. For instance, the internal decoding audio output of A2DP sink can be achieved through components such as `bredr_app_common_utils`, `a2dp_sink_common_utils`, and `a2dp_sink_int_codec_utils`. On this basis, adding the code `avrcp_common_utils` and `avrcp_abs_vol_utils` can enable volume adjustment, or incorporating `avrcp_common_utils` and `avrcp_metadata_utils` can facilitate the transmission of metadata.

```
+---------------------------------------------------+---------------------+
|      AVRCP feature(avrcp_utils/unique code)       |                     |
+-------------------------+-------------------------+                     |
|a2dp_sink_int_codec_utils|a2dp_sink_ext_codec_utils|                     |
+-------------------------+-------------------------+  bt_app_core_utils  |
|               a2dp_sink_common_utils              |                     |
+---------------------------------------------------+                     |
|               bredr_app_common_utils              |                     |
+---------------------------------------------------+---------------------+
```
