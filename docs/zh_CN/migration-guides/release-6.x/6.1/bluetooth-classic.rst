经典蓝牙
========

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid Kconfig 选项已变更：

    - ``BT_ENC_KEY_SIZE_CTRL_ENABLED`` 已弃用，并将在后续版本中删除。

        Host 现在会在控制器通过 ``HCI_Read_Local_Supported_Commands`` 报告支持时，使用标准 ``HCI_Set_Min_Encryption_Key_Size`` 命令。新增 ``CONFIG_BT_CLASSIC_ENABLE_ENC_KEY_SIZE_CTRL_VSC`` 用于在标准命令不受支持时启用乐鑫厂商命令作为回退。
