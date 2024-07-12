集成电路总线 (I2C)
==================

:link_to_translation:`en:[English]`

简介
----

I2C 是一种串行同步半双工通信协议，总线上可以同时挂载多个主机和从机。I2C 使用两条双向开漏线：串行数据线 (SDA) 和串行时钟线 (SCL)，通过电阻上拉。

{IDF_TARGET_NAME} 有 {IDF_TARGET_SOC_HP_I2C_NUM} 个 I2C 控制器（也被称为端口），负责处理 I2C 总线上的通信。

.. only:: not esp32c2

    单个 I2C 控制器既可以是主机也可以是从机。

.. only:: esp32c2

    单个 I2C 控制器只能是主机。

.. only:: SOC_LP_I2C_SUPPORTED

    此外，{IDF_TARGET_NAME} 芯片还有 1 个低功耗 (LP) I2C 控制器，是常规 I2C 的简化版本。通常，LP I2C 控制器的 RAM 较小，仅支持基本的 I2C 功能，不支持从机模式。有关 HP I2C 和 LP I2C 的所有差异，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *I2C 控制器 (I2C)* > *特性* [`PDF <{IDF_TARGET_TRM_EN_URL}#i2c>`__]。

    当 HP I2C 不足以满足需求时，可以使用 LP I2C 外设。但请注意，LP I2C 不支持某些 HP I2C 功能，在使用 LP I2C 前，请阅读相关文档。

通常，I2C 从机设备具有 7 位地址或 10 位地址。{IDF_TARGET_NAME} 支持 I2C 标准模式 (Sm) 和快速模式 (Fm)，可分别达到 100 kHz 和 400 kHz。

.. warning::

    主机模式的 SCL 时钟频率不应大于 400 kHz。

.. note::

    SCL 的频率受上拉电阻及导线电容的影响。因此，强烈建议选择适当的上拉电阻，确保频率准确。推荐的上拉电阻值通常在 1 kΩ 到 10 kΩ 之间。

    请注意，SCL 的频率越高，上拉电阻应该越小（但不能小于 1 kΩ）。较大的电阻会降低电流，增加时钟切换时间并降低频率。通常推荐 2 kΩ 到 5 kΩ 左右的电阻，也可根据电流需求进行一定调整。


I2C 时钟配置
------------

.. list::

    - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_DEFAULT`：默认的 I2C 时钟源。
    :SOC_I2C_SUPPORT_XTAL: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_XTAL`：以外部晶振作为 I2C 时钟源。
    :SOC_I2C_SUPPORT_RTC: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_RC_FAST`：以内部 20 MHz RC 振荡器作为 I2C 时钟源。
    :SOC_I2C_SUPPORT_APB: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_APB`：以 APB 时钟作为 I2C 时钟源。
    :SOC_I2C_SUPPORT_REF_TICK: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_REF_TICK`：1 MHZ 时钟。

I2C 文件结构
------------

.. figure:: ../../../_static/diagrams/i2c/i2c_code_structure.png
    :align: center
    :alt: I2C 文件结构

    I2C 文件结构

**需要包含在 I2C 应用程序中的公共头文件**

- ``i2c.h``：遗留 I2C API 的头文件（用于使用旧驱动程序的应用）。
- ``i2c_master.h``：提供标准通信模式下特定 API 的头文件（用于使用主机模式的新驱动程序的应用）。
- ``i2c_slave.h``：提供标准通信模式下特定 API 的头文件（用于使从机模式的新驱动程序的应用）。

.. note::

    旧驱动程序与新驱动程序无法共存。包含 ``i2c.h`` 头文件可使用旧驱动程序，或包含 ``i2c_master.h`` 和 ``i2c_slave.h`` 来使用新驱动程序。请注意，现已弃用旧驱动程序，之后将移除。

**上述头文件中包含的公共头文件**

- ``i2c_types_legacy.h``：仅在旧驱动程序中使用的旧公共类型。
- ``i2c_types.h``：提供公共类型的头文件。

功能概述
--------

I2C 驱动程序提供以下服务：

- `资源分配 <#resource-allocation>`__ - 包括如何使用正确的配置来分配 I2C 总线，以及如何在完成工作后回收资源。
- `I2C 主机控制器 <#i2c_master_controller>`__ - 包括 I2C 主机控制器的行为，介绍了数据发送、数据接收和数据的双向传输。
- `I2C 从机控制器 <#i2c_slave_controller>`__ - 包括 I2C 从机控制器的行为，涉及数据发送和数据接收。
- `电源管理 <#power-management>`__ - 描述了不同时钟源对功耗的影响。
- `IRAM 安全 <#iram-safe>`__ - 描述了如何在 cache 被禁用时正常运行 I2C 中断。
- `线程安全 <#thread-safety>`__ - 列出了驱动程序中线程安全的 API。
- `Kconfig 选项 <#kconfig-options>`__ - 列出了支持的 Kconfig 选项，这些选项可以对驱动程序产生不同影响。

资源分配
^^^^^^^^^

若系统支持 I2C 主机总线和 I2C 从机总线，则由驱动程序中的 :cpp:type:`i2c_bus_handle_t` 来表示。资源池管理可用的端口，并在有请求时分配空闲端口。

安装 I2C 主机总线和设备
~~~~~~~~~~~~~~~~~~~~~~~

I2C 主机总线是基于总线-设备模型设计的，因此需要分别使用 :cpp:type:`i2c_master_bus_config_t` 和 :cpp:type:`i2c_device_config_t` 来分配 I2C 主机总线实例和 I2C 设备实例。

.. figure:: ../../../_static/diagrams/i2c/i2c_master_module.png
    :align: center
    :alt: I2C 主机总线-设备模块

    I2C 主机总线-设备模块

I2C 主机总线需要 :cpp:type:`i2c_master_bus_config_t` 指定的配置：

- :cpp:member:`i2c_master_bus_config_t::i2c_port` 设置控制器使用的 I2C 端口。
- :cpp:member:`i2c_master_bus_config_t::sda_io_num` 设置串行数据总线 (SDA) 的 GPIO 编号。
- :cpp:member:`i2c_master_bus_config_t::scl_io_num` 设置串行时钟总线 (SCL) 的 GPIO 编号。
- :cpp:member:`i2c_master_bus_config_t::clk_source` 选择 I2C 总线的时钟源。可用时钟列表见 :cpp:type:`i2c_clock_source_t`。有关不同时钟源对功耗的影响，请参阅 `电源管理 <#power-management>`__ 部分。
- :cpp:member:`i2c_master_bus_config_t::glitch_ignore_cnt` 设置主机总线的毛刺周期。如果线上的毛刺周期小于设置的值（通常设为 7），则可以被滤除。
- :cpp:member:`i2c_master_bus_config_t::intr_priority` 设置中断的优先级。如果设置为 ``0``，则驱动程序将使用低或中优先级的中断（优先级可设为 1、2 或 3 中的一个），若未设置，则将使用 :cpp:member:`i2c_master_bus_config_t::intr_priority` 指示的优先级。请使用数字形式（1、2、3），不要用位掩码形式（(1<<1)、(1<<2)、(1<<3)）。
- :cpp:member:`i2c_master_bus_config_t::trans_queue_depth` 设置内部传输队列的深度，但仅在异步传输中有效。
- :cpp:member:`i2c_master_bus_config_t::enable_internal_pullup` 启用内部上拉电阻。注意：该设置无法在高速频率下拉高总线，此时建议使用合适的外部上拉电阻。


如果在 :cpp:type:`i2c_master_bus_config_t` 中指定了配置，则可调用 :cpp:func:`i2c_new_master_bus` 来分配和初始化 I2C 主机总线。如果函数运行正确，则将返回一个 I2C 总线句柄。若没有可用的 I2C 端口，此函数将返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

I2C 主机设备需要 :cpp:type:`i2c_device_config_t` 指定的配置：

- :cpp:member:`i2c_device_config_t::dev_addr_length` 配置从机设备的地址位长度，可从枚举 :cpp:enumerator:`I2C_ADDR_BIT_LEN_7` 或 :cpp:enumerator:`I2C_ADDR_BIT_LEN_10` （如果支持）中进行选择。
- :cpp:member:`i2c_device_config_t::device_address` 设置 I2C 设备原始地址，请直接将设备地址解析到此成员。例如，若设备地址为 0x28，则将 0x28 解析到 :cpp:member:`i2c_device_config_t::device_address`，不要带写入或读取位。
- :cpp:member:`i2c_device_config_t::scl_speed_hz` 设置此设备的 SCL 线频率。
- :cpp:member:`i2c_device_config_t::scl_wait_us` 设置 SCL 等待时间（以微秒为单位）。通常此值较大，因为从机延伸时间会很长（甚至可能延伸到 12 ms）。设置为 ``0`` 表示使用默认的寄存器值。


一旦填充好 :cpp:type:`i2c_device_config_t` 结构体的必要参数，就可调用 :cpp:func:`i2c_master_bus_add_device` 来分配 I2C 设备实例，并将设备挂载到主机总线上。如果函数运行正确，则将返回一个 I2C 设备句柄。若未正确初始化 I2C 总线，此函数将返回 :c:macro:`ESP_ERR_INVALID_ARG` 错误。

.. code:: c

    #include "driver/i2c_master.h"

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

通过端口获取 I2C 主控句柄
~~~~~~~~~~~~~~~~~~~~~~~~~

当在某个模块（例如音频模块）中已经初始化了 I2C 主控句柄，但在另一个模块（例如视频模块）中不方便获取该句柄。使用辅助函数 :cpp:func:`i2c_master_get_bus_handle` 可通过端口获取已初始化的句柄。但请确保句柄已经提前初始化，否则可能会报错。

.. code:: c

    // 源文件 1
    #include "driver/i2c_master.h"
    i2c_master_bus_handle_t bus_handle;
    i2c_master_bus_config_t i2c_mst_config = {
        ... // 与其他相同
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // 源文件 2
    #include "esp_private/i2c_platform.h"
    #include "driver/i2c_master.h"
    i2c_master_bus_handle_t handle;
    ESP_ERROR_CHECK(i2c_master_get_bus_handle(0, &handle));

.. only:: SOC_LP_I2C_SUPPORTED

    使用 LP I2C 外设来安装 I2C 主机总线
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    使用 LP I2C 外设来安装 I2C 主机总线的流程与安装 HP I2C 外设几乎相同，但仍有一些区别，包括 IO、时钟源及 I2C 端口编号等。以下代码展示了如何使用 LP_I2C 来安装 I2C 主机总线。

    .. code:: c

        #include "driver/i2c_master.h"

        i2c_master_bus_config_t i2c_mst_config = {
            .clk_source = LP_I2C_SCLK_DEFAULT,    // LP I2C 的时钟源，可能与 HP I2C 不同
            .i2c_port = LP_I2C_NUM_0,             // 分配给 LP I2C 端口
            .scl_io_num = 7,                      // SCL IO 编号。请参阅技术参考手册
            .sda_io_num = 6,                      // SDA IO 编号号。请参阅技术参考手册
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };

        i2c_master_bus_handle_t bus_handle;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = 0x58,
            .scl_speed_hz = 100000,
        };

        i2c_master_dev_handle_t dev_handle;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

卸载 I2C 主机总线和设备
~~~~~~~~~~~~~~~~~~~~~~~~

如果不再需要之前安装的 I2C 总线或设备，建议调用 :cpp:func:`i2c_master_bus_rm_device` 或 :cpp:func:`i2c_del_master_bus` 来回收资源，以释放底层硬件。

安装 I2C 从机设备
~~~~~~~~~~~~~~~~~~~

I2C 从机设备需要 :cpp:type:`i2c_slave_config_t` 指定的配置：

.. list::

    - :cpp:member:`i2c_slave_config_t::i2c_port` 设置控制器使用的 I2C 端口。
    - :cpp:member:`i2c_slave_config_t::sda_io_num` 设置串行数据总线 (SDA) 的 GPIO 编号。
    - :cpp:member:`i2c_slave_config_t::scl_io_num` 设置串行时钟总线 (SCL) 的 GPIO 编号。
    - :cpp:member:`i2c_slave_config_t::clk_source` 选择 I2C 总线的时钟源。可用时钟列表见 :cpp:type:`i2c_clock_source_t`。有关不同时钟源对功耗的影响，请参阅 `电源管理 <#power-management>`__。
    - :cpp:member:`i2c_slave_config_t::send_buf_depth` 设置发送 buffer 的长度。
    - :cpp:member:`i2c_slave_config_t::slave_addr` 设置从机地址。
    - :cpp:member:`i2c_master_bus_config_t::intr_priority` 设置中断的优先级。如果设置为 ``0`` ，则驱动程序将使用低或中优先级的中断（优先级可设为 1、2 或 3 中的一个），若未设置，则将使用 :cpp:member:`i2c_master_bus_config_t::intr_priority` 指示的优先级。请使用数字形式（1、2、3），不要用位掩码形式（(1<<1)、(1<<2)、(1<<3)）。请注意，中断优先级一旦设置完成，在调用 :cpp:func:`i2c_del_master_bus` 之前都无法更改。
    - :cpp:member:`i2c_slave_config_t::addr_bit_len`。如果需要从机设备具有 10 位地址，则将该成员变量设为 ``I2C_ADDR_BIT_LEN_10``。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::stretch_en`。如果要启用从机控制器拉伸功能，请将该成员变量设为 true。有关 I2C 拉伸的工作原理，请参阅 [`TRM <{IDF_TARGET_TRM_EN_URL}#i2c>`__]。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::broadcast_en`。如果要启用从机广播，请将该成员变量设为 true。当从机设备接收到来自主机设备的通用调用地址 0x00，且后面的读写位为 0 时，无论从机设备自身地址如何，都会响应主机设备。
    :SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS: - :cpp:member:`i2c_slave_config_t::access_ram_en`。如果要启用 non-FIFO 模式，请将该成员变量设为 true，则 I2C 数据 FIFO 可用作 RAM，并将同步打开双地址。
    :SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH: - :cpp:member:`i2c_slave_config_t::slave_unmatch_en`。将该成员变量设为 true，将启用从机设备不匹配中断。如果主机设备发送的命令地址与从机设备地址不匹配，则会触发不匹配中断。

一旦填充好 :cpp:type:`i2c_slave_config_t` 结构体的必要参数，就可调用 :cpp:func:`i2c_new_slave_device` 来分配和初始化 I2C 主机总线。如果函数运行正确，则将返回一个 I2C 总线句柄。若没有可用的 I2C 端口，此函数将返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

.. code:: c

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

卸载 I2C 从机设备
~~~~~~~~~~~~~~~~~~

如果不再需要之前安装的 I2C 总线，建议调用 :cpp:func:`i2c_del_slave_device` 来回收资源，以释放底层硬件。


I2C 主机控制器
^^^^^^^^^^^^^^^

通过调用 :cpp:func:`i2c_new_master_bus` 安装好 I2C 主机控制器驱动程序后，{IDF_TARGET_NAME} 就可以与其他 I2C 设备进行通信了。I2C API 允许标准事务，如下图所示：

.. wavedrom:: /../_static/diagrams/i2c/i2c_trans_wave.json

I2C 主机写入
~~~~~~~~~~~~~~

在成功安装 I2C 主机总线之后，可以通过调用 :cpp:func:`i2c_master_transmit` 来向从机设备写入数据。下图解释了该函数的原理。

简单来说，驱动程序用一系列命令填充了一个命令链，并将该命令链传递给 I2C 控制器执行。

.. figure:: ../../../_static/diagrams/i2c/i2c_master_write_slave.png
    :align: center
    :alt: I2C 主机向从机设备写入数据

    I2C 主机向从机设备写入数据

将数据写入从机设备的简单示例：

.. code:: c

    #define DATA_LENGTH 100
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));


I2C 主机写入操作还支持在单次传输事务中传输多个 buffer。如下所示：

.. code:: c

    uint8_t control_phase_byte = 0;
    size_t control_phase_size = 0;
    if (/*condition*/) {
        control_phase_byte = 1;
        control_phase_size = 1;
    }

    uint8_t *cmd_buffer = NULL;
    size_t cmd_buffer_size = 0;
    if (/*condition*/) {
        uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
        cmd_buffer = cmds;
        cmd_buffer_size = 4;
    }

    uint8_t *lcd_buffer = NULL;
    size_t lcd_buffer_size = 0;
    if (buffer) {
        lcd_buffer = (uint8_t*)buffer;
        lcd_buffer_size = buffer_size;
    }

    i2c_master_transmit_multi_buffer_info_t lcd_i2c_buffer[3] = {
        {.write_buffer = &control_phase_byte, .buffer_size = control_phase_size},
        {.write_buffer = cmd_buffer, .buffer_size = cmd_buffer_size},
        {.write_buffer = lcd_buffer, .buffer_size = lcd_buffer_size},
    };

    i2c_master_multi_buffer_transmit(handle, lcd_i2c_buffer, sizeof(lcd_i2c_buffer) / sizeof(i2c_master_transmit_multi_buffer_info_t), -1);


I2C 主机读取
~~~~~~~~~~~~~~

在成功安装 I2C 主机总线后，可以通过调用 :cpp:func:`i2c_master_receive` 从从机设备读取数据。下图解释了该函数的原理。

.. figure:: ../../../_static/diagrams/i2c/i2c_master_read_slave.png
    :align: center
    :alt: I2C 主机从从机设备读取数据

    I2C 主机从从机设备读取数据

从从机设备读取数据的简单示例：

.. code:: c

    #define DATA_LENGTH 100
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1);

I2C 主机写入后读取
~~~~~~~~~~~~~~~~~~~

从一些 I2C 设备中读取数据之前需要进行写入配置，可通过 :cpp:func:`i2c_master_transmit_receive` 接口进行配置。下图解释了该函数的原理。

.. figure:: ../../../_static/diagrams/i2c/i2c_master_write_read_slave.png
    :align: center
    :alt: I2C 主机向从机设备写入并从从机设备读取数据

    I2C 主机向从机设备写入并从从机设备读取数据

请注意，在写入操作和读取操作之间没有插入 STOP 条件位，因此该功能适用于从 I2C 设备读取寄存器。以下是向从机设备写入数据并从从机设备读取数据的简单示例：

.. code:: c

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(I2C_PORT_NUM_0, &dev_cfg, &dev_handle));
    uint8_t buf[20] = {0x20};
    uint8_t buffer[2];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, buf, sizeof(buf), buffer, 2, -1));

I2C 主机探测
~~~~~~~~~~~~

I2C 驱动程序可以使用 :cpp:func:`i2c_master_probe` 来检测设备是否已经连接到 I2C 总线上。如果该函数返回 ``ESP_OK``，则表示该设备已经被检测到。

.. important::

    在调用该函数时，必须将上拉电阻连接到 SCL 和 SDA 管脚。如果在正确解析 `xfer_timeout_ms` 时收到 `ESP_ERR_TIMEOUT`，则应检查上拉电阻。若暂无合适的电阻，也可将 `flags.enable_internal_pullup` 设为 true。

.. figure:: ../../../_static/diagrams/i2c/i2c_master_probe.png
    :align: center
    :alt: I2C 主机探测

    I2C 主机探测

探测 I2C 设备的简单示例：

.. code:: c

    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x22, -1));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));


I2C 从机控制器
^^^^^^^^^^^^^^

通过调用 :cpp:func:`i2c_new_slave_device` 安装好 I2C 从机驱动程序后，{IDF_TARGET_NAME} 就可以作为从机与其他 I2C 主机进行通信了。

I2C 从机写入
~~~~~~~~~~~~~

I2C 从机的发送 buffer 可作为 FIFO 来存储要发送的数据。在主机请求这些数据前，它们会一直排队。可通过调用 :cpp:func:`i2c_slave_transmit` 来传输数据。

将数据写入 FIFO 的简单示例：

.. code:: c

    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,   // 7 位地址
        .clk_source = I2C_CLK_SRC_DEFAULT,    // 设置时钟源
        .i2c_port = 0,                        // 设置 I2C 端口编号
        .send_buf_depth = 256,                // 设置 TX buffer 长度
        .scl_io_num = 2,                      // SCL 管脚编号
        .sda_io_num = 1,                      // SDA 管脚编号
        .slave_addr = 0x58,                   // 从机地址
    };

    i2c_bus_handle_t i2c_bus_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &i2c_bus_handle));
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_ERROR_CHECK(i2c_slave_transmit(i2c_bus_handle, data_wr, DATA_LENGTH, 10000));

I2C 从机读取
~~~~~~~~~~~~~

每当主机将数据写入从机，从机都会自动将数据存储在接收 buffer 中，从而使从机应用程序能自由调用 :cpp:func:`i2c_slave_receive`。:cpp:func:`i2c_slave_receive` 为非阻塞接口，因此要想知道接收是否完成，需注册回调函数 :cpp:func:`i2c_slave_register_event_callbacks`。

.. code:: c

    static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
    {
        BaseType_t high_task_wakeup = pdFALSE;
        QueueHandle_t receive_queue = (QueueHandle_t)user_data;
        xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
        return high_task_wakeup == pdTRUE;
    }

    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    uint32_t size_rd = 0;

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    ESP_ERROR_CHECK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    // 接收完成。

.. only:: SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS

    将数据放入 I2C 从机 RAM 中
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    如上所述，I2C 从机 FIFO 可被用作 RAM，即可以通过地址字段直接访问 RAM。例如，可参照下图将数据写入第三个 RAM 块。请注意，在进行操作前需要先将 :cpp:member:`i2c_slave_config_t::access_ram_en` 设为 true。

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_write_slave_ram.png
        :align: center
        :alt: 将数据放入 I2C 从机 RAM 中

        将数据放入 I2C 从机 RAM 中

    .. code:: c

        uint8_t data_rd[DATA_LENGTH_RAM] = {0};

        i2c_slave_config_t i2c_slv_config = {
            .addr_bit_len = I2C_ADDR_BIT_LEN_7,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = TEST_I2C_PORT,
            .send_buf_depth = 256,
            .scl_io_num = I2C_SLAVE_SCL_IO,
            .sda_io_num = I2C_SLAVE_SDA_IO,
            .slave_addr = 0x58,
            .flags.access_ram_en = true,
        };

        // 主机将数据写入从机。

        i2c_slave_dev_handle_t slave_handle;
        ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
        ESP_ERROR_CHECK(i2c_slave_read_ram(slave_handle, 0x5, data_rd, DATA_LENGTH_RAM));
        ESP_ERROR_CHECK(i2c_del_slave_device(slave_handle));

    从 I2C 从机 RAM 中获取数据
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    数据可被存储在相对从机一定偏移量的 RAM 中，且主机可直接通过 RAM 地址读取这些数据。例如，如果数据被存储在第三个 RAM 块中，则主机可参照下图读取这些数据。请注意，在操作前需要先将 :cpp:member:`i2c_slave_config_t::access_ram_en` 设为 true。

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_read_slave_ram.png
        :align: center
        :alt: 从 I2C 从机 RAM 中获取数据

        从 I2C 从机 RAM 中获取数据

    .. code:: c

        uint8_t data_wr[DATA_LENGTH_RAM] = {0};

        i2c_slave_config_t i2c_slv_config = {
            .addr_bit_len = I2C_ADDR_BIT_LEN_7,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = TEST_I2C_PORT,
            .send_buf_depth = 256,
            .scl_io_num = I2C_SLAVE_SCL_IO,
            .sda_io_num = I2C_SLAVE_SDA_IO,
            .slave_addr = 0x58,
            .flags.access_ram_en = true,
        };

        i2c_slave_dev_handle_t slave_handle;
        ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
        ESP_ERROR_CHECK(i2c_slave_write_ram(slave_handle, 0x2, data_wr, DATA_LENGTH_RAM));
        ESP_ERROR_CHECK(i2c_del_slave_device(slave_handle));

注册事件回调函数
^^^^^^^^^^^^^^^^^

I2C 主机回调
~~~~~~~~~~~~~

当 I2C 主机总线触发中断时，将生成特定事件并通知 CPU。如果需要在发生这些事件时调用某些函数，可通过 :cpp:func:`i2c_master_register_event_callbacks` 将这些函数挂接到中断服务程序 (ISR) 上。由于注册的回调函数是在中断上下文中被调用的，所以应确保这些函数不会阻塞（例如，确保仅从函数内部调用带有 ``ISR`` 后缀的 FreeRTOS API）。回调函数需要返回一个布尔值，告诉 ISR 是否唤醒了高优先级任务。

I2C 主机事件回调函数列表见 :cpp:type:`i2c_master_event_callbacks_t`。

虽然 I2C 是一种同步通信协议，但也支持通过注册上述回调函数来实现异步行为，此时 I2C API 将成为非阻塞接口。但请注意，在同一总线上只有一个设备可以采用异步操作。

.. important::

    I2C 主机异步传输仍然是一个实验性功能（问题在于当异步传输量过大时，会导致内存异常。）

- :cpp:member:`i2c_master_event_callbacks_t::on_recv_done` 可设置用于主机“传输完成”事件的回调函数。该函数原型在 :cpp:type:`i2c_master_callback_t` 中声明。

I2C 从机回调
~~~~~~~~~~~~~

当 I2C 从机总线触发中断时，将生成特定事件并通知 CPU。如果需要在发生这些事件时调用某些函数，可通过 :cpp:func:`i2c_slave_register_event_callbacks` 将这些函数挂接到中断服务程序 (ISR) 上。由于注册的回调函数是在中断上下文中被调用的，所以应确保这些函数不会导致延迟（例如，确保仅从函数中调用带有 ``ISR`` 后缀的 FreeRTOS API）。回调函数需要返回一个布尔值，告诉调用者是否唤醒了高优先级任务。

I2C 从机事件回调函数列表见 :cpp:type:`i2c_slave_event_callbacks_t`。

.. list::

    - :cpp:member:`i2c_slave_event_callbacks_t::on_recv_done` 可设置用于“接收完成”事件的回调函数。该函数原型在 :cpp:type:`i2c_slave_received_callback_t` 中声明。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_event_callbacks_t::on_stretch_occur` 可设置用于“时钟拉伸”事件的回调函数。该函数原型在 :cpp:type:`i2c_slave_stretch_callback_t` 中声明。

电源管理
^^^^^^^^^^

.. only:: SOC_I2C_SUPPORT_APB

    启用电源管理（即打开 :ref:`CONFIG_PM_ENABLE`），系统会在进入 Light-sleep 模式前调整或暂停 I2C FIFO 的时钟源，这可能会导致 I2C 信号改变，传输或接收到无效数据。

    但驱动程序可以通过获取 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 类型的电源管理锁来防止系统改变 APB 频率。每当用户创建一个以 :cpp:enumerator:`I2C_CLK_SRC_APB` 为时钟源的 I2C 总线，驱动程序将在开始 I2C 操作时获取电源管理锁，并在结束 I2C 操作时自动释放锁。

.. only:: SOC_I2C_SUPPORT_REF_TICK

    如果控制器以 :cpp:enumerator:`I2C_CLK_SRC_REF_TICK` 为时钟源，则驱动程序不会为其安装电源管理锁，因为对于低功耗应用，只要时钟源能够提供足够的精度即可。

.. only:: SOC_I2C_SUPPORT_XTAL

    如果控制器以 :cpp:enumerator:`I2C_CLK_SRC_XTAL` 为时钟源，则驱动程序不会为其安装电源管理锁，因为对于低功耗应用，只要时钟源能够提供足够的分辨率即可。

IRAM 安全
^^^^^^^^^

默认情况下，若 cache 因写入或擦除 flash 等原因而被禁用时，将推迟 I2C 中断。此时事件回调函数将无法按时执行，会影响实时应用的系统响应。

Kconfig 选项 :ref:`CONFIG_I2C_ISR_IRAM_SAFE` 能够做到以下几点：

1. 即使 cache 被禁用，I2C 中断依旧正常运行。
2. 将 ISR 使用的所有函数放入 IRAM 中。
3. 将驱动程序对象放入 DRAM 中（以防它被意外映射到 PSRAM 中）。

启用以上选项，即使 cache 被禁用，I2C 中断依旧正常运行，但会增加 IRAM 的消耗。

线程安全
^^^^^^^^^^^^^

工厂函数 :cpp:func:`i2c_new_master_bus` 和 :cpp:func:`i2c_new_slave_device` 由驱动程序保证其线程安全，不需要额外的锁保护，也可从不同的 RTOS 任务中调用这些函数。应避免从多个任务中调用其他非线程安全的公共 I2C API，若确实需要调用，请添加额外的锁。

Kconfig 选项
^^^^^^^^^^^^^^^

- :ref:`CONFIG_I2C_ISR_IRAM_SAFE` 将在 cache 被禁用时控制默认的 ISR 处理程序正常工作，详情请参阅 `IRAM 安全 <#iram-safe>`__。
- :ref:`CONFIG_I2C_ENABLE_DEBUG_LOG` 可启用调试日志，但会增加固件二进制文件大小。

应用示例
--------

.. list::

    - :example:`peripherals/i2c/i2c_eeprom` 通过读取和写入 I2C 连接的 EEPROM 展示了 I2C 驱动程序的使用方法。
    - :example:`peripherals/i2c/i2c_tools` 基于 ESP32 控制台组件实现了一些 I2C 工具的基本功能。

API 参考
--------

.. include-build-file:: inc/i2c_master.inc

.. only:: SOC_I2C_SUPPORT_SLAVE

    .. include-build-file:: inc/i2c_slave.inc

.. include-build-file:: inc/components/esp_driver_i2c/include/driver/i2c_types.inc
.. include-build-file:: inc/components/hal/include/hal/i2c_types.inc
