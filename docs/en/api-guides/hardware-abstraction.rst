Hardware Abstraction
====================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF provides a group of APIs for hardware abstraction. These APIs allow you to control peripherals at different levels of abstraction, giving you more flexibility compared to using only the ESP-IDF drivers to interact with hardware. ESP-IDF Hardware abstraction is likely to be useful for writing high-performance bare-metal drivers, or for attempting to port an ESP chip to another platform.

This guide is split into the following sections:

    1. :ref:`hw-abstraction-architecture`
    2. :ref:`hw-abstraction-ll-layer`
    3. :ref:`hw-abstraction-hal-layer`

.. warning::

    Hardware abstraction API (excluding the driver and ``xxx_types.h``) should be considered an experimental feature, thus cannot be considered public API. The hardware abstraction API does not adhere to the API name changing restrictions of ESP-IDF's versioning scheme. In other words, it is possible that Hardware Abstraction API may change in between non-major release versions.

.. note::

    Although this document mainly focuses on hardware abstraction of peripherals, e.g., UART, SPI, I2C, certain layers of hardware abstraction extend to other aspects of hardware as well, e.g., some of the CPU's features are partially abstracted.

.. _hw-abstraction-architecture:

Architecture
------------

Hardware abstraction in ESP-IDF is comprised of the following layers, ordered from low level of abstraction that is closer to hardware, to high level of abstraction that is further away from hardware.

- Low Level (LL) Layer
- Hardware Abstraction Layer (HAL)
- Driver Layers

The LL Layer, and HAL are entirely contained within the ``hal`` component. Each layer is dependent on the layer below it, i.e, driver depends on HAL, HAL depends on LL, LL depends on the register header files.

For a particular peripheral ``xxx``, its hardware abstraction generally consists of the header files described in the table below. Files that are **Target Specific** have a separate implementation for each target, i.e., a separate copy for each chip. However, the ``#include`` directive is still target-independent, i.e., is the same for different targets, as the build system automatically includes the correct version of the header and source files.

.. |br| raw:: html

    <br>

.. list-table:: Hardware Abstraction Header Files
    :widths: 25 5 70
    :header-rows: 1

    * - Include |br| Directive
      - Target |br| Specific
      - Description
    * - ``#include 'soc/xxx_caps.h"``
      - Y
      - This header contains a list of C macros specifying the various capabilities of the {IDF_TARGET_NAME}'s peripheral ``xxx``. Hardware capabilities of a peripheral include things such as the number of channels, DMA support, hardware FIFO/buffer lengths, etc.
    * - ``#include "soc/xxx_struct.h"`` |br| ``#include "soc/xxx_reg.h"``
      - Y
      - The two headers contain a representation of a peripheral's registers in C structure and C macro format respectively, allowing you to operate a peripheral at the register level via either of these two header files.
    * - ``#include "soc/xxx_pins.h"``
      - Y
      - If certain signals of a peripheral are mapped to a particular pin of the {IDF_TARGET_NAME}, their mappings are defined in this header as C macros.
    * - ``#include "soc/xxx_periph.h"``
      - N
      - This header is mainly used as a convenience header file to automatically include ``xxx_caps.h``, ``xxx_struct.h``, and ``xxx_reg.h``.
    * - ``#include "hal/xxx_types.h``
      - N
      - This header contains type definitions and macros that are shared among the LL, HAL, and driver layers. Moreover, it is considered public API thus can be included by the application level. The shared types and definitions usually related to non-implementation specific concepts such as the following:

          - Protocol-related types/macros such a frames, modes, common bus speeds, etc.
          - Features/characteristics of an ``xxx`` peripheral that are likely to be present on any implementation (implementation-independent) such as channels, operating modes, signal amplification or attenuation intensities, etc.
    * - ``#include "hal/xxx_ll.h"``
      - Y
      - This header contains the Low Level (LL) Layer of hardware abstraction. LL Layer API are primarily used to abstract away register operations into readable functions.
    * - ``#include "hal/xxx_hal.h"``
      - Y
      - The Hardware Abstraction Layer (HAL) is used to abstract away peripheral operation steps into functions (e.g., reading a buffer, starting a transmission, handling an event, etc). The HAL is built on top of the LL Layer.
    * - ``#include "driver/xxx.h"``
      - N
      - The driver layer is the highest level of ESP-IDF's hardware abstraction. Driver layer API are meant to be called from ESP-IDF applications, and internally utilize OS primitives. Thus, driver layer API are event-driven, and can used in a multi-threaded environment.


.. _hw-abstraction-ll-layer:

LL (Low Level) Layer
--------------------

The primary purpose of the LL Layer is to abstract away register field access into more easily understandable functions. LL functions essentially translate various in/out arguments into the register fields of a peripheral in the form of get/set functions. All the necessary bit shifting, masking, offsetting, and endianness of the register fields should be handled by the LL functions.

.. code-block:: c

    //Inside xxx_ll.h

    static inline void xxx_ll_set_baud_rate(xxx_dev_t *hw,
                                            xxx_ll_clk_src_t clock_source,
                                            uint32_t baud_rate) {
        uint32_t src_clk_freq = (source_clk == XXX_SCLK_APB) ? APB_CLK_FREQ : REF_CLK_FREQ;
        uint32_t clock_divider = src_clk_freq / baud;
        // Set clock select field
        hw->clk_div_reg.divider = clock_divider >> 4;
        // Set clock divider field
        hw->config.clk_sel = (source_clk == XXX_SCLK_APB) ? 0 : 1;
    }

    static inline uint32_t xxx_ll_get_rx_byte_count(xxx_dev_t *hw) {
        return hw->status_reg.rx_cnt;
    }

The code snippet above illustrates typical LL functions for a peripheral ``xxx``. LL functions typically have the following characteristics:

- All LL functions are defined as ``static inline`` so that there is minimal overhead when calling these functions due to compiler optimization. These functions are not guaranteed to be inlined by the compiler, so any LL function that is called when the cache is disabled (e.g., from an IRAM ISR context) should be marked with ``__attribute__((always_inline))``.
- The first argument should be a pointer to a ``xxx_dev_t`` type. The ``xxx_dev_t`` type is a structure representing the peripheral's registers, thus the first argument is always a pointer to the starting address of the peripheral's registers. Note that in some cases where the peripheral has multiple channels with identical register layouts, ``xxx_dev_t *hw`` may point to the registers of a particular channel instead.
- LL functions should be short, and in most cases are deterministic. In other words, in the worst case, runtime of the LL function can be determined at compile time. Thus, any loops in LL functions should be finite bounded; however, there are currently a few exceptions to this rule.
- LL functions are not thread-safe, it is the responsibility of the upper layers (driver layer) to ensure that registers or register fields are not accessed concurrently.


.. _hw-abstraction-hal-layer:

HAL (Hardware Abstraction Layer)
--------------------------------

The HAL layer models the operational process of a peripheral as a set of general steps, where each step has an associated function. For each step, the details of a peripheral's register implementation (i.e., which registers need to be set/read) are hidden (abstracted away) by the HAL. By modeling peripheral operation as a set of functional steps, any minor hardware implementation differences of the peripheral between different targets or chip versions can be abstracted away by the HAL (i.e., handled transparently). In other words, the HAL API for a particular peripheral remains mostly the same across multiple targets/chip versions.

The following HAL function examples are selected from the Watchdog Timer HAL as each function maps to one of the steps in a WDT's operation life cycle, thus illustrating how a HAL abstracts a peripheral's operation into functional steps.

.. code-block:: c

    // Initialize one of the WDTs
    void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr);

    // Configure a particular timeout stage of the WDT
    void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

    // Start the WDT
    void wdt_hal_enable(wdt_hal_context_t *hal);

    // Feed (i.e., reset) the WDT
    void wdt_hal_feed(wdt_hal_context_t *hal);

    // Handle a WDT timeout
    void wdt_hal_handle_intr(wdt_hal_context_t *hal);

    // Stop the WDT
    void wdt_hal_disable(wdt_hal_context_t *hal);

    // De-initialize the WDT
    void wdt_hal_deinit(wdt_hal_context_t *hal);

.. _hw-abstraction-hal-layer-disable-rtc-wdt:

To Disable RTC_WDT
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

.. _hw-abstraction-hal-layer-feed-rtc-wdt:

To Reset the RTC_WDT Counter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_feed(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

HAL functions generally have the following characteristics:

- The first argument to a HAL function has the ``xxx_hal_context_t *`` type. The HAL context type is used to store information about a particular instance of the peripheral (i.e., the context instance). A HAL context is initialized by the ``xxx_hal_init()`` function and can store information such as the following:

    - The channel number of this instance
    - Pointer to the peripheral's (or channel's) registers  (i.e., a ``xxx_dev_t *`` type)
    - Information about an ongoing transaction (e.g., pointer to DMA descriptor list in use)
    - Some configuration values for the instance (e.g., channel configurations)
    - Variables to maintain state information regarding the instance (e.g., a flag to indicate if the instance is waiting for transaction to complete)

- HAL functions should not contain any OS primitives such as queues, semaphores, mutexes, etc. All synchronization/concurrency should be handled at higher layers (e.g., the driver).
- Some peripherals may have steps that cannot be further abstracted by the HAL, thus end up being a direct wrapper (or macro) for an LL function.
- Some HAL functions may be placed in IRAM thus may carry an ``IRAM_ATTR`` or be placed in a separate ``xxx_hal_iram.c`` source file.
