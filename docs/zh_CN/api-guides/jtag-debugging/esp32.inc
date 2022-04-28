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

.. |run-openocd-device-name| replace:: 板载 ESP32-WROOM-32 模组的 ESP-WROVER-KIT 开发板

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32-wrover-kit-3.3v.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20190708 (2019-07-08-11:04)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
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

.. list-table:: ESP32 相关的 OpenOCD 配置文件
    :widths: 25 75
    :header-rows: 1

    * - 名字
      - 描述
    * - ``board/esp32-wrover-kit-3.3v.cfg``
      - 板载 3.3 V 模组（ESP32-WROOM-32，ESP32-WROVER-B，ESP32-WROVER-E）的 ESP-WROVER-KIT 开发板配置文件
    * - ``board/esp32-wrover-kit-1.8v.cfg``
      - 板载 1.8 V 模组（ESP32-WROVER）的 ESP-WROVER-KIT 开发板配置文件
    * - ``board/esp32-ethernet-kit-3.3v.cfg``
      - 板载 3.3 V 模组（ESP32-WROVER-B / ESP32-WROVER-E）的 ESP-Ethernet-KIT 开发板配置文件
    * - ``target/esp32.cfg``
      - ESP32 的目标配置文件，可以和某个 ``interface/`` 下的配置文件一同使用
    * - ``target/esp32-solo-1.cfg``
      - ESP32-SOLO-1 模组的目标配置文件，和 ``esp32.cfg`` 的差别在于它仅配置一个 CPU
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - 适用于 ESP-WROVER-KIT 和 ESP-Prog 的 JTAG 适配器配置文件

---

.. openocd-target-specific-config-vars

.. list-table:: ESP32 相关的 OpenOCD 变量
    :widths: 25 75
    :header-rows: 1

    * - 名字
      - 描述
    * - ``ESP32_FLASH_VOLTAGE``
      - 如果 ESP32 模组集成的是 1.8 V 的 flash，将该变量设置为 ``1.8``，详情请参考 :ref:`jtag-debugging-tip-code-flash-voltage`
    * - ``ESP32_ONLYCPU``
      - 对于多核芯片，将该值设置为 ``1`` 可以仅启用单核调试功能

---

.. jtag-pins

.. list-table:: ESP32 管脚和 JTAG 接口信号
    :widths: 25 75
    :header-rows: 1

    * - ESP32 管脚
      - JTAG 信号
    * - MTDO / GPIO15
      - TDO
    * - MTDI / GPIO12
      - TDI
    * - MTCK / GPIO13
      - TCK
    * - MTMS / GPIO14
      - TMS
    * - GND
      - GND

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
.. |devkit-name-with-link| replace:: :doc:`ESP-WROVER-KIT 开发板 <../../hw-reference/index>`

---

.. devkit-hw-config

* 根据 :doc:`../../hw-reference/esp32/get-started-wrover-kit` 文档中 :ref:`get-started-esp-wrover-kit-v4.1-setup-options` 章节所描述的信息，设置 JP8 便可以启用 JTAG 功能。

---
