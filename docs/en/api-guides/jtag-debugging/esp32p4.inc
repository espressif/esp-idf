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

    openocd -f board/esp32p4-builtin.cfg

.. |run-openocd-device-name| replace:: ESP32-P4 through built-in USB connection

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32p4-builtin.cfg
    Open On-Chip Debugger v0.12.0-esp32-20240318 (2024-03-18-18:25)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselecting 'jtag'
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    adapter speed: 40000 kHz
    Info : esp_usb_jtag: serial (60:55:F9:F8:80:5C)
    Info : esp_usb_jtag: Device found. Base speed 24000KHz, div range 1 to 255
    Info : clock speed 24000 kHz
    Info : JTAG tap: esp32p4.lp.cpu tap/device found: 0x00012c25 (mfg: 0x612 (Espressif Systems), part: 0x0012, ver: 0x0)
    Info : JTAG tap: esp32p4.hp.cpu tap/device found: 0x00012c25 (mfg: 0x612 (Espressif Systems), part: 0x0012, ver: 0x0)
    Info : [esp32p4.hp.cpu] datacount=1 progbufsize=2
    Info : [esp32p4.hp.cpu] Examined RISC-V core; found 2 harts
    Info : [esp32p4.hp.cpu]  XLEN=32, misa=0x40901125
    Info : [esp32p4.hp.cpu] Examination succeed
    Info : starting gdb server for esp32p4.hp.cpu on 3333
    Info : Listening on port 3333 for gdb connections
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32p4-builtin.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32p4-builtin.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32p4-builtin.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32p4-builtin.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32p4-builtin.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32p4-builtin.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-P4
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32p4-builtin.cfg``
      - Board configuration file for ESP32-P4 through built-in USB, includes target and adapter configuration.
    * - ``board/esp32p4-ftdi.cfg``
      - Board configuration file for ESP32-P4 for via externally connected FTDI-based probe like ESP-Prog, includes target and adapter configuration.
    * - ``target/esp32p4.cfg``
      - ESP32-P4 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/esp_usb_jtag.cfg``
      - JTAG adapter configuration file for ESP32-P4.
    * - ``interface/ftdi/esp32p4_ftdi.cfg``
      - JTAG adapter configuration file for ESP-Prog boards.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-P4 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-P4 Pin
      - JTAG Signal
    * - MTDO / GPIO5
      - TDO
    * - MTDI / GPIO3
      - TDI
    * - MTCK / GPIO2
      - TCK
    * - MTMS / GPIO4
      - TMS

.. |jtag-sel-gpio| replace:: GPIO34
.. |jtag-gpio-list| replace:: GPIO2-GPIO5

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32p4-builtin.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32p4-builtin.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-P4
.. |devkit-name-with-link| replace:: :doc:`ESP32-P4 <../../hw-reference/index>`

---

.. devkit-hw-config

* Out of the box, ESP32-P4 doesn't need any additional hardware configuration for JTAG debugging.

---
