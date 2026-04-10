=============================================
Coordinate Rotation Digital Computer (CORDIC)
=============================================

:link_to_translation:`zh_CN:[中文]`

.. contents::
   :local:
   :depth: 2

Overview
========

CORDIC (Coordinate Rotation Digital Computer) is an iterative method for computing trigonometric, hyperbolic, and arctangent functions. The algorithm converts these operations into simple additions and shifts through iterative rotations and scaling, enabling efficient hardware-based computation.

CORDIC is primarily implemented in hardware with the following characteristics:

- High computation speed
- High precision
- Lower resource usage

The table below compares the performance of computing the cosine of a random value using a CORDIC driver and the software IQmath library, measured in CPU cycles.

+------------------+----------------+----------------+
| Function         | IQmath         |   CORDIC       |
+==================+================+================+
| cos              | 29640          | 8080           |
+------------------+----------------+----------------+

Driver Introduction
===================

Before using the CORDIC driver, you should understand its calculation methods and parameter meanings.

Calculation Methods
-------------------

The CORDIC function is selected via :cpp:member:`cordic_calculate_config_t::function`. Supported functions are listed in the table below. Some functions take two arguments, others one; some produce two results, others one.

The input scaling factor can be set with :cpp:member:`cordic_calculate_config_t::scale_exp` (effective scaling is :math:`2^n`). In the table below, :math:`n` denotes this parameter.

:math:`arg_{1}` and :math:`arg_{2}` are the inputs, corresponding to :cpp:member:`cordic_input_buffer_desc_t::p_data_arg1` and :cpp:member:`cordic_input_buffer_desc_t::p_data_arg2`. :math:`res_{1}` and :math:`res_{2}` are the outputs, corresponding to :cpp:member:`cordic_output_buffer_desc_t::p_data_res1` and :cpp:member:`cordic_output_buffer_desc_t::p_data_res2`. The mapping is summarized in the following table.

.. list-table:: CORDIC Supported Functions and Formulas
   :header-rows: 1
   :widths: 18 22 32 11 24
   :stub-columns: 0

   * - Function
     - Input
     - Output
     - Scale exponent :math:`n`
     - Input/Output range (scale = 1)
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_COS` Cosine
     - :math:`\frac{\theta}{\pi}, \quad \theta \text{ in radians}`
     - :math:`res_1 = \cos(\theta)`, :math:`res_2 = \sin(\theta)`
     - 0
     - :math:`arg_1, res_1, res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SIN` Sine
     - :math:`\frac{\theta}{\pi}, \quad \theta \text{ in radians}`
     - :math:`res_1 = \sin(\theta)`, :math:`res_2 = \cos(\theta)`
     - 0
     - :math:`arg_1, res_1, res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_PHASE` Phase
     - :math:`arg_1 = x \cdot 2^{-n}`, :math:`arg_2 = y \cdot 2^{-n}`
     - :math:`res_1 = \arctan(y/x) \cdot 2^{-n}/\pi`, :math:`res_2 = \sqrt{x^2+y^2} \cdot 2^{-n}`
     - [0, 15]
     - :math:`arg_{1,2} \in \left[-1, 1\right)`, :math:`res_1 \in \left[-1, 1\right)`, :math:`res_2 \in \left[0, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_MODULUS` Modulus
     - :math:`arg_1 = x \cdot 2^{-n}`, :math:`arg_2 = y \cdot 2^{-n}`
     - :math:`res_1 = \sqrt{x^2+y^2} \cdot 2^{-n}`, :math:`res_2 = \arctan(y/x) \cdot 2^{-n}/\pi`
     - [0, 15]
     - :math:`arg_{1,2} \in \left[-1, 1\right)`, :math:`res_1 \in \left[0, 1\right)`, :math:`res_2 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_ARCTAN` Arctangent
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \arctan(x) \cdot 2^{-n}/\pi`
     - [0, 15]
     - :math:`arg_1, res_1 \in \left[-1, 1\right)`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_COSH` Hyperbolic cosine
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \cosh(x) \cdot 2^{-n}`, :math:`res_2 = \sinh(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.559, 0.559\right]`, :math:`res_1 \in \left[0.5, 0.846\right]`, :math:`res_2 \in \left[-0.683, 0.683\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SINH` Hyperbolic sine
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \sinh(x) \cdot 2^{-n}`, :math:`res_2 = \cosh(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.559, 0.559\right]`, :math:`res_1 \in \left[-0.683, 0.683\right]`, :math:`res_2 \in \left[0.5, 0.846\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_ARCHTANH` Inverse hyperbolic tangent
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \mathrm{arctanh}(x) \cdot 2^{-n}`
     - 1
     - :math:`arg_1 \in \left[-0.403, 0.403\right]`, :math:`res_1 \in \left[-0.559, 0.559\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_LOGE` Natural logarithm
     - :math:`arg_1 = (x-1)/(x+1) \cdot 2^{-1}`
     - :math:`res_1 = \ln(x) \cdot 2^{-2}`
     - 1
     - :math:`arg_1 \in \left[-0.403, 0.403\right]`, :math:`res_1 \in \left[-0.559, 0.559\right]`
   * - :cpp:enumerator:`ESP_CORDIC_FUNC_SQUARE_ROOT` Square root
     - :math:`arg_1 = x \cdot 2^{-n}`
     - :math:`res_1 = \sqrt{x} \cdot 2^{-n}`
     - [0, 4]
     - :math:`arg_1 \in \left[0.1069, 1\right)`, :math:`res_1 \in \left[0.177, 1\right)`

Data Format
-----------

Input and output data format is selected via :cpp:member:`cordic_calculate_config_t::iq_format`. Supported formats:

- :cpp:enumerator:`ESP_CORDIC_FORMAT_Q15` Q15 format

  In Q15 format, data has one sign bit plus 15 fractional bits. Range: [-1 (0x8000), 1 - 2^-15 (0x7FFF)].

.. figure:: ../../../_static/diagrams/cordic/cordic_q15.svg
   :align: center
   :alt: Q15 format

   Q15 format

.. math::

   value = \sum_{i=0}^{14} \frac{1}{2^{(15-i)}} \times B_i

- :cpp:enumerator:`ESP_CORDIC_FORMAT_Q31` Q31 format

  In Q31 format, data has one sign bit plus 31 fractional bits. Range: [-1 (0x80000000), 1 - 2^-31 (0x7FFFFFFF)].

.. figure:: ../../../_static/diagrams/cordic/cordic_q31.svg
   :align: center
   :alt: Q31 format

   Q31 format

.. math::

   value = \sum_{i=0}^{30} \frac{1}{2^{(31-i)}} \times B_i

Precision
---------

The number of iterations (precision) is set via :cpp:member:`cordic_calculate_config_t::iteration_count`. Supported values are given below.

**Phase, Modulus, Arctan functions**

+------------------+----------------+----------------+--------------------------+--------------------------+
| Function         | Iterations     | Cycles         | Max residual (q1.31)     | Max residual (q1.15)     |
+==================+================+================+==========================+==========================+
| Phase, Modulus,  | 4              | 1              | :math:`2^{-4}`           | :math:`2^{-4}`           |
| Arctan           +----------------+----------------+--------------------------+--------------------------+
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

**Sin, Cos, Sinh, Cosh, Arctanh, Ln functions**

+------------------+----------------+----------------+--------------------------+--------------------------+
| Function         | Iterations     | Cycles         | Max residual (q1.31)     | Max residual (q1.15)     |
+==================+================+================+==========================+==========================+
| Sin, Cos,        | 4              | 1              | :math:`2^{-3}`           | :math:`2^{-3}`           |
| Sinh, Cosh,      +----------------+----------------+--------------------------+--------------------------+
| Arctanh, Ln      | 8              | 2              | :math:`2^{-7}`           | :math:`2^{-7}`           |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 12             | 3              | :math:`2^{-11}`          | :math:`2^{-11}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 16             | 4              | :math:`2^{-14}`          | :math:`2^{-14}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 20             | 5              | :math:`2^{-18}`          | :math:`2^{-15}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 24             | 6              | :math:`2^{-22}`          | :math:`2^{-15}`          |
+------------------+----------------+----------------+--------------------------+--------------------------+

**Sqrt function**

+------------------+----------------+----------------+--------------------------+--------------------------+
| Function         | Iterations     | Cycles         | Max residual (q1.31)     | Max residual (q1.15)     |
+==================+================+================+==========================+==========================+
| Sqrt             | 4              | 1              | :math:`2^{-7}`           | :math:`2^{-7}`           |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 8              | 2              | :math:`2^{-14}`          | :math:`2^{-14}`          |
|                  +----------------+----------------+--------------------------+--------------------------+
|                  | 12             | 3              | :math:`2^{-22}`          | :math:`2^{-15}`          |
+------------------+----------------+----------------+--------------------------+--------------------------+

.. note::

   More iterations give higher precision but longer computation time. Once precision reaches the limit of the data format, further increasing iterations does not improve results and only increases latency.

Quick Start
===========

This section shows how to create a CORDIC engine and run calculations. Typical usage flow:

Create CORDIC Engine
--------------------

Create a CORDIC engine as follows:

.. code:: c

   cordic_engine_handle_t engine = NULL;
   cordic_engine_config_t config = {
       .clock_source = CORDIC_CLK_SRC_DEFAULT,  // Select clock source
   };
   ESP_ERROR_CHECK(cordic_new_engine(&config, &engine));

Run CORDIC Calculation
-----------------------

Use the engine to perform a calculation:

.. code:: c

   cordic_calculate_config_t calc_config = {
       .function = ESP_CORDIC_FUNC_COS,           // Select function
       .iq_format = ESP_CORDIC_FORMAT_Q15,        // Input/output data format
       .iteration_count = 4,                     // Iteration count (precision)
       .scale_exp = 0,                            // Input scale exponent n (effective scale 2^n)
   };

   size_t buffer_depth = 60;

   uint32_t data_x[buffer_depth] = {};  // Input 1, length 60
   uint32_t data_y[buffer_depth] = {};  // Input 2, length 60

   uint32_t res1[buffer_depth] = {};  // Output 1, length 60
   uint32_t res2[buffer_depth] = {};  // Output 2, length 60

   // Configure input
   cordic_input_buffer_desc_t input_buffer = {
       .p_data_arg1 = data_x,   // Input 1
       .p_data_arg2 = data_y,   // Input 2
   };

   // Configure output
   cordic_output_buffer_desc_t output_buffer = {
       .p_data_res1 = res1,   // Output 1
       .p_data_res2 = res2,   // Output 2
   };

   // Run calculation
   ESP_ERROR_CHECK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, buffer_depth));

Format Conversion
-----------------

The driver provides conversion between fixed-point and floating-point via :cpp:func:`cordic_convert_fixed_to_float` and :cpp:func:`cordic_convert_float_to_fixed`.

.. code:: c

   // Fixed-point to float
   // res1[0], res2[0] are CORDIC fixed-point outputs
   float res1_float = cordic_convert_fixed_to_float(res1[0], ESP_CORDIC_FORMAT_Q15);
   float res2_float = cordic_convert_fixed_to_float(res2[0], ESP_CORDIC_FORMAT_Q15);

   // Float to fixed-point
   uint32_t res1_fixed = cordic_convert_float_to_fixed(res1_float, ESP_CORDIC_FORMAT_Q15);
   uint32_t res2_fixed = cordic_convert_float_to_fixed(res2_float, ESP_CORDIC_FORMAT_Q15);

Resource Cleanup
----------------

When the CORDIC engine is no longer needed, call :cpp:func:`cordic_delete_engine` to release the underlying hardware.

.. code:: c

   ESP_ERROR_CHECK(cordic_delete_engine(engine));

Advanced
========

Thread Safety
-------------

The following CORDIC APIs are thread-safe and can be called from different RTOS tasks without extra locking:

Factory functions:

- :cpp:func:`cordic_new_engine`
- :cpp:func:`cordic_delete_engine`

.. note::

   :cpp:func:`cordic_calculate_polling` is **not** thread-safe. Do not call it from multiple tasks without mutex protection.

Cache Safety
------------

:cpp:func:`cordic_calculate_polling` can be used safely in an ISR. For correct behavior when cache is disabled (e.g., in cache-safe ISRs), enable the Kconfig option `CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM` to place the control functions in IRAM.

Kconfig Options
---------------

The following Kconfig option configures the CORDIC driver:

- :ref:`CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM` - Place calculation control functions in IRAM.

API Reference
=============

CORDIC Driver API
-----------------

.. include-build-file:: inc/cordic.inc

CORDIC HAL API
--------------

.. include-build-file:: inc/components/esp_hal_cordic/include/hal/cordic_types.inc

CORDIC Types
------------

.. include-build-file:: inc/components/esp_driver_cordic/include/driver/cordic_types.inc
