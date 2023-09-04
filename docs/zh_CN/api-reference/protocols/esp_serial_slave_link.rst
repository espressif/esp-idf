ESP 串行从机链路
=================

:link_to_translation:`en:[English]`

概述
----

乐鑫有多款芯片可用作从机的芯片。这些从机依赖于一些通用总线，并在总线上实现了各自的通信协议。 ``esp_serial_slave_link`` 组件能让主机通过总线驱动和相应的协议与 ESP 从机进行通信。

``esp_serial_slave_link`` 设备初始化完成后，应用程序就能通过它与 ESP 从机方便地通信。

.. note::

  ``esp_serial_slave_link`` 组件自 ESP-IDF 版本 v5.0 起移到了单独的仓库：

  * `ESSL component on GitHub <https://github.com/espressif/idf-extra-components/tree/master/esp_serial_slave_link>`__

  运行 ``idf.py add-dependency espressif/esp_serial_slave_link`` 将 ESSL 组件添加到你的项目中。


乐鑫设备协议
--------------------

如需了解关于乐鑫设备协议详情，请参考以下文档：

.. toctree::
    :maxdepth: 1

    :SOC_SDIO_SLAVE_SUPPORTED: esp_sdio_slave_protocol
    esp_spi_slave_protocol

术语解释
--------

- ESSL：ESP 串行从机链路 (ESP Serial Slave Link)，即本文档描述的组件。

- 主机：运行 ``esp_serial_slave_link`` 组件的设备。

- ESSL 设备：主机上的虚拟设备，关联到一个 ESP 从机，其设备上下文中具有如何通过总线驱动和从机的总线协议与其通信的信息。

- ESSL 设备句柄：ESSL 设备上下文的一个句柄，包含配置信息、状态信息和 ESSL 组件所需的数据。设备上下文中储存了驱动配置、通信状态和主从机共享的数据等。

   - 在使用前，应将上下文初始化；如不再使用，应该反初始化。主机应用程序通过这一句柄操作 ESSL 设备。

- ESP 从机：连接到总线的从机，ESSL 组件的通信对象。

- 总线：特指用于主机和从机相互通信的总线。

- 从机协议：Espressif 硬件和软件在总线上使用的特殊通信协议。

- TX buffer num：计数器，位于从机，可由主机读取。指示由从机加载到硬件上、用于接收主机数据的累计 buffer 总数。

- RX data size：计数器，位于从机，可由主机读取。指示由从机加载到硬件上、需发送给主机的累计数据大小。

ESP 从机提供的服务
---------------------------

Espressif 从机提供下列服务：

1. Tohost 中断：从机可通过中断线向主机通知某些事件。（可选）

2. Frhost 中断：主机可向从机通知某些事件。

3. TX FIFO（主机到从机）：从机能够以接收 buffer 为单位，接收主机发送的数据。

   从机通过更新 TX buffer num 的方式，将可以接收多少数据的信息通知主机。主机读取 TX buffer num，减去已使用的 buffer 数，得到剩余 buffer 数量。

4. RX FIFO（从机到主机）：从机可向主机发送数据流。SDIO 从机也可通过中断线通知主机，从机有待发送的新数据。

   从机通过更新 RX data size，将准备发送的数据大小通知主机。主机读取该数据大小，减去已接收的数据长度，得到剩余数据大小。

5. 共享寄存器：主机可以读取从机寄存器上的部分内容，也可写入从机寄存器供从机读取。

.. only:: SOC_SDIO_SLAVE_SUPPORTED

   从机提供的服务取决于从机的模型。如需了解详情，请参考 :ref:`esp_sdio_slave_caps` 和 :ref:`esp_spi_slave_caps`。

.. only:: not SOC_SDIO_SLAVE_SUPPORTED

   从机提供的服务取决于从机的模型。如需了解详情，请参考 :ref:`esp_spi_slave_caps`。


初始化 ESP 串行从机链路
--------------------------------

.. _essl_sdio_slave_init:

ESP SDIO 从机
^^^^^^^^^^^^^^^^^

ESP SDIO 从机链路 (ESSL SDIO) 设备依赖于 SDMMC 组件。它可通过 SDMMC Host 或 SDSPI Host 功能均可与 ESP SDIO 从机通信。ESSL 设备初始化步骤如下：

1. 初始化 SDMMC 卡（参考 :doc:`SDMMC 驱动相关文档 </api-reference/storage/sdmmc>` ）结构体。

2. 调用 :cpp:func:`sdmmc_card_init` 初始化该卡。

3. 用 :cpp:type:`essl_sdio_config_t` 初始化 ESSL 设备。其中， ``card`` 成员应为第二步中的 :cpp:type:`sdmmc_card_t`， ``recv_buffer_size`` 成员应填写为预先协商的值。

4. 调用 :cpp:func:`essl_init` 对 SDIO 部分进行初始化。

5. 调用 :cpp:func:`essl_wait_for_ready` 等待从机就绪。

ESP SPI 从机
^^^^^^^^^^^^^^^^^^

.. note::

    如果你正在用 SPI 接口与 ESP SDIO 从机进行通信，则应该用 :ref:`SDIO 接口 <essl_sdio_slave_init>` 代替。

暂不支持。

API
--------

初始化完成后，你可调用以下 API 使用从机提供的服务：

Tohost 中断（可选）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. 调用 :cpp:func:`essl_get_intr_ena` 了解哪些事件触发了对主机的中断。

2. 调用 :cpp:func:`essl_set_intr_ena` 设置能够触发主机中断的事件。

3. 调用 :cpp:func:`essl_wait_int` 等待从机中断或超时。

4. 中断触发后，调用 :cpp:func:`essl_get_intr` 了解哪些事件处于活跃状态，并调用 :cpp:func:`essl_clear_intr` 将其清空。

Frhost 中断
^^^^^^^^^^^^^^^^^

1. 调用 :cpp:func:`essl_send_slave_intr` 触发从机的通用中断。

TX FIFO
^^^^^^^

1. 调用 :cpp:func:`essl_get_tx_buffer_num` 了解从机准备用于接收主机数据的 buffer 数。你可选择是否调用该函数。主机向从机发送数据包前，也会轮询 ``tx_buffer_num``，直到从机的 buffer 数量足够或超时。

2. 调用 :cpp:func:`essl_send_packet` 向从机发送数据。

RX FIFO
^^^^^^^

1. 调用 :cpp:func:`essl_get_rx_data_size` 了解从机需发送给主机的数据大小。你可选择是否调用该函数。当主机尝试接收数据时，如果目前的 ``rx_data_size`` 小于主机准备接收数据的 buffer 大小，就会对 ``rx_data_size`` 进行一次更新。如果 ``rx_data_size`` 一直为 0，主机可能会轮询 ``rx_data_size`` 直到超时。

2. 调用 :cpp:func:`essl_get_packet` 接收来自从机的数据。

重置计数器（可选）
^^^^^^^^^^^^^^^^^^^^^^^^^

如果从机计数器已重置，调用 :cpp:func:`essl_reset_cnt` 重置内部计数器。


应用示例
--------------

以下示例展示了 {IDF_TARGET_NAME} SDIO 主机如何与从机互相通信，其中主机使用了 ESSL SDIO：

:example:`peripherals/sdio`

如需了解详情，请参考 README.md 中的示例。

API 参考
----------

.. include-build-file:: inc/essl.inc
.. include-build-file:: inc/essl_sdio.inc
.. include-build-file:: inc/essl_spi.inc
