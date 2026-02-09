哈希消息认证码 (HMAC)
=============================================

:link_to_translation:`en:[English]`

哈希消息认证码 (HMAC) 是一种安全的身份认证技术，支持使用预共享的密钥验证消息的真实性和完整性。利用烧录在 eFuse 块中的密钥，HMAC 可以为生成 SHA256-HMAC 提供硬件加速。

更多有关应用操作流程，及 HMAC 计算过程的详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **HMAC 加速器 (HMAC)** [`PDF <{IDF_TARGET_TRM_CN_URL}#hmac>`__]。

通用应用方案
-------------------

现有 A、B 双方，他们都需要验证对方发送消息的真实性和完整性。那么在开始发送消息前，双方应通过安全通道交换密钥。

要验证来自 A 的信息，B 可遵循以下步骤：

- A 计算要发送的消息的 HMAC。
- A 将消息及消息的 HMAC 一并发送给 B。
- B 自行计算所接收消息的 HMAC。
- B 检查接收到的 HMAC 是否与计算得出的 HMAC 匹配。

若两个 HMAC 匹配，消息为真。

但 HMAC 本身还可以应用于更多场景，如支持 HMAC 的挑战-应答协议，或作为更多安全模块（详见下文）的密钥输入等。

{IDF_TARGET_NAME} 上的 HMAC
-----------------------------

在 {IDF_TARGET_NAME} HMAC 模块的 eFuse 中会烧录一个密钥，可将该 eFuse 密钥设置为禁止所有外部资源访问，避免密钥泄露。

此外，在 {IDF_TARGET_NAME} 上的 HMAC 有以下三种应用场景：

#. HMAC 支持软件使用
#. HMAC 用作数字签名 (DS) 的密钥
#. HMAC 用于启用软禁用的 JTAG 接口

第一种应用场景称为 **上行** 模式，后两种应用场景称为 **下行** 模式。

HMAC 的 eFuse 密钥
^^^^^^^^^^^^^^^^^^^

在 {IDF_TARGET_NAME} 中，有六个物理 eFuse 块可用作 HMAC 的密钥，分别是块 4 到块 9。在 API 中，枚举类型 :cpp:enum:`hmac_key_id_t` 将这些块映射为 ``HMAC_KEY0`` ~ ``HMAC_KEY5``。

每个密钥都有相应的 eFuse 参数 **密钥功能 (key purpose)**，决定密钥应用于 HMAC 的哪种应用场景。

.. list-table::
   :widths: 15 70
   :header-rows: 1

   * - 密钥功能
     - 应用场景
   * - 8
     - HMAC 支持软件使用
   * - 7
     - HMAC 用作数字签名 (DS) 的密钥
   * - 6
     - HMAC 启用软禁用的 JTAG 接口
   * - 5
     - HMAC 既用作数字签名 (DS) 的密钥，又用于启用 JTAG 接口

这样一来，可以确保密钥用于原定场景。

要计算 HMAC，软件必须同时提供包含密钥的密钥块 ID，以及 **密钥功能** （详情请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **eFuse 控制器 (eFuse)** [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__])。

在进行 HMAC 密钥计算前，HMAC 会验证软件所提供密钥块的功能。在软件所提供 ID 的对应密钥块中，eFuse 存储了密钥块的功能。只有当软件所提供密钥块的功能与 eFuse 中存储的密钥块功能匹配，才会继续进行计算。

HMAC 支持软件使用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

密钥功能值：8

在此情况下，HMAC 支持软件使用，如验证消息真实性等。

:cpp:func:`psa_mac_compute` 用于计算 HMAC，该函数接收一个不透明的 PSA 密钥，该密钥引用了包含密钥机密的 eFuse 密钥块，并且该密钥块的用途被设置为上行模式。

HMAC 用作数字签名 (DS) 的密钥
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

密钥功能值：7、5

HMAC 可用作密钥派生函数，解码数字签名模块使用的私钥参数。在此情况下，硬件使用标准信息进行计算。在 HMAC 部分只需提供 eFuse 密钥块和功能；而在数字签名模块则还需要一些额外参数。

无论是密钥还是实际的 HMAC，都不会暴露在 HMAC 和数字签名模块之外。对 HMAC 的计算，以及将其传递给数字签名模块的过程，均在内部进行。

详情请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

.. _hmac_for_enabling_jtag:

HMAC 启用 JTAG 接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

密钥功能值：6、5

HMAC 的第三种应用场景是将其作为密钥，启用软禁用的 JTAG 接口。

重新启用 JTAG 接口的步骤如下：

**第一步：设置**

1. 生成一个 256 位的 HMAC 密钥，用于重新启用 JTAG。
2. 将步骤 1 获得的密钥写入 eFuse 块，且 eFuse 块的密钥功能参数应为 HMAC_DOWN_ALL (5) 或 HMAC_DOWN_JTAG (6)。为此，可以使用固件中的 ``esp_efuse_write_key()`` 函数，或使用主机上的 ``idf.py efuse-burn-key`` 完成操作。
3. 使用 ``esp_efuse_set_read_protect()`` 将 eFuse 密钥块配置为读保护，防止软件读取写入到 eFuse 密钥块中的 HMAC 密钥值。
4. 在烧录到 {IDF_TARGET_NAME} 上时，将特定的位或位组设置为 ``soft JTAG disable``。这样可以永久禁用 JTAG 接口，除非软件提供正确的密钥值进行验证。

.. only:: esp32s2

    .. note::

        API **esp_efuse_write_field_bit(ESP_EFUSE_SOFT_DIS_JTAG)** 支持在 {IDF_TARGET_NAME} 上烧录 ``soft JTAG disable`` 位。

.. only:: not esp32s2

    .. note::

        API **esp_efuse_write_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, ESP_EFUSE_SOFT_DIS_JTAG[0]->bit_count)** 支持在 {IDF_TARGET_NAME} 上烧录 ``soft JTAG disable`` 位。

.. only:: esp32s2 or esp32s3

    .. note::

        置位 ``HARD_DIS_JTAG`` eFuse 时，JTAG 处于永久禁用状态，``SOFT_DIS_JTAG`` 功能将失效。

.. only:: not esp32s2 and not esp32s3

    .. note::

        置位 ``DIS_PAD_JTAG`` eFuse 时，JTAG 处于永久禁用状态，``SOFT_DIS_JTAG`` 功能将失效。

启用 JTAG

1. 以 eFuse 中的密钥和 32 个 ``0x00`` 字节为输入，经过 HMAC-SHA256 函数处理，得到的函数输出结果即重新启用 JTAG 的密钥。
2. 从固件调用 :cpp:func:`esp_hmac_jtag_enable` 函数时，传递上一步获取的密钥值。
3. 要在固件中重新禁用 JTAG，可以重置系统，或调用 :cpp:func:`esp_hmac_jtag_disable`。

关于如何软禁用以及重新启用 JTAG 的完整示例，请参考 :example:`security/hmac_soft_jtag`。该示例演示了如何使用 HMAC 重新启用已软禁用的 JTAG 接口，包括生成 HMAC 密钥、将其烧录到 eFuse 中以及根据密钥创建令牌数据等步骤。

更多有关详情，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **HMAC 加速器 (HMAC)** [`PDF <{IDF_TARGET_TRM_CN_URL}#hmac>`__]。


应用示例
-------------------

以下为针对特定应用场景的实例代码，可用于设置 eFuse 密钥，并将其用于计算支持软件使用的 HMAC。

``esp_efuse_write_key`` 可以设置 eFuse 中的物理密钥块 4，并设置其功能。``ESP_EFUSE_KEY_PURPOSE_HMAC_UP`` (8) 表明，该密钥仅适用于生成支持软件使用的 HMAC。

.. code-block:: c

    #include "esp_efuse.h"

    const uint8_t key_data[32] = { ... };

    esp_err_t status = esp_efuse_write_key(EFUSE_BLK_KEY4,
                        ESP_EFUSE_KEY_PURPOSE_HMAC_UP,
                        key_data, sizeof(key_data));

    if (status == ESP_OK) {
        // 密钥写入成功
    } else {
        // 密钥写入失败，可能已写入过
    }

接下来可以通过 PSA Crypto API，使用已存储的密钥来计算供软件使用的 HMAC。

.. code-block:: c

    #include "psa/crypto.h"
    #include "psa_crypto_driver_esp_hmac_opaque.h"

    uint8_t hmac[32];
    size_t hmac_length = 0;

    const char *message = "Hello, HMAC!";
    const size_t msg_len = 12;

    // 为 ESP-HMAC 不透明驱动设置密钥属性
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 256);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

    // 创建不透明密钥引用
    esp_hmac_opaque_key_t opaque_key = {
        .use_km_key = false,
        .efuse_block = EFUSE_BLK_KEY4,
    };

    // 导入不透明密钥
    psa_key_id_t key_id = 0;
    psa_status_t status = psa_import_key(&attributes, (uint8_t *)&opaque_key,
                                         sizeof(opaque_key), &key_id);
    if (status != PSA_SUCCESS) {
        // 导入密钥失败
        psa_reset_key_attributes(&attributes);
        return;
    }

    // 计算 HMAC
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                             (uint8_t *)message, msg_len,
                             hmac, sizeof(hmac), &hmac_length);

    // 清理
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);

    if (status == PSA_SUCCESS) {
        // HMAC 已写入 hmac 数组
    } else {
        // 计算 HMAC 失败
    }

API 参考
-------------

.. include-build-file:: inc/psa_crypto_driver_esp_hmac_opaque_contexts.inc
