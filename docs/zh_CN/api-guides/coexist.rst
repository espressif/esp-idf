RF 共存
==================
:link_to_translation:`en:[English]`

概览
-----

{IDF_TARGET_NAME} 只支持一路 RF，Bluetooth (BT 和 BLE）和 Wi-Fi 共享这一路 RF，无法同时收发数据，因此采用时分复用的方法进行收发数据包。


{IDF_TARGET_NAME} 支持的共存场景
------------------------------------
.. only:: esp32c3 or esp32s3 or esp32

  .. table:: 表 1  Wi-Fi 和 BLE 共存支持功能

      +-------+--------+-----------+-----+------------+-----------+----------+
      |                            |BLE                                      |
      +                            +-----+------------+-----------+----------+
      |                            |Scan |Advertising |Connecting |Connected |
      +-------+--------+-----------+-----+------------+-----------+----------+
      | Wi-Fi |STA     |Scan       |Y    |Y           |Y          |Y         |
      +       +        +-----------+-----+------------+-----------+----------+
      |       |        |Connecting |Y    |Y           |Y          |Y         |
      +       +        +-----------+-----+------------+-----------+----------+
      |       |        |Connected  |Y    |Y           |Y          |Y         |
      +       +--------+-----------+-----+------------+-----------+----------+
      |       |SOFTAP  |TX Beacon  |Y    |Y           |Y          |Y         |
      +       +        +-----------+-----+------------+-----------+----------+
      |       |        |Connecting |C1   |C1          |C1         |C1        |
      +       +        +-----------+-----+------------+-----------+----------+
      |       |        |Connected  |C1   |C1          |C1         |C1        |
      +       +--------+-----------+-----+------------+-----------+----------+
      |       |Sniffer |RX         |C1   |C1          |C1         |C1        |
      +       +--------+-----------+-----+------------+-----------+----------+
      |       |ESP-NOW |RX         |S    |S           |S          |S         |
      +       +        +-----------+-----+------------+-----------+----------+
      |       |        |TX         |Y    |Y           |Y          |Y         |
      +-------+--------+-----------+-----+------------+-----------+----------+


.. only:: esp32

  .. table:: 表 2  Wi-Fi 和经典蓝牙 (BT) 共存支持功能

      +-------+--------+-----------+--------+-------------+-----+----------+-----------+
      |                            |BR/EDR                                             |
      +                            +--------+-------------+-----+----------+-----------+
      |                            |Inquiry |Inquiry scan |Page |Page scan | Connected |
      +-------+--------+-----------+--------+-------------+-----+----------+-----------+
      | Wi-Fi |STA     |Scan       |Y       |Y            |Y    |Y         |Y          |
      +       +        +-----------+--------+-------------+-----+----------+-----------+
      |       |        |Connecting |Y       |Y            |Y    |Y         |Y          |
      +       +        +-----------+--------+-------------+-----+----------+-----------+
      |       |        |Connected  |Y       |Y            |Y    |Y         |Y          |
      +       +--------+-----------+--------+-------------+-----+----------+-----------+
      |       |SOFTAP  |TX Beacon  |Y       |Y            |Y    |Y         |Y          |
      +       +        +-----------+--------+-------------+-----+----------+-----------+
      |       |        |Connecting |C1      |C1           |C1   |C1        |C1         |
      +       +        +-----------+--------+-------------+-----+----------+-----------+
      |       |        |Connected  |C1      |C1           |C1   |C1        |C1         |
      +       +--------+-----------+--------+-------------+-----+----------+-----------+
      |       |Sniffer |RX         |C1      |C1           |C1   |C1        |C1         |
      +       +--------+-----------+--------+-------------+-----+----------+-----------+
      |       |ESP-NOW |RX         |S       |S            |S    |S         |S          |
      +       +        +-----------+--------+-------------+-----+----------+-----------+
      |       |        |TX         |Y       |Y            |Y    |Y         |Y          |
      +-------+--------+-----------+--------+-------------+-----+----------+-----------+


.. note::

  Y：支持且性能稳定。
  C1：不能保证性能处于稳定状态。
  X：不支持。
  S：在STA模式下支持且性能稳定，否则不支持。


共存机制与策略
----------------------------------

共存机制
^^^^^^^^^^^^^^

基于优先级抢占的 RF 资源分配机制，如下图所示，Bluetooth 模块和 Wi-Fi 模块向共存模块申请 RF 资源，共存模块根据二者的优先级高低裁决 RF 归谁使用。

.. blockdiag::
    :scale: 100%
    :caption: 共存机制
    :align: center

    blockdiag {

      # global attributes
      node_height = 60;
      node_width = 120;
      span_width = 100;
      span_height = 60;
      default_shape = roundedbox;
      default_group_color = none;

      # node labels
   	  Wi-Fi [shape = box];
   	  Bluetooth [shape = box];
   	  Coexistence [shape = box, label = 'Coexistence module'];
   	  RF [shape = box, label = 'RF module'];

      # node connections
   	  Wi-Fi -> Coexistence;
   	  Bluetooth  -> Coexistence;
   	  Coexistence -> RF;
    }


共存策略
^^^^^^^^^^^^^^

共存周期和时间片
"""""""""""""""""""

.. only:: esp32

  Wi-Fi、BT、BLE 三者对于 RF 的使用，主要是按照时间片来划分的。在一个共存周期内，按照 Wi-Fi、BT、BLE 的顺序划分时间片。在 Wi-Fi 的时间片内，Wi-Fi 会向共存仲裁模块发出较高优先级的请求，同理，BT/BLE 在自己的时间片内会具有较高优先级。共存周期大小和各个时间片占比根据 Wi-Fi 的状态分成四类：


.. only:: esp32c3 or esp32s3

  Wi-Fi、BLE 二者对于 RF 的使用，主要是按照时间片来划分的。在 Wi-Fi 的时间片内，Wi-Fi 会向共存仲裁模块发出较高优先级的请求，在 Bluetooth 的时间片内，BLE 会具有较高优先级。共存周期大小和各个时间片占比根据 Wi-Fi 的状态分成四类：


.. list::

  :esp32: 1) IDLE 状态：BT 和 BLE 共存由 Bluetooth 模块控制。
  :esp32c3 or esp32s3: 1) IDLE 状态：RF 模块由 Bluetooth 模块控制。
  #) CONNECTED 状态：共存周期以目标信标传输时间 (Target Beacon Transmission Time, TBTT) 点为起始点，周期大于 100 ms。
  #) SCAN 状态：Wi-Fi 时间片以及共存周期都比在 CONNECTED 状态下的长。为了确保蓝牙的性能，蓝牙的时间片也会做相应的调整。
  #) CONNECTING 状态：Wi-Fi 时间片比在 CONNECTED 状态下的长。为了确保蓝牙的性能，蓝牙的时间片也会做相应的调整。


共存逻辑会根据当前 Wi-Fi 和 Bluetooth 的使用场景来选取不同的共存周期和共存时间片的划分策略。对应一个使用场景的共存策略，我们称之为“共存模板”。比如，Wi-Fi CONNECTED 与 BLE CONNECTED 的场景，就对应有一个共存模板。在这个共存模板中，一个共存周期内 Wi-Fi 和 BLE 的时间片各占 50%，时间分配如下图所示：

.. figure:: ../../_static/coexist_wifi_connected_and_ble_connected_time_slice.png
    :align: center
    :alt: Wi-Fi CONNECTED 和 BLE CONNECTED 状态下时间片划分图
    :figclass: align-center

    Wi-Fi CONNECTED 和 BLE CONNECTED 共存状态下时间片划分图


动态优先级
"""""""""""""""""""

共存模块对 Wi-Fi 和 Bluetooth 不同的状态赋予其不同的优先级。每种状态下的优先级并不是一成不变的，例如每 N 个广播事件 (Advertising event) 中会有一个广播事件使用高优先级。如果高优先级的广播事件发生在 Wi-Fi 时间片内，RF 的使用权可能会被 BLE 抢占。

.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi 非连接模块的共存
    """"""""""""""""""""""""""""

    在一定程度上，某些 Wi-Fi 非连接模块功耗参数 Window 与 Interval 的组合会导致共存模块在 Wi-Fi 时间片外申请共存优先级。这是为了按设定的功耗参数在共存时获取 RF 资源，但这会影响既定的蓝牙性能。

    如果 Wi-Fi 非连接模块功耗参数为默认值时，上述动作不会发生，共存模块会按照性能稳定的模式运行。因此，除非您对特定非连接功耗参数下的共存性能有足够的测试，请在共存场景下将 Wi-Fi 非连接模块功耗参数配置为默认参数。

    请参考 :ref:`非连接模块功耗管理 <connectionless-module-power-save-cn>` 获取更多信息。

如何使用共存功能
----------------------------------

共存 API 的使用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在大多数共存情况下，{IDF_TARGET_NAME} 会自动进行共存状态切换，无需调用 API 对其进行干预。但是对于 BLE MESH 和 Wi-Fi 的共存，{IDF_TARGET_NAME} 对其提供了两个 API。当 BLE MESH 的状态发生变化时，应先调用 :code:`esp_coex_status_bit_clear` 对上一个状态进行清除，然后调用 :code:`esp_coex_status_bit_set` 设置当前状态。

BLE MESH 共存状态描述
""""""""""""""""""""""""""""

由于 Wi-Fi 和 Bluetooth 固件无法获知当前的上层应用的场景，一些共存模板需要应用代码调用共存的 API 才能生效。BLE MESH 的工作状态就需要由应用层通知给共存模块，用于选择共存模板。

  - ESP_COEX_BLE_ST_MESH_CONFIG：正在组网。
  - ESP_COEX_BLE_ST_MESH_TRAFFIC：正在传输数据。
  - ESP_COEX_BLE_ST_MESH_STANDBY：处于空闲状态，无大量数据交互。


共存 API 错误代码
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

所有共存 API 都有自定义的返回值，即错误代码。这些代码可分类为：

  - 无错误，例如：返回值为 ESP_OK 代表 API 成功返回。
  - 可恢复错误，例如: 返回值为 ESP_ERR_INVALID_ARG 代表 API 参数错误。


设置共存编译时选项
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::

    - 在完成共存程序编写的时候，您必须通过 menuconfig 选择 :ref:`CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE` 打开软件共存配置选项，否则就无法使用上文中提到的共存功能。

    :esp32: - 为了在共存情况下获得更好的 Wi-Fi 和蓝牙的通信性能，建议将 Wi-Fi 协议栈的 task 和蓝牙 Controller 以及 Host 协议栈的 task 运行在不同的 CPU 上。您可以通过 :ref:`CONFIG_BTDM_CTRL_PINNED_TO_CORE_CHOICE` 和 :ref:`CONFIG_BT_BLUEDROID_PINNED_TO_CORE_CHOICE` （或者 :ref:`CONFIG_BT_NIMBLE_PINNED_TO_CORE_CHOICE` ）选择将蓝牙 controller 以及 host 协议栈的 task 放在同一个 CPU 上，再通过 :ref:`CONFIG_ESP32_WIFI_TASK_CORE_ID`  选择将 Wi-Fi 协议栈 task 放在另一个 CPU 上。

    :esp32s3: - 为了在共存情况下获得更好的 Wi-Fi 和蓝牙的通信性能，建议将 Wi-Fi 协议栈的 task 和蓝牙 Controller 以及 Host 协议栈的 task 运行在不同的 CPU 上，您可以通过 :ref:`CONFIG_BT_CTRL_PINNED_TO_CORE_CHOICE` 和 :ref:`CONFIG_BT_BLUEDROID_PINNED_TO_CORE_CHOICE` （或者 :ref:`CONFIG_BT_NIMBLE_PINNED_TO_CORE_CHOICE` ）选择将蓝牙 controller 以及 host 协议栈的 task 放在同一个 CPU 上，再通过 :ref:`CONFIG_ESP32_WIFI_TASK_CORE_ID`  选择将 Wi-Fi 协议栈 task 放在另一个 CPU 上。

    :esp32: - 在共存情况下 BLE SCAN 可能会被 Wi-Fi 打断且 Wi-Fi 在当前的 BLE scan window 结束前释放了 RF 资源。为了使 BLE 在当前的 scan window 内再次获取 RF 资源，您可以通过 :ref:`CONFIG_BTDM_CTRL_FULL_SCAN_SUPPORTED` 选择打开 FULL SCAN 配置选项。

    :esp32c3 or esp32s3: - 在 BLE 连接过程中使用了 LE Coded PHY 时，为了防止蓝牙数据包持续时间较长而对 Wi-Fi 的性能产生严重影响，您可以在 :ref:`CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM` 的子选项中选择 `BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_EN` 选择打开对 TX/RX 最大时间的限制配置选项。

    - 您可以通过修改以下 menuconfig 选项，以减小内存开销：

      1) :ref:`CONFIG_BT_BLE_DYNAMIC_ENV_MEMORY` 选择打开蓝牙协议栈动态内存配置选项。
      #) :ref:`CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM`  选择减少 Wi-Fi 静态接收数据缓冲区的数量。
      #) :ref:`CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM` 选择减少 Wi-Fi 动态接收数据缓冲区的数量。
      #) :ref:`CONFIG_ESP32_WIFI_TX_BUFFER` 选择使用动态分配发送数据缓冲区配置选项。
      #) :ref:`CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM` 选择减少 Wi-Fi 动态发送数据缓冲区的数量。
      #) :ref:`CONFIG_ESP32_WIFI_TX_BA_WIN` 选择减少 Wi-Fi Block Ack TX 窗口的数量。
      #) :ref:`CONFIG_ESP32_WIFI_RX_BA_WIN` 选择减少 Wi-Fi Block Ack RX 窗口的数量。
      #) :ref:`CONFIG_ESP32_WIFI_MGMT_SBUF_NUM` 选择减少 Wi-Fi 管理短缓冲区的数量。
      #) :ref:`CONFIG_ESP32_WIFI_RX_IRAM_OPT` 选择关闭此配置选项，关闭此配置选项将会减少大约 17 KB 的 IRAM 内存。
      #) :ref:`CONFIG_LWIP_TCP_SND_BUF_DEFAULT` 选择减小 TCP 套接字默认发送缓存区大小。
      #) :ref:`CONFIG_LWIP_TCP_WND_DEFAULT` 选择减小 TCP 套接字默认接收窗口的大小。
      #) :ref:`CONFIG_LWIP_TCP_RECVMBOX_SIZE` 选择减小 TCP 接收邮箱的大小。
      #) :ref:`CONFIG_LWIP_UDP_RECVMBOX_SIZE` 选择减小 UDP 接收邮箱的大小。
      #) :ref:`CONFIG_LWIP_TCPIP_RECVMBOX_SIZE` 选择减小 TCPIP 任务接收邮箱大小。

.. note::

  由于共存配置选项依赖于蓝牙配置选项，所以请先打开蓝牙配置选项，然后在 Wi-Fi 配置选项中打开共存配置选项。
