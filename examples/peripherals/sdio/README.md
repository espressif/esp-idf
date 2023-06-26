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

Please run wires between the slave and master to make the example function, and don't forget the grounding wires.

### Slave

On ESP32 / ESP32C6, the pins of SDIO Slave are fixed:

|        |  ESP32   |  ESP32C6 |
|--------|----------|----------|
| Signal | GPIO NUM | GPIO NUM |
|--------|----------|----------|
| CLK    | GPIO-14  | GPIO-19  |
| CMD    | GPIO-15  | GPIO-18  |
| DAT0   | GPIO-2   | GPIO-20  |
| DAT1   | GPIO-4   | GPIO-21  |
| DAT2   | GPIO-12  | GPIO-22  |
| DAT3   | GPIO-13  | GPIO-23  |

#### Note

The SD peripheral works at a high frequency
and uses native pins, there's no way to configure it to other pins through
the GPIO matrix.

Be aware that these pins are normally reserved for JTAG on ESP32. If you're
using a board with JTAG functions, please remember to remove jumpers
connecting to the JTAG adapter. (ESP32 Only).

### Host

On ESP32, the pins of the SDMMC Host are fixed (same as the SDIO slave, see above).

When using SPI Master as the host, or using SDMMC Host on ESP32-S3, the pins are flexible. There are Kconfig options to
select all 6 pins to communicate with the slave. Even if the pins are not used (for example D2 in SPI mode) or
disconnected, the config options are still valid, and the host example will still initialize all the pins to help the
slave meet the "all pins should be pulled up" requirement.

For the SDIO Slave, CMD and DAT0-3 lines require to be pulled up (suggested resistor value: 10 KOhm) even in 1-bit mode
or SPI mode, which is required by the SD specification. See *Board Compability* below for details.

In 1-bit mode, the host can make use of DAT2 and DAT3, however the slave should
leave them alone but pulled up.

The 4-bit mode can be configured in the menuconfig. If the 4-bit mode is not
used, the host will not control the DAT3 line, the slave hardware is
responsible to pull-up the line (or the slave may run into the SPI mode and
cause a crash).

The host uses HS mode by default. If the example does not work properly (especially when you see CRC error or timeout),
please try:

1. Connecting two boards by short wires
2. Grounding between two boards better (**Reliable grounding is very important for the example to work properly!**)
3. Disabling the HS mode in menuconfig

## Board compatibility

Pull-up resistors is needed. As the SD specification and the eMMC datasheet clarify,
minimum 10k pull-up resistors are required for the bus IOs to protect the IOs against bus floating issue.
Note these pull-up resistors are needed, even if the pin is not used (For example,
you use 1-line-mode, the pull-up resistor is still required for the D1 pin).

Some other notes:

1. (ESP32 Only) If you're using a board (e.g. WroverKit v2 and before, PICO, DevKitC)
    which is not able to drive GPIO2 low on downloading, please remember to
    disconnect GPIO2 between two boards when downloading the application.

2. (ESP32 Only) It is suggested to use the official Wrover Kit as the slave. This is
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

3. (ESP32 Only) Moreover, if your slave devkit is using code flash of 3.3V, it is required
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

## About `esp_serial_slave_link` component used in the host example

The host example is based on [esp_serial_slave_link component](https://components.espressif.com/components/espressif/esp_serial_slave_link), which is used to communicate to a ESP slave device.

The component can be installed by esp component manager. Since this example already installed it, no need to re-installed it again, but if you want to install this component in your own project, you can input the following command:

```
idf.py add-dependency espressif/esp_serial_slave_link
```

If the dependency is added, you can check `idf_component.yml` for more detail. When building this example or other projects with managed components, the component manager will search for the required components online and download them into the `managed_componets` folder.
