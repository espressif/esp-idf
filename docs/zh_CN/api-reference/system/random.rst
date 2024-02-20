随机数发生器
========================

:link_to_translation:`en:[English]`

{IDF_TARGET_RF_NAME: default="Wi-Fi 或蓝牙", esp32s2="Wi-Fi", esp32h2="蓝牙或 802.15.4 Thread/Zigbee", esp32c6="Wi-Fi 或蓝牙或 802.15.4 Thread/Zigbee"}
{IDF_TARGET_RF_IS: default="已启用", esp32s2="已启用"}
{IDF_TARGET_BOOTLOADER_RANDOM_INCOMPATIBLE: default="", esp32="I2S、"}

{IDF_TARGET_NAME} 中包含一个硬件随机数发生器 (RNG)，可以调用 API :cpp:func:`esp_random` 和 :cpp:func:`esp_fill_random` 从中获取随机数值。

满足下列任意一个或多个条件时，硬件 RNG 会产生真随机数：

- RF 子系统已启用，即 {IDF_TARGET_RF_NAME} {IDF_TARGET_RF_IS}。
- 调用 :cpp:func:`bootloader_random_enable` 启用了内部熵源，并且熵源尚未被 :cpp:func:`bootloader_random_disable` 禁用。
- 在 ESP-IDF :ref:`second-stage-bootloader` 运行时。这是因为默认的 ESP-IDF 引导加载程序启动时会调用 :cpp:func:`bootloader_random_enable`，并在执行应用程序前调用 :cpp:func:`bootloader_random_disable`。

当上述任一条件为真时，物理噪声样本会连续混合到内部硬件 RNG 状态中来提供熵。如需了解详情，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **随机数发生器 (RNG)** [`PDF <{IDF_TARGET_TRM_CN_URL}#rng>`__] 章节。

如果上述条件都不满足，那么 RNG 的输出仅应被看作伪随机数。

启动
-------

在启动过程中，ESP-IDF 引导加载程序暂时会启用一个非 RF 熵源（内部参考电压噪声），为首次生成的启动密钥提供熵。当应用程序开始执行后，一直到 {IDF_TARGET_RF_NAME} 初始化完成前，通常只有伪随机数可用。

如需在应用程序启动期间临时重启熵源，或为不使用 {IDF_TARGET_RF_NAME} 的应用程序临时重启熵源，请调用函数 :cpp:func:`bootloader_random_enable` 重启内部熵源。在使用 ADC、{IDF_TARGET_BOOTLOADER_RANDOM_INCOMPATIBLE} 或使用 {IDF_TARGET_RF_NAME} 前，必须调用函数 :cpp:func:`bootloader_random_disable` 以禁用熵源。

.. note::

    ESP-IDF 第二阶段引导加载程序在启动过程中启用的熵源会用熵来初始化内部 RNG 状态。但是，内部硬件 RNG 状态的大小并不足以提供连续的真随机数流。因此，在需要真随机数时必须启用连续的熵源。

.. note::

    如果应用程序需要真随机数源，但无法永久性地启用硬件熵源，可以考虑使用软件 DRBG （确定性随机数发生器）来实现，如 mbedTLS CTR-DRBG 或 HMAC-DRBG，并使用来自硬件 RNG 真随机数来获取初始熵。

.. only:: not esp32

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
