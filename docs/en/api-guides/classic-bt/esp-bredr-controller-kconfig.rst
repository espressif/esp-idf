BR/EDR Controller Feature Configuration
=======================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

{IDF_TARGET_NAME} uses the **Orca BR/EDR Controller** for Classic Bluetooth. Its capabilities are selected at build time through Kconfig options in :component_file:`bt/porting_btdm/controller/bredr/Kconfig.in`. Options fall into two groups:

- **Feature options**: gate Link Manager (LM/LMP) features and the HCI commands that implement them. Disabled features are not registered in the controller; related HCI commands return **Unknown HCI Command** (``0x01``).
- **Resource and tuning options**: set connection limits, buffer counts, default TX power, CCA, and similar parameters. They do not register or unregister HCI commands by themselves.

Configuration path:

**Component config** > **Bluetooth** > **Controller Options** > **BR/EDR Controller Options**

.. note::

   When ESP-Bluedroid HFP is enabled and :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` is ``0``, the build system sets the effective SCO/eSCO connection count to ``1``. See :component_file:`bt/porting_btdm/controller/bredr/include/bredr_user_cfg.h`.

When BR/EDR is enabled (**BR/EDR Only** or **Dual Mode**), standard commands for inquiry, ACL connection, pairing, encryption, sniff, role switch, scan/page, AFH, EIR, and informational queries are available without enabling the optional feature options below.

HCI names and opcodes follow the Bluetooth Core Specification. The table lists opcodes as Opcode Group Field (OGF) and Opcode Command Field (OCF) pairs, e.g., ``0x01/0x0028``. Espressif vendor commands use OGF ``0x3F``.

Feature Options, LM Features, and HCI Commands
----------------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 28 42

   * - Kconfig option(s)
     - LM / LMP features
     - HCI commands (opcode)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0
     - SCO link; eSCO (EV3)
     - | ``HCI_Setup_Synchronous_Connection`` (``0x01/0x0028``)
       | ``HCI_Accept_Synchronous_Connection_Request`` (``0x01/0x0029``)
       | ``HCI_Reject_Synchronous_Connection_Request`` (``0x01/0x002A``)
       | ``HCI_Enhanced_Setup_Synchronous_Connection`` (``0x01/0x003D``)
       | ``HCI_Enhanced_Accept_Synchronous_Connection_Request`` (``0x01/0x003E``)
       | ``HCI_Read_Voice_Setting`` (``0x03/0x0025``)
       | ``HCI_Write_Voice_Setting`` (``0x03/0x0026``)
       | ``HCI_Read_Synchronous_Flow_Control_Enable`` (``0x03/0x002E``)
       | ``HCI_Write_Synchronous_Flow_Control_Enable`` (``0x03/0x002F``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV4_SUPP`
     - eSCO EV4 packet type
     - No dedicated HCI command. Affects ``HCI_Read_Local_Extended_Features`` (``0x04/0x0004``). Visible only when :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV5_SUPP`
     - eSCO EV5 packet type
     - Same as above.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV3_2_SUPP`
     - eSCO 2-EV3 packet type
     - Same as above.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV3_3_SUPP`
     - eSCO 3-EV3 packet type
     - Same as above.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_3_SLOTS_SUPP`
     - eSCO 3-slot packet types
     - Same as above.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_HOLD_EN`
     - Hold Mode
     - | ``HCI_Hold_Mode`` (``0x02/0x0001``)
       | ``HCI_Read_Hold_Mode_Activity`` (``0x03/0x002B``)
       | ``HCI_Write_Hold_Mode_Activity`` (``0x03/0x002C``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EN`
     - Active Peripheral Broadcast (APB); truncated page support at LM
     - No dedicated HCI command for APB alone. Truncated page / CSB receive HCI commands are registered by :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_RX_EN`.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EXT_EN`
       | + ``CONFIG_BT_CTRL_BR_EDR_APB_EXT_BCST_ENC_EN``
     - Broadcast Encryption
     - ``HCI_Link_Key_Selection`` (``0x01/0x0017``), same as ``HCI_Master_Link_Key``
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EXT_EN`
       | + ``CONFIG_BT_CTRL_BR_EDR_APB_EXT_PCA_EN``
     - Piconet Clock Adjustment (PCA)
     - | None. Clock Adjustment is carried over LMP.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_TX_EN`
     - Synchronization train (master); Connectionless Peripheral Broadcast – Transmit
     - | ``HCI_Set_Connectionless_Peripheral_Broadcast`` (``0x01/0x0041``)
       | ``HCI_Start_Synchronization_Train`` (``0x01/0x0043``)
       | ``HCI_Set_Reserved_LT_ADDR`` (``0x03/0x0074``)
       | ``HCI_Delete_Reserved_LT_ADDR`` (``0x03/0x0075``)
       | ``HCI_Set_Connectionless_Peripheral_Broadcast_Data`` (``0x03/0x0076``)
       | ``HCI_Read_Synchronization_Train_Parameters`` (``0x03/0x0077``)
       | ``HCI_Write_Synchronization_Train_Parameters`` (``0x03/0x0078``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_RX_EN`
     - Synchronization scan; Connectionless Peripheral Broadcast – Receive
     - | ``HCI_Truncated_Page`` (``0x01/0x003F``)
       | ``HCI_Truncated_Page_Cancel`` (``0x01/0x0040``)
       | ``HCI_Set_Connectionless_Peripheral_Broadcast_Receive`` (``0x01/0x0042``)
       | ``HCI_Receive_Synchronization_Train`` (``0x01/0x0044``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_LK_STORE_EN`
     - Controller link key storage (RAM only)
     - | ``HCI_Read_Stored_Link_Key`` (``0x03/0x000D``)
       | ``HCI_Write_Stored_Link_Key`` (``0x03/0x0011``)
       | ``HCI_Delete_Stored_Link_Key`` (``0x03/0x0012``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_TEST_MODE_EN`
     - Test Mode
     - | ``HCI_Read_Loopback_Mode`` (``0x06/0x0001``)
       | ``HCI_Write_Loopback_Mode`` (``0x06/0x0002``)
       | ``HCI_Enable_Device_Under_Test_Mode`` (``0x06/0x0003``)
       | ``HCI_Write_Simple_Pairing_Debug_Mode`` (``0x06/0x0004``)
       | ``HCI_Write_Secure_Connections_Test_Mode`` (``0x06/0x000A``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_DTM_EN`
     - Direct Test Mode (DTM)
     - Enables Espressif custom vendor HCI commands for RF testing (non-Bluetooth Core Spec feature).
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_SAM_EN`
       | *(requires* :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES` *)*
     - Slot Availability Mask (SAM)
     - None. SAM is negotiated entirely over LMP feature bits. Partially implemented; currently used for ACL air time scheduling.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_MWS_EN`
       | *(requires* :ref:`CONFIG_BT_CTRL_BR_EDR_SAM_EN` *)*
     - Mobile Wireless Standards (MWS) coexistence
     - | ``HCI_Set_External_Frame_Configuration`` (``0x03/0x006F``)
       | ``HCI_Set_MWS_PATTERN_Configuration`` (``0x03/0x0073``)
       | Partially implemented; only the commands above are supported.

Resource and Tuning Options
---------------------------

These options configure controller capacity and RF defaults. They do not enable or disable LM feature bits or HCI command tables by themselves.

.. list-table::
   :header-rows: 1
   :widths: 36 64

   * - Kconfig option(s)
     - Description
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_ACL_CONN`
     - Maximum BR/EDR ACL connections (1–7). Limits how many ACL links the controller can maintain.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_ACLU_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_DYNAMIC_ACLU_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_ACLU_RX_BUF_NB`
     - | ACL-U TX/RX buffer counts.
       | ``ACLU_TX_BUF_NB`` is the total ACL-U TX buffer count and is reflected in the ``HCI_Read_Buffer_Size`` response.
       | ``DYNAMIC_ACLU_TX_BUF_NB`` is taken from that total. If non-zero, those buffers are allocated from the heap when needed at runtime instead of during controller initialization; unused dynamic buffers save memory, but allocating them on demand may slow the controller.
       | ``ACLU_RX_BUF_NB`` is the ACL-U RX buffer count.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_STATIC_SYNC_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_DYNAMIC_SYNC_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_STATIC_SYNC_RX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_SYNC_RX_BUF_NB_PER_LINK`
     - | SCO/eSCO TX/RX buffer counts. Visible only when :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0. Defaults for the static counts scale with the max sync connection count.
       | Static TX buffers are allocated at controller initialization and shared by all sync links. Dynamic TX buffers are allocated when the first sync link is established and freed after the last sync link releases them.
       | ``SYNC_RX_BUF_NB_PER_LINK`` is the average RX buffer count per link (static + dynamic). If static RX buffers are fewer than ``SYNC_RX_BUF_NB_PER_LINK`` × active sync links, the RX pool grows or shrinks as links are created or torn down. Set it to ``0`` to disable dynamic sync RX buffers.
   * - ``CONFIG_BT_CTRL_BR_EDR_SCO_DATA_PATH_HCI``
     - Default SCO/eSCO data path. Only HCI is supported; PCM/I2S routing is not yet available.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_CCA_ENABLED`
       | + :ref:`CONFIG_BT_CTRL_BR_EDR_CCA_RSSI_THRESH`
     - TX Clear Channel Assessment. When enabled, the controller may cancel a transmission if the measured signal is stronger than the CCA threshold (unit: -1 dBm). No dedicated HCI command.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MIN`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MAX`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_PAGE`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_PSCAN`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ISCAN`
     - | Default TX power levels (dBm) for ACL, Page, Page Scan, and Inquiry Scan.
       | The actual chip-supported TX power range can be obtained with the function :cpp:func:`esp_bredr_tx_power_range_get`. Values outside that range are rejected.
       | ACL link power can be adjusted on peer LMP power-control requests. The initial ACL power is taken from ``TX_PWR_PAGE`` (Central) or ``TX_PWR_PSCAN`` (Peripheral); keep those within the ACL min/max range when possible.
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_APB`
     - Default TX power for APB. Available when :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EN` is enabled.
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_CPB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_STRAIN`
     - Default TX power for CPB transmit and Synchronization Train. Available when :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_TX_EN` is enabled.

Vendor Events
-------------

.. list-table::
   :header-rows: 1
   :widths: 36 64

   * - Kconfig option
     - Related HCI vendor event
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_LEGACY_AUTH_VENDOR_EVT`
     - Enables the Legacy Authentication Completed vendor event to help mitigate BIAS attacks during legacy authentication. Enabled by default.
