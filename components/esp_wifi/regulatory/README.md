# REGULATORY

## Introduction
`esp_wifi_regulatory.txt` and `esp_wifi_regulatory.c` defines wireless communication regulations for different countries and regions to ensure that the device operates in compliance with local regulations.

`esp_wifi_regulatory.c` is generated from `esp_wifi_regulatory.txt` by using the `reg2fw.py` script.

The num of all suppported countries in `esp_wifi_regulatory.txt` is `WIFI_MAX_SUPPORT_COUNTRY_NUM` that is defined in `esp_wifi_types_generic.h`

- Generate `esp_wifi_regulatory.c`
  - `cd ~/esp-idf/components/esp_wifi/regulatory`
  - `python reg2fw.py`