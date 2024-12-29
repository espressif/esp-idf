版权和许可证
************

:link_to_translation:`en:[English]`

软件版权
========

2015-2023 乐鑫科技享有本代码库中的所有原创源代码版权。源代码根据 LICENSE 文件描述的 Apache License 2.0 进行许可。

以下各许可证还包含一些第三方版权代码。

如果源代码头文件中指定了版权和许可证信息，则该信息优先于此处的摘要。

部分示例使用了未经 Apache 许可的外部组件，请查看每个示例源代码中的版权描述。

固件组件
--------

以下这些第三方库包含在 ESP-IDF 生成的应用程序（固件）中。

* 如 :component_file:`COPYING.NEWLIB <newlib/COPYING.NEWLIB>` 中所述， :component:`Newlib <newlib>` 经 BSD 许可证许可，版权归各方所有。如启用了 :ref:`CONFIG_LIBC_PICOLIBC<CONFIG_LIBC_PICOLIBC>`，请参阅 :component_file:`COPYING.picolibc <newlib/COPYING.picolibc>`。

* 如 :component_file:`COPYING.picolibc <newlib/COPYING.NEWLIB>` 中所述， :component:`Picolibc <newlib>` 经 BSD 许可证许可，版权归各方所有。

* :component:`Xtensa 头文件 <xtensa/include/xtensa>` 版权归 2013 Tensilica 公司所有，并根据各头文件中复制的 MIT 许可证进行许可。

* FreeRTOS_ (components/freertos) 的原始部分，版权归 2017 亚马逊公司或其附属公司所有，并根据 :component_file:`license.txt <freertos/FreeRTOS-Kernel/LICENSE.md>` 中描述的 MIT 许可证进行许可。

* LWIP_ (components/lwip) 的原始部分，版权归 2001、2002 瑞典计算机科学研究所所有，根据 :component_file:`COPYING 文件 <lwip/lwip/COPYING>` 中描述的 BSD 许可证进行许可。

* `wpa_supplicant`_ 版权归 2003-2022 Jouni Malinen <j@w1.fi> 及其他贡献者所有，并根据 BSD 许可证进行许可。

* :component_file:`Fast PBKDF2 <wpa_supplicant/esp_supplicant/src/crypto/crypto_mbedtls.c>` 版权归 2015 Joseph Birr-Pixton 所有，并根据 CC0 公共领域奉献许可证进行许可。

* `FreeBSD net80211`_ 版权归 2004-2008 Sam Leffler 和 Errno 咨询公司所有，并根据 BSD 许可证进行许可。

* `argtable3`_ 参数解析库版权归 1998-2001、2003-2011、2013 Stewart Heitmann 所有，并根据三条款 BSD 许可证进行许可。argtable3 还包含以下软件组件。详情请参阅 argtable3 :component_file:`LICENSE 文件 <console/argtable3/LICENSE>`。

    * C 哈希表库，版权归 2002 Christopher Clark 所有，并根据三条款 BSD 许可证进行许可。
    * Better String 库，版权归 2014 Paul Hsieh 所有，并根据三条款 BSD 许可证进行许可。
    * TCL 库，版权归加州大学董事会、太阳计算机系统有限公司、Scriptics 公司、ActiveState 公司及其他方所有，并根据 TCL/TK 许可证进行许可。

* `linenoise`_ 行编辑库版权归 2010-2014 Salvatore Sanfilippo 及 2010-2013 Pieter Noordhuis 所有，根据二条款 BSD 许可证进行许可。

* `FatFS`_ 库，版权归 2017 ChaN 所有，并根据 :component_file:`BSD 式条款 <fatfs/src/ff.h#L1-L18>` 进行许可。

* `cJSON`_ 库，版权归 2009-2017 Dave Gamble 及 cJSON 库贡献者所有，根据 :component_file:`LICENSE 文件 <json/cJSON/LICENSE>` 中描述的 MIT 许可证进行许可。

* `micro-ecc`_ 库，版权归 2014 Kenneth MacKay 所有，根据二条款 BSD 许可证进行许可。

* `Mbed TLS`_ 库，版权归 2006-2018 安谋控股公司所有，根据 :component_file:`LICENSE 文件 <mbedtls/mbedtls/LICENSE>` 中描述的 Apache License 2.0 进行许可。

* `SPIFFS`_ 库，版权归 2013-2017 Peter Andersson 所有，根据 :component_file:`LICENSE 文件 <spiffs/spiffs/LICENSE>` 中描述的 MIT 许可证进行许可。

* :component_file:`SD/MMC 驱动程序 <sdmmc/sdmmc_cmd.c>` 派生自 `OpenBSD SD/MMC 驱动程序`_，版权归 2006 Uwe Stuehler 所有，并根据 BSD 许可证进行许可。

* :component:`ESP-MQTT <mqtt>` 包 (contiki-mqtt)，版权归 2014 Stephen Robinson 和 MQTT-ESP - Tuan PM <tuanpm at live dot com> 所有，根据 :component_file:`LICENSE 文件 <mqtt/esp-mqtt/LICENSE>` 中描述的 Apache License 2.0 进行许可。

* :component:`BLE Mesh <bt/esp_ble_mesh>` 改编自 Zephyr 项目，版权归 2017-2018 英特尔公司所有，并根据 Apache License 2.0 进行许可。

* `mynewt-nimble`_，版权归 2015-2018 Apache 软件基金会所有，根据 :component_file:`LICENSE 文件 <bt/host/nimble/nimble/LICENSE>` 中描述的 Apache License 2.0 进行许可。

* `TLSF 分配器 <https://github.com/espressif/tlsf>`_，版权归 2006-2016 Matthew Conte 所有，并根据三条款 BSD 许可证进行许可。

* :component:`openthread`，版权归 OpenThread 作者所有，并根据 :component_file:`LICENSE 文件 <openthread/openthread/LICENSE>` 中描述的 BSD 许可证进行许可。

* :component_file:`UBSAN 运行时库 <esp_system/ubsan.c>` 版权归 2016 Linaro Limited 和 Jiří Zárevúcky 所有，并根据二条款 BSD 许可证进行许可。

* :component:`HTTP 解析器 <http_parser>` 基于 NGINX 中的 src/http/ngx_http_parse.c 文件，版权归 Igor Sysoev 所有。任何对源代码的额外修改经过与 NGINX、Joyent 公司及其他 Node 贡献者相同条款的许可。详情请参阅 :component_file:`LICENSE 文件 <http_parser/LICENSE.txt>`。

* `SEGGER SystemView`_ 目标端库，版权归 1995-2024 赛格集团所有，并根据一条款 BSD 许可证进行许可。

* `protobuf-c`_ 是 C 语言的 Protocol Buffers 实现，版权归 2008-2022 Dave Benson 及 protobuf-c 作者所有。详情请参阅 :component_file:`LICENSE 文件 <protobuf-c/protobuf-c/LICENSE>`。

* `CMock`_ C 语言模拟和存根生成器，版权归 2007-14 Mike Karlesky、 Mark VanderVoord 和 Greg Williams 所有，并根据 :component_file:`LICENSE 文件 <cmock/CMock/LICENSE.txt>` 中描述的 MIT 许可证进行许可。

* `Unity`_ 简单单元测试库，版权归 2007-23 Mike Karlesky、Mark VanderVoord 和 Greg Williams 所有，并根据 :component_file:`LICENSE 文件 <unity/unity/LICENSE.txt>` 中描述的 MIT 许可证进行许可。

文档
----

* `ESP-IDF 编程指南`_ 的 HTML 版本使用 Sphinx 主题 `sphinx_idf_theme`_，版权归 2013-2020 Dave Snider、Read the Docs 平台及其贡献者，以及乐鑫科技所有，基于 `sphinx_rtd_theme`_。`sphinx_idf_theme`_ 和 `sphinx_rtd_theme`_ 均根据 MIT 许可证进行许可。

ROM 源代码版权
==============

乐鑫芯片中的 Mask ROM 包含以下部分第三方软件编译的二进制文件：

* :component:`Newlib <newlib>`，如 :component_file:`COPYING.NEWLIB <newlib/COPYING.NEWLIB>` 所述，由 BSD 许可证进行许可，版权归各方所有。

* Xtensa libhal，版权归 Tensilica 公司所有，并根据下述 MIT 许可证进行许可。

* TinyBasic_ Plus，版权归 Mike Field 和 Scott Lawrence 所有，并根据下述 MIT 许可证进行许可。

* miniz_，由 Rich Geldreich 提供，已置于公共领域。

* TJpgDec_ 版权归 2011 ChaN 所有，见下文许可证。

* 以下是 Zephyr RTOS USB 协议栈的部分内容：
    * `DesignWare USB 设备驱动程序`_ 版权归 2016 英特尔公司所有，并根据 Apache License 2.0 进行许可。
    * `Generic USB 设备驱动程序`_ 版权归 2006 Bertrik Sikken (bertrik@sikken.nl)、2016 英特尔公司所有，并根据三条款 BSD 许可证进行许可。
    * `USB 描述符功能`_ 版权归 2017 PHYTEC Messtechnik GmbH 和 2017-2018 英特尔公司所有，并根据 Apache License 2.0 进行许可。
    * `USB DFU 类驱动程序`_ 版权归 2015-2016 英特尔公司和 2017 PHYTEC Messtechnik GmbH 所有，并根据三条款 BSD 许可证进行许可。
    * `USB CDC ACM 类驱动程序`_ 版权归 2015-2016 英特尔公司所有，并根据 Apache License 2.0 进行许可。

.. only:: CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB

    * `Mbed TLS`_ 库，版权归 2006-2018 安谋控股公司所有，并根据 Apache License 2.0 进行许可。

Xtensa libhal MIT 许可证
========================

版权归 2003、2006、2010 Tensilica 公司所有。

特此免费授予获得本软件及相关文档（以下简称“本软件”）副本的任何人、授予其他被提供软件的人，无限制地处理本软件的权利，包括但不限于使用、复制、修改、合并、出版、发布、再许可和/或销售本软件的副本，但需符合以下条件：

上述版权声明和本许可声明应包含在本软件的所有副本或重要内容中。

本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性、特定用途适用性和非侵权性的担保。在任何情况下，作者或版权持有人均不对因本软件、使用本软件、或其他与本软件相关的交易而产生的任何索赔、损害或其他责任负责，无论是在合同诉讼、侵权诉讼或其他诉讼中。

TinyBasic Plus MIT 许可证
=========================

版权归 2012-2013 Mike Field 和 Scott Lawrence 所有。

特此免费授予获得本软件及相关文档（以下简称“本软件”）副本的任何人、授予其他被提供软件的人，无限制地处理本软件的权利，包括但不限于使用、复制、修改、合并、出版、发布、再许可和/或销售本软件的副本，但需符合以下条件：

上述版权声明和本许可声明应包含在本软件的所有副本或重要内容中。

本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性、特定用途适用性和非侵权性的担保。在任何情况下，作者或版权持有人均不对因本软件、使用本软件、或其他与本软件相关的交易而产生的任何索赔、损害或其他责任负责，无论是在合同诉讼、侵权诉讼或其他诉讼中。

TJpgDec 许可证
==============

TJpgDec - Tiny JPEG 解压器 R0.01 (C) 2011 ChaN，是一个用于小型嵌入式系统的通用 JPEG 解压缩模块。这是一个开源的自由软件，可用于教育、研究和商业开发，许可证政策如下：

版权归 2011 ChaN 所有。

* TJpgDec 模块是一个自由软件，不提供任何担保。
* 没有使用限制，可以在个人、非营利或商业产品中使用、修改和重新发布本软件，但须自行承担责任。
* 重新发布源代码时必须保留上述版权声明。


.. _Newlib: https://sourceware.org/newlib/
.. _Picolibc: https://keithp.com/picolibc/
.. _FreeRTOS: https://freertos.org/
.. _esptool.py: https://github.com/espressif/esptool
.. _LWIP: https://savannah.nongnu.org/projects/lwip/
.. _TinyBasic: https://github.com/BleuLlama/TinyBasicPlus
.. _miniz: https://code.google.com/archive/p/miniz/
.. _wpa_supplicant: https://w1.fi/wpa_supplicant/
.. _FreeBSD net80211: https://github.com/freebsd/freebsd-src/tree/master/sys/net80211
.. _TJpgDec: http://elm-chan.org/fsw/tjpgd/00index.html
.. _argtable3: https://github.com/argtable/argtable3
.. _linenoise: https://github.com/antirez/linenoise
.. _fatfs: http://elm-chan.org/fsw/ff/00index_e.html
.. _cJSON: https://github.com/DaveGamble/cJSON
.. _micro-ecc: https://github.com/kmackay/micro-ecc
.. _OpenBSD SD/MMC 驱动程序: https://github.com/openbsd/src/blob/f303646/sys/dev/sdmmc/sdmmc.c
.. _Mbed TLS: https://github.com/Mbed-TLS/mbedtls
.. _spiffs: https://github.com/pellepl/spiffs
.. _CMock: https://github.com/ThrowTheSwitch/CMock
.. _protobuf-c: https://github.com/protobuf-c/protobuf-c
.. _Unity: https://github.com/ThrowTheSwitch/Unity
.. _asio: https://github.com/chriskohlhoff/asio
.. _mqtt: https://github.com/espressif/esp-mqtt
.. _zephyr: https://github.com/zephyrproject-rtos/zephyr
.. _mynewt-nimble: https://github.com/apache/mynewt-nimble
.. _ESP-IDF 编程指南: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/
.. _sphinx_idf_theme: https://github.com/espressif/sphinx_idf_theme
.. _sphinx_rtd_theme: https://github.com/readthedocs/sphinx_rtd_theme
.. _SEGGER SystemView: https://www.segger.com/downloads/systemview/
.. _DesignWare USB 设备驱动程序: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/drivers/usb/device/usb_dc_dw.c
.. _Generic USB 设备驱动程序: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/usb_device.c
.. _USB 描述符功能: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/usb_descriptor.c
.. _USB DFU 类驱动程序: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/class/usb_dfu.c
.. _USB CDC ACM 类驱动程序: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/class/cdc_acm.c
