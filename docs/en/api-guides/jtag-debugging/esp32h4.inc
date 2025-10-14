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

    openocd -f board/esp32h4-builtin.cfg

.. |run-openocd-device-name| replace:: ESP32-H4 through built-in USB connection

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32h4-builtin.cfg
    Open On-Chip Debugger v0.12.0-esp32-20250707-152-g5b64e9cc-dirty (2025-09-02-09:30)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselecting 'jtag'
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Info : esp_usb_jtag: serial (00:00:00:00:00:00)
    Info : esp_usb_jtag: Device found. Base speed 24000KHz, div range 1 to 255
    Info : clock speed 24000 kHz
    Info : JTAG tap: esp32h4.tap0 tap/device found: 0x0001cc25 (mfg: 0x612 (Espressif Systems), part: 0x001c, ver: 0x0)
    Info : [esp32h4.hp.cpu0] datacount=1 progbufsize=2
    Info : [esp32h4.hp.cpu0] Core 0 made part of halt group 1.
    Info : [esp32h4.hp.cpu0] Examined RISC-V core; found 2 harts
    Info : [esp32h4.hp.cpu0]  XLEN=32, misa=0x40903127
    Info : [esp32h4.hp.cpu0] Examination succeed
    Info : [esp32h4.hp.cpu1] datacount=1 progbufsize=2
    Info : [esp32h4.hp.cpu1] Hart unexpectedly reset!
    Info : [esp32h4.hp.cpu1] Core 1 made part of halt group 1.
    Info : [esp32h4.hp.cpu1] Examined RISC-V core; found 2 harts
    Info : [esp32h4.hp.cpu1]  XLEN=32, misa=0x40903127
    Info : [esp32h4.hp.cpu1] Examination succeed
    Info : [esp32h4.hp.cpu0] starting gdb server on 3333
    Info : Listening on port 3333 for gdb connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32h4-builtin.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32h4-builtin.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32h4-builtin.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32h4-builtin.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32h4-builtin.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32h4-builtin.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-H4
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32h4-builtin.cfg``
      - Board configuration file for ESP32-H4 through built-in USB, includes target and adapter configuration.
    * - ``board/esp32h4-ftdi.cfg``
      - Board configuration file for ESP32-H4 for via externally connected FTDI-based probe like ESP-Prog, includes target and adapter configuration.
    * - ``target/esp32h4.cfg``
      - ESP32-H4 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/esp_usb_jtag.cfg``
      - JTAG adapter configuration file for ESP32-H4.
    * - ``interface/ftdi/esp_ftdi.cfg``
      - JTAG adapter configuration file for ESP-Prog boards.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-H4 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-H4 Pin
      - JTAG Signal
    * - MTDO / GPIO31
      - TDO
    * - MTDI / GPIO29
      - TDI
    * - MTCK / GPIO30
      - TCK
    * - MTMS / GPIO28
      - TMS

.. |jtag-sel-gpio| replace:: GPIO36
.. |jtag-gpio-list| replace:: GPIO28-GPIO31

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32h4-builtin.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32h4-builtin.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-H4
.. |devkit-name-with-link| replace:: :doc:`ESP32-H4 <../../hw-reference/index>`

---

.. devkit-hw-config

* Out of the box, ESP32-H4 doesn't need any additional hardware configuration for JTAG debugging.

---
