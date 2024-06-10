| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB: Enumeration Driver - Local Test with emulated device based on TinyUSB

## Introduction

To verify the process how Enumeration Driver handles different scenarios the artificial approach of emulation USB device with TinyUSB is used.
This test helps to verify all possible scenarios of detaching device under the enumeration process.

## Key Terms

**Mocked Device** - A esp32-cpu based board, which represents the USB device, build on TinyUSB component (**tud_teardown()** feature is required, thus the version of the component should be higher than v0.17.0). The Mocked Device is used to emulate different scenarios with ability of controlling the USB Enumeration flow and detach the Mocked Device when necessary.

**Host** - A esp32-cpu based board with USB Host functionality. Used to verify the part of the USB Host Library - Enumeration Driver.

## Test Hardware requirements

For testing purpose two esp32-cpu based hardware with USB-OTG support are required (One for Host, one for Device (mocked dev)):

```
+----------------+
| USB Host Board |
| +------------+ |      USB
| | USB-to-UART|========++
| +------------+ |      ||
| +------------+ |      \/
| |  USB OTG   | |      \/
| +------------+ |      ||  +-----------------+
+-------||-------+      ++==|                 |
        /\                  |     PC Host     |
        /\ USB              |  (Test runner)  |
+-------||-------+      ++==|                 |
|       ||       |      ||  +-----------------+
| +------------+ |      /\
| |  USB OTG   | |      /\
| +------------+ |      ||
| +------------+ |      ||
| | USB-to-UART|========++
| +------------+ |      USB
| USB Dev Board  |
|  (mocked dev)  |
+----------------+
```

## Mocked Device

Implementation could be found in [mock_dev.c](main/mock_dev.c) and [mock_dev.h](main/mock_dev.h).

The Mocked Device has a Final State Machine (FSM), which could be configured to stop at any stage and run the teardown process after. This flow represents the emulation of device detachment.

FSM is described in the **mock_dev_stage_t** structure and could be found in [mock_dev.h](main/mock_dev.h).

Available stages: 
- Request Short Device Descriptor
- Request Full Device Descriptor
- Request Short Configuration Descriptor
- Request Full Configuration Descriptor
- Request Short LANGID Table
- Request Full LANDID Table
- Request Short Manufacturer String Descriptor
- Request Full Manufacturer String Descriptor
- Request Short Product String Descriptor
- Request Full Product String Descriptor
- Request Short Serial String Descriptor
- Request Full Serial String Descriptor
- Set configuration

Not covered stages:
- Device Reset (First and Second)
- Set Address

These stages are handled internally in TinyUSB and there is no user callbacks available. That means, that without specific changes in the TinyUSB component code, it is impossible to achieve emulation of device detaching during these stages.



### Mocked Device - Stages

To specify the descriptor, which should be used on every stage, there is a possibility to configure the pointer at every stage.

Example of fully-defined non-detachable Mocked Device configuration:

``` c
static const tusb_desc_device_t mock_device_desc = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x0001,
    .idProduct = 0x0002,
    .bcdDevice = 0x0000,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

static const uint8_t mock_config_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 0, 0, TUD_CONFIG_DESC_LEN, TUSB_DESC_CONFIG_ATT_SELF_POWERED | TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
};

static const char *mock_langid_str_desc = {
    (char[]){0x09, 0x04},       // 0: LANGID
};

static const char *mock_manu_str_desc = {
    "Espressif",                // 1: Manufacturer
};

static const char *mock_prod_str_desc = {
    "Enum Testing",             // 2: Product
};

static const char *mock_ser_str_desc = {
    "123456",                   // 3: Serials, should use chip ID
};

TEST_CASE("enum::complete", "[mock_enum_device]")
{
    mock_dev_cfg_t mock_dev_cfg = {
            .enumeration = {
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
                    .desc = (const uint8_t *) &mock_device_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
                    .desc = (const uint8_t *) &mock_device_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC] = {
                    .desc = mock_config_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC] = {
                    .desc = mock_config_desc,
                },
                // String Descriptors
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE] = {
                    .str_desc = mock_langid_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_LANGID_TABLE] = {
                    .str_desc = mock_langid_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_MANU_STR_DESC] = {
                    .str_desc = mock_manu_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_MANU_STR_DESC] = {
                    .str_desc = mock_manu_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_PROD_STR_DESC] = {
                    .str_desc = mock_prod_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_PROD_STR_DESC] = {
                    .str_desc = mock_prod_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_SHORT_SER_STR_DESC] = {
                    .str_desc = mock_ser_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_REQ_FULL_SER_STR_DESC] = {
                    .str_desc = mock_ser_str_desc,
                },
                .stage_config[MOCK_DEV_STAGE_SET_CONFIG] = {
                    .desc = mock_config_desc,
                }
            },
        };
        
    // Test remained logic
}
```

Every stage can be stopped with providing the special flag **stop_test**.

To specify the test, provide the following configuration to teardown the Mocked Device after getting the request:

```
mock_dev_cfg_t mock_dev_cfg = {
    .enumeration = {
        .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
            .desc = NULL,
            .stop_test = true,
        },
    },
};

```

If we want to emulate the device detachment during the requesting of Full Device Descriptor, we can change the test configuration accordingly:

```
mock_dev_cfg_t mock_dev_cfg = {
    .enumeration = {
        .stage_config[MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC] = {
            .desc = (const uint8_t *) &mock_device_desc,
        },
        .stage_config[MOCK_DEV_STAGE_REQ_FULL_DEV_DESC] = {
            .desc = NULL,
            .stop_test = true,
        },
    },
};

```

### USB Host Library

To verify the USB Host Library - Enumeration Driver, **usb_host_lib** example is used.

## Run Test

When all the hardware have been configured and prepared, the Test can be run via following commands. As the Test requires both Host and Device parts, the following description will be for Host and Device.

The description is provided, assuming that the test is run under Linux and Host board has a /dev/ttyACM0 path, Mocked Device has a /dev/ttyUSB0 path, and we are in the esp-idf root folder.

To run the pytest, esp-idf must be installed and configured with ```--enable-pytest```.

### Prepare Host

Host part is controlled by visual control of debug output from the Host.

The following description is based on the usb_host_lib example as the application on the Host side.

To run the example on the Host:

```bash
cd examples/peripherals/usb/host/usb_host_lib
idf.py set-target esp32s3
idf.py -p /dev/ttyACM0 flash monitor
```

### Run Mocked Device

To run the Device part:

```bash
cd components/usb/test_apps/enum
idf.py set-target esp32s3
idf.py build
pytest --target=esp32s3 --port=/dev/ttyUSB0
```

After running the pytest on the device, the debug output from the Host works as the usb_host_lib example:

```
I (270) main_task: Started on CPU0
I (280) main_task: Calling app_main()
I (280) USB host lib: USB host library example
I (280) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2
I (290) USB host lib: Installing USB Host Library
I (320) CLASS: Registering Client
I (700) CLASS: Opening device at address 1
I (700) CLASS: Getting device information
I (700) CLASS: 	Full speed
I (700) CLASS: 	Parent info:
I (700) CLASS: 		Port: ROOT
I (700) CLASS: 	bConfigurationValue 1
I (700) CLASS: Getting device descriptor
*** Device descriptor ***
bLength 18
bDescriptorType 1
bcdUSB 2.00
bDeviceClass 0xef
bDeviceSubClass 0x2
bDeviceProtocol 0x1
bMaxPacketSize0 64
idVendor 0x1
idProduct 0x2
bcdDevice 0.00
iManufacturer 1
iProduct 2
iSerialNumber 3
bNumConfigurations 1
I (90980) CLASS: Getting config descriptor
*** Configuration descriptor ***
bLength 9
bDescriptorType 2
wTotalLength 9
bNumInterfaces 0
bConfigurationValue 1
iConfiguration 0
bmAttributes 0xe0
bMaxPower 100mA
I (91000) CLASS: Getting Manufacturer string descriptor
Espressif
I (91010) CLASS: Getting Product string descriptor
Enum Testing
I (91020) CLASS: Getting Serial Number string descriptor
123456
```

Which means that the Mocked Device is connected to Host and the enumeration process was completed.
Different scenarios are being tested during the test run, so there could be errors in the log
That means, that the device emulates the specific behavior, to cause this error.

Important things are:
- During the test there must be no KERNEL PANIC, CPU reset etc. (not to miss the reset, it is better to configure the Panic handler behaviour to: Print registers and halt)
- The Host application should be responsive after the test. (In terms of usb_host_lib example, that means handle new device connection and show the descriptors from the connected device).

"Normal" test flow error are:

```
E (116330) USBH: Dev 28 EP 0 STALL
```

Or

```
E (96200) ENUM: Short dev desc has wrong bDescriptorType
E (96200) ENUM: [0:0] CHECK_SHORT_DEV_DESC FAILED
E (96820) ENUM: Full dev desc has wrong bDescriptorType
E (96820) ENUM: [0:0] CHECK_FULL_DEV_DESC FAILED
W (97470) ENUM: Device has more than 1 configuration
E (97470) USBH: Dev 16 EP 0 STALL
E (97470) ENUM: Bad transfer status 4: CHECK_SHORT_CONFIG_DESC
E (97470) ENUM: [0:0] CHECK_SHORT_CONFIG_DESC FAILED
W (98120) ENUM: [0:0] Unexpected (8) device response length (expected 16)
E (98120) ENUM: Device returned less bytes than requested
E (98120) ENUM: [0:0] CHECK_SHORT_CONFIG_DESC FAILED
W (98770) ENUM: [0:0] Unexpected (8) device response length (expected 17)
E (98770) ENUM: Device returned less bytes than requested
E (98770) ENUM: [0:0] CHECK_FULL_CONFIG_DESC FAILED
E (99420) ENUM: Short config desc has wrong bDescriptorType
E (99420) ENUM: [0:0] CHECK_SHORT_CONFIG_DESC FAILED
E (100080) ENUM: Full config desc has wrong bDescriptorType
E (100080) ENUM: [0:0] CHECK_FULL_CONFIG_DESC FAILED
E (100660) USBH: Dev 0 EP 0 STALL
E (100660) ENUM: Bad transfer status 4: CHECK_SHORT_DEV_DESC
E (100660) ENUM: [0:0] CHECK_SHORT_DEV_DESC FAILED
E (101280) USBH: Dev 16 EP 0 STALL
E (101280) ENUM: Bad transfer status 4: CHECK_FULL_DEV_DESC
E (101280) ENUM: [0:0] CHECK_FULL_DEV_DESC FAILED
E (101930) USBH: Dev 16 EP 0 STALL
E (101930) ENUM: Bad transfer status 4: CHECK_SHORT_CONFIG_DESC
E (101930) ENUM: [0:0] CHECK_SHORT_CONFIG_DESC FAILED
E (102580) USBH: Dev 16 EP 0 STALL
E (102580) ENUM: Bad transfer status 4: CHECK_FULL_CONFIG_DESC
E (102580) ENUM: [0:0] CHECK_FULL_CONFIG_DESC FAILED
E (103230) USBH: Dev 16 EP 0 STALL
```

Or

```
E (121120) USBH: Dev 33 EP 0 STALL
E (121120) ENUM: Bad transfer status 4: CHECK_CONFIG
E (121120) ENUM: [0:0] CHECK_CONFIG FAILED
```

