.. This file gets included from other .rst files in this folder.
.. It contains target-specific snippets.
.. Comments and '---' lines act as delimiters.
..
.. This is necessary mainly because RST doesn't support substitutions
.. (defined in RST, not in Python) inside code blocks. If that is ever implemented,
.. These code blocks can be moved back to the main .rst files, with target-specific
.. file names being replaced by substitutions.


.. run-openocd

::

    openocd -f board/esp32-wrover-kit-3.3v.cfg

.. |run-openocd-device-name| replace:: ESP-WROVER-KIT with ESP32-WROOM-32 module

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32-wrover-kit-3.3v.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20190708 (2019-07-08-11:04)
    Licensed under GNU GPL v2
    For bug reports, read
            https://openocd.org/doc/doxygen/bugs.html
    none separate
    adapter speed: 20000 kHz
    force hard breakpoints
    Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
    Info : clock speed 20000 kHz
    Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : esp32: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
    Info : esp32: Core was reset (pwrstat=0x5F, after clear 0x0F).

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32-wrover-kit-3.3v.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32-wrover-kit-3.3v.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32-wrover-kit-3.3v.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board\esp32-wrover-kit-3.3v.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32-wrover-kit-3.3v.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32-wrover-kit-3.3v.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32-wrover-kit-3.3v.cfg``
      - Board configuration file for ESP-WROVER-KIT with a 3.3 V ESP32-WROOM-32 module or ESP32-WROVER-B / ESP32-WROVER-E module.
    * - ``board/esp32-wrover-kit-1.8v.cfg``
      - Board configuration file for ESP-WROVER-KIT with an 1.8 V ESP32-WROVER module.
    * - ``board/esp32-ethernet-kit-3.3v.cfg``
      - Board configuration file for ESP-Ethernet-KIT with a 3.3 V ESP32-WROVER-B / ESP32-WROVER-E module.
    * - ``target/esp32.cfg``
      - ESP32 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``target/esp32-solo-1.cfg``
      - Target configuration file for ESP32-SOLO-1 module. Different from ``esp32.cfg`` in that it only configures one CPU.
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - JTAG adapter configuration file for ESP-WROVER-KIT and ESP-Prog boards.

---

.. openocd-target-specific-config-vars

.. list-table:: ESP32-specific OpenOCD variables
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``ESP32_FLASH_VOLTAGE``
      - When using 1.8 V flash ESP32 based modules, set this variable to ``1.8``. Refer to :ref:`jtag-debugging-tip-code-flash-voltage`.
    * - ``ESP32_ONLYCPU``
      - For multi-core targets, can be set to ``1`` to only enable single core debugging.

---

.. jtag-pins

.. list-table:: ESP32 JTAG pins
    :widths: 25 75
    :header-rows: 1

    * - ESP32 Pin
      - JTAG Signal
    * - MTDO / GPIO15
      - TDO
    * - MTDI / GPIO12
      - TDI
    * - MTCK / GPIO13
      - TCK
    * - MTMS / GPIO14
      - TMS

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32-wrover-kit-3.3v.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32-wrover-kit-3.3v.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    xtensa-esp32-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP-WROVER-KIT
.. |devkit-name-with-link| replace:: :doc:`ESP-WROVER-KIT <../../hw-reference/index>`

---

.. devkit-hw-config

* Enable on-board JTAG functionality by setting JP8 according to :doc:`../../hw-reference/esp32/get-started-wrover-kit`, Section :ref:`get-started-esp-wrover-kit-v4.1-setup-options`.

---
