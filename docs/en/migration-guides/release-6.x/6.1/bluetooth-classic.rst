Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid Kconfig options have been changed:

    - ``BT_ENC_KEY_SIZE_CTRL_ENABLED`` is deprecated and will be removed in a future release.

        The host now uses the standard ``HCI_Set_Min_Encryption_Key_Size`` command when the controller reports support for it via ``HCI_Read_Local_Supported_Commands``. A new ``CONFIG_BT_CLASSIC_ENABLE_ENC_KEY_SIZE_CTRL_VSC`` option has been added to enable the Espressif vendor-specific command as a fallback when the standard command is not supported.

.. only:: SOC_ORCA_BREDR_CONTROLLER

    BR/EDR Controller Feature Configuration
    ---------------------------------------

    Different from ESP32, ESP32-S31 adopts a new Bluetooth Classic controller solution, named Orca BR/EDR Controller. The new solution provides different user options in Kconfig.

    Please note the following:

    - Controller options live under **Component config** > **Bluetooth** > **Controller Options** > **BR/EDR Controller Options**. They are not interchangeable with the ESP32 options.
    - Optional LM/LMP features must be enabled explicitly in Kconfig. Disabled features are not registered; related HCI commands return **Unknown HCI Command** (``0x01``).
    - See :doc:`../../../api-guides/classic-bt/esp-bredr-controller-kconfig` for the mapping of Kconfig options to LMP features and HCI commands.
