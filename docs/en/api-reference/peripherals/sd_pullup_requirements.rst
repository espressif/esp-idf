SD Pullup Requirements
======================

CMD and DATA lines D0-D3 of the slave should be pulled up by 50KOhm resistor
even in 1-bit mode or SPI mode. The pullups of the slave cards should be
connected even if they're not connected to the host.

The MTDI strapping pin is incompatible with DAT2 line pull-up by default
when the code flash is 3.3V. See :ref:`mtdi_strapping_pin` below.

Pullup inside Official Modules
------------------------------

For Espressif official modules, different weak pullups / pulldowns are
connected to CMD, and DATA pins as below. To use these modules,
these pins are required to be pulled up by 50KOhm resistors, since internal
weak pullups are insufficient.

+-----------------------+-----+--------------------------+------+----------------------+------+
| GPIO                  | 15  | 2                        | 4    | 12                   | 13   |
+=======================+=====+==========================+======+======================+======+
| Name                  | CMD | DAT0                     | DAT1 | DAT2                 | DAT3 |
+-----------------------+-----+--------------------------+------+----------------------+------+
| At startup            | WPU | WPD                      | WPD  | PU for 1.8v flash;   | WPU  |
|                       |     |                          |      | WPD for 3.3v flash   |      |
+-----------------------+-----+--------------------------+------+----------------------+------+
| Strapping requirement |     | Low to download to flash |      | High for 1.8v flash; |      |
|                       |     |                          |      | Low for 3.3v flash   |      |
+-----------------------+-----+--------------------------+------+----------------------+------+

- WPU: Weak pullup
- WPD: Weak pulldown
- PU: Pullup inside the module

For Wrover modules, they use 1.8v flash, and have pullup on GPIO12 inside.
For Wroom-32 Series, PICO-D4 modules, they use 3.3v flash, and is weakly
pulled down internally. See :ref:`mtdi_strapping_pin` below.

Pullup on Official Devkit (WroverKit)
--------------------------------------

For official Wrover Kit (till version 3), some of the pullups are provided on
the board as the table below. For other devkits that don't have pullups,
please connect them yourselves.

+-----------------------+-----+------+------+------+---------+
| GPIO                  | 15  | 2    | 4    | 12   | 13      |
+=======================+=====+======+======+======+=========+
| Name                  | CMD | DAT0 | DAT1 | DAT2 | DAT3    |
+-----------------------+-----+------+------+------+---------+
| Pullup on the Kit     | PU  | PU   | PU   |      | PU & PD |
+-----------------------+-----+------+------+------+---------+

- PU: Pullup
- PD: Pulldown

The DAT3 pullup conflicts with JTAG pulldown in WroverKit v3 and earlier, please
either:

1. pull it up by resistor less than 5KOhm (2kOhm suggested) in 4-bit mode.
2. pull it up or drive it high by host or VDD3.3V in 1-bit mode.

.. _mtdi_strapping_pin:

MTDI strapping pin
------------------

MTDI (GPIO12) is used as a bootstrapping pin to select output voltage of an
internal regulator which powers the flash chip (VDD_SDIO). This pin has an
internal pulldown so if left unconnected it will read low at reset (selecting
default 3.3V operation). When adding a pullup to this pin for SD card
operation, consider the following:

- For boards which don't use the internal regulator (VDD_SDIO) to power the
  flash, GPIO12 can be pulled high.
- For boards which use 1.8V flash chip, GPIO12 needs to be pulled high at
  reset. This is fully compatible with SD card operation.
- On boards which use the internal regulator and a 3.3V flash chip, GPIO12
  must be low at reset. This is incompatible with SD card operation. Please
  check the table below to see whether your modules/kits use 3.3v flash.

+-----------------+---------------+--------------------------------------+
| Module          | Flash voltage | DAT2 connections                     |
+=================+===============+======================================+
| PICO-D4         | 3.3V          | Internal PD, change EFUSE and pullup |
+-----------------+               + or disable DAT2 line*                +
| Wroom-32 Series |               |                                      |
+-----------------+---------------+--------------------------------------+
| Wrover          | 1.8V          | Internal PU, pullup suggested        |
+-----------------+---------------+--------------------------------------+

Official devkits of different types and version mount different types of
modules, please refer to the table below to see whether your devkit can
support SDIO slave without steps above.

+--------------------------+-----------------+---------------+
| Devkit                   | Module          | Flash voltage |
+==========================+=================+===============+
| PICO Kit                 | PICO-D4         | 3.3V          |
+--------------------------+-----------------+ (see steps    +
| DevKitC                  | Wroom-32 Series | below)        |
+--------------------------+                 +               +
| WroverKit v2 and earlier |                 |               |
+--------------------------+-----------------+---------------+
| WroverKit v3             | Wrover          | 1.8V          |
+--------------------------+-----------------+---------------+

If your board requires internal regulator with 3.3v output, to make it
compatible with SD pullup, you can either:

  - **In the case using ESP32 host only**, external pullup can be omitted and an
    internal pullup can be enabled using a ``gpio_pullup_en(GPIO_NUM_12);`` call.
    Most SD cards work fine when an internal pullup on GPIO12 line is enabled.
    Note that if ESP32 experiences a power-on reset while the SD card is
    sending data, high level on GPIO12 can be latched into the bootstrapping
    register, and ESP32 will enter a boot loop until external reset with
    correct GPIO12 level is applied.
  - **In the case using ESP32 slave in 1-bit mode**, speicfy
    ``SDIO_SLAVE_FLAG_DAT2_DISABLED`` in the slave to avoid slave detecting on
    DAT2 line. Note the host will not know 4-bit mode is not supported any more
    by the standard CCCR register. You have to tell the host use 1-bit only.
  - **For ESP32 host or slave**, another option is to burn the flash voltage
    selection efuses. This will permanently select 3.3V output voltage for the
    internal regulator, and GPIO12 will not be used as a bootstrapping pin.
    Then it is safe to connect a pullup resistor to GPIO12. This option is
    suggested for production use. NOTE this cannot be reverted once the EFUSE
    is burnt.

      The following command can be used to program flash voltage selection efuses **to 3.3V**:

          components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V

      This command will burn the `XPD_SDIO_TIEH`, `XPD_SDIO_FORCE`, and
      `XPD_SDIO_REG` efuses. With all three burned to value 1, the internal
      VDD_SDIO flash voltage regulator is permanently enabled at 3.3V. See
      the technical reference manual for more details.

      `espefuse.py` has a `--do-not-confirm` option if running from an automated flashing script.

GPIO2 Strapping pin
-------------------

GPIO2 pin is used as a bootstrapping pin, and should be low to enter UART
download mode. You may find it unable to enter the UART download mode if you
correctly connect the pullup of SD on GPIO2. For WroverKit v3, there are
dedicated circuits to pulldown the GPIO2 when downloading. For other boards,
one way to do this is to connect GPIO0 and GPIO2 using a jumper, and then the
auto-reset circuit on most development boards will pull GPIO2 low along with
GPIO0, when entering download mode.

- Some boards have pulldown and/or LED on GPIO2. LED is usually ok, but
  pulldown will interfere with D0 signals and must be removed. Check the
  schematic of your development board for anything connected to GPIO2.
