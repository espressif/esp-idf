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

    openocd -f board/esp32s2-kaluga-1.cfg

.. |run-openocd-device-name| replace:: ESP32-S2-Kaluga-1 board

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32s2-kaluga-1.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20200420 (2020-04-20-16:15)
    Licensed under GNU GPL v2
    For bug reports, read
            https://openocd.org/doc/doxygen/bugs.html
    none separate
    adapter speed: 20000 kHz
    force hard breakpoints
    Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
    Info : clock speed 20000 kHz
    Info : JTAG tap: esp32s2.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : esp32s2: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
    Info : esp32s2: Core was reset (pwrstat=0x5F, after clear 0x0F).

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32s2-kaluga-1.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32s2-kaluga-1.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32s2-kaluga-1.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32s2-kaluga-1.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32s2-kaluga-1.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32s2-kaluga-1.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-S2
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32s2-kaluga-1.cfg``
      - Board configuration file for ESP32-S2-Kaluga-1, includes target and adapter configuration.
    * - ``target/esp32s2.cfg``
      - ESP32-S2 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/ftdi/esp32s2_kaluga_v1.cfg``
      - JTAG adapter configuration file for ESP32-S2-Kaluga-1 board.
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - JTAG adapter configuration file for ESP-Prog boards.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-S2 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-S2 Pin
      - JTAG Signal
    * - MTDO / GPIO40
      - TDO
    * - MTDI / GPIO41
      - TDI
    * - MTCK / GPIO39
      - TCK
    * - MTMS / GPIO42
      - TMS

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32s2-kaluga-1.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32s2-kaluga-1.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    xtensa-esp32s2-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-S2-Kaluga-1
.. |devkit-name-with-link| replace:: :doc:`ESP32-S2-Kaluga-1 <../../hw-reference/index>`

---

.. devkit-hw-config

* Out of the box, ESP32-S2-Kaluga-1 doesn't need any additional hardware configuration for JTAG debugging. However if you are experiencing issues, check that the "JTAG" DIP switches (SW5 in the schematic) labelled TCK, TDO, TDI, TMS are in "ON" position.

---
