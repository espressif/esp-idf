Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid Kconfig options have been changed:

    - ``BT_ENC_KEY_SIZE_CTRL_ENABLED`` is deprecated and will be removed in a future release.

        The host now uses the standard ``HCI_Set_Min_Encryption_Key_Size`` command when the controller reports support for it via ``HCI_Read_Local_Supported_Commands``. A new ``CONFIG_BT_CLASSIC_ENABLE_ENC_KEY_SIZE_CTRL_VSC`` option has been added to enable the Espressif vendor-specific command as a fallback when the standard command is not supported.
