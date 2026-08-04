经典蓝牙
========

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid Kconfig 选项已变更：

    - ``BT_ENC_KEY_SIZE_CTRL_ENABLED`` 已弃用，并将在后续版本中删除。

        Host 现在会在控制器通过 ``HCI_Read_Local_Supported_Commands`` 报告支持时，使用标准 ``HCI_Set_Min_Encryption_Key_Size`` 命令。新增 ``CONFIG_BT_CLASSIC_ENABLE_ENC_KEY_SIZE_CTRL_VSC`` 用于在标准命令不受支持时启用乐鑫厂商命令作为回退。

.. only:: SOC_ORCA_BREDR_CONTROLLER

    BR/EDR 控制器功能配置
    ---------------------

    与 ESP32 不同，ESP32-S31 采用全新的经典蓝牙控制器方案，名为 Orca BR/EDR Controller。新方案在 Kconfig 中提供了不同的用户配置选项。

    请注意以下事项：

    - 控制器相关选项位于 **Component config** > **Bluetooth** > **Controller Options** > **BR/EDR Controller Options**，与 ESP32 选项不通用。
    - 可选 LM/LMP 功能需在 Kconfig 中显式使能。未使能的功能不会注册，相关 HCI 命令将返回 **Unknown HCI Command** (``0x01``)。
    - Kconfig 选项与 LMP 特性、HCI 命令的对应关系见 :doc:`../../../api-guides/classic-bt/esp-bredr-controller-kconfig`。
