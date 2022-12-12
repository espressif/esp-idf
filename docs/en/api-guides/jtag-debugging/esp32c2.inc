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

    CHIP SPECIFIC LOG OUTPUT

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
    * - Content
      - Content

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
