介绍
=======

:link_to_translation:`en:[English]`

此文档概述了 ESP-IDF 中低功耗蓝牙协议栈的架构，并提供了一些相关文档和应用示例的快速链接。

.. only:: esp32

    {IDF_TARGET_NAME} 支持双模蓝牙 4.2，并且已经通过双模蓝牙 4.2 认证和低功耗蓝牙 5.0 认证。

.. only:: esp32c3 or esp32s3

    {IDF_TARGET_NAME} 支持蓝牙 5.0 (LE)，并且已经通过低功耗蓝牙 5.4 认证。

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    {IDF_TARGET_NAME} 支持蓝牙 5.0 (LE)，并且已经通过低功耗蓝牙 5.3 认证。

ESP-IDF 中的低功耗蓝牙协议栈是一个分层架构，可在 {IDF_TARGET_NAME} 系列芯片上实现低功耗蓝牙功能，详见下图。

.. only:: esp32 or esp32s3 or esp32c3 or esp32c6 or esp32c5 or esp32c61

    .. figure:: ../../../_static/bluetooth-architecture.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} 蓝牙协议栈架构

        {IDF_TARGET_NAME} 蓝牙协议栈架构

.. only:: esp32c2

    .. figure:: ../../../_static/bluetooth-architecture-no-ble-mesh.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} 蓝牙协议栈架构

        {IDF_TARGET_NAME} 蓝牙协议栈架构

.. only:: esp32h2

    .. figure:: ../../../_static/bluetooth-architecture-no-blufi.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} 蓝牙协议栈架构

        {IDF_TARGET_NAME} 蓝牙协议栈架构

参考下表可知特定芯片是否支持低功耗蓝牙模块。

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - 芯片系列
      - 控制器
      - ESP-Bluedroid
      - ESP-NimBLE
      - ESP-BLE-MESH
      - BluFi
    * - ESP32
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-S3
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C2
      - Y
      - Y
      - Y
      - \–
      - Y
    * - ESP32-C3
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C5
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C6
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C61
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-H2
      - Y
      - Y
      - Y
      - Y
      - \–

以下各节简要介绍了每个层，并提供了相关文档和应用示例的快速链接。


ESP 蓝牙控制器
--------------

底层为 ESP 蓝牙控制器，包含 PHY、基带、链路控制器、链路管理器、设备管理器和 HCI 等各种模块。该层管理硬件接口和链路，以库的形式提供功能，并通过 API 访问，且直接与硬件和低级别蓝牙协议交互。

- :doc:`API 参考 <../../api-reference/bluetooth/controller_vhci>`
- :example:`应用示例 <bluetooth/hci>`


ESP 蓝牙主机
-------------

有 ESP-Bluedroid 和 ESP-NimBLE 两个主机，其主要区别如下：

- 虽然两者都支持低功耗蓝牙，但 ESP-NimBLE 需要的堆和 flash 空间更少。

.. only:: esp32

  - ESP-Bluedroid 支持经典蓝牙和低功耗蓝牙，而 ESP-NimBLE 仅支持低功耗蓝牙。


ESP-Bluedroid
^^^^^^^^^^^^^

ESP-Bluedroid 是原生 Android 蓝牙协议栈 Bluedroid 的修改版，由两层组成：蓝牙上层 (BTU) 和蓝牙传输控制器层 (BTC)。BTU 层负责处理 L2CAP、GATT/ATT、SMP、GAP 等底层蓝牙协议以及其他配置文件，提供以 "bta" 为前缀的接口。BTC 层主要负责向应用层提供以 "esp" 为前缀的支持接口，并处理基于 GATT 的配置文件以及其他任务。所有的 API 都位于 ESP_API 层，开发者应使用以 "esp" 为前缀的低功耗蓝牙 API。

.. only:: esp32

  {IDF_TARGET_NAME} 的 ESP-Bluedroid 支持经典蓝牙和低功耗蓝牙。

.. only:: not esp32

  {IDF_TARGET_NAME} 的 ESP-Bluedroid 仅支持低功耗蓝牙，不支持经典蓝牙。

- API 参考

  - :doc:`../../api-reference/bluetooth/bt_common`
  - :doc:`低功耗蓝牙 <../../api-reference/bluetooth/bt_le>`

.. only:: esp32

    - :example:`低功耗蓝牙 4.2 应用程序示例 <bluetooth/bluedroid/ble>`

.. only:: not esp32

    - :example:`低功耗蓝牙 4.2 应用程序示例 <bluetooth/bluedroid/ble>`
    - :example:`低功耗蓝牙 5.0 应用程序示例 <bluetooth/bluedroid/ble_50>`

ESP-NimBLE
^^^^^^^^^^

ESP-NimBLE 是建立在 Apache Mynewt 开发的 NimBLE 主机协议栈之上的主机协议栈，已经为 {IDF_TARGET_NAME} 系列芯片和 FreeRTOS 进行了移植。通过维持现有 NimBLE API，并添加一个单独的 ESP-NimBLE API 进行初始化，使端口层保持简洁，也便于开发者操作。

ESP-NimBLE 仅支持低功耗蓝牙，不支持经典蓝牙。

- `Apache Mynewt NimBLE 用户指南 <https://mynewt.apache.org/latest/network/index.html>`__
- API 参考

  - `NimBLE API 参考 <https://mynewt.apache.org/latest/network/ble_hs/ble_hs.html>`__
  - :doc:`ESP-NimBLE 初始化 API 参考 initialization <../../api-reference/bluetooth/nimble/index>`

- :example:`应用程序示例 <bluetooth/nimble>`


蓝牙规范
--------

主机协议层之上是 Espressif 的蓝牙规范和一些常见的蓝牙规范。根据具体配置，这些规范可以在 ESP-Bluedroid 或 ESP-NimBLE 上运行。


.. only:: SOC_BLE_MESH_SUPPORTED

  ESP-BLE-MESH
  ^^^^^^^^^^^^

  ESP-BLE-MESH 基于 Zephyr 蓝牙 Mesh 协议栈，其实现支持设备配网和节点控制，还支持代理、中继、低功耗和朋友等节点功能。

  - :doc:`ESP-BLE-MESH 文档 <../esp-ble-mesh/ble-mesh-index>`：功能列表、快速入门、架构、应用示例描述、常见问题等。
  - :example:`应用示例 <bluetooth/esp_ble_mesh>`


.. only:: SOC_BLUFI_SUPPORTED

  BluFi
  ^^^^^

  {IDF_TARGET_NAME} 的 BluFi 是通过蓝牙信道进行的 Wi-Fi 网络配置功能。BluFi 提供了将 Wi-Fi 配置和凭据传递给 {IDF_TARGET_NAME} 的安全协议，从而使 {IDF_TARGET_NAME} 连接到 AP 或搭建软 AP。

  - :doc:`BluFi 文档 <blufi>`
  - :example:`应用示例 <bluetooth/blufi>`


应用
----

最上层是应用层。利用上述 API 和蓝牙规范，可以在 ESP-Bluedroid 和 ESP-NimBLE 协议栈之上创建特定用例的低功耗蓝牙应用程序。
