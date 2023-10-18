芯片版本
=============

:link_to_translation:`en:[English]`

概述
--------

{IDF_TARGET_NAME} 可能包含多个芯片版本。引入不同的版本主要用于修复问题，有时也用于添加新功能。:ref:`versioning scheme` 描述了这些芯片修订的版本以及运行时读取版本信息所用的 API。

须考虑应用程序、ESP-IDF 版本和芯片版本之间的兼容性：

- 应用程序可能依赖于某个芯片版本的修复内容或功能。
- 较新版本的硬件可能与较早版本的 ESP-IDF 不兼容。

本文档的 :ref:`revision_limitation` 部分描述了应用程序如何明确芯片版本需求，以及 ESP-IDF 如何对兼容性进行检查。文档最后提供了此方案的故障排除相关信息。

.. _versioning scheme:

版本控制方案
-----------------

芯片版本号通常以 ``vX.Y`` 的形式表示，其中：

- ``X`` 表示主版本号。该号码变更表示在旧版芯片上使用的软件与新版芯片不兼容，必须升级软件方可使用。

- ``Y`` 表示次版本号。该号码变更表示在旧版芯片上使用的软件与新版芯片兼容，无需升级软件。

如果新版芯片不包含重大变更，则可以兼容旧版芯片上运行的软件。此时，新版芯片的主版本号不变，次版本号加 1。例如，版本号从 ``v1.1`` 变为 ``v1.2``。

相反，如果新版芯片包含重大变更，则 **无法兼容** 旧版芯片的软件。此时，新版芯片的主版本号增加，次版本号设置为 0。例如，版本号从 ``v1.1`` 变为 ``v2.0``。

此版本控制方式能够表示芯片版本之间的衍生关系，并清晰表明芯片变更是否为重大变更。

在芯片发生非重大变更时，ESP-IDF 的执行逻辑应与此前最近版本的逻辑相同，从而达到无缝衔接。在这样便可直接将编译后的二进制文件迁移到更新了次版本的芯片上，无需升级 ESP-IDF 版本并重新编译整个项目。

当二进制文件在更新了主版本的芯片上意外执行时，软件也能够根据主版本报告问题。此主次版本方案还允许硬件变更分支化。

.. note::

    目前的主次版本号芯片版本方案是从 ESP-IDF v5.0 开始引入的。因此，早期 ESP-IDF 创建的引导加载程序将继续使用晶圆版本表示的芯片版本方案。

用于芯片版本的 eFuse 位
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

芯片使用以下几个 eFuse 字段来进行版本控制：

- 主版本号 (``WAFER_VERSION_MAJOR`` eFuse)
- 次版本号 (``WAFER_VERSION_MINOR`` eFuse)
- 忽略最大版本限制位 (``DISABLE_WAFER_VERSION_MAJOR`` eFuse)。请参考 :ref:`revision_limitation` 了解此 eFuse 字段。

.. note::

    此前的版本控制逻辑基于单一的 eFuse 版本字段，即 ``WAFER_VERSION``。这种方式无法表明芯片的更新是否为重大更新，是一种线性的版本控制逻辑。


芯片版本 API
^^^^^^^^^^^^^^^^^^

使用下列 API 从 eFuse 中读取芯片版本：

- :cpp:func:`efuse_hal_chip_revision`，返回的版本格式为 ``major * 100 + minor``。
- :cpp:func:`efuse_hal_get_major_chip_version` 返回主版本号。
- :cpp:func:`efuse_hal_get_minor_chip_version` 返回次版本号。

下列 Kconfig 选项（格式为 ``major * 100 + minor``）可以将芯片版本依赖添加到代码中：

- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN_FULL``
- ``CONFIG_ESP_REV_MIN_FULL``
- ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``
- ``CONFIG_ESP_REV_MAX_FULL``


.. _revision_limitation:

ESP-IDF 兼容性检查
-------------------------------

如果构建的应用程序需要支持特定芯片的多个版本，可通过 Kconfig 指定支持的最小和最大芯片版本号。

最小芯片版本号可以通过 Kconfig 选项 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 来选择。设置最小芯片版本后，软件只能在较新的芯片版本上运行，以便支持某些功能或修复某些错误。

最大芯片版本号无法指定，只能由当前使用的 ESP-IDF 版本自动决定。ESP-IDF 会拒绝启动任何超过最大芯片版本号的芯片版本。由于特定版本的 ESP-IDF 无法预知未来的芯片版本更新，因此最大芯片版本号通常设置为 ``maxinum supported MAJOR version + 99``。可以设置 “忽略最大版本” eFuse 来绕过最大版本限制，但这不能确保软件正常工作。

下文介绍了芯片版本未通过兼容性检查时显示的故障排除信息及解决方法，并描述了在早期 ESP-IDF 版本中与软件行为和兼容性检查相关的技术细节。

故障排除
^^^^^^^^^^^^^^^

1. 如果第二阶段引导加载程序所运行的芯片版本低于镜像（如软件镜像）中指定的最小版本，会发生重启并显示以下消息：

.. code-block:: none

    Image requires chip rev >= v3.0, but chip is v1.0

要解决此问题，

- 确保使用的芯片达到了要求的最低版本及以上。
- 减小 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 的值并重建镜像，使镜像的版本与当前芯片版本兼容。

1. 如果应用程序所需的芯片版本不处于最小和最大芯片版本的区间范围内，会发生重启并显示以下消息：

.. code-block:: none

    Image requires chip rev <= v2.99, but chip is v3.0

为解决此问题，需更新 ESP-IDF 到较新版本，以支持该芯片版本 (``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``)。也可以在 eFuse 中设置 ``Ignore maximal revision`` 位，或使用与当前 ESP-IDF 版本兼容的其他芯片版本。

二进制镜像的常见版本需求
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

二级引导程序和应用程序二进制镜像中包含 :cpp:type:`esp_image_header_t` 头文件，其中记录了可以运行该软件的芯片版本号。这一头文件有 3 个与版本相关的字段：

- ``min_chip_rev`` - 镜像所需芯片的最小主版本号（但对于 ESP32-C3，该字段指次版本号）。其值由 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 确定。
- ``min_chip_rev_full`` - 镜像所需芯片的最小次版本号，格式为 ``major * 100 + minor``。其值由 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 确定。
- ``max_chip_rev_full`` - 镜像所需芯片的最大版本，格式为 ``major * 100 + minor``。其值由 ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL`` 确定。用户无法对其进行修改，仅当 ESP-IDF 支持新版本时由乐鑫官方进行更改。

最大和最小版本限制
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

应用启动过程中，检查最小和最大版本的顺序如下：

1. 在运行第 2 阶段引导启动程序前，第 1 阶段引导启动程序（ROM 引导启动程序）不会在 :cpp:type:`esp_image_header_t` 中检查最小和最大版本字段。

2. 在第 2 阶段引导启动程序的初始化阶段，会检查引导程序自身是否可以在此版本的芯片上启动。它从引导启动程序镜像的头文件中读取最小版本，并与 eFuse 中的芯片版本进行比较。如果芯片版本低于最小版本，引导启动程序会拒绝启动并中止运行。此阶段不检查最大版本。

3. 然后，第 2 阶段引导启动程序会检查应用程序的版本要求。它从应用程序镜像的头文件中读取最小和最大版本，并与 eFuse 中的芯片版本进行比较。如果该芯片版本低于最小版本或高于最大版本，引导程序会拒绝启动并中止。然而，如果设置了忽略最大版本位，则可以忽略最大版本限制。软件确定可以使用此芯片版本时，用户可以自行设置忽略位。

4. 在空中升级 (OTA) 阶段，运行中的应用程序会检查新软件是否与芯片版本相匹配。它会从新应用程序镜像的标头中提取最小和最大版本，并与 eFuse 中的芯片版本进行比较。应用程序检查版本匹配的方式与引导启动程序相同，即芯片版本须处在最小和最大版本之间（忽略最大版本的逻辑也相同）。

向后兼容旧版 ESP-IDF 构建的引导启动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32 or esp32c3 or esp32s2 or esp32s3

    主要版本号和次版本号的 eFuse 位对于旧版引导启动程序（由早于 v5.0 版本的 ESP-IDF 进行构建）而言是未知的。旧版启动引导程序只使用一个 eFuse 位来表示芯片版本。

.. only:: esp32

    旧版引导启动程序不读取次版本号的 eFuse 位，且主版本号只能低于或等于 v3。这表明，旧版引导启动程序只能正确检测范围在 ``v0.0`` 至 ``v3.0`` 的芯片版本，其中，次版本号总是设置为 ``0``。

.. only:: esp32c2

    ESP-IDF v5.0 中添加了对 {IDF_TARGET_NAME} 芯片的支持。引导启动程序能够检测范围在 ``v0.0`` 至 ``v3.15`` 内的所有芯片版本。

.. only:: esp32c3

    在 ESP-IDF v4.3 中添加了对 {IDF_TARGET_NAME} 芯片的支持。旧版引导启动程序无法读取晶圆版本 eFuse 的所有位，只能读取前 3 个最低有效位。因此，旧版引导启动程序无法正确检测芯片版本。具体而言，只能正确检测 ``v0.0`` 至 ``v0.8`` 范围内的芯片版本，其他芯片版本则会被错误识别为该范围内的某个版本。

.. only:: esp32s2 or esp32s3

    在 ESP-IDF v4.2 中添加了对 {IDF_TARGET_NAME} 芯片的支持。 由于 ``Minimum Supported ESP32-S2 Revision`` Kconfig 选项未引入，{IDF_TARGET_NAME} 芯片在 :cpp:type:`esp_image_header_t` 头文件中将 ``rev_min`` 设置为 0。这表明旧版引导启动程序不会检查芯片版本。在 v0.0 至 v3.15 范围内，任何应用程序都可以通过此类引导加载程序加载。

请使用 ``esptool chip_id`` 命令查看芯片版本。

参考链接
----------

- `芯片版本编号方案兼容指南 <https://www.espressif.com.cn/sites/default/files/advisory_downloads/AR2022-005%20Compatibility%20Advisory%20for%20Chip%20Revision%20Numbering%20%20Scheme.pdf>`_
- `ESP-IDF 版本与乐鑫芯片版本兼容性 <https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY.md>`_
- `SoC Errata <https://www.espressif.com.cn/en/support/documents/technical-documents?keys=errata>`_
- :doc:`/versions`

API 参考
-------------

.. include-build-file:: inc/efuse_hal.inc
