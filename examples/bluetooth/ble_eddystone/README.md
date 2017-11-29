ESP-IDF Eddystone demo
========================
This example demonstrates Eddystone-compatible BLE scanning of eddystone frame,including UID,URL.

Eddystone is an open beacon protocol specification from Google aimed at improving “proximity-based experiences” 
with support for both Android and iOS smart device platforms.

Learn more on https://developers.google.com/beacons and https://github.com/google/eddystone.

esp_eddystone_protocol.h
==========================
This header file includes some eddystone-protocol related definitions.

esp_eddystone_api.h & esp_eddystone_api.c
===========================================
These files contains the decoding and decoded result of an eddystone frame packet.

You just need to include esp_eddystone_protocol.h, esp_eddystone_api.h and esp_eddystone_api.c for development.

esp_eddystone_demo.c
======================
This is an example/demo of using esp_eddystone_protocol.h, esp_eddystone_api.h and esp_eddystone_api.c files to resolve eddystone packet.
