Wi-Fi
======

:link_to_translation:`zh_CN:[中文]`


Wi-Fi Scan and Connect
------------------------

    The following types have been modified:

    - :component_file:`esp_wifi/include/esp_wifi_he_types.h`

        - :cpp:struct:`esp_wifi_htc_omc_t`:

            - ``uph_id``, ``ul_pw_headroom``, ``min_tx_pw_flag`` are deprecated.

    - :component_file:`esp_wifi/include/esp_wifi_types_generic.h`

        - :cpp:struct:`wifi_ap_record_t`:

            - The type of ``bandwidth`` has been changed from ``uint8_t`` to ``wifi_bandwidth_t``
