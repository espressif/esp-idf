Copyrights and Licenses
***********************

Software Copyrights
===================

All original source code in this repository is Copyright (C) 2015-2018 Espressif Systems. This source code is licensed under the Apache License 2.0 as described in the file LICENSE.

Additional third party copyrighted code is included under the following licenses.

Where source code headers specify Copyright & License information, this information takes precedence over the summaries made here.

Firmware Components
-------------------

These third party libraries can be included into the application (firmware) produced by ESP-IDF.

* :component:`Newlib <newlib>` is licensed under the BSD License and is Copyright of various parties, as described in :component_file:`COPYING.NEWLIB<newlib/COPYING.NEWLIB>`.

* :component:`Xtensa header files<esp32/include/xtensa>` are Copyright (C) 2013 Tensilica Inc and are licensed under the MIT License as reproduced in the individual header files.

* Original parts of FreeRTOS_ (components/freertos) are Copyright (C) 2015 Real Time Engineers Ltd and is licensed under the GNU General Public License V2 with the FreeRTOS Linking Exception, as described in :component_file:`license.txt<freertos/license.txt>`.

* Original parts of LWIP_ (components/lwip) are Copyright (C) 2001, 2002 Swedish Institute of Computer Science and are licensed under the BSD License as described in :component_file:`COPYING file<lwip/COPYING>`.

* `wpa_supplicant`_ Copyright (c) 2003-2005 Jouni Malinen and licensed under the BSD license.

* `FreeBSD net80211`_ Copyright (c) 2004-2008 Sam Leffler, Errno Consulting and licensed under the BSD license.

* `JSMN`_ JSON Parser (components/jsmn) Copyright (c) 2010 Serge A. Zaitsev and licensed under the MIT license.

* `argtable3`_ argument parsing library Copyright (C) 1998-2001,2003-2011,2013 Stewart Heitmann and licensed under 3-clause BSD license.

* `linenoise`_ line editing library Copyright (c) 2010-2014 Salvatore Sanfilippo, Copyright (c) 2010-2013 Pieter Noordhuis, licensed under 2-clause BSD license.

* `libcoap`_ COAP library Copyright (c) 2010-2017 Olaf Bergmann and others, is licensed under 2-clause BSD license.

* `libexpat`_ XML parsing library Copyright (c) 1998-2000 Thai Open Source Software Center Ltd and Clark Cooper, Copyright (c) 2001-2017 Expat maintainers, is licensed under MIT license.

* `FatFS`_ library, Copyright (C) 2017 ChaN, is licensed under :component_file:`a BSD-style license <fatfs/src/ff.h#L1-L18>`.

* `cJSON`_ library, Copyright (c) 2009-2017 Dave Gamble and cJSON contributors, is licensed under MIT license.

* `libsodium`_ library, Copyright (c) 2013-2018 Frank Denis, is licensed under ISC license.

* `micro-ecc`_ library, Copyright (c) 2014 Kenneth MacKay, is licensed under 2-clause BSD license.

* `nghttp2`_ library, Copyright (c) 2012, 2014, 2015, 2016 Tatsuhiro Tsujikawa, Copyright (c) 2012, 2014, 2015, 2016 nghttp2 contributors, is licensed under MIT license.

* `Mbed TLS`_ library, Copyright (C) 2006-2018 ARM Limited, is licensed under Apache License 2.0.

* `SPIFFS`_ library, Copyright (c) 2013-2017 Peter Andersson, is licensed under MIT license.

* `TinyCBOR`_ library, Copyright (c) 2017 Intel Corporation, is licensed under MIT License.

* :component_file:`SD/MMC driver <sdmmc/sdmmc_cmd.c>` is derived from `OpenBSD SD/MMC driver`_, Copyright (c) 2006 Uwe Stuehler, and is licensed under BSD license.

* :component:`Asio <asio>`, Copyright (c) 2003-2018 Christopher M. Kohlhoff is licensed under the Boost Software License.

* :component:`ESP-MQTT <mqtt>` MQTT Package (contiki-mqtt) - Copyright (c) 2014, Stephen Robinson, MQTT-ESP - Tuan PM <tuanpm at live dot com> is licensed under Apache License 2.0.
* :component:`BLE Mesh <bt/ble_mesh>` is adapted from Zephyr Project, Copyright (c) 2017-2018 Intel Corporation and licensed under Apache License 2.0

* `mynewt-nimble`_ Apache Mynewt NimBLE, Copyright 2015-2018, The Apache Software Foundation, is licensed under Apache License 2.0.


Build Tools
-----------

This is the list of licenses for tools included in this repository, which are used to build applications. The tools do not become part of the application (firmware), so their license does not affect licensing of the application.

* :component:`esptool.py <esptool_py/esptool>` is Copyright (C) 2014-2016 Fredrik Ahlberg, Angus Gratton and is licensed under the GNU General Public License v2, as described in :component_file:`LICENSE file<esptool_py/LICENSE>`.

* :idf:`KConfig <tools/kconfig>` is Copyright (C) 2002 Roman Zippel and others, and is licensed under the GNU General Public License V2.


ROM Source Code Copyrights
==========================

ESP32 mask ROM hardware includes binaries compiled from portions of the following third party software:

* :component:`Newlib <newlib>`, licensed under the BSD License and is Copyright of various parties, as described in :component_file:`COPYING.NEWLIB<newlib/COPYING.NEWLIB>`.

* Xtensa libhal, Copyright (c) Tensilica Inc and licensed under the MIT license (see below).

* TinyBasic_ Plus, Copyright Mike Field & Scott Lawrence and licensed under the MIT license (see below).

* miniz_, by Rich Geldreich - placed into the public domain.

* `wpa_supplicant`_ Copyright (c) 2003-2005 Jouni Malinen and licensed under the BSD license.

* TJpgDec_ Copyright (C) 2011, ChaN, all right reserved. See below for license.

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

TJpgDec - Tiny JPEG Decompressor R0.01                       (C)ChaN, 2011
The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
This is a free software that opened for education, research and commercial
developments under license policy of following terms.

Copyright (C) 2011, ChaN, all right reserved.

* The TJpgDec module is a free software and there is NO WARRANTY.
* No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
* Redistributions of source code must retain the above copyright notice.


.. _Newlib: https://sourceware.org/newlib/
.. _FreeRTOS: http://freertos.org/
.. _esptool.py: https://github.com/espressif/esptool
.. _LWIP: https://savannah.nongnu.org/projects/lwip/
.. _TinyBasic: https://github.com/BleuLlama/TinyBasicPlus
.. _miniz: https://code.google.com/archive/p/miniz/
.. _wpa_supplicant: https://w1.fi/wpa_supplicant/
.. _FreeBSD net80211: https://github.com/freebsd/freebsd/tree/master/sys/net80211
.. _TJpgDec: http://elm-chan.org/fsw/tjpgd/00index.html
.. _JSMN: http://zserge.com/jsmn.html
.. _argtable3: https://github.com/argtable/argtable3
.. _linenoise: https://github.com/antirez/linenoise
.. _libcoap: https://github.com/obgm/libcoap
.. _fatfs: http://elm-chan.org/fsw/ff/00index_e.html
.. _cJSON: https://github.com/DaveGamble/cJSON
.. _libsodium: https://github.com/jedisct1/libsodium
.. _libexpat: https://github.com/libexpat/libexpat
.. _micro-ecc: https://github.com/kmackay/micro-ecc
.. _nghttp2: https://github.com/nghttp2/nghttp2
.. _OpenBSD SD/MMC driver: https://github.com/openbsd/src/blob/f303646/sys/dev/sdmmc/sdmmc.c
.. _Mbed TLS: https://github.com/ARMmbed/mbedtls
.. _spiffs: https://github.com/pellepl/spiffs
.. _asio: https://github.com/chriskohlhoff/asio
.. _mqtt: https://github.com/espressif/esp-mqtt
.. _zephyr: https://github.com/zephyrproject-rtos/zephyr
.. _mynewt-nimble: https://github.com/apache/mynewt-nimble
.. _TinyCBOR: https://github.com/intel/tinycbor
