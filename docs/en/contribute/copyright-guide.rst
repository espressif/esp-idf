Copyright Header Guide
======================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF is released under :project_file:`the Apache License 2.0 <LICENSE>` with some additional third-party copyrighted code released under various licenses. For further information please refer to :doc:`the list of copyrights and licenses <../../../COPYRIGHT>`.

This page explains how the source code should be properly marked with a copyright header. ESP-IDF uses the `Software Package Data Exchange (SPDX) <https://spdx.dev>`_ format which is short and can be easily read by humans or processed by automated tools for copyright checks.

How to Check the Copyright Headers
----------------------------------

Please make sure you have installed the :doc:`pre-commit hooks <install-pre-commit-hook>` which contain a copyright header checker as well. The checker can suggest a header if it is not able to detect a properly formatted SPDX header.

What If the Checker's Suggestion Is Incorrect?
----------------------------------------------

No automated checker (no matter how good is) can replace humans. So the developer's responsibility is to modify the offered header to be in line with the law and the license restrictions of the original code on which the work is based on. Certain licenses are not compatible between each other. Such corner cases will be covered by the following examples.

The checker can be configured with the ``tools/ci/check_copyright_config.yaml`` configuration file. Please check the options it offers and consider updating it in order to match the headers correctly.

Common Examples of Copyright Headers
------------------------------------

The simplest case is when the code is not based on any licensed previous work, e.g., it was written completely from scratch. Such code can be decorated with the following copyright header and put under the license of ESP-IDF::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Apache-2.0
     */

Less Restrictive Parts of ESP-IDF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some parts of ESP-IDF are deliberately under less restrictive licenses in order to ease their reuse in commercial closed source projects. This is the case for :project:`ESP-IDF examples <examples>` which are in Public domain or under the Creative Commons Zero Universal (CC0) license. The following header can be used in such source files::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Unlicense OR CC0-1.0
     */

The option allowing multiple licenses joined with the ``OR`` keyword from the above example can be achieved with the definition of multiple allowed licenses in the ``tools/ci/check_copyright_config.yaml`` configuration file. Please use this option with care and only selectively for a limited part of ESP-IDF.

Third Party Licenses
~~~~~~~~~~~~~~~~~~~~

Code licensed under different licenses, modified by Espressif Systems and included in ESP-IDF cannot be licensed under Apache License 2.0 not even if the checker suggests it. It is advised to keep the original copyright header and add an SPDX before it.

The following example is a suitable header for a code licensed under the "GNU General Public License v2.0 or later" held by John Doe with some additional modifications done by Espressif Systems::

    /*
     * SPDX-FileCopyrightText: 1991 John Doe
     *
     * SPDX-License-Identifier: GPL-2.0-or-later
     *
     * SPDX-FileContributor: 2019-2023 Espressif Systems (Shanghai) CO LTD
     */

The licenses can be identified and the short SPDX identifiers can be found in the official `SPDX license list`_. Other very common licenses are the GPL-2.0-only, the BSD-3-Clause, and the BSD-2-Clause.

In exceptional case, when a license is not present on the `SPDX license list`_, it can be expressed by using the `LicenseRef-[idString]`_ custom license identifier, for example ``LicenseRef-Special-License``. The full license text must be added into the ``LICENSES`` directory under ``Special-License`` filename. ::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: LicenseRef-Special-License
     */

Dedicated ``LicenseRef-Included`` custom license identifier can be used to express a situation when the custom license is included directly in the source file. ::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: LicenseRef-Included
     *
     * <Full custom license text>
     */

The configuration stored in ``tools/ci/check_copyright_config.yaml`` offers features useful for third party licenses:

* A different license can be defined for the files part of a third party library.
* The check for a selected set of files can be permanently disabled. Please use this option with care and only in cases when none of the other options are suitable.

.. _SPDX license list: https://spdx.org/licenses
.. _LicenseRef-[idString]: https://spdx.github.io/spdx-spec/v2.3/other-licensing-information-detected/#101-license-identifier-field
