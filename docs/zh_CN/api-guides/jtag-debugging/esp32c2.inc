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

    openocd -f board/esp32c2-ftdi.cfg

.. |run-openocd-device-name| replace:: ESP32-C2 development board with ESP-Prog

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32c2-ftdi.cfg
    Open On-Chip Debugger v0.11.0-esp32-20221026 (2022-10-26-14:48)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    adapter speed: 20000 kHz

    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Warn : libusb_detach_kernel_driver() failed with LIBUSB_ERROR_ACCESS, trying to continue anyway
    Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi tdo_sample_edge falling"
    Info : clock speed 20000 kHz
    Info : JTAG tap: esp32c2.cpu tap/device found: 0x0000cc25 (mfg: 0x612 (Espressif Systems), part: 0x000c, ver: 0x0)
    Info : datacount=2 progbufsize=16
    Info : Examined RISC-V core; found 1 harts
    Info :  hart 0: XLEN=32, misa=0x40101104
    Info : starting gdb server for esp32c2 on 3333
    Info : Listening on port 3333 for gdb connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32c2-ftdi.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32c2-ftdi.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32c2-ftdi.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32c2-ftdi.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32c2-ftdi.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32c2-ftdi.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32-C2
    :widths: 25 75
    :header-rows: 1

    * - Name
      - Description
    * - ``board/esp32c2-ftdi.cfg``
      - Board configuration file for ESP32-C2 debug through an ESP-Prog compatible FTDI, includes target and adapter configuration.
    * - ``target/esp32c2.cfg``
      - ESP32-C2 target configuration file. Can be used together with one of the ``interface/`` configuration files.
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - JTAG adapter configuration file for ESP-Prog boards.

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-C2 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-C2 Pin
      - JTAG Signal
    * - MTDO / GPIO7
      - TDO
    * - MTDI / GPIO5
      - TDI
    * - MTCK / GPIO6
      - TCK
    * - MTMS / GPIO4
      - TMS

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32c2-ftdi.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32c2-ftdi.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: There is no ESP32-C2 devkit with FT2232H
.. |devkit-name-with-link| replace:: add a link here if such a devkit is added.

---

.. devkit-hw-config

---
