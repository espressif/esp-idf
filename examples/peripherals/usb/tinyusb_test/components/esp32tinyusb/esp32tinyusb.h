#ifndef  __ESP32S2_TINYUSB_H__
#define  __ESP32S2_TINYUSB_H__
// #
// # TinyUSB
// #
// CONFIG_USB_ENABLED=y
// CONFIG_USB_CDC_ENABLED=y
// CONFIG_USB_CDC_RX_BUFSIZE=64
// CONFIG_USB_CDC_TX_BUFSIZE=64
// CONFIG_USB_MSC_ENABLED=y
// CONFIG_USB_MSC_BUFSIZE=512
// # CONFIG_USB_HID_ENABLED is not set
// # CONFIG_USB_MIDI_ENABLED is not set
// # CONFIG_USB_CUSTOM_CLASS_ENABLED is not set
// # CONFIG_USB_DEBUG is not set

// #
// # Descriptor configuration
// #
// # CONFIG_USB_DESC_USE_ESPRESSIF_VID is not set
// CONFIG_USB_DESC_CUSTOM_VID=0x303A
// # CONFIG_USB_DESC_USE_DEFAULT_PID is not set
// CONFIG_USB_DESC_CUSTOM_PID=0x3000
// CONFIG_USB_DESC_BCDDEVICE=0x0100
// CONFIG_USB_DESC_MANUFACTURER_STRING="Espressif Systems"
// CONFIG_USB_DESC_PRODUCT_STRING="Espressif Device"
// CONFIG_USB_DESC_SERIAL_STRING="123456"
// CONFIG_USB_DESC_CDC_STRING="Espressif CDC Device"
// CONFIG_USB_DESC_MSC_STRING="Espressif MSC Device"
// # end of Descriptor configuration
// # end of TinyUSB
#include "sdkconfig.h"

// #ifndef CONFIG_USB_CDC_ENABLED
// #   define CONFIG_USB_CDC_ENABLED
// #define CFG_TUD_CDC 1
// #endif

// #ifdef CONFIG_USB_MSC_ENABLED
// #   undef CONFIG_USB_MSC_ENABLED 
// #endif

// #ifdef CONFIG_USB_HID_ENABLED
// #undef CONFIG_USB_HID_ENABLED
// #   undef CFG_TUD_HID 
// #   define CFG_TUD_HID 0 
// #endif

// #ifdef CONFIG_USB_MIDI_ENABLED
// #   undef CFG_TUD_MIDI 
// #   define CFG_TUD_MIDI 0 
// #endif

// #ifdef CONFIG_USB_CUSTOM_CLASS_ENABLED
// #   undef CONFIG_USB_CUSTOM_CLASS_ENABLED 
// #endif
// #include "tusb_config.h"

    #pragma once

extern "C" {
#include "tinyusb.h"
}

class TinyUSB
{
private:
    /* data */
public:
    TinyUSB();
    void init();
    void initCustom();
    void addCDCdescriptor();
    void addMSCdescriptor();
    void addMIDIdescriptor();
    void addHIDdescriptor();
};


#endif  // __ESP32S2_TINYUSB_H__
