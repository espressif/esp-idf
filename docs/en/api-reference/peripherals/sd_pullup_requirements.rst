SD Pullup Requirements
======================

CMD and DATA lines D0-D3 of the slave should be pulled up by 10 kOhm resistor
even in 1-bit mode or SPI mode. The pullups of the slave cards should be
connected even if they're not connected to the host.

The MTDI strapping pin by default is incompatible with DAT2 line pullup when
the flash is 3.3 V. See :ref:`technical_detail_sdio` below.

.. _existing_issues_official_modules_sdio:

Possible Issues
---------------

Here is a list of Espressif chips, official modules and development kits and
the issues they may have during SDIO development. Since the requirements are
for the pullups on the SD bus, the issues should be resolved no matter if it
is a host or slave. Each issue is linked to its solution. The solution for a
specific issue may be different for host and slave.

Official modules are usually without the sufficient pullups on all 6 pins, it
is suggested to select one of the development kits for either host or slave
to provide such pullups.

Chips
^^^^^

  - ESP32 (except D2WD, see `ESP32 datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_):
      :ref:`sd_pullup_no_pullups`, whether the strapping conflicts with DAT2 is determined
      by the flash you are using. Please see :ref:`strapping_conflicts_dat2` if
      your flash chip is 3.3 V.

  - ESP32-D2WD:
      :ref:`sd_pullup_no_pullups`, :ref:`no_pullup_on_gpio12`

  - ESP32-PICO-D4:
      :ref:`sd_pullup_no_pullups`, :ref:`strapping_conflicts_dat2`

Modules
^^^^^^^

  - ESP32-WROOM-32 Series:
      Including ESP32-WROOM-32, ESP32-WROOM-32D, ESP32-WROOM-32U and
      ESP32-SOLO-1.

      :ref:`sd_pullup_no_pullups`, :ref:`strapping_conflicts_dat2`.

  - ESP32-WROVER Series:
      Including ESP32-WROVER and ESP32-WROVER-I.

      :ref:`sd_pullup_no_pullups`.

  - ESP32-WROVER-B Series:
      Including ESP32-WROVER-B and ESP32-WROVER-IB.

      :ref:`sd_pullup_no_pullups`, :ref:`strapping_conflicts_dat2`.

.. _sdio_dev_kits:

Development Boards
^^^^^^^^^^^^^^^^^^

  - ESP32-PICO-KIT:
      Including PICO-KIT v4.1, v4.0 and v3.

      :ref:`sd_pullup_no_pullups`, :ref:`strapping_conflicts_dat2`,
      :ref:`gpio2_strapping_pin`.

  - ESP32-DevKitC:
      Including ESP32-DevKitC v4 and v2.

      :ref:`sd_pullup_no_pullups`, :ref:`strapping_conflicts_dat2`,
      :ref:`gpio2_strapping_pin`.

  - ESP-WROVER-KIT:
      v4.1: Have pullups, but :ref:`pullup_conflicts_on_gpio13` ,
      :ref:`strapping_conflicts_dat2`.

      v3: Have pullups, but :ref:`pullup_conflicts_on_gpio13`.

      v2 and v1: Have pullups, but :ref:`pullup_conflicts_on_gpio13`,
      :ref:`strapping_conflicts_dat2`, :ref:`gpio2_strapping_pin`.

      You can tell the version of your ESP23-WROVER-KIT version from the module
      on it: v4.1 are with ESP32-WROVER-B modules, v3 are with ESP32-WROVER
      modules, while v2 and v1 are with ESP32-WROOM-32 modules.

  - ESP32-LyraTD-MSC:
      :ref:`strapping_conflicts_dat2`. Have pullups.

  - ESP32-LyraT:
      Have pullups, but :ref:`pullup_conflicts_on_gpio13`

Non-Espressif Hosts
^^^^^^^^^^^^^^^^^^^

Please make sure that your 3rd party SDIO host has correct pullups for all
the signals.

Solutions
---------

.. _sd_pullup_no_pullups:

No Pullups
^^^^^^^^^^

When developing on boards without pullups, you can either:

1. If your host and slave are on seperated boards, you can change one of them
   to a board with pullups. Please see :ref:`sdio_dev_kits` to find Espressif
   official boards with pullups.
2. Connect external pullups to VDD by yourself. Connect these pins without
   pullups to the VDD through a 10 kOhm resistor.

.. _pullup_conflicts_on_gpio13:

Pullup Conflicts on GPIO13
^^^^^^^^^^^^^^^^^^^^^^^^^^

The DAT3 of slave may not be properly pulled up. You can either:

1. Use 1-bit mode, and tie DAT3 of slave to VDD.
2. Use SPI mode.
3. Remove the pulldown resistors on GPIO13; or pull it up by resistor less
   than 5 kOhm (2 kOhm suggested); or pull it up or drive it high by host or
   VDD3.3V in 1-bit mode.

.. _strapping_conflicts_dat2:

Conflicts Between Bootstrap and SDIO on DAT2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The strapping requirements conflict with SDIO protocol. See
:ref:`mtdi_strapping_pin` for the details of this issue. You can either:

1. (Recomended) Burn the flash voltage selection eFuses. This will
   permanently select 3.3 V output voltage for the internal regulator, and GPIO12
   will not be used as a bootstrapping pin. Then connect a pullup resistor
   to GPIO12.

   .. warning:: Burning eFuse is irreversible. The issue list above may be
      out of date. Do make sure the module you are burning is using a 3.3 V flash
      according to the information on http://www.espressif.com/. If you burn the
      3.3 V eFuses on an 1.8 V module, the module will get broken.

   Run the command below under your IDF folder:
   ::

       components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V

   This command will burn the `XPD_SDIO_TIEH`, `XPD_SDIO_FORCE`, and
   `XPD_SDIO_REG` eFuses. With all three burned to value 1, the internal
   VDD_SDIO flash voltage regulator is permanently set to 3.3 V. You will
   see the following log if the burning succeeds:
   ::

       espefuse.py v2.6
       Connecting....

       Enable internal flash voltage regulator (VDD_SDIO) to 3.3 V.
       The following eFuses are burned: XPD_SDIO_FORCE, XPD_SDIO_REG, XPD_SDIO_TIEH.
       This is an irreversible operation.
       Type 'BURN' (all capitals) to continue.
       BURN
       VDD_SDIO setting complete.

   You can also run ``components/esptool_py/esptool/espefuse.py summary`` to
   check the status of the eFuses above.

   `espefuse.py` has a ``--do-not-confirm`` option if running from an
   automated flashing script.

   See the ESP32 Technical Reference Manual for more details.

2. **When using 1-bit mode or SPI mode**, DAT2 signal is not needed (though it
   still has to be pulled up). If the device works as the host, you can leave
   the DAT2 of host floating, and directly connect DAT2 of slave to VDD; or if
   the device works as the slave, specify ``SDIO_SLAVE_FLAG_DAT2_DISABLED`` in
   the slave app to avoid slave detecting on DAT2 line. Note the host will
   not know that 4-bit mode is not supported any more by the standard CCCR
   register. You have to forbid the host from using 4-bit mode.

.. _no_pullup_on_gpio12:

No Pullup on GPIO12
^^^^^^^^^^^^^^^^^^^

Your module is compatible with the SDIO protocol. Just connect GPIO12 to the
VDD through a 10 kOhm resistor.

.. _gpio2_strapping_pin:

Auto-program Not Working (minor issue)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

GPIO2 pin is used as a bootstrapping pin, and should be low to enter UART
download mode. You may find it unable to enter the UART download mode if you
correctly connect the pullup of SD on GPIO2.

Some official kits pull down GPIO2 when downloading. For other boards, you
may try to connect GPIO0 and GPIO2 using a jumper, and then the auto-reset
circuit on most development boards will pull GPIO2 low along with GPIO0, when
entering download mode. (Some boards have pulldown and/or LED on GPIO2. LED
is usually ok, but the pulldown resistor will interfere with D0 signals and
must be removed. Check the schematic of your development board for anything
connected to GPIO2.)

If the above way is not working, please just turn off the other device and
remove the pullups on GPIO2 when you are programming the slave.


.. _technical_detail_sdio:

Technical Details
-----------------

.. _mtdi_strapping_pin:

MTDI Strapping Pin
^^^^^^^^^^^^^^^^^^

MTDI (GPIO12) is used as a bootstrapping pin to select output voltage of an
internal regulator which powers the flash chip (VDD_SDIO). This pin has an
internal pulldown so if left unconnected it will read low at startup
(selecting default 3.3 V operation).

For ESP32-WROVER modules, excluding ESP32-WROVER-B, they use 1.8 V flash, and
have pullup on GPIO12 inside. For other modules, which use 3.3 V flash, have
no pullups on GPIO12, and GPIO12 is weakly pulled down internally.

When adding a pullup to this pin for SD card operation, consider the
following:

- For boards which don't use the internal regulator (VDD_SDIO) to power the
  flash, GPIO12 can be pulled high.
- For boards which use 1.8 V flash chip, GPIO12 needs to be pulled high at
  reset. This is fully compatible with SD card operation.
- On boards which use the internal regulator and a 3.3 V flash chip, GPIO12
  must be low at reset. This is incompatible with SD card operation. Please
  check :ref:`existing_issues_official_modules_sdio` to see whether your board
  has this issue, and how to solve it.

Internal Pullups and Strapping Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is never recommended to rely on internal weak pullups for SDIO
communications, since internal weak pullups are insufficient. But information
of the strapping requirements and internal pullups may be useful. For
Espressif official modules, different weak pullups / pulldowns are connected
to CMD, and DATA pins as below.

+-----------------------+-----+--------------------------+------+-----------------------+------+
| GPIO                  | 15  | 2                        | 4    | 12                    | 13   |
+=======================+=====+==========================+======+=======================+======+
| Name                  | CMD | DAT0                     | DAT1 | DAT2                  | DAT3 |
+-----------------------+-----+--------------------------+------+-----------------------+------+
| At startup            | WPU | WPD                      | WPD  | PU for 1.8 V flash;   | WPU  |
|                       |     |                          |      | WPD for 3.3 V flash   |      |
+-----------------------+-----+--------------------------+------+-----------------------+------+
| Strapping requirement |     | Low to download to flash |      | High for 1.8 V flash; |      |
|                       |     |                          |      | Low for 3.3 V flash   |      |
+-----------------------+-----+--------------------------+------+-----------------------+------+

- WPU: Weak pullup
- WPD: Weak pulldown
- PU: Pullup inside the module