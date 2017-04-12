ESP-IDF GATT SERVER demo
========================

This is the demo for user to use ESP_APIs to create a GATT Server.

Options choose step:
    1. make menuconfig.
    2. enter menuconfig "Component config".
    3. enter menuconfig "Example 'GATT SERVER' Config".
    4. choose your options.

UPDATE NOTE
===========

2017-01-19:
    1. Use New APIs to set raw advertising data and raw scan response data.
    2. Could use macro CONFIG_SET_RAW_ADV_DATA (should use menuconfig) to config use raw advertising/scan_response
       or use structure do automatically config. The macro CONFIG_SET_RAW_ADV will effect both advertising data
       and scan_response data.

