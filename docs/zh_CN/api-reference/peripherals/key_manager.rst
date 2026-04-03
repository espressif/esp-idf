.. _key-manager:

密钥管理器
==========

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} 中的密钥管理器外设为加密密钥提供了硬件辅助的 **密钥部署和密钥恢复** 功能，用于配置和使用加密密钥，而无需将明文密钥内容存储在 flash、RAM 或 eFuse 中。

密钥管理器适用于需要安全处理常驻加密密钥的应用程序。

.. only:: esp32p4

  .. note::

        密钥管理器外设仅支持 ESP32-P4 芯片版本 v3.x (rev >= 3)。

.. only:: esp32c5

    .. note::

        密钥管理器外设仅支持 ESP32-C5 芯片版本 >= v1.2。

密钥管理器具有以下特性：

- **设备唯一性**

  密钥通过每块芯片独有的硬件唯一密钥 (HUK) 进行加密绑定。

- **无明文密钥存储**

  密钥材料不会暴露于软件可访问的内存中。

- **灵活的密钥生命周期**

  密钥可以被部署、恢复或替换为新密钥，无需为每个密钥重新烧录 eFuse。

- **抗物理提取**

  读取 flash 或 eFuse 的内容不会泄露可用的密钥材料。

硬件唯一密钥 (HUK)
-------------------

硬件唯一密钥 (HUK) 是完全由硬件 HUK 外设生成的设备专属密钥，源自 SRAM 物理不可克隆函数 (PUF)。重建 HUK 时，需要用到 HUK 恢复信息，该信息包含在密钥管理器为已部署密钥生成的密钥恢复信息中。有关 HUK 外设的更多信息，请参阅 **《{IDF_TARGET_NAME} 技术参考手册》** > **密钥管理器章节** [`PDF <{IDF_TARGET_TRM_CN_URL}>`__] > **HUK 生成器**。

HUK 是所有通过密钥管理器部署的密钥的信任根。

密钥部署与恢复
--------------

密钥管理器分为两个不同的阶段运行：

- **密钥部署**

  加密密钥在芯片内部生成或被安全导入，并与 HUK 绑定。此步骤通常在生产阶段、首次启动时，或在应用程序运行并生成临时/持久密钥时执行。

- **密钥恢复**

  在后续启动过程中，密钥管理器部署的持久密钥会使用先前生成的密钥恢复信息进行还原，而不会暴露密钥值。

在部署过程中，密钥管理器生成 :cpp:type:`esp_key_mgr_key_recovery_info_t` 数据结构。对于持久密钥，应用程序必须将此数据保存在非易失性存储器（例如 flash）中，以便在后续启动时恢复密钥。

支持的密钥类型
--------------

密钥管理器可以管理以下密钥类型：

.. list::

    :SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY: - ECDSA
    :SOC_KEY_MANAGER_FE_KEY_DEPLOY: - flash 加密 (XTS-AES)
    :SOC_KEY_MANAGER_HMAC_KEY_DEPLOY: - HMAC
    :SOC_KEY_MANAGER_DS_KEY_DEPLOY: - 数字签名外设
    :SOC_KEY_MANAGER_FE_KEY_DEPLOY: - PSRAM 加密

每个密钥都与一个 :cpp:type:`esp_key_mgr_key_purpose_t` 相关联，该类型定义了硬件外设如何使用密钥。

密钥部署模式
------------

密钥管理器提供多种密钥部署模式，以满足不同的配置和安全需求。

随机部署模式
^^^^^^^^^^^^

在此模式下，密钥管理器在内部生成一个随机私钥。

- 应用程序软件无法获取密钥值。
- 不需要外部密钥材料。
- 适用于不需要备份或导出密钥的场景。

AES 部署模式
^^^^^^^^^^^^

在此模式下，可以安全部署用户指定的私钥。

- 密钥在传输到芯片之前进行加密。
- 使用辅助密钥材料保护部署过程。
- 适用于需要预定义密钥值的出厂配置场景。

ECDH0 部署模式
^^^^^^^^^^^^^^

在此模式下，私钥通过椭圆曲线 Diffie-Hellman (ECDH) 协商生成。

- 最终私钥不会被传输。
- 部署过程可以通过不受信任的信道进行。
- 适用于高安全性的配置环境。

有关各种部署模式的详细信息，请参阅 **《{IDF_TARGET_NAME} 技术参考手册》** > **密钥管理器章节** [`PDF <{IDF_TARGET_TRM_EN_URL}>`__] > **密钥管理器小节**。

.. ECDH1 Deploy Mode
.. ~~~~~~~~~~~~~~~~~
..
.. This mode is similar to ECDH0 Deploy Mode, with additional flexibility for manufacturing workflows.
..
.. - Supports negotiated key deployment using auxiliary recovery data
.. - Allows updating deployed keys by replacing auxiliary information
.. - Intended for large-scale manufacturing with controlled trust assumptions

典型工作流程
------------

首次启动或生产配置
^^^^^^^^^^^^^^^^^^

典型的配置流程包括：

1. 生成硬件唯一密钥 (HUK)
2. 使用恰当的部署模式部署所需的加密密钥
3. 将生成的 ``key_recovery_info`` 保存至非易失性存储器中
4. 如有需要，锁定相关安全配置 eFuse

此过程通常每台设备仅执行一次。

正常启动
^^^^^^^^

在正常启动过程中：

1. 应用程序提供先前由密钥管理器部署的密钥所生成的 ``key_recovery_info``
2. HUK 由硬件自动重建
3. 密钥管理器在内部恢复已部署的密钥
4. 密码外设可以使用已恢复的密钥

安全注意事项
------------

使用密钥管理器的应用程序应注意以下事项：

- 防止密钥管理器部署密钥的 ``key_recovery_info`` 遭受未授权的修改或丢失。
- 密钥部署成功后，锁定密钥管理器安全相关的 eFuse，防止重新部署同类型密钥。
- 除非明确需要，否则在已启用 flash 加密时，应避免部署新的 XTS-AES 密钥。

API 参考
--------

.. include-build-file:: inc/esp_key_mgr.inc
.. include-build-file:: inc/key_mgr_types.inc

应用示例
--------

:example:`security/key_manager` 演示了如何使用密钥管理器进行密钥部署，以及如何使用已部署的密钥执行签名操作。

该示例展示了如何：

- 初始化密钥管理器
- 使用 AES 部署模式部署密钥
- 使用 PSA 接口通过密钥管理器部署的密钥执行签名操作
