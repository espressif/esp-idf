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

    openocd -f board/esp32c5-builtin.cfg

.. |run-openocd-device-name| replace:: ESP32-C5 through built-in USB connection

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32c5-builtin.cfg
    Open On-Chip Debugger v0.12.0-esp32-20240318-139-g35ddcf93-dirty (2024-07-16-15:00)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselecting 'jtag'
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    WARNING: ESP flash support is disabled!
    force hard breakpoints
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Info : esp_usb_jtag: serial (60:55:F9:FF:FE:F9:54:A4)
    Info : esp_usb_jtag: Device found. Base speed 24000KHz, div range 1 to 255
    Info : clock speed 24000 kHz
    Info : JTAG tap: esp32c5.cpu0 tap/device found: 0x00017c25 (mfg: 0x612 (Espressif Systems), part: 0x0017, ver: 0x0)
    Info : JTAG tap: esp32c5.cpu1 tap/device found: 0x00017c25 (mfg: 0x612 (Espressif Systems), part: 0x0017, ver: 0x0)
    Info : [esp32c5.cpu1] datacount=1 progbufsize=2
    Info : [esp32c5.cpu1] Examined RISC-V core; found 1 harts
    Info : [esp32c5.cpu1]  XLEN=32, misa=0x40101105
    Info : [esp32c5.cpu1] Examination succeed
    Info : [esp32c5.cpu1] starting gdb server on 3333
    Info : Listening on port 3333 for gdb connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32c5-builtin.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32c5-builtin.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32c5-builtin.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32c5-builtin.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32c5-builtin.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32c5-builtin.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-C5
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32c5-builtin.cfg``
      - Board configuration file for ESP32-C5 through built-in USB, includes target and adapter configuration.
    * - ``board/esp32c5-ftdi.cfg``
      - Board configuration file for ESP32-C5 for via externally connected FTDI-based probe like ESP-Prog, includes target and adapter configuration.
    * - ``target/esp32c5.cfg``
      - ESP32-C5 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/esp_usb_jtag.cfg``
      - JTAG adapter configuration file for ESP32-C5.
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - JTAG adapter configuration file for ESP-Prog boards.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-C5 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-C5 Pin
      - JTAG Signal
    * - MTDO / GPIO5
      - TDO
    * - MTDI / GPIO3
      - TDI
    * - MTCK / GPIO4
      - TCK
    * - MTMS / GPIO2
      - TMS

.. |jtag-sel-gpio| replace:: GPIO7
.. |jtag-gpio-list| replace:: GPIO2-GPIO5

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32c5-builtin.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32c5-builtin.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-C5
.. |devkit-name-with-link| replace:: :doc:`ESP32-C5 <../../hw-reference/index>`

---

.. devkit-hw-config

* Out of the box, ESP32-C5 doesn't need any additional hardware configuration for JTAG debugging.

---
