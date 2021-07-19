# USB Host MSC (Mass Storage Class) Driver

This directory contains an implementation of a USB Mass Storage Class Driver implemented on top of the [USB Host Library](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/usb_host.html).

MSC driver allows access to USB flash drivers using the BOT “Bulk-Only Transport” protocol and the Transparent SCSI command set.

## Usage

- First, usb host library has to be initialized by calling `usb_host_install`
- USB Host Library events have to be handled by invoking `usb_host_lib_handle_events` periodically.
  In general, an application should spawn a dedicated task handle USB Host Library events.
  However, in order to save RAM, an already existing task can also be used to call `usb_host_lib_handle_events`.
- Mass Storage Class driver is installed by calling `usb_msc_install` function along side with configuration.
- Supplied configuration contains user provided callback function invoked whenever MSC device is connected/disconnected
  and optional parameters for creating background task handling MSC related events. 
  Alternatively, user can call `usb_msc_handle_events` function from already existing task.
- After receiving `MSC_DEVICE_CONNECTED` event, user has to install device with `usb_msc_install_device` function,
  obtaining MSC device handle.
- USB descriptors can be printed out with `usb_msc_print_descriptors` and general information about MSC device retrieved
  with `from usb_msc_get_device_info` function.
- Obtained device handle is then used in helper function `usb_msc_vfs_register` mounting USB Disk to Virtual filesystem.
- At this point, standard C functions for accessing storage (`fopen`, `fwrite`, `fread`, `mkdir` etc.) can be carried out.
- In order to uninstall the whole USB stack, deinitializing counterparts to functions above has to be called in reverse order. 

## Known issues

- Driver only supports USB 2.0 flash drives using the BOT “Bulk-Only Transport” protocol and the Transparent SCSI command set
- Composite USB devices are not supported

## Troubleshooting

After connecting composite USB device, driver prints `COMPOSITE DEVICES UNSUPPORTED` 
