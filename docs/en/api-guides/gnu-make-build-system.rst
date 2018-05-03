GNU Make Build System
*********************

This document contains a "cheat sheet" for users of the older ESP-IDF GNU Make build system and the newer CMake build system.

Currently, this version of ESP-IDF contains support for both build systems, and either build system can be used. However, the rest of the documentation for this version has been updated for the CMake build system.

.. note::
   This is documentation for the CMake-based build system which is currently in preview release. The documentation may have gaps, and you may enocunter bugs (please report either of these). To view dedicated documentation for the older GNU Make based build system, switch versions to the 'latest' master branch or a stable release.

Dependencies
============

For Linux & Mac OS, the dependencies for the GNU Make and CMake-based build systems are very similar. The only additional tool required for GNU Make is ``make`` 3.81 or newer. This is pre-installed on Mac OS and can be installed on Linux via the distribution package manager.

For Windows, the requirements are different as the GNU Make based build system requires MSYS2 for a Unix compatibility layer. Consult the Getting Started docs for IDF v3.0 for steps to set up MSYS2.

Cheat Sheet
===========

Equivalents between GNU make-based build system commands and the CMake-based build system:

====================  ==================  =================
Summary               CMake-based         GNU Make based
====================  ==================  =================
Configure project     idf.py menuconfig   make menuconfig
Build project         idf.py build        make
Flash project         idf.py flash        make flash
Monitor project       idf.py monitor      make monitor
Clean project         idf.py fullclean    make clean
====================  ==================  =================

To override the default build directory:

- CMake based: ``idf.py -b (directory) build``
- GNU Make based: ``make BUILD_DIR_BASE=(directory)``.

To set the serial port for flashing/monitoring:

- CMake based: ``idf.py -p (port) flash``
- GNU Make based: ``make flash ESPPORT=/dev/ttyUSB0``, or set in ``menuconfig``.

To set the baud rate for flashing:

- CMake based: ``idf.py -b 921600 flash``
- GNU Make based: ``make flash ESPBAUD=921600``, or set in ``menuconfig``.

Porting GNU Make Based Components to CMake
==========================================

See :ref:`gnu-make-to-cmake`.
