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

    openocd -f board/esp32s31-builtin.cfg

.. |run-openocd-device-name| replace:: ESP32-S31

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32s31-builtin.cfg
    Open On-Chip Debugger v0.12.0-esp32-20260424 (2026-04-24-15:13)
    Licensed under GNU GPL v2
    For bug reports, read
      http://openocd.org/doc/doxygen/bugs.html
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Info : esp_usb_jtag: serial (30:ED:A0:ED:77:E4)
    Info : esp_usb_jtag: Device found. Base speed 24000KHz, div range 1 to 255
    Info : clock speed 24000 kHz
    Info : JTAG tap: esp32s31.tap0 tap/device found: 0x00003c25 (mfg: 0x612 (Espressif Systems), part: 0x0003, ver: 0x0)
    Info : JTAG tap: esp32s31.tap1 tap/device found: 0x00020c25 (mfg: 0x612 (Espressif Systems), part: 0x0020, ver: 0x0)
    Info : [esp32s31.hp.cpu0] datacount=1 progbufsize=2
    Info : [esp32s31.hp.cpu0] Core 0 made part of halt group 1.
    Info : [esp32s31.hp.cpu0] Examined RISC-V core
    Info : [esp32s31.hp.cpu0]  XLEN=32, misa=0x40943127
    Info : [esp32s31.hp.cpu0] Chip revision v0.0
    Info : [esp32s31.hp.cpu0] Examination succeed
    Info : [esp32s31.hp.cpu1] datacount=1 progbufsize=2
    Info : [esp32s31.hp.cpu1] Core 1 made part of halt group 1.
    Info : [esp32s31.hp.cpu1] Examined RISC-V core
    Info : [esp32s31.hp.cpu1]  XLEN=32, misa=0x40943127
    Info : [esp32s31.hp.cpu1] Chip revision v0.0
    Info : [esp32s31.hp.cpu1] Examination succeed
    Info : [esp32s31.hp.cpu0] starting gdb server on 3333
    Info : Listening on port 3333 for gdb connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32s31-builtin.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32s31-builtin.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32s31-builtin.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32s31-builtin.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32s31-builtin.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32s31-builtin.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-S31
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32s31-builtin.cfg``
      - Board configuration file for ESP32-S31 for debugging via builtin USB JTAG, includes target and adapter configuration.
    * - ``board/esp32s31-ftdi.cfg``
      - Board configuration file for ESP32-S31 for via externally connected FTDI-based probe like ESP-Prog, includes target and adapter configuration.
    * - ``target/esp32s31.cfg``
      - ESP32-S31 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/esp_usb_jtag.cfg``
      - JTAG adapter configuration file for ESP32-S31 builtin USB JTAG.
    * - ``interface/ftdi/esp_ftdi.cfg``
      - JTAG adapter configuration file for ESP-Prog debug adapter board.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-S31 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-S31 Pin
      - JTAG Signal
    * - MTDO / GPIO54
      - TDO
    * - MTDI / GPIO56
      - TDI
    * - MTCK / GPIO55
      - TCK
    * - MTMS / GPIO57
      - TMS

.. |jtag-sel-gpio| replace:: GPIO37
.. |jtag-gpio-list| replace:: GPIO54-GPIO57

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32s31-builtin.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32s31-builtin.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-S31
.. |devkit-name-with-link| replace:: :doc:`ESP32-S31 <../../hw-reference/index>`

---

.. devkit-hw-config

* Out of the box, ESP32-S31 doesn't need any additional hardware configuration for JTAG debugging.

---
