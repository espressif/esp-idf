### SDIO Example

## Introduction

These two projects illustrate the SDIO driver (host and slave). The host
example shows how to initialize a SDIO card, respond to a slave interrupt, as
well as reading and writing registers and buffers. The slave is a dedicated
peripheral, providing 8 interrupts, 52 8-bit R/W registers, an input FIFO and
an output FIFO. The example shows how to configure the driver and use these
feature.

The host first tell the slave to write the registers to a specified value,
then reads and prints the value from the slave. Then tell the slave to send 8
interrupts to the host. Then the host start sending data to the slave FIFO
and then reads from the slave FIFO in loops.

## Wiring

The SDIO protocol requires at least 4 lines (one more line than SD memory
protocol): CMD, CLK, DAT0 and DAT1. DAT1 is mandatory for the interrupt. DAT2
is required if 4-bit mode is used. DAT3 is required in 4-bit mode (connected
to host), or required by the slave as mode detect in 1-bit mode (pull up). It
is okay in 1-bit mode to leave DAT3 of host disconnected.

Please run wires between the slave and master to make the example function
(pins are the same for the host and the slave):

| Signal | GPIO NUM |
|--------|----------|
| CLK    | GPIO-14  |
| CMD    | GPIO-15  |
| DAT0   | GPIO-2   |
| DAT1   | GPIO-4   |
| DAT2   | GPIO-12  |
| DAT3   | GPIO-13  |
| Ground | GND      |

CMD and DAT0-3 lines require to be pulled up by 50KOhm resistors even in
1-bit mode. See *Board Compability* below for details. In 1-bit mode, the
host can make use of DAT2 and DAT3, however the slave should leave them alone
but pulled up.

Be aware that the example uses lines normally reserved for JTAG. If you're
using a board with JTAG functions, please remember to remove jumpers
connecting to the JTAG adapter. The SD peripheral works at a high frequency
and uses native pins, there's no way to configure it to other pins through
the GPIO matrix.

Please make sure CMD and DATA lines are pulled up by 50KOhm resistors even in
1-bit mode or SPI mode, which is required by the SD specification.

The 4-bit mode can be configured in the menuconfig. If the 4-bit mode is not
used, the host will not control the DAT3 line, the slave hardware is
responsible to pull-up the line (or the slave may run into the SPI mode and
cause a crash).

The host uses HS mode by default. If the example does not work properly,
please try connecting two boards by short wires, grounding between two boards
better or disabling the HS mode in menuconfig.

## Board compatibility

1. If you're using a board (e.g. WroverKit v2 and before, PICO, DevKitC)
    which is not able to drive GPIO2 low on downloading, please remember to
    disconnect GPIO2 between two boards when downloading the application.

2. It is suggested to use the official Wrover Kit as the slave. This is
    because Wrover Kits have pullups on CMD, DAT0 and DAT1. Otherwise you'll have
    to connect the pullups manually (or use the Wrover Kit as the host). However,
    due to a PCB issue, Wrover Kits v3 and earlier have pullup v.s. pulldown
    conflicts on DAT3 line. You'll have to:

        1. Pull up GPIO13 by resistor of 5KOhm or smaller (2KOhm suggested)
           in 4-bit mode.
        2. Pull up, or tie GPIO13 to VDD3.3 in 1-bit mode.

    To help you faster evaluate the SDIO example on devkits without pullups,
    you can uncomment the pullup enable flags in the initialization code of
    the app_main of host or slave. This enables internal weak pullups on CMD,
    DAT0 and DAT1 and DAT3 lines. However please don't rely on internal weak
    pullups in your own design.

3. Moreover, if your slave devkit is using code flash of 3.3V, it is required
    to pull down DAT2 line to set proper flash voltage. This conflicts with SDIO
    pullup requirements. Currently devkits using PICO-D4 and Wroom-32 series
    modules have this problem. You can either:
    
        - Use Wrover Kit v3 which integrates a Wrover module
        - Still use PICO-D4 or Wroom-32 Series modules as the slave, however:
            - Don't connect the DAT2 pin and leave it floating. This means
                you have to use 1-bit mode in the host. ``SDIO_DAT2_DISABLED``
                option should be enabled in the menuconfig to avoid using of
                DAT2. Or:
            - Burn the EFUSE to force the module using 3.3V as the flash
                voltage. In this way the voltage of flash doesn't depend on MTDI
                any more, connect DAT2 to the host and make sure it is pulled up
                correctly. See document below.

See docs in the programming guide ``api_reference/peripherals/sdio_slave``
and ``api_reference/peripherals/sd_pullup_requirements`` to see more
descriptions about pullups and MTDI requirements and solutions of official
modules and devkits.

## About esp_slave component in this example

The component in this example shows how to communicate with esp32 sdio slave
correctly. However, currently it is for example purpose only.

The example shows how to talk with the slave, but doesn't show how to handle
exceptions. Assertion fails if any of the preconditions (connections,
grounding, slave data preparation, etc.) is not met.

Please do check and handle the return value in your real product.
