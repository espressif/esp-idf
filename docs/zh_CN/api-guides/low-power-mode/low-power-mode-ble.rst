低功耗蓝牙\ :sup:`®` 场景下低功耗模式介绍
========================================================================

:link_to_translation:`en:[English]`

本节介绍低功耗蓝牙 (Bluetooth LE) 在低功耗模式下的时钟源选择，以及常见相关问题。

低功耗模式下的时钟源选择
--------------------------------------------

在低功耗蓝牙应用场景中，由于协议要求休眠时钟精度需在 500 PPM 以内，light-sleep 和 modem-sleep 模式下所用的时钟源必须满足该要求。如果时钟精度不足，可能会出现 ACL 连接失败或超时断开等问题。**因此在使用前请确保所选时钟源及其精度满足要求。**

选择主晶振
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

要选择主晶振作为低功耗蓝牙内部时钟源，请配置以下选项：

.. only:: esp32

    :ref:`CONFIG_BTDM_CTRL_LOW_POWER_CLOCK` = Main crystal (`CONFIG_BTDM_CTRL_LPCLK_SEL_MAIN_XTAL`)

.. only:: esp32c3 or esp32s3

    :ref:`CONFIG_BT_CTRL_LOW_POWER_CLOCK` = Main crystal (`CONFIG_BT_CTRL_LPCLK_SEL_MAIN_XTAL`)

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    :ref:`CONFIG_BT_LE_LP_CLK_SRC` = Use main XTAL as RTC clock source (`CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL`)

选择主晶振后，light-sleep 模式下主晶振电源不会关闭，因此电流消耗更高。有关使用主晶振与 32 kHz 外部晶振在 light-sleep 模式下的典型电流消耗，请参考 :example_file:`Power Save README <bluetooth/nimble/power_save/README.md>` 。

选择 32 kHz 外部晶振
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

要使用 32 kHz 外部晶振作为低功耗蓝牙内部时钟源，请配置以下选项：

**配置路径 1：**

.. only:: esp32

    :ref:`CONFIG_BTDM_CTRL_LOW_POWER_CLOCK` = External 32 kHz crystal/oscillator (`CONFIG_BTDM_CTRL_LPCLK_SEL_EXT_32K_XTAL`)

.. only:: esp32c3 or esp32s3

    :ref:`CONFIG_BT_CTRL_LOW_POWER_CLOCK` = External 32 kHz crystal/oscillator (`CONFIG_BT_CTRL_LPCLK_SEL_EXT_32K_XTAL`)

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    :ref:`CONFIG_BT_LE_LP_CLK_SRC` = Use system RTC slow clock source (`CONFIG_BT_LE_LP_CLK_SRC_DEFAULT`)

**配置路径 2：**

:ref:`CONFIG_RTC_CLK_SRC` = External 32 kHz crystal (`CONFIG_RTC_CLK_SRC_EXT_CRYS`)

**注意：** 即使在 menuconfig 中选择了 32 kHz 外部晶振，如果低功耗蓝牙初始化时未检测到外部晶振，系统会自动切换为主晶振，可能导致 light-sleep 电流高于预期。

选择 136 kHz RC 振荡器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32c3 or esp32s3

    要使用 136 kHz 内部 RC 振荡器作为低功耗蓝牙内部时钟源，请配置以下选项：

    **配置路径 1：**

    :ref:`CONFIG_BT_CTRL_LOW_POWER_CLOCK` = Internal 136kHz RC oscillator (`CONFIG_BT_CTRL_LPCLK_SEL_RTC_SLOW`)

    一般来说，136 kHz RC 振荡器难以满足低功耗蓝牙的精度要求，仅适用于对时钟精度要求不高的场景，如传统广播 (ADV) 或扫描 (SCAN)。它不支持以中心角色或外设角色建立连接。

.. only:: esp32

    **注意：** ESP32 不支持 136 kHz RC 振荡器作为低功耗蓝牙时钟源。

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    要使用 136 kHz 内部 RC 振荡器作为低功耗蓝牙内部时钟源，请配置以下选项：

    **配置路径 1：**

    :ref:`CONFIG_BT_LE_LP_CLK_SRC` = Use system RTC slow clock source (`CONFIG_BT_LE_LP_CLK_SRC_DEFAULT`)

.. only:: not esp32

    **配置路径 2：**

    :ref:`CONFIG_RTC_CLK_SRC` = Internal 136 kHz RC oscillator (`CONFIG_RTC_CLK_SRC_INT_RC`)

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    对于需要低功耗且没有 32 kHz 外部晶振的场景，可以选择 136 kHz RC 振荡器。然而，该时钟无法满足低功耗蓝牙的 500 PPM 的休眠时钟精度需求。如果对端设备使用 ESP 芯片，低功耗蓝牙功能仍可正常工作；但如果对端设备不是 ESP 芯片，则以下低功耗蓝牙行为将无法支持：

      1. 作为连接的 Central 方
      2. 作为 Periodic Advertising 的广播方

    如果对端设备也用 136 kHz RC 作为时钟源，需要如下配置:

    **配置路径：**

      - :ref:`CONFIG_BT_LE_LL_PEER_SCA_SET_ENABLE` = y
      - :ref:`CONFIG_BT_LE_LL_PEER_SCA` = 3000

    **注意：** 使用 136 kHz RC 振荡器可能偶发连接断开或连接失败。


如何确认当前低功耗蓝牙使用的时钟源
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

可通过低功耗蓝牙初始化时的日志判断当前时钟源：

.. list-table:: 低功耗蓝牙初始化日志与时钟源对应关系
    :widths: 50 50
    :header-rows: 1

    * - 日志内容
      - 时钟源
    * - Using main XTAL as clock source
      - 主晶振 (Main XTAL)
    * - Using 136 kHz RC as clock source
      - 内部 136 kHz RC 振荡器 (Internal 136 kHz RC oscillator)
    * - Using external 32.768 kHz crystal as clock source
      - 外部 32 kHz 晶振 (External 32 kHz crystal)
    * - Using external 32.768 kHz oscillator at 32K_XP pin as clock source
      - 外部 32 kHz 振荡器 (32K_XP 引脚) (External 32 kHz oscillator at 32K_XP pin)


常见问题
--------------------------------------

1. 低功耗蓝牙 ACL 连接在低功耗模式下建立失败或断开
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如时钟源选择部分所述，ACL 连接建立失败或断开时，请首先检查当前时钟源是否满足低功耗蓝牙精度要求。


2. 实测 light-sleep 电流高于预期
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如时钟源选择部分所述，若主晶振为时钟源，light-sleep 模式下主晶振持续供电，电流消耗高于其他时钟源。平均电流可能会因具体应用而异，并取决于低功耗蓝牙的配置以及处于 light-sleep 模式的时间周期。某些应用的平均电流可能会更大，这是因为低功耗蓝牙在其中花费了更高比例的时间进行发射和接收。

3. 无法进入 light-sleep 模式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

启用 Auto light-sleep 后，若设备无法进入 light-sleep，通常是 IDLE 时间不足，未满足自动进入条件。这可能由日志过多或低功耗蓝牙配置导致 IDLE 时间过短（如连续扫描）引起。
