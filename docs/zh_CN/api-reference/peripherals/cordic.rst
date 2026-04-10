=============================
坐标旋转数字计算方法 (CORDIC)
=============================

:link_to_translation:`en:[English]`

.. contents::
    :local:
    :depth: 2

概述
======

CORDIC 是一种坐标旋转数字计算方法，用于计算三角函数、双曲函数和反正切函数等数学运算。CORDIC 算法通过迭代旋转和缩放操作，将复杂的数学运算转换为简单的加法和移位操作，从而实现高效的计算。

CORDIC 主要通过硬件进行计算，其特点主要包括：

- 计算速度快
- 计算精度高
- 计算资源消耗低

下表对比了使用 CORDIC 驱动和软件 iqmath 库计算 1 个随机值的余弦值的性能，单位 CPU 周期。

+------------------+----------------+----------------+
| 函数             | iqmath         |   cordic       |
+==================+================+================+
| cos              | 29640          | 8080           |
+------------------+----------------+----------------+

驱动介绍
==============

在使用 CORDIC 驱动之前，需要先了解一些关于该驱动的计算方法及参数含义。

计算方法
----------------

CORDIC 驱动所计算的函数通过 :cpp:member:`cordic_calculate_config_t::function` 参数进行选择，其支持的函数如下表所示。其中，有部分函数需要两个参数，有部分函数需要一个参数。有部分函数可以生成两个结果，有部分函数只能生成一个结果。

此外，CORDIC 驱动还可以通过设置 :cpp:member:`cordic_calculate_config_t::scale_exp` 参数来设置输入数据的缩放因子（有效缩放为 :math:`2^n`），在下面表格中用 :math:`n` 表示该参数的值。

:math:`arg_{1}` 和 :math:`arg_{2}` 表示输入数据，分别对应 :cpp:member:`cordic_input_buffer_desc_t::p_data_arg1` 和 :cpp:member:`cordic_input_buffer_desc_t::p_data_arg2` ，:math:`res_{1}` 和 :math:`res_{2}` 表示输出数据，分别对应 :cpp:member:`cordic_output_buffer_desc_t::p_data_res1` 和 :cpp:member:`cordic_output_buffer_desc_t::p_data_res2`。其对应关系如下表所示。

.. list-table:: CORDIC 支持函数及公式
   :header-rows: 1
   :widths: 18 22 32 11 24
   :stub-columns: 0

   * - 函数
     - 输入
     - 输出
     - 可缩放指数 :math:`n`
     - 输入输出范围（缩放为1时）
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_COS` 余弦
     - :math:`\frac{\theta}{\pi}, \quad \theta \text{ in radians}`
     - :math:`res_1 = \cos(\theta)`, :math:`res_2 = \sin(\theta)`
     - 0
     - :math:`arg_1, res_1, res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SIN` 正弦
     - :math:`\frac{\theta}{\pi}, \quad \theta \text{ in radians}`
     - :math:`res_1 = \sin(\theta)`，:math:`res_2 = \cos(\theta)`
     - 0
     - :math:`arg_1, res_1, res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_PHASE` 相位
     - :math:`arg_1 = x \cdot 2^{-n}`，:math:`arg_2 = y \cdot 2^{-n}`
     - :math:`res_1 = \arctan(y/x) \cdot 2^{-n}/\pi`，:math:`res_2 = \sqrt{x^2+y^2} \cdot 2^{-n}`
     - [0, 15]
     - :math:`arg_{1,2} \in \left[-1, 1\right)`，:math:`res_1 \in \left[-1, 1\right)`，:math:`res_2 \in \left[0, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_MODULUS` 模长
     - :math:`arg_1 = x \cdot 2^{-n}`，:math:`arg_2 = y \cdot 2^{-n}`
     - :math:`res_1 = \sqrt{x^2+y^2} \cdot 2^{-n}`，:math:`res_2 = \arctan(y/x) \cdot 2^{-n}/\pi`
     - [0, 15]
     - :math:`arg_{1,2} \in \left[-1, 1\right)`，:math:`res_1 \in \left[0, 1\right)`，:math:`res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_ARCTAN` 反正切
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \arctan(x) \cdot 2^{-n}/\pi`
     - [0, 15]
     - :math:`arg_1, res_1 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_COSH` 双曲余弦
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \cosh(x) \cdot 2^{-n}`，:math:`res_2 = \sinh(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.559, 0.559\right]`，:math:`res_1 \in \left[0.5, 0.846\right]`，:math:`res_2 \in \left[-0.683, 0.683\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SINH` 双曲正弦
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \sinh(x) \cdot 2^{-n}`，:math:`res_2 = \cosh(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.559, 0.559\right]`，:math:`res_1 \in \left[-0.683, 0.683\right]`，:math:`res_2 \in \left[0.5, 0.846\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_ARCHTANH` 反双曲正切
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \mathrm{arctanh}(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.403, 0.403\right]`，:math:`res_1 \in \left[-0.559, 0.559\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_LOGE` 自然对数
     - :math:`arg_1 = (x-1)/(x+1) \cdot 2^{-1}`
     - :math:`res_1 = \ln(x) \cdot 2^{-2}`
     - 1
     - :math:`arg_1 \in \left[-0.403, 0.403\right]`，:math:`res_1 \in \left[-0.559, 0.559\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SQUARE_ROOT` 平方根
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \sqrt{x} \cdot 2^{-n}`
     - [0, 4]
     - :math:`arg_1 \in \left[0.1069, 1\right)`，:math:`res_1 \in \left[0.177, 1\right)`

数据格式
----------------

CORDIC 驱动 通过 :cpp:member:`cordic_calculate_config_t::iq_format` 参数来选择输入和输出数据的格式，其支持的格式如下所示。

- :cpp:enumerator:`ESP_CORDIC_FORMAT_Q15` Q15 格式

在 Q15 格式下，数据由一个符号位加15位小数位组成，其范围为 [-1(0x8000), 1 - 2^-15(0x7FFF)]。

.. figure:: ../../../_static/diagrams/cordic/cordic_q15.svg
    :align: center
    :alt: Q15 格式

    Q15 格式

.. math::

    value = \sum_{i=0}^{14} \frac{1}{2^{(15-i)}} \times B_i

- :cpp:enumerator:`ESP_CORDIC_FORMAT_Q31` Q31 格式

在 Q31 格式下，数据由一个符号位加31位小数位组成，其范围为 [-1(0x80000000), 1 - 2^-31(0x7FFFFFFF)]。

.. figure:: ../../../_static/diagrams/cordic/cordic_q31.svg
    :align: center
    :alt: Q31 格式

    Q31 格式

.. math::

    value = \sum_{i=0}^{30} \frac{1}{2^{(31-i)}} \times B_i

精度
----------------

CORDIC 驱动通过 :cpp:member:`cordic_calculate_config_t::iteration_count` 参数来设置计算的迭代周期（精度），其支持的精度如下所示。

**Phase, Modulus, arctan 函数**

+------------------+----------------+----------------+--------------------------+--------------------------+
| 函数             | 迭代次数       | 周期数         | 最大残差误差 (q1.31格式) | 最大残差误差 (q1.15格式) |
+==================+================+================+==========================+==========================+
| Phase, Modulus,  | 4              | 1              | :math:`2^{-4}`           | :math:`2^{-4}`           |
| arctan           +----------------+----------------+--------------------------+--------------------------+
|                  | 8              | 2              | :math:`2^{-8}`           | :math:`2^{-8}`           |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 12             | 3              | :math:`2^{-12}`          | :math:`2^{-12}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 16             | 4              | :math:`2^{-16}`          | :math:`2^{-15}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 20             | 5              | :math:`2^{-20}`          | :math:`2^{-15}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 24             | 6              | :math:`2^{-24}`          | :math:`2^{-15}`          |
+------------------+----------------+----------------+--------------------------+--------------------------+

**sin, cos, sinh, cosh, arctanh, ln 函数**

+------------------+----------------+----------------+--------------------------+--------------------------+
| 函数             | 迭代次数       | 周期数         | 最大残差误差 (q1.31格式) | 最大残差误差 (q1.15格式) |
+==================+================+================+==========================+==========================+
| sin, cos,        | 4              | 1              | :math:`2^{-3}`           | :math:`2^{-3}`           |
| sinh, cosh,      +----------------+----------------+--------------------------+--------------------------+
| arctanh, ln      | 8              | 2              | :math:`2^{-7}`           | :math:`2^{-7}`           |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 12             | 3              | :math:`2^{-11}`          | :math:`2^{-11}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 16             | 4              | :math:`2^{-14}`          | :math:`2^{-14}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 20             | 5              | :math:`2^{-18}`          | :math:`2^{-15}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 24             | 6              | :math:`2^{-22}`          | :math:`2^{-15}`          |
+------------------+----------------+----------------+--------------------------+--------------------------+

**Sqrt 函数**

+------------------+----------------+----------------+--------------------------+--------------------------+
| 函数             | 迭代次数       | 周期数         | 最大残差误差 (q1.31格式) | 最大残差误差 (q1.15格式) |
+==================+================+================+==========================+==========================+
| Sqrt             | 4              | 1              | :math:`2^{-7}`           | :math:`2^{-7}`           |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 8              | 2              | :math:`2^{-14}`          | :math:`2^{-14}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 12             | 3              | :math:`2^{-22}`          | :math:`2^{-15}`          |
+------------------+----------------+----------------+--------------------------+--------------------------+

.. note::

   迭代周期越多，计算精度越高，但计算时间也会越长。当运算精度到达数据格式所能表达的精度极限时，继续增加迭代周期将不会产生更好的效果，反而会使时间变长。

快速入门
==========

本节将带你快速了解如何使用 CORDIC 驱动。通过实际的使用场景，展示如何创建 CORDIC 引擎并进行计算。一般的使用流程如下：

创建 CORDIC 引擎
------------------

首先，需要创建一个 CORDIC 引擎。以下代码展示了如何创建一个 CORDIC 引擎：

.. code:: c

    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,  // 选择时钟源
    };
    ESP_ERROR_CHECK(cordic_new_engine(&config, &engine));

使用 CORDIC 引擎进行计算
----------------------------

使用 CORDIC 引擎进行计算的流程如下：

.. code:: c

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,           // 选择计算的函数
        .iq_format = ESP_CORDIC_FORMAT_Q15,       // 选择输入和输出数据的格式
        .iteration_count = 4,                     // 选择计算的迭代周期（精度）
        .scale_exp = 0,                           // 输入缩放指数 n，有效缩放为 2^n
    };

    size_t buffer_depth = 60;

    uint32_t data_x[buffer_depth] = {};  // 输入数据1，长度为60
    uint32_t data_y[buffer_depth] = {};  // 输入数据2，长度为60

    uint32_t res1[buffer_depth] = {};  // 输出数据1，长度为60
    uint32_t res2[buffer_depth] = {};  // 输出数据2，长度为60

    // 配置输入数据
    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,   // 输入数据1
        .p_data_arg2 = data_y,   // 输入数据2
    };

    // 配置输出数据
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,   // 输出数据1
        .p_data_res2 = res2,   // 输出数据2
    };

    // 开始计算
    ESP_ERROR_CHECK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, buffer_depth));

格式转换
-----------

CORDIC 驱动中提供了定点数与浮点数之间的转换函数，用户可以通过 :cpp:func:`cordic_convert_fixed_to_float` 和 :cpp:func:`cordic_convert_float_to_fixed` 函数实现定点数与浮点数之间的转换。

.. code:: c

    // 将定点数转换为浮点数
    // res1[0] res2[0] 为 cordic 输出的定点数
    float res1_float = cordic_convert_fixed_to_float(res1[0], ESP_CORDIC_FORMAT_Q15);
    float res2_float = cordic_convert_fixed_to_float(res2[0], ESP_CORDIC_FORMAT_Q15);

    // 将浮点数转换为定点数
    uint32_t res1_fixed = cordic_convert_float_to_fixed(res1_float, ESP_CORDIC_FORMAT_Q15);
    uint32_t res2_fixed = cordic_convert_float_to_fixed(res2_float, ESP_CORDIC_FORMAT_Q15);

资源回收
---------

当不再需要之前安装的 CORDIC 引擎，请调用 :cpp:func:`cordic_delete_engine` 来回收资源，以释放底层硬件。

.. code:: c

    ESP_ERROR_CHECK(cordic_delete_engine(engine));

高级功能
========

线程安全
---------

CORDIC 驱动程序的以下函数是线程安全的，可以从不同的 RTOS 任务调用，无需额外的锁保护：

工厂函数：

- :cpp:func:`cordic_new_engine`
- :cpp:func:`cordic_delete_engine`

.. note::

    请注意 :cpp:func:`cordic_calculate_polling` 函数是非线程安全的，在没有设置互斥锁保护的任务中，应避免从多个任务中调用此函数。

Cache 安全
-----------

:cpp:func:`cordic_calculate_polling` 函数可以安全地在 ISR 中使用，但为了保证 ISR 在 cache 被禁用时也能正常运行，请启用 Kconfig 选项 `CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM`，将控制函数放入 IRAM 中。

Kconfig 选项
---------------

以下 Kconfig 选项可用于配置 CORDIC 驱动：

- :ref:`CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM` - 计算函数放入 IRAM。

API 参考
============

CORDIC 驱动 API
----------------

.. include-build-file:: inc/cordic.inc

CORDIC HAL API
----------------

.. include-build-file:: inc/components/esp_hal_cordic/include/hal/cordic_types.inc

CORDIC 类型
----------------

.. include-build-file:: inc/components/esp_driver_cordic/include/driver/cordic_types.inc
