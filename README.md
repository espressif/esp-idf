# Espressif IoT Development Framework

This is a fork of esp-idf. The sole purpose is to provide patches (included in the root directory), to enable multi-CDC Channels in tinyusb. Then, when you plug ex. esp32s2 into a PC, you will see several independent usb-cdc-acm serial ports appear. The theoretical maximum allowed by the usb specification is 5 channels, but due to hardware limitations of esp32s2 (only 5 active usb IN-endpoints simultaneously allowed) maximally 3 are actually usable. The others might partially work in one direction though.


# How to Install:
The easiest way is to look under "Releases" to the right of this github page. Look for the Multi-CDC release tagged with v4.3.1.1. It is the same release v4.3.1 zip-file downloadable from the espressif website, but it has the patches already applied & repacked.

If you want the manual method instead:
Apply the patch called "multi-cdc_esp-idf.patch" into the esp-idf folder in your system, by copying it there, then using the following command:
```
git apply multi-cdc_esp-idf.patch
```
Then to into components/tinyusb/tinyusb. Copy the file "multi-cdc_tinyusb.patch" into it, then run the following command there:
```
git apply multi-cdc_tinyusb.patch
```

# How to Use:
Now, whenever you start menuconfig, extra options will appear under TinyUSB when you enable the CDC option. It will ask you how many CDC channels to set up. Note a value of 0 or more than 5, won't compile. If you don't want CDC, then disable the option entirely instead. Also, note that if you choose more than 3, all those above 3 won't work partially or at all.
After finishing menuconfig, you initialize tinyusb and the cdc as you would normally, except you have to call tusb_cdc_acm_init(...) for each CDC channel, while adjusting the value of tinyusb_config_cdcacm_t.cdc_port on each call, to reflect the index of the CDC channel. So if you have 3 channels, you have to call it for cdc_port=0, cdc_port=1, and cdc_port=2. The callbacks other parameters can be the same or different. Note that the callback functions always have an "itf" parameter. This value will be equal to the index of the corresponding CDC channel involved in the event. So if line state changes on the 2nd CDC channel, then itf will have a value of 1. Likewise, when writing to cdc, you have to specify the correct itf index. It is no longer always 0.

For an example, see https://github.com/moefear85/ESPill-Godzilla.


# Issues:
At the last second, I decided to move the endpoint addresses for MSC & HID upwards so they don't conflict with the CDC endpoint addresses even if you choose 5 channels. Later inspection revealed that this might not be compatible without further modifications to tinyUSB. So MSC & HID might not work, even if CDC is fully disabled. I will fix or revert this issue later if necessary. Even if they do work, you can't use 3 channels simutlaneously with MSC & HID. MSC will require 1 in-endpoint, and hid does too, while each CDC channel  requires 2 in-endpoints, while the esp32s2 usb only provides a budget of 5 simultaneously active in-endpoints (or 6 depending on whether the control endpoint is counted with them).
