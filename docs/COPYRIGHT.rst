Copyrights and Licenses
***********************

Software Copyrights
===================

All original source code in this repository is Copyright (C) 2015-2016
Espressif Systems. This source code is licensed under the Apache
License 2.0 as described in the file LICENSE.

Additional third party copyrighted code is included under the following licenses:

* Newlib_ (components/newlib) is licensed under the BSD License and is Copyright of various parties, as described in the file components/newlib/COPYING.NEWLIB.

* Xtensa header files (components/esp32/include/xtensa) are Copyright (C) 2013 Tensilica Inc and are licensed under the MIT License as reproduce in the individual header files.

* `esptool.py`_ (components/esptool_py/esptool) is Copyright (C) 2014-2016 Fredrik Ahlberg, Angus Gratton and is licensed under the GNU General Public License v2, as described in the file components/esptool_py/LICENSE.

* Original parts of FreeRTOS_ (components/freertos) are Copyright (C) 2015 Real Time Engineers Ltd and is licensed under the GNU General Public License V2 with the FreeRTOS Linking Exception, as described in the file components/freertos/license.txt.

* Original parts of LWIP_ (components/lwip) are Copyright (C) 2001, 2002 Swedish Institute of Computer Science and are licensed under the BSD License as described in the file components/lwip/COPYING.

* KConfig (tools/kconfig) is Copyright (C) 2002 Roman Zippel and others, and is licensed under the GNU General Public License V2.

* `wpa_supplicant`_ Copyright (c) 2003-2005 Jouni Malinen and licensed under the BSD license.

* `FreeBSD net80211`_ Copyright (c) 2004-2008 Sam Leffler, Errno Consulting and licensed under the BSD license.

Where source code headers specify Copyright & License information, this information takes precedence over the summaries made here.

ROM Source Code Copyrights
==========================

ESP32 mask ROM hardware includes binaries compiled from portions of the following third party software:

* Newlib_, as licensed under the BSD License and Copyright of various parties, as described in the file components/newlib/COPYING.NEWLIB.

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
.. _LWIP: http://savannah.nongnu.org/projects/lwip/
.. _TinyBasic: https://github.com/BleuLlama/TinyBasicPlus
.. _miniz: https://code.google.com/archive/p/miniz/
.. _wpa_supplicant: http://w1.fi/wpa_supplicant/
.. _FreeBSD net80211: https://github.com/freebsd/freebsd/tree/master/sys/net80211
.. _TJpgDec: http://elm-chan.org/fsw/tjpgd/00index.html
