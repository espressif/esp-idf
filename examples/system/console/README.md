# Console examples

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Examples in this directory illustrate the usage of the [Console Component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html#console) to create an interactive shell on the ESP chip.

## basic example

This example illustrates high-level Read-Eval-Print Loop API (`esp_console_repl`).

This example can be used with UART, USB_OTG or USB_SERIAL_JTAG peripherals. It works on all ESP chips.

It is the recommended starting point when getting familiar with console component.

## advanced example

This example illustrates lower-level APIs for line editing and autocompletion (`linenoise`), argument parsing (`argparse3`) and command registration (`esp_console`).

These APIs allow for a lot of flexibility when building a console application, but require more code to be written.

While these APIs allow for a console to be implemented over various interfaces (UART, USB, TCP), this example works with UART only.

## advanced_usb_cdc example

This example is similar to the one above, except it works over USB CDC provided by USB_OTG peripheral.

Currently it can be used on ESP32-S2.
