专用 GPIO
===============

:link_to_translation:`en:[English]`

概述
--------

专用 GPIO 专为 CPU 与 GPIO 矩阵和 IO MUX 交互而设计。任何配置为“专用”的 GPIO 都可以通过 CPU 指令直接访问，从而轻松提高 GPIO 翻转速度，方便用户以 bit-banging 的方式模拟串行/并行接口。通过 CPU 指令的方式控制 GPIO 的软件开销非常小，因此能够胜任一些特殊场合，比如通过示波器观测 “GPIO 翻转信号” 来间接测量某些性能指标。


创建/销毁 GPIO 捆绑包
--------------------------

GPIO 捆绑包是一组 GPIO，该组 GPIO 可以在一个 CPU 周期内同时操作。一个包能够包含 GPIO 的最大数量受每个 CPU 的限制。另外，GPIO 捆绑包与派生它的 CPU 有很强的相关性。**注意，任何对 GPIO 捆绑包操作的任务都必须运行在 GPIO 捆绑包所属的 CPU 内核。** 同理，只有那些安装在同一个 CPU 内核上的 ISR 才允许对该 GPIO 捆绑包进行操作。

.. note::

    专用 GPIO 更像是 CPU 外设，因此与 CPU 内核关系密切。强烈建议在 pin-to-core 任务中安装和操作 GPIO 捆绑包。例如，如果 GPIOA 连接到了 CPU0，而专用的 GPIO 指令却是从 CPU1 发出的，那么就无法控制 GPIOA。

安装 GPIO 捆绑包需要调用 :cpp:func:`dedic_gpio_new_bundle` 来分配软件资源并将专用通道连接到用户选择的 GPIO。GPIO 捆绑包的配置在 :cpp:type:`dedic_gpio_bundle_config_t` 结构体中：

- :cpp:member:`gpio_array`：包含 GPIO 编号的数组。
- :cpp:member:`array_size`：:cpp:member:`gpio_array` 的元素个数。
- :cpp:member:`flags`：用于控制 GPIO 捆绑包行为的标志。

  - :cpp:member:`in_en` 和 :cpp:member:`out_en` 用于选择是否开启输入输出功能（这两个功能可以同时开启）。
  - :cpp:member:`in_invert` 和 :cpp:member:`out_invert` 用于选择是否反转 GPIO 信号。

以下代码展示了如何安装只有输出功能的 GPIO 捆绑包：

.. highlight:: c

::

    // 配置 GPIO
    const int bundleA_gpios[] = {0, 1};
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
    };
    for (int i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++) {
        io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
        gpio_config(&io_conf);
    }
    // 创建 bundleA，仅输出
    dedic_gpio_bundle_handle_t bundleA = NULL;
    dedic_gpio_bundle_config_t bundleA_config = {
        .gpio_array = bundleA_gpios,
        .array_size = sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]),
        .flags = {
            .out_en = 1,
        },
    };
    ESP_ERROR_CHECK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));

如需卸载 GPIO 捆绑包，可调用 :cpp:func:`dedic_gpio_del_bundle`。

.. note::

    :cpp:func:`dedic_gpio_new_bundle` 不包含任何 GPIO pad 配置（例如上拉/下拉、驱动能力、输出/输入使能）。因此，在安装专用 GPIO 捆绑包之前，必须使用 GPIO 驱动程序 API（如 :cpp:func:`gpio_config`）单独配置 GPIO。更多关于 GPIO 驱动的信息，请参考 :doc:`GPIO API 参考 <gpio>`。


GPIO 捆绑包操作
----------------------

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - 操作
     - 函数
   * - 以掩码的方式指定 GPIO 捆绑包的输出
     - :cpp:func:`dedic_gpio_bundle_write`
   * - 读取 GPIO 捆绑包实际输出的电平值
     - :cpp:func:`dedic_gpio_bundle_read_out`
   * - 读取 GPIO 捆绑包中输入的电平值
     - :cpp:func:`dedic_gpio_bundle_read_in`

.. note::

    由于函数调用的开销和内部涉及的位操作，使用上述函数可能无法获得较高的 GPIO 翻转速度。用户可以尝试 :ref:`manipulate_gpios_by_writing_assembly_code` 来减少开销，但应自行注意线程安全。

.. _manipulate_gpios_by_writing_assembly_code:

通过编写汇编代码操作 GPIO
------------------------------------------

高阶用户可以通过编写汇编代码或调用 CPU 低层 API 来操作 GPIO。常见步骤为：

1. 分配一个 GPIO 捆绑包：:cpp:func:`dedic_gpio_new_bundle`
2. 查询该包占用的掩码：:cpp:func:`dedic_gpio_get_out_mask` 和/或 :cpp:func:`dedic_gpio_get_in_mask`
3. 调用 CPU LL apis（如 `cpu_ll_write_dedic_gpio_mask`）或使用该掩码编写汇编代码
4. 切换 IO 的最快捷方式是使用专用的“设置/清除”指令：

    .. only:: esp32s2 or esp32s3

        - 设置 GPIO 位：``set_bit_gpio_out imm[7:0]``
        - 清除 GPIO 位：``clr_bit_gpio_out imm[7:0]``
        - 注意：立即数宽度取决于专用 GPIO 通道的数量

    .. only:: esp32c2 or esp32c3 or esp32c6 or esp32h2

        - 设置 GPIO 位：``csrrsi rd, csr, imm[4:0]``
        - 清除 GPIO 位：``csrrci rd, csr, imm[4:0]``
        - 注意：只能控制最低位的 4 个 GPIO 通道

.. only:: esp32s2

    有关支持的专用 GPIO 指令的详细信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **IO MUX 和 GPIO 矩阵 (GPIO, IO_MUX)** [`PDF <{IDF_TARGET_TRM_CN_URL}#iomuxgpio>`__].

.. only:: esp32s3

    有关支持的专用 GPIO 指令的详细信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **处理器指令拓展 (PIE)（稍后发布）** [`PDF <{IDF_TARGET_TRM_CN_URL}#pie>`__].

.. only:: not (esp32s2 or esp32s3)

    有关支持的专用 GPIO 指令的详细信息，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **ESP-RISC-V CPU** [`PDF <{IDF_TARGET_TRM_CN_URL}#riscvcpu>`__]。

一些专用的 CPU 指令也包含在 ``hal/dedic_gpio_cpu_ll.h`` 中，作为辅助内联函数。

.. note::

    由于自定义指令在不同目标上可能会有不同的格式，在应用程序中编写汇编代码可能会让代码难以在不同的芯片架构之间移植。

.. only:: SOC_DEDIC_GPIO_HAS_INTERRUPT

    中断处理
    ------------------

    专用 GPIO 还可以在特定输入事件时触发中断。:cpp:type:`dedic_gpio_intr_type_t` 定义了所有支持的事件。

    可以通过调用 :cpp:func:`dedic_gpio_bundle_set_interrupt_and_callback` 来启用和注册中断回调。:cpp:type:`dedic_gpio_isr_callback_t` 定义了回调函数的原型。如果唤醒了一些高优先级任务，回调应该返回 true。

    .. highlight:: c

    ::

        // 用户定义 ISR 回调
        IRAM_ATTR bool dedic_gpio_isr_callback(dedic_gpio_bundle_handle_t bundle, uint32_t index, void *args)
        {
            SemaphoreHandle_t sem = (SemaphoreHandle_t)args;
            BaseType_t high_task_wakeup = pdFALSE;
            xSemaphoreGiveFromISR(sem, &high_task_wakeup);
            return high_task_wakeup == pdTRUE;
        }

        // 在捆绑包中的第二个 GPIO 上（即索引为 1）启用上升沿中断
        ESP_ERROR_CHECK(dedic_gpio_bundle_set_interrupt_and_callback(bundle, BIT(1), DEDIC_GPIO_INTR_POS_EDGE, dedic_gpio_isr_callback, sem));

        // 等待完成信号量
        xSemaphoreTake(sem, portMAX_DELAY);


应用示例
-------------------

.. list::

    * 通过汇编代码使用专用的 CPU 指令来操作 GPIO 以模拟 UART/I2C/SPI 协议（Bit Banging） :example:`peripherals/dedicated_gpio`.
    :SOC_DEDIC_GPIO_HAS_INTERRUPT: * 基于专用 GPIO 驱动的矩阵键盘：:example:`peripherals/gpio/matrix_keyboard`.


API 参考
-------------

.. include-build-file:: inc/dedic_gpio.inc
