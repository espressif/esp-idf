Vulnerabilities
===============

This page briefly lists all of the vulnerabilities that are discovered and fixed in each release. Please note that for the on-going issues or the issues under embargo period, the information on this page may reflect once the desired resolution has been achieved.


.. note::
   Please refer to ``latest`` version of this documentation guide for up-to-date information.

CVE-2024
--------

CVE-2024-30949
~~~~~~~~~~~~~~

RISC-V gettimeofday system call vulnerability in Newlib's

* Impact: ESP-IDF does not use system call implementations from Newlib
* Resolution: NA


CVE-2024-28183
~~~~~~~~~~~~~~

Bootloader TOCTOU Vulnerability in Anti-rollback Scheme

* Espressif Advisory: NA (Published on GitHub)
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details
* Advisory pointer: `GHSA-22x6-3756-pfp8`_


CVE-2023
--------

CVE-2023-35818
~~~~~~~~~~~~~~

Security Advisory Concerning Bypassing Secure Boot and Flash Encryption Using EMFI

* Espressif Advisory: `AR2023-005`_
* Impact: Applicable for ESP32 Chip Revision v3.0/v3.1
* Resolution: Please see advisory for details


CVE-2023-24023
~~~~~~~~~~~~~~

Security Advisory Concerning the Bluetooth BLUFFS Vulnerability

* Espressif Advisory: `AR2023-010`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details


CVE-2023-52160
~~~~~~~~~~~~~~

Security Advisory for PEAP Phase-2 Authentication

* Espressif Advisory: `AR2024-003`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details


CVE-2022
--------

CVE-2022-24893
~~~~~~~~~~~~~~

Espressif Bluetooth Mesh Stack Vulnerability

* Espressif Advisory: NA (Published on GitHub)
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details
* Advisory pointer: `GHSA-7f7f-jj2q-28wm`_


CVE-2021
--------

CVE-2021-32020
~~~~~~~~~~~~~~

Insufficient bounds checking during management of heap memory in FreeRTOS

* Impact: ESP-IDF uses its own heap allocator and hence not applicable
* Resolution: NA

CVE-2021-43997
~~~~~~~~~~~~~~

Privilege escalation issue in FreeRTOS ARMv7-M and ARMv8-M MPU ports

* Impact: Not applicable for Espressif chips
* Resolution: NA

CVE-2021-3420
~~~~~~~~~~~~~

Security Advisory on "BadAlloc" Vulnerabilities

* Espressif Advisory: `AR2021-005`_
* Impact: Not applicable for ESP-IDF
* Resolution: NA

CVE-2021-31571
~~~~~~~~~~~~~~

Security Advisory on "BadAlloc" Vulnerabilities

* Espressif Advisory: `AR2021-005`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details

CVE-2021-31572
~~~~~~~~~~~~~~

Security Advisory on "BadAlloc" Vulnerabilities

* Espressif Advisory: `AR2021-005`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details

CVE-2021-28139
~~~~~~~~~~~~~~

Security Advisory for Bluetooth Vulnerability

* Covers additional CVEs: CVE-2020-10135, CVE-2020-13595, CVE-2020-26555, CVE-2020-26556, CVE-2020-26557, CVE-2020-26558, CVE-2020-26559, CVE-2020-26560, CVE-2021-28135, CVE-2021-28136
* Espressif Advisory: `AR2021-004`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details


CVE-2020
--------

CVE-2020-22283
~~~~~~~~~~~~~~

Buffer overflow vulnerability in lwIP stack

* Espressif Advisory: NA
* Impact: Applicable for ESP-IDF
* Resolution: Fix cherry-picked and available in ESP-IDF >= v4.4.1

CVE-2020-22284
~~~~~~~~~~~~~~

Buffer overflow vulnerability in lwIP stack

* Espressif Advisory: NA
* Impact: Applicable for ESP-IDF
* Resolution: Fix cherry-picked and available in ESP-IDF >= v4.4.1

CVE-2020-26142
~~~~~~~~~~~~~~

Security Advisory for WLAN FragAttacks

* Espressif Advisory: `AR2023-008`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details

CVE-2020-12638
~~~~~~~~~~~~~~

Security Advisory Concerning Wi-Fi Authentication Bypass

* Espressif Advisory: `AR2020-002`_
* Impact: Applicable for ESP-IDF
* Resolution: Please see advisory for details


.. _`AR2020-002`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2020-002%20Security%20Advisory%20Concerning%20Wi-Fi%20Authentication%20Bypass%20V1.1%20EN.pdf
.. _`AR2021-004`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2021-004%20Bluetooth%20Security%20Advisory.pdf
.. _`AR2021-005`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2021-005%20Security%20Advisory%20on%20BadAlloc%20Vulnerabilities.pdf
.. _`AR2023-005`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2023-005%20Security%20Advisory%20Concerning%20Bypassing%20Secure%20Boot%20and%20Flash%20Encryption%20Using%20EMFI%20EN.pdf
.. _`AR2023-008`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2023-008%20Security%20Advisory%20for%20WLAN%20FragAttacks%20v1.1%20EN_0.pdf
.. _`AR2023-010`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2023-010%20Security%20Advisory%20Concerning%20the%20Bluetooth%20BLUFFS%20Vulnerability%20EN.pdf
.. _`AR2024-003`: https://www.espressif.com/sites/default/files/advisory_downloads/AR2024-003%20Security%20Advisory%20for%20PEAP%20Phase-2%20authentication%20EN.pdf
.. _`GHSA-22x6-3756-pfp8` : https://github.com/espressif/esp-idf/security/advisories/GHSA-22x6-3756-pfp8
.. _`GHSA-7f7f-jj2q-28wm` : https://github.com/espressif/esp-idf/security/advisories/GHSA-7f7f-jj2q-28wm
