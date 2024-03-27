版权标头指南
=========================

:link_to_translation:`en:[English]`

ESP-IDF 基于 :project_file:`the Apache License 2.0 <LICENSE>`，并包含一些不同许可证下的第三方版权代码。要了解更多信息，请参考 :doc:`../../../COPYRIGHT`。

本页面介绍了如何在源代码中正确标注版权标头。ESP-IDF 使用 `Software Package Data Exchange (SPDX) <https://spdx.dev>`_ 格式，简短易读，能够方便自动化工具处理及进行版权检查。

如何检查版权标头
---------------------------

请确保你已安装了包含版权标头检查器的 :doc:`pre-commit hooks <install-pre-commit-hook>`。若该检查器无法检测到格式正确的 SPDX 标头，则会提供标头建议。

检查器的建议不正确怎么办
---------------------------------

再好的自动化检查工具都无法取代人工检查。因此，开发者有责任修改检查器提供的标头，使其符合源代码的法律规范和许可证要求。某些许可证可能并不兼容，请参考后文示例。

检查器由 ``tools/ci/check_copyright_config.yaml`` 进行配置。请检查该配置文件提供的选项，并根据实际情况进行更新，以正确匹配标头。

常见版权标头示例
--------------------------

最简单的情况是，你的代码不基于任何此前已获得许可的代码，例如你完全独立完成的代码。此类代码可以使用如下版权标头，并放在 ESP-IDF 许可证下::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Apache-2.0
     */

ESP-IDF 限制性较小的部分
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 的某些部分特意采用了限制性较小的许可证，方便在商业闭源代码项目中重复使用。例如公有域下或免费知识共享 (CC0) 许可下的 :project:`ESP-IDF examples <examples>`。此类源文件可使用如下标头::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Unlicense OR CC0-1.0
     */

如允许使用多个许可证，可采用关键字 ``OR`` 连接，如上例所示。这可以通过在 ``tools/ci/check_copyright_config.yaml`` 配置文件中定义多个许可证实现。但是，请谨慎使用该方法，并仅在 ESP-IDF 项目限制性较小的部分中选择性使用。

第三方许可证
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

受到不同许可证许可，经乐鑫修改，并包含在 ESP-IDF 中的代码不能使用 Apache License 2.0 进行许可，即便检查器可能提出此类建议。建议保留原有版权标头，并在前面添加 SPDX 标识。

例如，对于一个由 "GNU General Public License v2.0 及以上" 许可证许可、John Doe 持有、且由 Espressif Systems 做出额外修改的代码文件，请按照如下示例提供标头::

    /*
     * SPDX-FileCopyrightText: 1991 John Doe
     *
     * SPDX-License-Identifier: GPL-2.0-or-later
     *
     * SPDX-FileContributor: 2019-2023 Espressif Systems (Shanghai) CO LTD
     */

这些许可证能够得到识别，并且可以在官方 `SPDX license list`_ 中找到 SPDX 标识符。其他常见许可证还包括 GPL-2.0-only，BSD-3-Clause 和 BSD-2-Clause。

特殊情况下， 如果一个许可证没有包含在 `SPDX license list`_ 中，可使用 `LicenseRef-[idString]`_ 自定义许可证标识符，如以下示例中的 ``LicenseRef-Special-License``。另外，必须把完整的许可文本添加到 ``Special-License`` 文件夹下的 ``LICENSES`` 路径中::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: LicenseRef-Special-License
     */

如果自定义许可证直接包含在源文件中，可使用专门的自定义许可证标识符 ``LicenseRef-Included`` 进行说明::

    /*
     * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: LicenseRef-Included
     *
     * <Full custom license text>
     */

``tools/ci/check_copyright_config.yaml`` 中的配置为第三方许可证提供了许多有用的功能：

* 对第三方程序库的文件部分，可以定义一个不同的许可证。
* 可以永久禁用对选定文件集的检查。请谨慎使用该选项，并且仅在其他选项都不适用时，才可使用该选项。

.. _SPDX license list: https://spdx.org/licenses
.. _LicenseRef-[idString]: https://spdx.github.io/spdx-spec/v2.3/other-licensing-information-detected/#101-license-identifier-field
