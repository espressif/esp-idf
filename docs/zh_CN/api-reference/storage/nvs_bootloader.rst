在引导程序中使用 NVS
======================

:link_to_translation:`en:[English]`

概述
-----

本指南概述了可用于自定义引导加载程序代码的 NVS（非易失性存储）功能及其限制。

由于引导加载程序运行环境的限制，自定义引导加载程序代码无法直接使用完整的 NVS API。为此，NVS 提供了一个简化 API，仅支持只读访问 NVS 数据。

该 API 支持读取除 blob 之外的所有 NVS 数据类型。一次 API 调用可以同时读取多个 NVS 条目。可以从同一 NVS 分区的不同命名空间读取值，读取结果存储在输入/输出结构数组中，每个数据项的固定大小最多为 8 字节。

由于引导加载程序中的堆内存分配限制，读取字符串条目时，API 要求调用者提供缓冲区及其大小。

读取加密的 NVS 分区
^^^^^^^^^^^^^^^^^^^^^^^

如果 NVS 分区使用 :doc:`nvs_encryption` 指南中的加密方案，该 API 还支持解密 NVS 数据。

应用程序应按照以下步骤使用 :cpp:func:`nvs_bootloader_read` API 读取加密 NVS 分区数据：

    1. 根据所选的 NVS 加密方案填充 NVS 安全配置结构 :cpp:type:`nvs_sec_cfg_t` （详情请参阅 :doc:`nvs_encryption`）。
    2. 使用 :cpp:func:`nvs_bootloader_read_security_cfg` API 从指定的安全方案中读取 NVS 安全配置。
    3. 获取安全配置后，使用 :cpp:func:`nvs_bootloader_secure_init` API 初始化 NVS flash 分区。
    4. 使用 :cpp:func:`nvs_bootloader_read` API 执行 NVS 读取操作。
    5. 使用 :cpp:func:`nvs_bootloader_secure_deinit` API 反初始化 NVS flash 分区，并清除安全配置。

.. only:: SOC_HMAC_SUPPORTED

    .. note::

        在使用基于 HMAC 的方案进行上述流程时，可以直接调用 :cpp:func:`nvs_flash_secure_init` API 对默认和自定义 NVS 分区进行加密，而无需启用 NVS 加密相关配置选项（如 :ref:`CONFIG_NVS_ENCRYPTION`， :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` -> ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC``， :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID`）。


应用示例
-----------

代码示例请参阅 ESP-IDF 示例 :example:`storage/nvs` 目录下的 :example:`storage/nvs/nvs_bootloader`。

本节演示了如何在输入/输出结构中准备数据，以支持不同的数据类型、命名空间和键。此外，还包含从 NVS 读取字符串数据的示例。

示例还演示了如何检查读取操作是否成功，数据是否存在不一致，或是否在 NVS 中找不到某些值的情形。该示例能够将 API 返回的值（或错误码）打印到控制台。

API 参考
-----------

.. include-build-file:: inc/nvs_bootloader.inc
