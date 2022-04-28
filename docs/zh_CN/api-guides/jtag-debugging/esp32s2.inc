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

.. |run-openocd-device-name| replace:: ESP32-S2-Kaluga-1 开发板

---

.. run-openocd-output

::

    user-name@computer-name:~/esp/esp-idf$ openocd -f board/esp32s2-kaluga-1.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20200420 (2020-04-20-16:15)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
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

.. list-table:: ESP32-S2 相关的 OpenOCD 配置文件
    :widths: 25 75
    :header-rows: 1

    * - 名字
      - 描述
    * - ``board/esp32s2-kaluga-1.cfg``
      - ESP32-S2-Kaluga-1 开发板配置文件，包含 ESP32-S2 目标配置和 JTAG 适配器配置
    * - ``target/esp32s2.cfg``
      - ESP32-S2 目标配置文件，可以和某个 ``interface/`` 下的配置文件一同使用
    * - ``interface/ftdi/esp32s2_kaluga_v1.cfg``
      - 适用于 ESP32-S2-Kaluga-1 开发板的 JTAG 适配器配置文件
    * - ``interface/ftdi/esp32_devkitj_v1.cfg``
      - 适用于 ESP-Prog 板的 JTAG 适配器配置文件

---

.. openocd-target-specific-config-vars

---

---

.. jtag-pins

.. list-table:: ESP32-S2 管脚和 JTAG 接口信号
    :widths: 25 75
    :header-rows: 1

    * - ESP32-S2 管脚
      - JTAG 信号
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

.. |devkit-name| replace:: ESP-S2-Kaluga-1
.. |devkit-name-with-link| replace:: :doc:`ESP-S2-Kaluga-1 <../../hw-reference/index>`

---

.. devkit-hw-config

* 开箱即用，ESP32-S2-Kaluga-1 不需要任何其他硬件配置即可进行 JTAG 调试。 但是，如果遇到问题，请检查标有 TCK、TDO、TDI、TMS 的 “JTAG” DIP 开关（原理图中的 SW5）是否在 “ON” 位置。

---
