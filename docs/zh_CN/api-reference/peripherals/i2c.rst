I2C 驱动程序
===============

:link_to_translation:`en:[English]`

概述
---------

I2C 是一种串行同步半双工通信协议，总线上可以同时挂载多个主机和从机。I2C 总线由串行数据线 (SDA) 和串行时钟线 (SCL) 线构成。这些线都需要上拉电阻。

I2C 具有简单且制造成本低廉等优点，主要用于低速外围设备的短距离通信（一英尺以内）。

{IDF_TARGET_NAME} 有{IDF_TARGET_SOC_I2C_NUM}个 I2C 控制器（也称为端口），负责处理在 I2C 总线上的通信。每个控制器都可以设置为主机或从机。

驱动程序的功能
---------------

I2C 驱动程序管理在 I2C 总线上设备的通信，该驱动程序具备以下功能：

- 在主机模式下读写字节

.. only:: SOC_I2C_SUPPORT_SLAVE

    - 支持从机模式

- 读取并写入寄存器，然后由主机读取/写入


使用驱动程序
---------------

{IDF_TARGET_I2C_ROLE:default="主机或从机", esp32c2="主机"}

以下部分将指导您完成 I2C 驱动程序配置和工作的基本步骤：

1. :ref:`i2c-api-configure-driver` - 设置初始化参数（如主机模式或从机模式，SDA 和 SCL 使用的 GPIO 管脚，时钟速度等）
2. :ref:`i2c-api-install-driver`- 激活一个 I2C 控制器的驱动，该控制器可为主机也可为从机
3. 根据是为主机还是从机配置驱动程序，选择合适的项目

   a) :ref:`i2c-api-master-mode` - 发起通信（主机模式）

   .. only:: SOC_I2C_SUPPORT_SLAVE

       b) :ref:`i2c-api-slave-mode` - 响应主机消息（从机模式）

4. :ref:`i2c-api-interrupt-handling` - 配置 I2C 中断服务
5. :ref:`i2c-api-customized-configuration` - 调整默认的 I2C 通信参数（如时序、位序等）
6. :ref:`i2c-api-error-handling` - 如何识别和处理驱动程序配置和通信错误
7. :ref:`i2c-api-delete-driver`- 在通信结束时释放 I2C 驱动程序所使用的资源


.. _i2c-api-configure-driver:

配置驱动程序
^^^^^^^^^^^^^

建立 I2C 通信第一步是配置驱动程序，这需要设置 :cpp:type:`i2c_config_t` 结构中的几个参数：

- 设置 I2C **工作模式** - 从 :cpp:type:`i2c_mode_t` 中选择主机模式或从机模式
- 设置 **通信管脚**

    - 指定 SDA 和 SCL 信号使用的 GPIO 管脚
    - 是否启用 {IDF_TARGET_NAME} 的内部上拉电阻

- （仅限主机模式）设置 I2C **时钟速度**

.. only:: SOC_I2C_SUPPORT_SLAVE

    - （仅限从机模式）设置以下内容：

        * 是否应启用 **10 位寻址模式**
        * 定义 **从机地址**

然后，初始化给定 I2C 端口的配置，请使用端口号和 :cpp:type:`i2c_config_t` 作为函数调用参数来调用 :cpp:func:`i2c_param_config` 函数。

配置示例（主机）：

.. code-block:: c

    int i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,         // 配置 SDA 的 GPIO
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,         // 配置 SCL 的 GPIO
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,  // 为项目选择频率
        .clk_flags = 0,          // 可选项，可以使用 I2C_SCLK_SRC_FLAG_* 标志来选择 I2C 源时钟
    };

.. only:: SOC_I2C_SUPPORT_SLAVE

    配置示例（从机）：

    .. code-block:: c

        int i2c_slave_port = I2C_SLAVE_NUM;
        i2c_config_t conf_slave = {
            .sda_io_num = I2C_SLAVE_SDA_IO,            // 配置 SDA 的 GPIO
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = I2C_SLAVE_SCL_IO,            // 配置 SCL 的 GPIO
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .mode = I2C_MODE_SLAVE,
            .slave.addr_10bit_en = 0,
            .slave.slave_addr = ESP_SLAVE_ADDR,        // 项目从机地址
            .slave.maximum_speed = I2C_SLAVE_MAX_SPEED // 预期的最大时钟速度
            .clk_flags = 0,                            // 可选项，可以使用 I2C_SCLK_SRC_FLAG_* 标志来选择 I2C 源时钟
        };

在此阶段，:cpp:func:`i2c_param_config` 还将其他 I2C 配置参数设置为 I2C 总线协议规范中定义的默认值。有关默认值及修改默认值的详细信息，请参考 :ref:`i2c-api-customized-configuration`。

源时钟配置
^^^^^^^^^^^^^^^^^^^^^^^^^^

增加了 **时钟源分配器**，用于支持不同的时钟源。时钟分配器将选择一个满足所有频率和能力要求的时钟源（如 :cpp:member:`i2c_config_t::clk_flags` 中的要求）。

当 :cpp:member:`i2c_config_t::clk_flags` 为 0 时，时钟分配器将仅根据所需频率进行选择。如果不需要诸如 APB 之类的特殊功能，则可以将时钟分配器配置为仅根据所需频率选择源时钟。为此，请将 :cpp:member:`i2c_config_t::clk_flags` 设置为 0。有关时钟特性，请参见下表。

.. note::

    如果时钟不满足请求的功能，则该时钟不是有效的选项，即，请求的功能中的任何位（clk_flags）在时钟的功能中均为 0。

.. only:: esp32

    .. list-table:: {IDF_TARGET_NAME} 时钟源特性
       :widths: 5 5 50 20
       :header-rows: 1

       * - 时钟名称
         - 时钟频率
         - SCL 的最大频率
         - 时钟功能
       * - APB 时钟
         - 80 MHz
         - 4 MHz
         - /

.. only:: esp32s2

    .. list-table:: {IDF_TARGET_NAME} 时钟源特性
       :widths: 5 5 50 100
       :header-rows: 1

       * - 时钟名称
         - 时钟频率
         - SCL 的最大频率
         - 时钟功能
       * - APB 时钟
         - 80 MHz
         - 4 MHz
         - /
       * - REF_TICK
         - 1 MHz
         - 50 KHz
         - :c:macro:`I2C_SCLK_SRC_FLAG_AWARE_DFS`, :c:macro:`I2C_SCLK_SRC_FLAG_LIGHT_SLEEP`

    对 :cpp:member:`i2c_config_t::clk_flags` 的解释如下：
    1. :c:macro:`I2C_SCLK_SRC_FLAG_AWARE_DFS`：当 APB 时钟改变时，时钟的波特率不会改变。
    2. :c:macro:`I2C_SCLK_SRC_FLAG_LIGHT_SLEEP`：支持轻度睡眠模式，APB 时钟则不支持。

.. only:: esp32s3

    .. list-table:: {IDF_TARGET_NAME} 时钟源特性
       :widths: 5 5 50 20
       :header-rows: 1

       * - 时钟名称
         - 时钟频率
         - SCL 的最大频率
         - 时钟功能
       * - XTAL 时钟
         - 40 MHz
         - 2 MHz
         - /
       * - RTC 时钟
         - 20 MHz
         - 1 MHz
         - :c:macro:`I2C_SCLK_SRC_FLAG_AWARE_DFS`, :c:macro:`I2C_SCLK_SRC_FLAG_LIGHT_SLEEP`

.. only:: esp32c3

    .. list-table:: {IDF_TARGET_NAME} 时钟源特性
       :widths: 5 5 50 100
       :header-rows: 1

       * - 时钟名称
         - 时钟频率
         - SCL 的最大频率
         - 时钟功能
       * - XTAL 时钟
         - 40 MHz
         - 2 MHz
         - /
       * - RTC 时钟
         - 20 MHz
         - 1 MHz
         - :c:macro:`I2C_SCLK_SRC_FLAG_AWARE_DFS`, :c:macro:`I2C_SCLK_SRC_FLAG_LIGHT_SLEEP`

对 :cpp:member:`i2c_config_t::clk_flags` 的解释如下：

1. :c:macro:`I2C_SCLK_SRC_FLAG_AWARE_DFS`：当 APB 时钟改变时，时钟的波特率不会改变。
2. :c:macro:`I2C_SCLK_SRC_FLAG_LIGHT_SLEEP`：支持轻度睡眠模式，APB 时钟则不支持。
3. {IDF_TARGET_NAME} 可能不支持某些标志，请在使用前阅读技术参考手册。

.. note::

    在主机模式下，SCL 的时钟频率不应大于上表中提到的 SCL 的最大频率。

.. note::

    SCL 的时钟频率会被上拉电阻和线上电容（或是从机电容）一起影响。因此，用户需要自己选择合适的上拉电阻去保证 SCL 时钟频率是准确的。尽管 I2C 协议推荐上拉电阻值为 1 K 欧姆到 10 K 欧姆，但是需要根据不同的频率需要选择不同的上拉电阻。

    通常来说，所选择的频率越高，需要的上拉电阻越小（但是不要小于 1 K 欧姆）。这是因为高电阻会减小电流，这会延长上升时间从而使频率变慢。通常我们推荐的上拉阻值范围为 2 K 欧姆到 5 K 欧姆，但是用户可能也需要根据他们的实际情况做出一些调整。

.. _i2c-api-install-driver:

安装驱动程序
^^^^^^^^^^^^^^

配置好 I2C 驱动程序后，使用以下参数调用函数 :cpp:func:`i2c_driver_install` 安装驱动程序：

- 端口号，从 :cpp:type:`i2c_port_t` 中二选一
- 主机或从机模式，从 :cpp:type:`i2c_mode_t` 中选择

.. only:: SOC_I2C_SUPPORT_SLAVE

    - （仅限从机模式）分配用于在从机模式下发送和接收数据的缓存区大小。I2C 是一个以主机为中心的总线，数据只能根据主机的请求从从机传输到主机。因此，从机通常有一个发送缓存区，供从应用程序写入数据使用。数据保留在发送缓存区中，由主机自行读取。

- 用于分配中断的标志（请参考 :component_file:`esp_hw_support/include/esp_intr_alloc.h` 中 ESP_INTR_FLAG_* 值）

.. _i2c-api-master-mode:

主机模式下通信
^^^^^^^^^^^^^^^^^^

安装 I2C 驱动程序后， {IDF_TARGET_NAME} 即可与其他 I2C 设备通信。

{IDF_TARGET_NAME} 的 I2C 控制器在主机模式下负责与 I2C 从机设备建立通信，并发送命令让从机响应，如进行测量并将结果发给主机。

为优化通信流程，驱动程序提供一个名为 “命令链接” 的容器，该容器应填充一系列命令，然后传递给 I2C 控制器执行。


主机写入数据
"""""""""""""

下面的示例展示如何为 I2C 主机构建命令链接，从而向从机发送 *n* 个字节。

.. blockdiag:: ../../../_static/diagrams/i2c-command-link-master-write-blockdiag.diag
    :scale: 100
    :caption: I2C command link - master write example
    :align: center


下面介绍如何为 “主机写入数据” 设置命令链接及其内部内容：

1. 使用 :cpp:func:`i2c_cmd_link_create` 创建一个命令链接。

    然后，将一系列待发送给从机的数据填充命令链接：

   a) **启动位** - :cpp:func:`i2c_master_start`
   b) **从机地址** - :cpp:func:`i2c_master_write_byte`。提供单字节地址作为调用此函数的实参。
   c) **数据** - 一个或多个字节的数据作为 :cpp:func:`i2c_master_write` 的实参。
   d) **停止位** - :cpp:func:`i2c_master_stop`

    函数 :cpp:func:`i2c_master_write_byte` 和 :cpp:func:`i2c_master_write` 都有额外的实参，规定主机是否应确认其有无接受到 ACK 位。

2. 通过调用 :cpp:func:`i2c_master_cmd_begin` 来触发 I2C 控制器执行命令链接。一旦开始执行，就不能再修改命令链接。
3. 命令发送后，通过调用 :cpp:func:`i2c_cmd_link_delete` 释放命令链接使用的资源。


主机读取数据
""""""""""""""

下面的示例展示如何为 I2C 主机构建命令链接，以便从从机读取 *n* 个字节。

.. blockdiag:: ../../../_static/diagrams/i2c-command-link-master-read-blockdiag.diag
    :scale: 100
    :caption: I2C command link - master read example
    :align: center


在读取数据时，在上图的步骤 4 中，不是用 ``i2c_master_write...``，而是用 :cpp:func:`i2c_master_read_byte` 和/或 :cpp:func:`i2c_master_read` 填充命令链接。同样，在步骤 5 中配置最后一次的读取，以便主机不提供 ACK 位。


指示写入或读取数据
""""""""""""""""""

发送从机地址后（请参考上图中第 3 步），主机可以写入或从从机读取数据。

主机实际执行的操作信息存储在从机地址的最低有效位中。

因此，为了将数据写入从机，主机发送的命令链接应包含地址 ``(ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE``，如下所示：

.. code-block:: c

    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, ACK_EN);

同理，指示从从机读取数据的命令链接如下所示：

.. code-block:: c

    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_READ, ACK_EN);


.. only:: SOC_I2C_SUPPORT_SLAVE

    .. _i2c-api-slave-mode:

    从机模式下通信
    ^^^^^^^^^^^^^^^^^^^^^^

    安装 I2C 驱动程序后， {IDF_TARGET_NAME} 即可与其他 I2C 设备通信。

    API 为从机提供以下功能：

    - :cpp:func:`i2c_slave_read_buffer`

        当主机将数据写入从机时，从机将自动将其存储在接收缓存区中。从机应用程序可自行调用函数 :cpp:func:`i2c_slave_read_buffer`。如果接收缓存区中没有数据，此函数还具有一个参数用于指定阻塞时间。这将允许从机应用程序在指定的超时设定内等待数据到达缓存区。

    - :cpp:func:`i2c_slave_write_buffer`

        发送缓存区是用于存储从机要以 FIFO 顺序发送给主机的所有数据。在主机请求接收前，这些数据一直存储在发送缓存区。函数 :cpp:func:`i2c_slave_write_buffer` 有一个参数，用于指定发送缓存区已满时的块时间。这将允许从机应用程序在指定的超时设定内等待发送缓存区中足够的可用空间。

    在 :example:`peripherals/i2c` 中可找到介绍如何使用这些功能的代码示例。

    .. _i2c-api-interrupt-handling:

.. only:: not SOC_I2C_SUPPORT_SLAVE

    .. _i2c-api-interrupt-handling:

中断处理
^^^^^^^^^^^

安装驱动程序时，默认情况下会安装中断处理程序。

.. _i2c-api-customized-configuration:

用户自定义配置
^^^^^^^^^^^^^^^

如本节末尾所述 :ref:`i2c-api-configure-driver`，函数 :cpp:func:`i2c_param_config` 在初始化 I2C 端口的驱动程序配置时，也会将几个 I2C 通信参数设置为 I2C 总线协议规范规定的默认值。其他一些相关参数已在 I2C 控制器的寄存器中预先配置。

通过调用下表中提供的专用函数，可以将所有这些参数更改为用户自定义值。请注意，时序值是在 APB 时钟周期中定义。

.. list-table:: 其他可配置的 I2C 通信参数
   :widths: 65 35
   :header-rows: 1

   * - 要更改的参数
     - 函数
   * - SCL 脉冲周期的高电平和低电平
     - :cpp:func:`i2c_set_period`
   * - 在产生 **启动** 信号期间使用的 SCL 和 SDA 信号时序
     - :cpp:func:`i2c_set_start_timing`
   * - 在产生 **停止** 信号期间使用的 SCL 和 SDA 信号时序
     - :cpp:func:`i2c_set_stop_timing`
   * - 从机采样以及主机切换时，SCL 和 SDA 信号之间的时序关系
     - :cpp:func:`i2c_set_data_timing`
   * - I2C 超时
     - :cpp:func:`i2c_set_timeout`
   * - 优先发送/接收最高有效位 (LSB) 或最低有效位 (MSB），可在 :cpp:type:`i2c_trans_mode_t` 定义的模式中选择
     - :cpp:func:`i2c_set_data_mode`


上述每个函数都有一个 *_get_* 对应项来检查当前设置的值。例如，调用 :cpp:func:`i2c_get_timeout` 来检查 I2C 超时值。

要检查在驱动程序配置过程中设置的参数默认值，请参考文件 :component_file:`driver/i2c/i2c.c` 并查找带有后缀 ``_DEFAULT`` 的定义。

通过函数 :cpp:func:`i2c_set_pin` 可以为 SDA 和 SCL 信号选择不同的管脚并改变上拉配置。如果要修改已经输入的值，请使用函数 :cpp:func:`i2c_param_config`。

.. 注解 ::

    {IDF_TARGET_NAME} 的内部上拉电阻范围为几万欧姆，因此在大多数情况下，它们本身不足以用作 I2C 上拉电阻。建议用户使用阻值在 I2C 总线协议规范规定范围内的上拉电阻。计算阻值的具体方法，可参考 `TI 应用说明 <https://www.ti.com/lit/an/slva689/slva689.pdf>`_


.. _i2c-api-error-handling:

错误处理
^^^^^^^^^^

大多数 I2C 驱动程序的函数在成功完成时会返回 ``ESP_OK`` ，或在失败时会返回特定的错误代码。实时检查返回的值并进行错误处理是一种好习惯。驱动程序也会打印日志消息，其中包含错误说明，例如检查输入配置的正确性。有关详细信息，请参考文件 :component_file:`driver/i2c/i2c.c` 并用后缀 ``_ERR_STR`` 查找定义。

使用专用中断来捕获通信故障。例如，如果从机将数据发送回主机耗费太长时间，会触发 ``I2C_TIME_OUT_INT`` 中断。详细信息请参考 :ref:`i2c-api-interrupt-handling`。

如果出现通信失败，可以分别为发送和接收缓存区调用 :cpp:func:`i2c_reset_tx_fifo` 和  :cpp:func:`i2c_reset_rx_fifo` 来重置内部硬件缓存区。


.. _i2c-api-delete-driver:

删除驱动程序
^^^^^^^^^^^^^

当使用 :cpp:func:`i2c_driver_install` 建立 I2C 通信，一段时间后不再需要 I2C 通信时，可以通过调用 :cpp:func:`i2c_driver_delete` 来移除驱动程序以释放分配的资源。

由于函数 :cpp:func:`i2c_driver_delete` 无法保证线程安全性，请在调用该函数移除驱动程序前务必确保所有的线程都已停止使用驱动程序。

应用示例
----------

I2C 主机和从机示例：:example:`peripherals/i2c`。


API 参考
----------

.. include-build-file:: inc/i2c.inc
.. include-build-file:: inc/i2c_types.inc
