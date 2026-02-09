========================
I3C 主机接口
========================

:link_to_translation:`en:[English]`

{IDF_TARGET_I3C_INTERNAL_PULLUP_PIN:default="未更新！", esp32p4="GPIO32/GPIO33"}

本文介绍了 ESP-IDF 的 I3C 主机驱动功能，章节目录如下：

.. contents::
    :local:
    :depth: 2

概述
====

I3C (Improved Inter‑Integrated Circuit) 是一种串行同步半双工通信协议，是 I2C 协议的增强版本。I3C 在保持与 I2C 大部分功能兼容的同时，提供了更高的速度、更低的功耗和更丰富的功能。

关于 I3C 的硬件相关信息，请参考 I3C 技术参考手册。

I3C 协议的主要特性包括：

- **向后兼容 I2C**：I3C 总线可以同时支持 I2C 设备和 I3C 设备
- **更高的速度**：I3C 速度最高可达 12.5 MHz，I2C 最高速度可达 1 MHz
- **静态地址分配**：通过 SETDASA 过程手动根据静态地址分配动态地址
- **动态地址分配**：通过 ENTDAA 过程自动分配动态地址，避免地址冲突
- **带内中断（IBI）**：支持从机通过 I3C 总线发送中断请求，无需额外的中断线
- **公共命令码（CCC）**：支持广播和直接 CCC 命令，用于总线管理和设备配置

.. important::

    1. 当兼容 I2C 设备时，请确保在 I3C 总线上挂载的 I2C 设备 **不启用** 时钟拉伸功能，否则当 I2C 从机拉伸时钟时会导致硬件状态机卡死。
    2. I3C 的频率取决于电路设计和时序调整，具体请参考使用的 I3C 设备手册。
    3. 部分 I3C 从机设备的应答机制（ACK/NACK）有严格的时序限制，请参考所使用的 I3C 从机设备手册。

快速入门
========

本节将带你快速了解如何使用 I3C 主机驱动。通过实际的使用场景，展示如何创建总线、添加设备并进行数据传输。一般的使用流程如下：

.. blockdiag::
    :scale: 100%
    :caption: I3C 驱动的一般使用流程（点击图片查看大图）
    :align: center

    blockdiag {
        default_fontsize = 14;
        node_width = 250;
        node_height = 80;
        class emphasis [color = pink, style = dashed];

        create_bus [label="创建 I3C 总线\n(i3c_new_master_bus)"];
        add_device [label="添加设备\n(add_i2c_device / \nadd_i3c_static_device / \nscan_devices_by_entdaa)"];
        transfer [label="数据传输\n(transmit / receive / \ntransmit_receive)", class="emphasis"];
        cleanup [label="删除设备和总线\n(rm_device / del_master_bus)"];

        create_bus -> add_device -> transfer -> cleanup;
    }

创建 I3C 总线
----------------

I3C 主机总线在驱动程序中使用 :cpp:type:`i3c_master_bus_handle_t` 来表示。驱动内部维护了一个资源池，可管理多条总线，并在有请求时分配空闲的总线端口。

.. figure:: ../../../_static/diagrams/i3c/i3c_bus_structure.svg
    :align: center
    :alt: I3C 总线结构

    I3C 总线结构

当创建 I3C 总线实例时，我们需要通过 :cpp:type:`i3c_master_bus_config_t` 配置 GPIO 引脚、时钟源、频率等参数。这些参数将决定总线的工作方式。以下代码展示了如何创建一个基本的 I3C 总线：

.. code:: c

    #include "driver/i3c_master.h"

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,        // SDA 信号线的 GPIO 编号
        .scl_io_num = I3C_MASTER_SCL_IO,        // SCL 信号线的 GPIO 编号
        .i3c_scl_freq_hz_od = 600 * 1000,       // Open-Drain 模式下的 SCL 时钟频率，请参考设备手册获取合适的值
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,  // Push-Pull 模式下的 SCL 时钟频率，请参考设备手册获取合适的值
        .i3c_sda_od_hold_time_ns = 25, // Open-Drain 模式下 SDA 在 SCL 下降沿后的保持时间（纳秒），建议设置为 25，请参考设备手册获取合适的值
        .i3c_sda_pp_hold_time_ns = 0,  // Push-Pull 模式下 SDA 在 SCL 下降沿后的保持时间（纳秒），默认值为 0，请参考设备手册获取合适的值
        .entdaa_device_num = 0,        // 最大允许通过 ENTDAA 动态发现的设备数量，范围从 [0x0, 0x7F]，0x0 表示不使用动态设备发现
    };

    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

.. note::

    I3C 协议需要在每次传输过程中在寻址过程与数据传输过程之间自动切换开漏和推挽模式。在 ESP32-P4 上，只有 {IDF_TARGET_I3C_INTERNAL_PULLUP_PIN} 支持自动打开/关闭内部上拉开关且支持用户调整内部上拉阻值。当使用其他 GPIO 时内部上拉不足建议额外添加外部上拉电阻，但此时在推挽模式下该上拉不可取消，可能会增加额外功耗。

添加并驱动传统 I2C 设备
----------------------------

.. figure:: ../../../_static/diagrams/i3c/i3c_i2c_write.svg
    :align: center
    :alt: 写入传统 I2C 设备

    写入传统 I2C 设备

.. figure:: ../../../_static/diagrams/i3c/i3c_i2c_read.svg
    :align: center
    :alt: 从传统 I2C 设备读取

    从传统 I2C 设备读取

I3C 总线支持和传统的 I2C 设备兼容，如果你需要在 I3C 总线上连接一个传统的 I2C 设备（例如 EEPROM、传感器等），请务必注意，I2C 从机不可在与 I3C 通信时发生时钟拉伸，具体流程可以使用以下方式：

.. code:: c

    // 1. 创建 I3C 总线（参考上面的代码）
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. 添加 I2C 设备
    i3c_device_i2c_config_t i2c_dev_cfg = {
        .device_address = 0x50,        // I2C 设备的 7 位地址
        .scl_freq_hz = 100 * 1000,     // I2C 设备的时钟频率（100 kHz）
    };
    i3c_master_i2c_device_handle_t i2c_dev_handle;
    ESP_ERROR_CHECK(i3c_master_bus_add_i2c_device(bus_handle, &i2c_dev_cfg, &i2c_dev_handle));

    // 3. 写入数据到 I2C 设备
    uint8_t write_data[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit(i2c_dev_handle, write_data, sizeof(write_data), -1)); // -1 表示传输超时时间为无限长

    // 4. 从 I2C 设备读取数据
    uint8_t read_data[10] = {0};
    ESP_ERROR_CHECK(i3c_master_i2c_device_receive(i2c_dev_handle, read_data, sizeof(read_data), -1));

    // 5. 写入-读取组合事务（先写寄存器地址，再读数据，中间没有 STOP）
    uint8_t reg_addr = 0x00;
    uint8_t read_buffer[5] = {0};
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit_receive(i2c_dev_handle, &reg_addr, 1, read_buffer, sizeof(read_buffer), -1));

    // 6. 清理资源
    ESP_ERROR_CHECK(i3c_master_bus_rm_i2c_device(i2c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

在这个场景中，我们：

1. 通过 :cpp:func:`i3c_new_master_bus` 创建了一个 I3C 总线实例
2. 通过 :cpp:func:`i3c_master_bus_add_i2c_device` 添加了一个 I2C 设备，需要指定设备的静态地址和时钟频率
3. 使用 :cpp:func:`i3c_master_i2c_device_transmit` 写入数据，默认为阻塞模式，在非阻塞模式下工作请参考 :ref:`dma-support`，其它传输函数同理。
4. 使用 :cpp:func:`i3c_master_i2c_device_receive` 读取数据
5. 使用 :cpp:func:`i3c_master_i2c_device_transmit_receive` 执行写入-读取组合事务（常用于先写寄存器地址再读数据，中间没有停止位）
6. 最后清理资源

通过 SETDASA 添加并驱动 I3C 设备
----------------------------------------

对 I3C 传输过程中可能出现的具体行为，请参考标准 I3C 协议。该图例用作简要解释 I3C 传输中的行为以理解本文档中出现的 I3C 传输示意图：

.. figure:: ../../../_static/diagrams/i3c/i3c_icon.svg
    :align: center
    :alt: I3C 传输图例

    I3C 传输图例

如果你知道 I3C 设备的静态地址，可以使用 SETDASA 方式添加设备：

.. figure:: ../../../_static/diagrams/i3c/i3c_setdasa.svg
    :align: center
    :alt: I3C 定向动态地址分配

    I3C 定向动态地址分配

.. code:: c

    // 1. 创建 I3C 总线
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. 添加 I3C 设备（使用 SETDASA）
    i3c_device_i3c_config_t i3c_dev_cfg = {
        .dynamic_addr = 0x08,          // 为设备分配的动态地址，可以是除了 I3C 协议中规定为保留地址外的任意值，也可以通过 `i3c_master_get_valid_address_slot` 获取一个可用的动态地址
        .static_addr = 0x74,            // 设备的静态地址（从设备手册获取）
    };
    i3c_master_i3c_device_handle_t i3c_dev_handle;
    ESP_ERROR_CHECK(i3c_master_bus_add_i3c_static_device(bus_handle, &i3c_dev_cfg, &i3c_dev_handle));

    // 3. 写入数据到 I3C 设备
    uint8_t write_data[100] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(i3c_dev_handle, write_data, sizeof(write_data), -1));

    // 4. 从 I3C 设备读取数据
    uint8_t read_data[100] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_receive(i3c_dev_handle, read_data, sizeof(read_data), -1));

    // 5. 写入-读取组合事务
    uint8_t reg_addr = 0x12;
    uint8_t read_buffer[10] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit_receive(i3c_dev_handle, &reg_addr, 1, read_buffer, sizeof(read_buffer), -1));

    // 6. 清理资源
    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(i3c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

在这个场景中：

1. 我们使用 :cpp:func:`i3c_master_bus_add_i3c_static_device` 添加 I3C 设备
2. 需要提供设备的静态地址（从设备手册获取）和要分配的动态地址
3. 驱动程序会自动执行 SETDASA 过程，将动态地址分配给设备, 若地址冲突会返回 ``ESP_ERR_INVALID_STATE``。
4. 之后可以使用动态地址 通过 :cpp:func:`i3c_master_i3c_device_transmit` 或 :cpp:func:`i3c_master_i3c_device_receive` 或 :cpp:func:`i3c_master_i3c_device_transmit_receive` 进行数据传输，默认为阻塞模式，在非阻塞模式下工作请参考 :ref:`dma-support`，其它传输函数同理。
5. 最后清理资源

通过 ENTDAA 添加并驱动 I3C 设备
--------------------------------

如果你不知道总线上有哪些 I3C 设备，或者想让系统自动发现和分配地址，可以使用 ENTDAA 方式：

.. figure:: ../../../_static/diagrams/i3c/i3c_entdaa.svg
    :align: center
    :alt: I3C 自动动态地址分配

    I3C 自动动态地址分配

.. code:: c

    // 1. 创建 I3C 总线（需要设置 entdaa_device_num）
    i3c_master_bus_config_t i3c_mst_config = {
        // ... 其他配置 ...
        .entdaa_device_num = 5,        // 驱动最大可以动态发现的设备数量
    };
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. 扫描总线上的 I3C 设备
    i3c_master_i3c_device_table_handle_t table_handle = NULL;
    ESP_ERROR_CHECK(i3c_master_scan_devices_by_entdaa(bus_handle, &table_handle));

    // 3. 获取发现的设备数量
    size_t device_count = 0;
    ESP_ERROR_CHECK(i3c_master_get_device_count(table_handle, &device_count));
    printf("Found %zu I3C devices\n", device_count);

    // 4. 遍历所有设备并获取设备信息
    i3c_master_i3c_device_handle_t dev = NULL;
    for (size_t i = 0; i < device_count; i++) {
        i3c_master_i3c_device_handle_t dev_handle = NULL;
        ESP_ERROR_CHECK(i3c_master_get_device_handle(table_handle, i, &dev_handle));

        // 获取设备信息
        i3c_device_information_t info;
        ESP_ERROR_CHECK(i3c_master_i3c_device_get_info(dev_handle, &info));
        printf("Device %d: Dynamic Addr=0x%02X, BCR=0x%02X, DCR=0x%02X, PID=0x%016llX\n",
               i, info.dynamic_addr, info.bcr, info.dcr, info.pid);

        if (info.pid == /* 设备 PID，从设备手册获取 */) {
            dev = dev_handle;
            break;
        }
    }
    // 释放设备句柄表，不再需要时调用
    ESP_ERROR_CHECK(i3c_master_free_device_handle_table(table_handle));

    // 5. 通过 transmit 或 receive 进行数据传输
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev, data, sizeof(data), -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_receive(dev, data, sizeof(data), -1));

在这个场景中：

1. 创建总线时需要设置 `entdaa_device_num`，表示预期发现的设备数量
2. 使用 :cpp:func:`i3c_master_scan_devices_by_entdaa` 扫描总线上的所有 I3C 设备
3. 系统会自动为每个设备分配动态地址
4. 可以通过 :cpp:func:`i3c_master_get_device_count` 获取设备数量
5. 通过 :cpp:func:`i3c_master_get_device_handle` 获取每个设备的句柄
6. 使用 :cpp:func:`i3c_master_i3c_device_get_info` 可以获取设备的详细信息（动态地址、BCR、DCR、PID）
7. 根据获得的设备信息通过 :cpp:func:`i3c_master_i3c_device_transmit` 或 :cpp:func:`i3c_master_i3c_device_receive` 进行数据传输

.. note::

    :cpp:func:`i3c_master_scan_devices_by_entdaa` 是线程安全的，不会有两个线程同时寻址的情况。根据协议，当从机被 :cpp:func:`i3c_master_scan_devices_by_entdaa` 寻址发现后，不再具有响应第二次寻址的能力。因此不会出现因为在不同线程寻址而导致的地址变化的情况。该接口支持在初始化后新增设备。若要重新扫描，请用 CCC 机制重置 I3C 总线上的地址，或通过重新上电清除总线上的地址信息。

公共命令码（CCC）传输
-----------------------

I3C 协议使用公共命令码（CCC）进行总线管理和设备配置。可以使用 :cpp:func:`i3c_master_transfer_ccc` 函数来发送 CCC 命令。

CCC 传输可以是广播的（发送给所有设备）或直接的（发送给特定设备）：

.. figure:: ../../../_static/diagrams/i3c/i3c_broadcast_ccc.svg
    :align: center
    :alt: I3C 广播命令

    I3C 广播命令

.. figure:: ../../../_static/diagrams/i3c/i3c_direct_ccc.svg
    :align: center
    :alt: I3C 直接命令

    I3C 直接命令

.. code:: c

    // 广播 CCC 命令示例：发送 RSTDAA（重置所有动态地址）
    i3c_master_ccc_transfer_config_t ccc_trans = {
        .ccc_command = I3C_CCC_RSTDAA,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_WRITE,
        .device_address = 0,  // 广播命令，此字段被忽略
        .data = NULL,
        .data_size = 0,
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(bus_handle, &ccc_trans));

    // 直接 CCC 命令示例：读取设备的 GETPID（获取设备 ID）
    uint8_t pid_data[6] = {0};
    ccc_trans = (i3c_master_ccc_transfer_config_t) {
        .ccc_command = I3C_CCC_GETPID,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_READ,
        .device_address = 0x08,  // 目标设备地址，为动态地址
        .data = pid_data,
        .data_size = sizeof(pid_data),
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(bus_handle, &ccc_trans));

.. note::

    :cpp:func:`i3c_master_transfer_ccc` 永远是阻塞的，不受到 DMA 和异步配置的影响。用户需要通过查询 I3C 协议获知 CCC 命令的具体格式，根据下发命令或获取数值的格式填充 :cpp:member:`i3c_master_ccc_transfer_config_t::direction` 为 ``I3C_MASTER_TRANSFER_DIRECTION_READ`` 或 ``I3C_MASTER_TRANSFER_DIRECTION_WRITE`` 并填充 :cpp:member:`i3c_master_ccc_transfer_config_t::data` 和 :cpp:member:`i3c_master_ccc_transfer_config_t::data_size`。

资源回收
---------

当不再需要之前安装的 I3C 总线或设备，请调用 :cpp:func:`i3c_master_bus_rm_i3c_device` 或 :cpp:func:`i3c_master_bus_rm_i2c_device` 来删除设备，然后调用 :cpp:func:`i3c_del_master_bus` 来回收资源，以释放底层硬件。

.. code:: c

    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(i3c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

高级功能
========

时钟及时序参数微调
-------------------

时钟源选择
^^^^^^^^^^^^^^^

I3C 总线的时钟源可以通过 :cpp:member:`i3c_master_bus_config_t::clock_source` 进行选择。

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... 其他配置 ...
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT, // 默认时钟源
    };

.. note::

    当 I3C 推挽输出频率大于 3 MHz 时，请将时钟源设置为 :cpp:enumerator:`i3c_master_clock_source_t::I3C_MASTER_CLK_SRC_PLL_F120M` 或 :cpp:enumerator:`i3c_master_clock_source_t::I3C_MASTER_CLK_SRC_PLL_F160M`。

I3C 驱动提供了丰富的时序参数配置选项，可以根据实际硬件情况调整这些参数以优化性能或解决时序问题。

占空比和保持时间
^^^^^^^^^^^^^^^^^

部分 I3C 从机设备的应答机制（ACK/NACK）有严格的时序限制，比如对 SCL 波形的占空比的要求以及对 SDA 保持时间的要求，这些参数可通过以下配置项进行配置。

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... 其他配置 ...
        .i3c_scl_pp_duty_cycle = 0.5,   // Push-Pull 模式占空比，通常为 0.5 (默认值 0 时占空比也为 0.5)
        .i3c_scl_od_duty_cycle = 0.5,   // Open-Drain 模式占空比，通常为 0.5 (默认值 0 时占空比也为 0.5)
        .i3c_sda_od_hold_time_ns = 25,   // Open-Drain 模式保持时间，默认 25 ns
        .i3c_sda_pp_hold_time_ns = 0,    // Push-Pull 模式保持时间，默认 0 ns
    };

这些参数的具体值需要根据设备手册和实际测试来确定。

关于事件回调函数
------------------

I3C 驱动支持事件回调机制，可以在传输完成或收到 IBI 中断时通知应用程序。

当 I3C 控制器生成发送或接收完成等事件时，会通过中断告知 CPU。如果需要在发生特定事件时调用函数，可以为 I3C 和 I2C 从机分别调用 :cpp:func:`i3c_master_i3c_device_register_event_callbacks` 和 :cpp:func:`i3c_master_i2c_device_register_event_callbacks`，向 I3C 驱动程序的中断服务程序 (ISR) 注册事件回调。由于上述回调函数是在 ISR 中调用的，因此，这些函数不应涉及阻塞操作。可以检查调用 API 的后缀，确保在函数中只调用了后缀为 ISR 的 FreeRTOS API。回调函数具有布尔返回值，指示回调是否解除了更高优先级任务的阻塞状态。

有关 I2C 从机的事件回调，请参阅 i2c_master_i2c_event_callbacks_t。

    * :cpp:member:`i3c_master_i2c_event_callbacks_t::on_trans_done` 可设置用于主机“传输完成”事件的回调函数。该函数原型在 :cpp:type:`i3c_master_i2c_callback_t` 中声明。请注意，当 I2C 从机设备使能 DMA 且使用异步传输时，才能使用该回调函数，具体请参考 :ref:`dma-support`。

有关 I3C 从机的事件回调，请参阅 i3c_master_i3c_event_callbacks_t。

    * :cpp:member:`i3c_master_i3c_event_callbacks_t::on_trans_done` 可设置用于主机“传输完成”事件的回调函数。该函数原型在 :cpp:type:`i3c_master_i3c_callback_t` 中声明。请注意，当 I3C 从机设备使能 DMA 且使用异步传输时，才能使用该回调函数，具体请参考 :ref:`dma-support`。

    * :cpp:member:`i3c_master_i3c_event_callbacks_t::on_ibi` 可设置用于 IBI 事件的回调函数。该函数原型在 :cpp:type:`i3c_master_ibi_callback_t` 中声明, 关于 IBI 事件的详细信息请参阅 :ref:`in-band-interrupt`

.. note::

    回调函数在 ISR 上下文中执行，因此：

    - 不能执行阻塞操作
    - 只能调用后缀为 ISR 的 FreeRTOS API
    - 如果启用了 ``CONFIG_I3C_MASTER_ISR_CACHE_SAFE``，回调函数必须放在 IRAM 中

.. _in-band-interrupt:

关于带内中断（IBI）
---------------------

I3C 协议支持带内中断（IBI），允许从机设备通过 I3C 总线发送中断请求，无需额外的中断线。

配置 IBI
^^^^^^^^^^^^^^^

I3C 总线配置结构体 :cpp:type:`i3c_master_bus_config_t` 中包含 IBI 相关的全局配置项：

- :cpp:member:`i3c_master_bus_config_t::ibi_rstart_trans_en` 在 IBI 上启用重启事务，I3C 控制器在 IBI 完成后继续执行之前被 IBI 中断的命令。若 IBI 发生在总线空闲期间，且 I3C 传输任务非空，则 I3C 控制器将继续执行该任务。若 IBI 与 I3C 控制器传输冲突，并赢得仲裁，则 IBI 处理完成后会继续执行被打断的任务。
- :cpp:member:`i3c_master_bus_config_t::ibi_silent_sir_rejected` 当写入为 0 时，当从机中断请求（SIR）被拒绝时不通知应用层。当写入为 1 时，仍将 IBI 状态写入 IBI FIFO，并通知应用层。
- :cpp:member:`i3c_master_bus_config_t::ibi_no_auto_disable` 如果设置，在控制器 NACK In-Band 中断后，不自动禁用 IBI，保持带内中断使能。

可以使用 :cpp:func:`i3c_master_i3c_device_ibi_config` 函数为特定设备配置 IBI：

.. code:: c

    i3c_ibi_config_t ibi_cfg = {
        .enable_ibi = true,
        .enable_ibi_payload = true,  // 允许 IBI 携带有效载荷
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_ibi_config(dev_handle, &ibi_cfg));

处理 IBI 事件
^^^^^^^^^^^^^^^^

IBI 事件的详细信息将通过 :cpp:type:`i3c_master_ibi_info_t` 从回调中给出：

:cpp:member:`i3c_master_ibi_info_t::ibi_id` 是 IBI 的原始标识符，通常编码自从机设备的动态地址；它是原始值，即 动态地址 + 读/写位。 :cpp:member:`i3c_master_ibi_info_t::ibi_sts` 是 IBI 状态字段，由控制器报告。:cpp:member:`i3c_master_ibi_info_t::data_length` 是有效载荷缓冲区 :cpp:member:`i3c_master_ibi_info_t::ibi_data` 中的有效字节数。 :cpp:member:`i3c_master_ibi_info_t::ibi_data` 是与 IBI 关联的可选有效载荷。只有前 :cpp:member:`i3c_master_ibi_info_t::data_length` 字节是有效的。

.. code:: c

    static bool i3c_ibi_callback(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_ibi_info_t *ibi_info, void *user_ctx)
    {
        // 可以将 IBI 事件数据复制到用户提供的上下文中, 并在任务中做进一步处理
        // i3c_master_ibi_event_t 是用户自定定义的结构体，在这里包括了ibi_id 和 ibi_data_len，可根据实际需求增删成员
        i3c_master_ibi_event_t evt = {
            .ibi_id = ibi_info->ibi_id,
            .ibi_data_len = ibi_info->data_length,
        };

        return false;
    }

    i3c_master_i3c_event_callbacks_t cbs = {
        .on_ibi = i3c_ibi_callback,
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_register_event_callbacks(dev_handle, &cbs, NULL));

.. _dma-support:

关于 DMA 和异步传输
-------------------

I3C 驱动程序支持 DMA 来支持大容量数据传输和异步传输，可以提高传输效率并减少 CPU 占用。

启用 DMA
^^^^^^^^^^^^

可以通过 :cpp:func:`i3c_master_bus_decorate_dma` 函数为总线配置 DMA：

.. code:: c

    i3c_master_dma_config_t dma_config = {
        .max_transfer_size = 4096,  // 最大传输大小（字节）
        .dma_burst_size = 16,       // DMA 突发大小（字节）
    };
    ESP_ERROR_CHECK(i3c_master_bus_decorate_dma(bus_handle, &dma_config));

启用异步传输
^^^^^^^^^^^^^^^

当 DMA 被启用时，可以进一步启用异步传输以提高性能：

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... 其他配置 ...
        .trans_queue_depth = 5,  // 设置内部传输队列的深度
        .flags = {
            .enable_async_trans = 1,  // 启用异步传输
        }
    };

此时，I3C 主机的传输类函数被调用后会立即返回，当每一次传输完成后会调用 :cpp:member:`i3c_master_i3c_event_callbacks_t::on_trans_done` 回调函数表示一次传输的完成。如果需要等待传输完成，可以调用 :cpp:func:`i3c_master_bus_wait_all_done` 函数来等待所有传输完成：

.. code:: c

    // 启动多个异步传输
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle1, data1, size1, -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle2, data2, size2, -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle3, data3, size3, -1));

    // 等待所有传输完成
    ESP_ERROR_CHECK(i3c_master_bus_wait_all_done(bus_handle, -1));

线程安全
--------

I3C 驱动程序的以下函数是线程安全的，可以从不同的 RTOS 任务调用，无需额外的锁保护：

工厂函数：

- :cpp:func:`i3c_new_master_bus`
- :cpp:func:`i3c_del_master_bus`

I3C 主机操作函数（通过总线操作信号保证线程安全）：

- :cpp:func:`i3c_master_bus_add_i3c_static_device`
- :cpp:func:`i3c_master_bus_rm_i3c_device`
- :cpp:func:`i3c_master_i3c_device_transmit`
- :cpp:func:`i3c_master_i3c_device_receive`
- :cpp:func:`i3c_master_i3c_device_transmit_receive`
- :cpp:func:`i3c_master_i2c_device_transmit`
- :cpp:func:`i3c_master_i2c_device_receive`
- :cpp:func:`i3c_master_i2c_device_transmit_receive`
- :cpp:func:`i3c_master_transfer_ccc`

Cache 安全
----------

默认情况下，当 cache 被禁用时（例如 SPI Flash 写入时），I3C 中断会被延迟，事件回调函数将无法按时执行，会影响实时应用的系统响应。

可以通过启用 Kconfig 选项 ``CONFIG_I3C_MASTER_ISR_CACHE_SAFE`` 来避免此种情况发生，启用后：

1. 即使在 cache 被禁用的情况下，中断仍可继续运行
2. 将 ISR 使用的所有函数放入 IRAM 中
3. 将驱动程序对象放入 DRAM 中（以防它被意外映射到 PSRAM 中）

启用该选项可以保证 cache 禁用时的中断运行，但会占用更多的 IRAM。

.. note::

    启用此选项后，当 cache 被禁用时，ISR 回调函数将继续运行。因此，必须确保回调函数及其上下文也是 IRAM 安全的。同时，数据传输的 buffer 也必须放在 DRAM 中。

关于低功耗
------------

当启用电源管理 :ref:`CONFIG_PM_ENABLE` 时，系统在进入睡眠模式前可能会调整或禁用时钟源，从而导致 I3C 传输出错。

为了防止这种情况发生， I3C 驱动内部创建了一个电源管理锁。当调用传输函数后，该锁将被激活，确保系统不会进入睡眠模式，从而保持定时器的正确工作，直至传输完成后，驱动自动释放该锁。使系统能够进入睡眠模式。

Kconfig 选项
------------

以下 Kconfig 选项可用于配置 I3C 驱动程序：

- :ref:`CONFIG_I3C_MASTER_ISR_CACHE_SAFE`：确保 I3C 中断在缓存被禁用时也能正常工作（例如 SPI Flash 写入时）
- :ref:`CONFIG_I3C_MASTER_ISR_HANDLER_IN_IRAM`：将 I3C 主机 ISR 处理程序放入 IRAM 以提高性能并减少缓存未命中
- :ref:`CONFIG_I3C_MASTER_ENABLE_DEBUG_LOG`：启用 I3C 调试日志

关于资源消耗
------------

使用 :doc:`/api-guides/tools/idf-size` 工具可以查看 I3C 驱动的代码和数据消耗。以下是测试前提条件（以 ESP32-P4 为例）：

- 编译器优化等级设置为 ``-Os``，以确保代码尺寸最小化。
- 默认日志等级设置为 ``ESP_LOG_INFO``，以平衡调试信息和性能。
- 关闭以下驱动优化选项：
    - :ref:`CONFIG_I3C_MASTER_ISR_HANDLER_IN_IRAM` - 中断处理程序不放入 IRAM。
    - :ref:`CONFIG_I3C_MASTER_ISR_CACHE_SAFE` - 不启用 Cache 安全选项。

**注意，以下数据不是精确值，仅供参考，在不同型号的芯片上，数据会有所出入。**

+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| Component Layer  | Total Size | DIRAM | .bss | .data | .text | Flash Code | .text | Flash Data | .rodata |
+==================+============+=======+======+=======+=======+============+=======+============+=========+
| hal              |         30 |     0 |    0 |     0 |     0 |         30 |    18 |          0 |      12 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| driver           |       9249 |    12 |   12 |     0 |     0 |       9237 |  8666 |        571 |     571 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+

应用示例
=============

- :example:`peripherals/i3c/i3c_i2c_basic` 演示了初始化 I3C 主机驱动程序并用 I2C 接口从 ICM42688 传感器读取数据的基本步骤。

- :example:`peripherals/i3c/i3c_lsm6dscx` 演示了如何使用 I3C 主机模式从连接的 LSM6DSOX 传感器读取和写入数据，并支持带内中断（IBI）事件处理。

API 参考
============

I3C 驱动 API
-----------------

.. include-build-file:: inc/i3c_master.inc

I3C 驱动 I2C 从机 API
------------------------

.. include-build-file:: inc/i3c_master_i2c.inc

I3C 驱动类型
---------------------

.. include-build-file:: inc/components/esp_hal_i3c/include/hal/i3c_master_types.inc

I3C HAL 类型
----------------

.. include-build-file:: inc/components/esp_driver_i3c/include/driver/i3c_master_types.inc
