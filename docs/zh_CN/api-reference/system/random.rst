随机数发生器
========================

:link_to_translation:`en:[English]`

{IDF_TARGET_RF_NAME: default="Wi-Fi 或蓝牙", esp32s2="Wi-Fi", esp32h2="蓝牙或 802.15.4 Thread/Zigbee", esp32c6="Wi-Fi 或蓝牙或 802.15.4 Thread/Zigbee", esp32c5="Wi-Fi 或蓝牙或 802.15.4 Thread/Zigbee"}
{IDF_TARGET_RF_IS: default="已启用", esp32s2="已启用"}

{IDF_TARGET_NAME} 中包含一个硬件随机数发生器 (RNG)，可以调用 API :cpp:func:`esp_random` 和 :cpp:func:`esp_fill_random` 从中获取随机数值。

系统从随机数生成器的 RNG_DATA_REG 寄存器中读取的每一个 32 位数值都是一个真正的随机数。这些真正的随机数是基于系统中的热噪声和异步时钟失配生成的。

.. only:: SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED

    - 热噪声来自高速 ADC、SAR ADC 或两者。当高速 ADC 或 SAR ADC 被启用时，会生成比特流，并作为随机种子通过 XOR 逻辑门输入到随机数生成器中。

.. only:: not SOC_WIFI_SUPPORTED and not SOC_IEEE802154_SUPPORTED and not SOC_BT_SUPPORTED

    - 热噪声来自 SAR ADC。当 SAR ADC 被启用时，会生成比特流，并作为随机种子通过 XOR 逻辑门输入到随机数生成器中。

.. only:: not esp32

    - RC_FAST_CLK 是一种异步时钟源，它通过引入电路亚稳态来增加 RNG 的熵值。详细信息请参阅 :ref:`secondary entropy` 部分。

下图展示了 {IDF_TARGET_NAME} 上 RNG 的噪声来源：

.. only:: SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED

    .. image:: /../_static/esp_rng_noise_source_rf_available.svg
        :align: center

.. only:: not SOC_WIFI_SUPPORTED and not SOC_IEEE802154_SUPPORTED and not SOC_BT_SUPPORTED

    .. image:: /../_static/esp_rng_noise_source_rf_unavailable.svg
        :align: center

满足下列任意一个或多个条件时，硬件 RNG 会产生真随机数：

.. list::

    :SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED: - RF 子系统，即 {IDF_TARGET_RF_NAME} {IDF_TARGET_RF_IS}。启用后，RF 子系统会在内部启用高速 ADC，该 ADC 可用作熵源。高速 ADC 可能仅在相应的 RF 子系统处于活动状态（例如未处于睡眠模式）时可用。详细信息请参阅 :ref:`enabling RF subsystem` 部分。
    - 调用 :cpp:func:`bootloader_random_enable` 启用了内部熵源 (SAR ADC)，且熵源尚未被 :cpp:func:`bootloader_random_disable` 禁用。
    - 在 :ref:`second-stage-bootloader` 运行时。这是因为默认的 ESP-IDF 引导加载程序启动时会调用 :cpp:func:`bootloader_random_enable`，并在执行应用程序前调用 :cpp:func:`bootloader_random_disable`。

当上述任一条件为真时，物理噪声样本会连续混合到内部硬件 RNG 状态中来提供熵。如需了解详情，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **随机数发生器 (RNG)** [`PDF <{IDF_TARGET_TRM_CN_URL}#rng>`__] 章节。

如果上述条件都不满足，那么 RNG 的输出仅应被看作伪随机数。

.. only:: SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED

    .. _enabling RF subsystem:

    启用 RF 子系统
    ---------------

    可以通过以下任一 API 来启用 RF 子系统：

    .. list::

        :SOC_WIFI_SUPPORTED: - Wi-Fi: :cpp:func:`esp_wifi_start`
        :SOC_BT_SUPPORTED: - 蓝牙 (NimBLE): :cpp:func:`nimble_port_init()` 会在内部调用 :cpp:func:`esp_bt_controller_enable()`
        :SOC_BT_SUPPORTED: - 蓝牙 (Bluedroid): :cpp:func:`esp_bt_controller_enable()`
        :SOC_IEEE802154_SUPPORTED: - Thread/Zigbee: :cpp:func:`esp_openthread_init`

启动
-------

在启动过程中，ESP-IDF 引导加载程序暂时会启用一个非 RF 内部熵源（SAR ADC 使用内部参考电压噪声），为首次生成的启动密钥提供熵。

.. only:: not SOC_WIFI_SUPPORTED and not SOC_IEEE802154_SUPPORTED and not SOC_BT_SUPPORTED

    对于 {IDF_TARGET_NAME}，高速 ADC 不可用。因此，在应用启动时，非 RF 内部熵源 (SAR ADC) 默认保持启用状态。

.. only:: SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED

    但是，当应用程序开始执行后，一直到 {IDF_TARGET_RF_NAME} 初始化完成前或内部熵源再次被启用前，通常只有伪随机数可用。

    如需在应用程序启动期间临时重启熵源，或为不使用 {IDF_TARGET_RF_NAME} 的应用程序临时重启熵源，请调用函数 :cpp:func:`bootloader_random_enable` 重启内部熵源。在使用下列任一功能前，必须调用函数 :cpp:func:`bootloader_random_disable` 以禁用熵源。

    .. list::

        - ADC
        :esp32: - I2S
        :SOC_WIFI_SUPPORTED or SOC_IEEE802154_SUPPORTED or SOC_BT_SUPPORTED: - {IDF_TARGET_RF_NAME}

.. note::

    ESP-IDF 二级引导加载程序在启动过程中启用的熵源会用熵来初始化内部 RNG 状态。但是，内部硬件 RNG 状态的大小并不足以提供连续的真随机数流。因此，在需要真随机数时必须启用连续的熵源。

.. note::

    如果应用程序需要真随机数源，但无法永久性地启用硬件熵源，可以考虑使用软件 DRBG （确定性随机数发生器）来实现，如 mbedTLS CTR-DRBG 或 HMAC-DRBG，并使用来自硬件 RNG 真随机数来获取初始熵。

.. only:: not esp32

    .. _secondary entropy:

    二级熵源
    -----------------

    {IDF_TARGET_NAME} RNG 包含一个基于异步 8 MHz 内部振荡器采样的二级熵源（详情请参阅技术参考手册）。该熵源在 ESP-IDF 中始终处于启用状态，并通过硬件持续混合到 RNG 状态中。在测试中，即使在不启用主熵源时，这个二级熵源也足以通过 `Dieharder`_ 随机数测试套件（测试输入数据是通过连续重置 {IDF_TARGET_NAME} 生成短样本并将其拼接来创建的）。但是，目前只有在同时启用上文所述的主熵源时，才能保证产生真随机数。

API 参考
-------------

.. include-build-file:: inc/esp_random.inc
.. include-build-file:: inc/bootloader_random.inc

``getrandom()``
---------------

为方便移植，还提供了与 Linux 的 ``getrandom()`` 函数兼容的版本：

.. code-block:: c

    #include <sys/random.h>

    ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

此函数通过内部调用 :cpp:func:`esp_fill_random` 来实现。

``flags`` 参数将被忽略。该函数始终是非阻塞的，但随机数的强度取决于本文档所述条件。

如果 ``buf`` 参数为 NULL，返回值为 -1，并将 ``errno`` 设置为 ``EFAULT``。否则返回 ``buflen``。

``getentropy()``
----------------

为了便于移植，还提供了与 Linux 的 ``getentropy()`` 函数兼容的版本：

.. code-block:: c

    #include <unistd.h>

    int getentropy(void *buffer, size_t length);

此函数通过内部调用 :cpp:func:`getrandom` 实现。

随机数强度取决于本文档所述条件。

如果执行成功则返回 0，否则返回 -1，同时：

    - 如果 ``buffer`` 参数为 NULL，``errno`` 设置为 ``EFAULT``。
    - 如果 ``length`` 超过 256，``errno`` 设置为 ``EIO``。

.. _Dieharder: https://webhome.phy.duke.edu/~rgb/General/dieharder.php
