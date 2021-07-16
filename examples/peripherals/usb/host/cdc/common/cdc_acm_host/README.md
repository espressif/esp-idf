# USB Host CDC-ACM Class Driver

This directory contains an implementation of a USB CDC-ACM Host Class Driver that is implemented on top of the [USB Host Library](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/usb_host.html).

## Supported Devices

The CDC-ACM Host driver supports the following types of CDC devices:

1. CDC-ACM devices
2. CDC-like vendor specific devices (usually found on USB to UART bridge devices)

### CDC-ACM Devices

The CDC-ACM Class driver supports CDC-ACM devices that meet the following requirements:
- The device class code must be set to the CDC class `0x02` or implement Interface Association Descriptor (IAD)
- The CDC-ACM must contain the following interfaces:
    - A Communication Class Interface containing a management element (EP0) and may also contain a notification element (an interrupt endpoint). The driver will check this interface for CDC Functional Descriptors.
    - A Data Class Interface with two BULK endpoints (IN and OUT). Other transfer types are not supported by the driver

### CDC-Like Vendor Specific Devices

The CDC-ACM Class driver supports CDC-like devices that meet the following requirements:
- The device class code must be set to the vendor specific class code `0xFF`
- The device needs to provide and interface containing the following endpoints:
    - (Mandatory) Two Bulk endpoints (IN and OUT) for data
    - (Optional) An interrupt endpoint (IN) for the notification element

For CDC-like devices, users are responsible for ensuring that they only call APIs (e.g., `cdc_acm_host_send_break()`) that are supported by the target device.


## Usage

The following steps outline the typical API call pattern of the CDC-ACM Class Driver

1. Install the USB Host Library via `usb_host_install()`
2. Install the CDC-ACM driver via `cdc_acm_host_install()`
3. Call `cdc_acm_host_open()`/`cdc_acm_host_open_vendor_specific()` to open a target CDC-ACM/CDC-like device. These functions will block until the target device is connected
4. To transmit data, call `cdc_acm_host_data_tx_blocking()`
5. When data is received, the driver will automatically run the receive data callback
6. An opened device can be closed via `cdc_acm_host_close()`
7. The CDC-ACM driver can be uninstalled via `cdc_acm_host_uninstall()`

## Examples

- For an example with a CDC-ACM device, refer to [cdc_acm_host](../../cdc_acm_host)
- For an example with a CDC-like device, refer to [cdc_acm_host_bg96](../../cdc_acm_bg96)
