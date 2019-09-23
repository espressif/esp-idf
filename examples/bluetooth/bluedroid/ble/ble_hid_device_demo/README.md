ESP-IDF BLE HID device demo
========================
This example Implemented BLE HID device profile related functions, in which the HID device has 
4 Reports (1 is mouse, 2 is keyboard and LED, 3 is Consumer Devices, 4 is Vendor devices). 
Users can choose different reports according to their own application scenarios.
BLE HID profile inheritance and USB HID class. 

ble_hidd_demo_main.c
==========================
This file is the demo to show how to used the HID(you can used it to connected to the smart phone act as the consumer device then can used the button to 
volume++ or volume-- etc., or connected to the Windows 10 PC act as a keyboard or mouse)

hidd_le_prf_int.h
==========================
This header file includes some HID profile related definitions.

esp_hidd_prf_api.h & esp_hidd_prf_api.c
===========================================
These files contains the the api of the HID profile

When you used the HID profile, you just need to added the esp_hidd_prf_api.h includes file and send the HID data used the function defined in the esp_hidd_prf_api.c file.

hid_dev.h & hid_dev.c
======================
These file define the HID spec related definitions

hid_device_le_prf.c
======================
This file is the HID profile definition file, it include the main function of the HID profile. 
It mainly includes how to create HID service. If you send and receive HID data and convert the data to keyboard keys, 
the mouse and consumer values are forwarded to the application.