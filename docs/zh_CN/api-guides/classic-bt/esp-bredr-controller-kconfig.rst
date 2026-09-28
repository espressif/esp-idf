BR/EDR 控制器功能配置
=====================

:link_to_translation:`en:[English]`

概述
----

{IDF_TARGET_NAME} 的经典蓝牙使用 **Orca BR/EDR Controller**。其功能通过 :component_file:`bt/porting_btdm/controller/bredr/Kconfig.in` 中的 Kconfig 选项在编译期选择。选项分为两类：

- **功能选项**：控制 Link Manager (LM/LMP) 特性以及对应 HCI 命令的注册。未使能的功能不会注册到控制器，相关 HCI 命令将返回 ``Unknown HCI Command`` (``0x01``)。
- **资源与调优选项**：配置连接数上限、缓冲区数量、默认发射功率、CCA 等参数，本身不会注册或注销 HCI 命令。

配置路径：

**Component config** > **Bluetooth** > **Controller Options** > **BR/EDR Controller Options**

.. note::

   使用 ESP-Bluedroid 且使能 HFP 时，若 :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` 为 ``0``，系统会将实际 SCO/eSCO 连接数设为 ``1``。详见 :component_file:`bt/porting_btdm/controller/bredr/include/bredr_user_cfg.h`。

在 BR/EDR 已使能（``BR/EDR Only`` 或 ``Dual Mode``）时，Inquiry、ACL 建链、配对、加密、Sniff、角色切换、Scan/Page、AFH、EIR 及信息查询等标准命令无需下方可选功能选项即可使用。

HCI 名称与 Opcode 遵循 Bluetooth Core Specification。表中标准命令以 Opcode Group Field (OGF) 和 Opcode Command Field (OCF) 成对给出，例如 ``0x01/0x0028``。乐鑫厂商命令使用 OGF ``0x3F``。

功能选项、LM 特性与 HCI 命令
----------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 28 42

   * - Kconfig 选项
     - LM / LMP 特性
     - HCI 命令 (Opcode)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0
     - SCO link；eSCO (EV3)
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
     - eSCO EV4 包类型
     - 无独立 HCI 命令。影响 ``HCI_Read_Local_Extended_Features`` (``0x04/0x0004``)。仅在 :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0 时可见。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV5_SUPP`
     - eSCO EV5 包类型
     - 同上。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV3_2_SUPP`
     - eSCO 2-EV3 包类型
     - 同上。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_EV3_3_SUPP`
     - eSCO 3-EV3 包类型
     - 同上。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_ESCO_3_SLOTS_SUPP`
     - eSCO 3-slot 包类型
     - 同上。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_HOLD_EN`
     - Hold Mode
     - | ``HCI_Hold_Mode`` (``0x02/0x0001``)
       | ``HCI_Read_Hold_Mode_Activity`` (``0x03/0x002B``)
       | ``HCI_Write_Hold_Mode_Activity`` (``0x03/0x002C``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EN`
     - Active Peripheral Broadcast (APB)；LM 层 truncated page 支持
     - APB 本身无独立 HCI 命令。Truncated page / CSB 接收相关 HCI 由 :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_RX_EN` 注册。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EXT_EN`
       | + ``CONFIG_BT_CTRL_BR_EDR_APB_EXT_BCST_ENC_EN``
     - Broadcast Encryption
     - ``HCI_Link_Key_Selection`` (``0x01/0x0017``)，同 ``HCI_Master_Link_Key``
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EXT_EN`
       | + ``CONFIG_BT_CTRL_BR_EDR_APB_EXT_PCA_EN``
     - Piconet Clock Adjustment (PCA)
     - 无。Clock Adjustment 通过 LMP 实现。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_TX_EN`
     - Synchronization train (master)；Connectionless Peripheral Broadcast – Transmit
     - | ``HCI_Set_Connectionless_Peripheral_Broadcast`` (``0x01/0x0041``)
       | ``HCI_Start_Synchronization_Train`` (``0x01/0x0043``)
       | ``HCI_Set_Reserved_LT_ADDR`` (``0x03/0x0074``)
       | ``HCI_Delete_Reserved_LT_ADDR`` (``0x03/0x0075``)
       | ``HCI_Set_Connectionless_Peripheral_Broadcast_Data`` (``0x03/0x0076``)
       | ``HCI_Read_Synchronization_Train_Parameters`` (``0x03/0x0077``)
       | ``HCI_Write_Synchronization_Train_Parameters`` (``0x03/0x0078``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_RX_EN`
     - Synchronization scan；Connectionless Peripheral Broadcast – Receive
     - | ``HCI_Truncated_Page`` (``0x01/0x003F``)
       | ``HCI_Truncated_Page_Cancel`` (``0x01/0x0040``)
       | ``HCI_Set_Connectionless_Peripheral_Broadcast_Receive`` (``0x01/0x0042``)
       | ``HCI_Receive_Synchronization_Train`` (``0x01/0x0044``)
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_LK_STORE_EN`
     - 控制器链路密钥存储（目前仅支持 RAM）
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
     - 使能乐鑫自定义的 RF 测试指令（非 Bluetooth Core Spec 特性）。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_SAM_EN`
       | 需要 :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES`。
     - Slot Availability Mask (SAM)
     - 无。SAM 通过 LMP feature bits 协商。部分实现，目前用于 ACL 空口时间调度。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_MWS_EN`
       | 依赖 :ref:`CONFIG_BT_CTRL_BR_EDR_SAM_EN`。
     - Mobile Wireless Standards (MWS) 共存
     - | ``HCI_Set_External_Frame_Configuration`` (``0x03/0x006F``)
       | ``HCI_Set_MWS_PATTERN_Configuration`` (``0x03/0x0073``)
       | 部分实现，仅支持上述命令。

资源与调优选项
--------------

以下选项配置控制器容量与射频默认参数，本身不会使能或关闭 LM feature bits，也不会单独注册/注销 HCI 命令。

.. list-table::
   :header-rows: 1
   :widths: 36 64

   * - Kconfig 选项
     - 说明
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_ACL_CONN`
     - BR/EDR ACL 最大连接数 (1–7)，限制控制器可维护的 ACL 链路数量。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_ACLU_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_DYNAMIC_ACLU_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_ACLU_RX_BUF_NB`
     - | ACL-U TX/RX buffer 数目。
       | ``ACLU_TX_BUF_NB`` 为 ACL-U TX buffer 总数，会体现在 ``HCI_Read_Buffer_Size`` 的返回结果中。
       | ``DYNAMIC_ACLU_TX_BUF_NB`` 从该总数中划出。若配置为非 0，这部分 buffer 会在运行中需要时从 heap 中分配，而非在控制器初始化时预分配；在运行中未使用时可减少内存占用，但用到时可能会拖慢控制器执行速度。
       | ``ACLU_RX_BUF_NB`` 为 ACL-U RX buffer 数目。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_STATIC_SYNC_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_DYNAMIC_SYNC_TX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_STATIC_SYNC_RX_BUF_NB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_SYNC_RX_BUF_NB_PER_LINK`
     - | SCO/eSCO TX/RX buffer 数目。仅在 :ref:`CONFIG_BT_CTRL_BR_EDR_MAX_SYNC_CONN` > 0 时可见；静态 buffer 默认值随最大同步连接数变化。
       | 静态 TX buffer 在控制器初始化时分配，由所有 sync 链路共享。动态 TX buffer 在第一条 sync 链路建立时分配，并在最后一条 sync 链路释放相关 buffer 后回收。
       | ``SYNC_RX_BUF_NB_PER_LINK`` 为每条链路的平均 RX buffer 数目（静态 + 动态）。若静态 RX buffer 少于 ``SYNC_RX_BUF_NB_PER_LINK`` × 当前 sync 链路数，则会随链路增减动态扩缩 RX pool。设为 ``0`` 可禁用动态 sync RX buffer。
   * - ``CONFIG_BT_CTRL_BR_EDR_SCO_DATA_PATH_HCI``
     - SCO/eSCO 默认数据通路。目前仅支持 HCI；PCM/I2S 路由尚不可用。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_CCA_ENABLED`
       | + :ref:`CONFIG_BT_CTRL_BR_EDR_CCA_RSSI_THRESH`
     - TX Clear Channel Assessment。使能后，若检测到的信号强度高于 CCA 阈值（单位：-1 dBm），控制器可取消本次发送。无独立 HCI 命令。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MIN`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ACL_MAX`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_PAGE`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_PSCAN`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_ISCAN`
     - | ACL、Page、Page Scan、Inquiry Scan 的默认发射功率 (dBm)。
       | 芯片实际支持的发射功率范围可通过函数 :cpp:func:`esp_bredr_tx_power_range_get` 获取；超出该范围的配置会被拒绝。
       | ACL 链路功率可随对端 LMP 功率控制请求调整。ACL 初始功率继承自 ``TX_PWR_PAGE`` (Central) 或 ``TX_PWR_PSCAN`` (Peripheral)，建议将二者配置在 ACL 最小/最大范围内。
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_APB`
     - APB 默认发射功率。需使能 :ref:`CONFIG_BT_CTRL_BR_EDR_APB_EN`。
   * - | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_CPB`
       | :ref:`CONFIG_BT_CTRL_BR_EDR_TX_PWR_STRAIN`
     - CPB 与 Synchronization Train 的默认发射功率。需使能 :ref:`CONFIG_BT_CTRL_BR_EDR_CPB_TX_EN`。

Vendor Events
-------------

.. list-table::
   :header-rows: 1
   :widths: 36 64

   * - Kconfig 选项
     - 相关 HCI vendor event
   * - :ref:`CONFIG_BT_CTRL_BR_EDR_LEGACY_AUTH_VENDOR_EVT`
     - 使能 Legacy Authentication Completed Vendor Event，以帮助缓解 Legacy 认证过程中的 BIAS 攻击。默认使能。
