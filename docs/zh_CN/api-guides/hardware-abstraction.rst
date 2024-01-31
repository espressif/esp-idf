硬件抽象
====================

:link_to_translation:`en:[English]`

ESP-IDF 提供了一组用于硬件抽象的 API，支持以不同抽象级别控制外设，相比仅使用 ESP-IDF 驱动程序与硬件进行交互，使用更加灵活。ESP-IDF 硬件抽象适用于编写高性能裸机驱动程序，或尝试将 ESP 芯片移植到另一个平台。

本指南分为以下三个小节：

    1. :ref:`hw-abstraction-architecture`
    2. :ref:`hw-abstraction-ll-layer`
    3. :ref:`hw-abstraction-hal-layer`

.. warning::

    硬件抽象 API（不包括驱动程序和 ``xxx_types.h``）尚处于试验阶段，因此不能算作公共 API。硬件抽象 API 不遵守 ESP-IDF 版本控制方案的 API 名称更改规范。换言之，非主要 ESP-IDF 版本迭代时，硬件抽象 API 的名称可能会更改。

.. note::

    尽管本文档主要关注外设的硬件抽象，如 UART、SPI、I2C 等，但硬件抽象可以扩展到外设以外其他的硬件部分，如某些 CPU 功能也进行了部分抽象。

.. _hw-abstraction-architecture:

架构
------------

ESP-IDF 的硬件抽象由以下层级各组成，从接近硬件的低层级抽象，到远离硬件的高层级抽象。

- 低级层 (LL)
- 硬件抽象层 (HAL)
- 驱动层

LL 层和 HAL 完全包含在 ``hal`` 组件中，每一层都依赖于其下方的层级，即驱动层依赖于 HAL 层，HAL 层依赖于 LL 层，LL 层依赖于寄存器头文件。

对于特定外设 ``xxx``，其硬件抽象通常由下表中的头文件组成。其中 **特定目标** 指的是文件对于不同目标（即芯片）有不同的实现。然而，对于不同的目标，``#include`` 指令相同，构建系统会自动包含正确版本的头文件和源文件。

.. |br| raw:: html

    <br>

.. list-table:: 硬件抽象头文件
    :widths: 25 5 70
    :header-rows: 1

    * - 包含 |br| 指令
      - 特定 |br| 目标
      - 描述
    * - ``#include 'soc/xxx_caps.h"``
      - 是
      - 此头文件包含了 C 宏列表，指明 {IDF_TARGET_NAME} 外设 ``xxx`` 的各种功能。外设的硬件功能包括通道数量、DMA 支持、硬件 FIFO/缓冲区长度等。
    * - ``#include "soc/xxx_struct.h"`` |br| ``#include "soc/xxx_reg.h"``
      - 是
      - 这两个头文件分别以 C 结构体和 C 宏的形式表示外设寄存器，支持通过其中任一头文件，在寄存器级别上操作外设。
    * - ``#include "soc/xxx_pins.h"``
      - 是
      - 如果某些外设的信号映射到 {IDF_TARGET_NAME} 的特定管脚上，则该头文件中以 C 宏的形式定义了它们的映射关系。
    * - ``#include "soc/xxx_periph.h"``
      - 否
      - 此头文件主要是为了方便，可以自动包含 ``xxx_caps.h``、``xxx_struct.h`` 和 ``xxx_reg.h``。
    * - ``#include "hal/xxx_types.h``
      - 否
      - 此头文件包含了在 LL、HAL 和驱动层间共享的类型定义和宏。此外，作为公共 API，该头文件可以包含在应用层中。共享的类型和定义通常与具体的实现无关，例如：

          - 协议相关的类型/宏，如帧、模式、常见总线速度等。
          - ``xxx`` 外设可能存在的特性/特点，可能存在于任何实现上（与实现无关），例如通道、工作模式、信号放大或衰减强度等。
    * - ``#include "hal/xxx_ll.h"``
      - 是
      - 此头文件包含了硬件抽象的 LL 层。LL 层 API 主要用于将寄存器操作抽象成可读的函数。
    * - ``#include "hal/xxx_hal.h"``
      - 是
      - HAL 层用于将外设操作步骤抽象成函数，如读取缓冲区、启动传输、处理事件等。HAL 层构建在 LL 层之上。
    * - ``#include "driver/xxx.h"``
      - 否
      - 驱动层是 ESP-IDF 硬件抽象的最高级别。驱动层 API 旨在从 ESP-IDF 应用程序中调用，并在内部使用操作系统的基本功能。因此，驱动层 API 由事件驱动，并可在多线程环境中使用。


.. _hw-abstraction-ll-layer:

LL 层（低级层）
------------------

LL 层主要目的是将寄存器字段访问抽象为更容易理解的函数。LL 函数本质是将各种输入/输出参数转换为外设寄存器的寄存器字段，并以获取/设置函数的形式呈现。所有必要的位移、掩码、偏移和寄存器字段的字节顺序都应由 LL 函数处理。

.. code-block:: c

    //在 xxx_ll.h 内

    static inline void xxx_ll_set_baud_rate(xxx_dev_t *hw,
                                            xxx_ll_clk_src_t clock_source,
                                            uint32_t baud_rate) {
        uint32_t src_clk_freq = (source_clk == XXX_SCLK_APB) ? APB_CLK_FREQ : REF_CLK_FREQ;
        uint32_t clock_divider = src_clk_freq / baud;
        // 设置时钟选择字段
        hw->clk_div_reg.divider = clock_divider >> 4;
        // 设置时钟分频器字段
        hw->config.clk_sel = (source_clk == XXX_SCLK_APB) ? 0 : 1;
    }

    static inline uint32_t xxx_ll_get_rx_byte_count(xxx_dev_t *hw) {
        return hw->status_reg.rx_cnt;
    }

以上代码片段展示了外设 ``xxx`` 的典型 LL 函数。LL 函数通常具有以下特点：

- 所有 LL 函数均定义为 ``static inline``，因此，由于编译器优化而调用这些函数时，开销最小。这些函数不保证由编译器内联，因此在禁用缓存时（例如从 IRAM ISR 上下文调用）调用的任何 LL 函数都应标记为 ``__attribute__((always_inline))``。
- 第一个参数应为指向 ``xxx_dev_t`` 类型的指针。``xxx_dev_t`` 类型表示外设寄存器的结构体，因此第一个参数始终是指向外设寄存器起始地址的指针。请注意，在某些情况下，如果外设具有多个相同寄存器布局的通道， ``xxx_dev_t *hw`` 可能指向特定通道的寄存器。
- LL 函数应尽可能简短，并且在大多数情况下是确定性的。换句话说，在最糟糕的情况下，LL 函数的运行时间可以在编译时确定。因此，LL 函数中的任何循环都应该是有限的；然而，目前也存在一些例外。
- LL 函数并非线程安全，其上层（驱动层）有责任确保不会同时访问寄存器和寄存器字段。


.. _hw-abstraction-hal-layer:

HAL（硬件抽象层）
--------------------------------

HAL 将外设的操作过程建模成一组通用步骤，其中每个步骤都有一个相关联的函数。对于每个步骤，HAL 隐藏（抽象）了外设寄存器的实现细节（即需要设置/读取的寄存器）。通过将外设操作过程建模为一组功能步骤，HAL 可以抽象化（即透明处理）不同目标或芯片版本间的微小硬件实现差异。换句话说，特定外设的 HAL API 在多个目标/芯片版本之间基本保持相同。

以下 HAL 函数示例选自看门狗定时器 (WDT) HAL，每个函数都映射到了 WDT 操作生命周期的某个步骤，从而展示了 HAL 如何将外设的操作抽象为功能步骤。

.. code-block:: c

    // 初始化某个 WDT
    void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr);

    // 配置 WDT 的特定超时阶段
    void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

    // 启动 WDT
    void wdt_hal_enable(wdt_hal_context_t *hal);

    // 喂养（即重置）WDT
    void wdt_hal_feed(wdt_hal_context_t *hal);

    // 处理 WDT 超时
    void wdt_hal_handle_intr(wdt_hal_context_t *hal);

    // 停止 WDT
    void wdt_hal_disable(wdt_hal_context_t *hal);

    // 去初始化 WDT
    void wdt_hal_deinit(wdt_hal_context_t *hal);

.. _hw-abstraction-hal-layer-disable-rtc-wdt:

禁用 RTC_WDT
^^^^^^^^^^^^

.. code-block:: c

    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

.. _hw-abstraction-hal-layer-feed-rtc-wdt:

重置 RTC_WDT 计数器
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_feed(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

HAL 函数通常具有以下特点：

- HAL 函数的第一个参数是 ``xxx_hal_context_t *`` 类型。HAL 上下文类型用于存储信息，这些信息与特定外设实例（即上下文实例）相关。HAL 上下文通过 ``xxx_hal_init()`` 函数初始化，可以存储以下信息：

    - 该实例的通道编号
    - 指向外设（或通道）寄存器的指针（即 ``xxx_dev_t *`` 类型）
    - 进行中的事务的信息（例如使用中的 DMA 描述符列表的指针）
    - 实例的一些配置值（例如通道配置）
    - 维护实例状态信息的变量（例如表明实例是否正在等待事务完成的标志）

- HAL 函数不应包含任何操作系统原语，如队列、信号量、互斥锁等。所有同步/并发操作应在更高层次（如驱动程序）处理。
- 某些外设的某些步骤可能无法由 HAL 进一步抽象，因此最终成为对 LL 函数的直接封装（或宏）。
- 某些 HAL 函数可能会放置在 IRAM 中，因此可能带有 ``IRAM_ATTR`` 或放置在单独的 ``xxx_hal_iram.c`` 源文件中。
