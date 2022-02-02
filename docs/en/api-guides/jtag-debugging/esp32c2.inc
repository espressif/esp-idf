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

    openocd -f board/esp32c2-BOARD_NAME.cfg

.. |run-openocd-device-name| replace:: ESP32C2

---

.. run-openocd-output

::

    CHIP SPECIFIC LOG OUTPUT

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32c2-BOARD_NAME.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32c2-BOARD_NAME.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32c2-BOARD_NAME.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32c2-BOARD_NAME.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32c2-BOARD_NAME.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32c2-BOARD_NAME.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: OpenOCD configuration files for ESP32C2
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

.. list-table:: ESP32C2 pins and JTAG signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32C2 Pin
      - JTAG Signal
    * - MTDO / GPIOX
      - TDO
    * - MTDI / GPIOX
      - TDI
    * - MTCK / GPIOX
      - TCK
    * - MTMS / GPIOX
      - TMS

.. |jtag-sel-gpio| replace:: GPIOX
.. |jtag-gpio-list| replace:: GPIOX

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32c2-BOARD_NAME.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32c2-BOARD_NAME.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    riscv32-esp-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32C2
.. |devkit-name-with-link| replace:: LINK TO DEVKIT

---

.. devkit-hw-config

---
