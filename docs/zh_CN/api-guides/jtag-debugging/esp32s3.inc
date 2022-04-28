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

    openocd -f board/esp32s3-builtin.cfg

.. |run-openocd-device-name| replace:: ESP32-S3

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32s3-builtin.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20210902 (2021-10-05-23:44)
    Licensed under GNU GPL v2
    For bug reports, read
      http://openocd.org/doc/doxygen/bugs.html
    debug_level: 2

    Info : only one transport option; autoselect 'jtag'
    Warn : Transport "jtag" was already selected
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Info : esp_usb_jtag: Device found. Base speed 40000KHz, div range 1 to 255
    Info : clock speed 40000 kHz
    Info : JTAG tap: esp32s3.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : JTAG tap: esp32s3.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : esp32s3.cpu0: Debug controller was reset.
    Info : esp32s3.cpu0: Core was reset.
    Info : esp32s3.cpu1: Debug controller was reset.
    Info : esp32s3.cpu1: Core was reset.
    Info : Listening on port 3333 for gdb connections

.. |run-openocd-cfg-file-err| replace:: ``Can't find board/esp32s3-builtin.cfg``

---

.. run-openocd-upload

::

    openocd -f board/esp32s3-builtin.cfg -c "program_esp filename.bin 0x10000 verify exit"

---

.. run-openocd-src-linux

.. code-block:: bash

    src/openocd -f board/esp32s3-builtin.cfg

---

.. run-openocd-src-win

.. code-block:: batch

    src\openocd -f board/esp32s3-builtin.cfg

---

.. idf-py-openocd-default-cfg

.. |idf-py-def-cfg| replace:: ``-f board/esp32s3-builtin.cfg``

---

.. run-openocd-appimage-offset

::

    openocd -f board/esp32s3-builtin.cfg -c "init; halt; esp appimage_offset 0x210000"

---

.. openocd-cfg-files

.. list-table:: ESP32-S3 相关的 OpenOCD 配置文件
    :widths: 25 75
    :header-rows: 1

    * - 名称
      - 描述
    * - ``board/esp32s3-builtin.cfg``
      - ESP32-S3 系列开发板的配置文件，用于通过内置的 USB JTAG 进行调试，包括 ESP32-S3 目标配置和适配器配置。
    * - ``board/esp32s3-ftdi.cfg``
      - ESP32-S3 系列开发板的配置文件，用于通过外部连接的基于 FTDI 的探头，如 ESP-Prog，包括 ESP32-S3 目标配置和适配器配置。
    * - ``target/esp32s3.cfg``
      - ESP32-S3 目标配置文件，可以和某个 ``interface/`` 下的配置文件一同使用
    * - ``interface/ftdi/esp_usb_jtag.cfg``
      - 适用于 ESP32-S3 的 JTAG 适配器配置文件。
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - 适用于 ESP-Prog 的 JTAG 适配器配置文件。

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-S3 管脚和 JTAG 接口信号
    :widths: 25 75
    :header-rows: 1

    * - ESP32-S3 管脚
      - JTAG 信号
    * - MTDO / GPIO40
      - TDO
    * - MTDI / GPIO41
      - TDI
    * - MTCK / GPIO39
      - TCK
    * - MTMS / GPIO42
      - TMS

.. |jtag-sel-gpio| replace:: GPIO3
.. |jtag-gpio-list| replace:: GPIO39-GPIO42

---

.. run-openocd-d3

::

    openocd -l openocd_log.txt -d3 -f board/esp32s3-builtin.cfg

---

.. run-openocd-d3-tee

::

    openocd -d3 -f board/esp32s3-builtin.cfg 2>&1 | tee openocd.log

---

.. run-gdb-remotelog

::

    xtensa-esp32s3-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

---

.. devkit-defs

.. |devkit-name| replace:: ESP32-S3
.. |devkit-name-with-link| replace:: :doc:`ESP32-S3 <../../hw-reference/index>`

---

.. devkit-hw-config

* 开箱即用，ESP32-S3 不需要任何其他硬件配置即可进行 JTAG 调试。 但是，如果遇到问题，请检查 “JTAG” DIP 开关的 2-5 号是否在 “ON” 位置。

---
