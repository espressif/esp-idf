Copyright Header Guide
======================

ESP-IDF is released under :project_file:`the Apache License 2.0 <LICENSE>` with some additional third-party copyrighted code released under various licenses. For further information please refer to :doc:`the list of copyrights and licenses <../../../COPYRIGHT>`.

This page explains how the source code should be properly marked with a copyright header. ESP-IDF uses `The Software Package Data Exchange (SPDX) <https://spdx.org>`_ format which is short and can be easily read by humans or processed by automated tools for copyright checks.

How to Check the Copyright Headers
----------------------------------

Please make sure you have installed :doc:`the pre-commit hooks <install-pre-commit-hook>` which contain a copyright header checker as well. The checker can suggest a header if it is not able to detect a properly formatted SPDX header.

What if the Checker's Suggestion is Incorrect?
----------------------------------------------

No automated checker (no matter how good is) can replace humans. So the developer's responsibility is to modify the offered header to be in line with the law and the license restrictions of the original code on which the work is based on. Certain licenses are not compatible between each other. Such corner cases will be covered by the following examples.

Common Examples of Copyright Headers
------------------------------------

The simplest case is when the code is not based on any licensed previous work, e.g. it was written completely from scratch. Such code can be decorated with the following copyright header and put under the license of ESP-IDF::

    /*
     * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Apache-2.0
     */

Less restrictive parts of ESP-IDF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some parts of ESP-IDF are deliberately under less restrictive licenses in order to ease their re-use in commercial closed source projects. This is the case for :project:`ESP-IDF examples <examples>` which are in Public domain or under the Creative Commons Zero Universal (CC0) license. The following header can be used in such source files::

    /*
     * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Unlicense OR CC0-1.0
     */

Third party licenses
~~~~~~~~~~~~~~~~~~~~

Code licensed under different licenses, modified by Espressif Systems and included in ESP-IDF cannot be licensed under Apache License 2.0 not even if the checker suggests it. It is advised to keep the original copyright header and add an SPDX before it.

The following example is a suitable header for a code licensed under the "GNU General Public License v2.0 or later" held by John Doe with some additional modifications done by Espressif Systems::

    /*
     * SPDX-FileCopyrightText: 1991 John Doe
     *
     * SPDX-License-Identifier: GPL-2.0-or-later
     *
     * SPDX-FileContributor: 2019-2021 Espressif Systems (Shanghai) CO LTD
     */

The licenses can be identified and the short SPDX identifiers can be found in the official `SPDX license list <https://spdx.org/licenses>`_. Other very common licenses are the GPL-2.0-only, the BSD-3-Clause, and the BSD-2-Clause.
