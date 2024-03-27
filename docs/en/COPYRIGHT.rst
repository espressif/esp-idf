Copyrights and Licenses
***********************

Software Copyrights
===================

All original source code in this repository is Copyright (C) 2015-2023 Espressif Systems. This source code is licensed under the Apache License 2.0 as described in the file LICENSE.

Additional third party copyrighted code is included under the following licenses.

Where source code headers specify Copyright & License information, this information takes precedence over the summaries made here.

Some examples use external components which are not Apache licensed, please check the copyright description in each example source code.

Firmware Components
-------------------

These third party libraries can be included into the application (firmware) produced by ESP-IDF.

* :component:`Newlib <newlib>` is licensed under the BSD License and is Copyright of various parties, as described in :component_file:`COPYING.NEWLIB <newlib/COPYING.NEWLIB>`.

* :component:`Xtensa header files <xtensa/include/xtensa>` are Copyright (C) 2013 Tensilica Inc and are licensed under the MIT License as reproduced in the individual header files.

* Original parts of FreeRTOS_ (components/freertos) are Copyright (C) 2017 Amazon.com, Inc. or its affiliates are licensed under the MIT License, as described in :component_file:`license.txt <freertos/FreeRTOS-Kernel/LICENSE.md>`.

* Original parts of LWIP_ (components/lwip) are Copyright (C) 2001, 2002 Swedish Institute of Computer Science and are licensed under the BSD License as described in :component_file:`COPYING file <lwip/lwip/COPYING>`.

* `wpa_supplicant`_ Copyright (c) 2003-2022 Jouni Malinen <j@w1.fi> and contributors and licensed under the BSD license.

* :component_file:`Fast PBKDF2 <wpa_supplicant/esp_supplicant/src/crypto/crypto_mbedtls.c>` Copyright (c) 2015 Joseph Birr-Pixton and licensed under CC0 Public Domain Dedication license.

* `FreeBSD net80211`_ Copyright (c) 2004-2008 Sam Leffler, Errno Consulting and licensed under the BSD license.

* `argtable3`_ argument parsing library Copyright (C) 1998-2001,2003-2011,2013 Stewart Heitmann and licensed under 3-clause BSD license. argtable3 also includes the following software components. For details, please see argtable3 :component_file:`LICENSE file <console/argtable3/LICENSE>`.

    * C Hash Table library, Copyright (c) 2002, Christopher Clark and licensed under 3-clause BSD license.
    * The Better String library, Copyright (c) 2014, Paul Hsieh and licensed under 3-clause BSD license.
    * TCL library, Copyright the Regents of the University of California, Sun Microsystems, Inc., Scriptics Corporation, ActiveState Corporation and other parties, and licensed under TCL/TK License.

* `linenoise`_ line editing library Copyright (c) 2010-2014 Salvatore Sanfilippo, Copyright (c) 2010-2013 Pieter Noordhuis, licensed under 2-clause BSD license.

* `FatFS`_ library, Copyright (C) 2017 ChaN, is licensed under :component_file:`a BSD-style license <fatfs/src/ff.h#L1-L18>`.

* `cJSON`_ library, Copyright (c) 2009-2017 Dave Gamble and cJSON contributors, is licensed under MIT license as described in :component_file:`LICENSE file <json/cJSON/LICENSE>`.

* `micro-ecc`_ library, Copyright (c) 2014 Kenneth MacKay, is licensed under 2-clause BSD license.

* `Mbed TLS`_ library, Copyright (C) 2006-2018 ARM Limited, is licensed under Apache License 2.0 as described in :component_file:`LICENSE file <mbedtls/mbedtls/LICENSE>`.

* `SPIFFS`_ library, Copyright (c) 2013-2017 Peter Andersson, is licensed under MIT license as described in :component_file:`LICENSE file <spiffs/spiffs/LICENSE>`.

* :component_file:`SD/MMC driver <sdmmc/sdmmc_cmd.c>` is derived from `OpenBSD SD/MMC driver`_, Copyright (c) 2006 Uwe Stuehler, and is licensed under BSD license.

* :component:`ESP-MQTT <mqtt>` MQTT Package (contiki-mqtt) - Copyright (c) 2014, Stephen Robinson, MQTT-ESP - Tuan PM <tuanpm at live dot com> is licensed under Apache License 2.0 as described in :component_file:`LICENSE file <mqtt/esp-mqtt/LICENSE>`.

* :component:`BLE Mesh <bt/esp_ble_mesh>` is adapted from Zephyr Project, Copyright (c) 2017-2018 Intel Corporation and licensed under Apache License 2.0.

* `mynewt-nimble`_ Apache Mynewt NimBLE, Copyright 2015-2018, The Apache Software Foundation, is licensed under Apache License 2.0 as described in :component_file:`LICENSE file <bt/host/nimble/nimble/LICENSE>`.

* `TLSF allocator <https://github.com/espressif/tlsf>`_ Two Level Segregated Fit memory allocator, Copyright (c) 2006-2016, Matthew Conte, and licensed under the BSD 3-clause license.

* :component:`openthread`, Copyright (c) The OpenThread Authors, is licensed under BSD License as described in :component_file:`LICENSE file <openthread/openthread/LICENSE>`.

* :component_file:`UBSAN runtime <esp_system/ubsan.c>` — Copyright (c) 2016, Linaro Limited and Jiří Zárevúcky, licensed under the BSD 2-clause license.

* :component:`HTTP Parser <http_parser>` Based on src/http/ngx_http_parse.c from NGINX copyright Igor Sysoev. Additional changes are licensed under the same terms as NGINX and Joyent, Inc. and other Node contributors. For details please check :component_file:`LICENSE file <http_parser/LICENSE.txt>`.

* `SEGGER SystemView`_ target-side library, Copyright (c) 1995-2021 SEGGER Microcontroller GmbH, is licensed under BSD 1-clause license.

* `protobuf-c`_ Protocol Buffers implementation in C, Copyright (c) 2008-2022, Dave Benson and the protobuf-c authors. For details please check :component_file:`LICENSE file <protobuf-c/protobuf-c/LICENSE>`.

* `CMock`_ Mock/stub generator for C, Copyright (c) 2007-14 Mike Karlesky, Mark VanderVoord, Greg Williams, is licensed under MIT license as described in :component_file:`LICENSE file <cmock/CMock/LICENSE.txt>`.

* `Unity`_ Simple Unit Testing library, Copyright (c) 2007-23 Mike Karlesky, Mark VanderVoord, Greg Williams, is licensed under MIT license as described in :component_file:`LICENSE file <unity/unity/LICENSE.txt>`.

Documentation
-------------

* HTML version of the `ESP-IDF Programming Guide`_ uses the Sphinx theme `sphinx_idf_theme`_, which is Copyright (c) 2013-2020 Dave Snider, Read the Docs, Inc. & contributors, and Espressif Systems (Shanghai) CO., LTD. It is based on `sphinx_rtd_theme`_. Both are licensed under MIT license.

ROM Source Code Copyrights
==========================

Espressif SoCs mask ROM hardware includes binaries compiled from portions of the following third party software:

* :component:`Newlib <newlib>`, licensed under the BSD License and is Copyright of various parties, as described in :component_file:`COPYING.NEWLIB <newlib/COPYING.NEWLIB>`.

* Xtensa libhal, Copyright (c) Tensilica Inc and licensed under the MIT license (see below).

* TinyBasic_ Plus, Copyright Mike Field & Scott Lawrence and licensed under the MIT license (see below).

* miniz_, by Rich Geldreich - placed into the public domain.

* TJpgDec_ Copyright (C) 2011, ChaN, all right reserved. See below for license.

* Parts of Zephyr RTOS USB stack:
    * `DesignWare USB device driver`_ Copyright (c) 2016 Intel Corporation and licensed under Apache 2.0 license.
    * `Generic USB device driver`_ Copyright (c) 2006 Bertrik Sikken (bertrik@sikken.nl), 2016 Intel Corporation and licensed under BSD 3-clause license.
    * `USB descriptors functionality`_ Copyright (c) 2017 PHYTEC Messtechnik GmbH, 2017-2018 Intel Corporation and licensed under Apache 2.0 license.
    * `USB DFU class driver`_ Copyright (c) 2015-2016 Intel Corporation, 2017 PHYTEC Messtechnik GmbH and licensed under BSD 3-clause license.
    * `USB CDC ACM class driver`_ Copyright (c) 2015-2016 Intel Corporation and licensed under Apache 2.0 license.

.. only:: CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB

    * `Mbed TLS`_ library, Copyright (C) 2006-2018 ARM Limited and licensed under Apache 2.0 License.

Xtensa libhal MIT License
=========================

Copyright (c) 2003, 2006, 2010 Tensilica Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

TinyBasic Plus MIT License
==========================

Copyright (c) 2012-2013

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

TJpgDec License
===============

TJpgDec - Tiny JPEG Decompressor R0.01 (C) ChaN, 2011
The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
This is a free software that opened for education, research and commercial
developments under license policy of following terms.

Copyright (C) 2011, ChaN, all right reserved.

* The TJpgDec module is a free software and there is NO WARRANTY.
* No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
* Redistributions of source code must retain the above copyright notice.


.. _Newlib: https://sourceware.org/newlib/
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
.. _OpenBSD SD/MMC driver: https://github.com/openbsd/src/blob/f303646/sys/dev/sdmmc/sdmmc.c
.. _Mbed TLS: https://github.com/Mbed-TLS/mbedtls
.. _spiffs: https://github.com/pellepl/spiffs
.. _CMock: https://github.com/ThrowTheSwitch/CMock
.. _protobuf-c: https://github.com/protobuf-c/protobuf-c
.. _Unity: https://github.com/ThrowTheSwitch/Unity
.. _asio: https://github.com/chriskohlhoff/asio
.. _mqtt: https://github.com/espressif/esp-mqtt
.. _zephyr: https://github.com/zephyrproject-rtos/zephyr
.. _mynewt-nimble: https://github.com/apache/mynewt-nimble
.. _ESP-IDF Programming Guide: https://docs.espressif.com/projects/esp-idf/en/latest/
.. _sphinx_idf_theme: https://github.com/espressif/sphinx_idf_theme
.. _sphinx_rtd_theme: https://github.com/readthedocs/sphinx_rtd_theme
.. _SEGGER SystemView: https://www.segger.com/downloads/systemview/
.. _DesignWare USB device driver: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/drivers/usb/device/usb_dc_dw.c
.. _Generic USB device driver: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/usb_device.c
.. _USB descriptors functionality: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/usb_descriptor.c
.. _USB DFU class driver: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/class/usb_dfu.c
.. _USB CDC ACM class driver: https://github.com/zephyrproject-rtos/zephyr/blob/v1.12-branch/subsys/usb/class/cdc_acm.c
