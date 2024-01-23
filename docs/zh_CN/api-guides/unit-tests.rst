{IDF_TARGET_NAME} 中的单元测试
============================

:link_to_translation:`en:[English]`

ESP-IDF 提供以下方法测试软件。

- 一种是基于目标的测试，该测试使用运行在 {IDF_TARGET_PATH_NAME} 上的中央单元测试应用程序。这些测试使用的是基于 `Unity <https://www.throwtheswitch.org/unity>`_ 的单元测试框架。通过把测试用例放在组件的 ``test`` 子目录，可以将其集成到 ESP-IDF 组件中。本文档主要介绍这种基于目标的测试方法。

- 另一种是基于 Linux 主机的单元测试，其中所有硬件行为都通过 Mock 组件进行模拟。此测试方法目前仍在开发中，暂且只有一小部分 ESP-IDF 组件支持 Mock，具体请参考 :doc:`基于 Linux 主机的单元测试 <host-apps>`。

添加常规测试用例
----------------

单元测试被添加在相应组件的 ``test`` 子目录中，测试用例写在 C 文件中，一个 C 文件可以包含多个测试用例。测试文件的名字要以 “test” 开头。

测试文件需要包含 ``unity.h`` 头文件，此外还需要包含待测试 C 模块需要的头文件。

测试用例需要通过 C 文件中特定的函数来添加，如下所示：

.. code-block:: c

   TEST_CASE("test name", "[module name]")
   {
           // 在这里添加测试用例
   }

-  第一个参数是此测试的描述性名称。
-  第二个参数是用方括号括起来的标识符。标识符用来对相关测试或具有特定属性的测试进行分组。

.. note::
    没有必要在每个测试用例中使用 ``UNITY_BEGIN()`` 和 ``UNITY_END()`` 来声明主函数的区域，``unity_platform.c`` 会自动调用 ``UNITY_BEGIN()``，然后运行测试用例，最后调用 ``UNITY_END()``。

``test`` 子目录应包含 :ref:`组件 CMakeLists.txt <component-directories>`，因为他们本身就是一种组件（即测试组件）。ESP-IDF 使用了 Unity 测试框架, 位于 ``unity`` 组件里。因此，每个测试组件都需要通过 ``REQUIRES`` 参数将 ``unity`` 组件设为依赖项。通常，组件需要 :ref:`手动指定待编译的源文件 <cmake-file-globbing>`，但是，对于测试组件来说，这个要求被放宽为仅建议将参数 ``SRC_DIRS`` 用于 ``idf_component_register``。

总的来说，``test`` 子目录下最小的 CMakeLists.txt 文件可能如下所示:

.. code:: cmake

    idf_component_register(SRC_DIRS "."
                           INCLUDE_DIRS "."
                           REQUIRES unity)

更多关于如何在 Unity 下编写测试用例的信息，请查阅 http://www.throwtheswitch.org/unity。


添加多设备测试用例
------------------

常规测试用例会在一个在试设备 (Device Under Test，DUT) 上执行。但是，由于要求互相通信的组件（比如 GPIO、SPI）需要与其他设备进行通信，因此不能使用常规测试用例进行测试。多设备测试用例包括写入多个测试函数，并在多个 DUT 运行测试。

以下是一个多设备测试用例：

.. code-block:: c

    void gpio_master_test()
    {
        gpio_config_t slave_config = {
                .pin_bit_mask = 1 << MASTER_GPIO_PIN,
                .mode = GPIO_MODE_INPUT,
        };
        gpio_config(&slave_config);
        unity_wait_for_signal("output high level");
        TEST_ASSERT(gpio_get_level(MASTER_GPIO_PIN) == 1);
    }

    void gpio_slave_test()
    {
        gpio_config_t master_config = {
                .pin_bit_mask = 1 << SLAVE_GPIO_PIN,
                .mode = GPIO_MODE_OUTPUT,
        };
        gpio_config(&master_config);
        gpio_set_level(SLAVE_GPIO_PIN, 1);
        unity_send_signal("output high level");
    }

    TEST_CASE_MULTIPLE_DEVICES("gpio multiple devices test example", "[driver]", gpio_master_test, gpio_slave_test);

宏 ``TEST_CASE_MULTIPLE_DEVICES`` 用来声明多设备测试用例。

-  第一个参数指定测试用例的名字。
-  第二个参数是测试用例的描述。
-  从第三个参数开始，可以指定最多 5 个测试函数，每个函数都是单独运行在一个 DUT 上的测试入口点。

在不同的 DUT 上运行的测试用例需要相互之间进行同步。可以通过 ``unity_wait_for_signal`` 和 ``unity_send_signal`` 这两个函数使用 UART 进行同步操作。上例的场景中，slave 应该在 master 设置好 GPIO 电平后再去读取 GPIO 电平，DUT 的 UART 终端会打印提示信息，并要求用户进行交互。

DUT1 (master) 终端::

   Waiting for signal: [output high level]!
   Please press "Enter" key once any board send this signal.

DUT2 (slave) 终端::

   Send signal: [output high level]!

一旦 DUT2 发送了该信号，你需要在 DUT1 的终端按回车键，然后 DUT1 会从 ``unity_wait_for_signal`` 函数中解除阻塞，并开始更改 GPIO 的电平。


添加多阶段测试用例
------------------

常规的测试用例无需重启就会结束（或者仅需要检查是否发生了重启），可有些时候我们想在某些特定类型的重启事件后运行指定的测试代码。例如，在深度睡眠唤醒后检查复位的原因是否正确。首先我们需要触发深度睡眠复位事件，然后检查复位的原因。为了实现这一点，可以通过定义多阶段测试用例来将这些测试函数组合在一起::

   static void trigger_deepsleep(void)
   {
       esp_sleep_enable_timer_wakeup(2000);
       esp_deep_sleep_start();
   }

   void check_deepsleep_reset_reason()
   {
       soc_reset_reason_t reason = esp_rom_get_reset_reason(0);
       TEST_ASSERT(reason == RESET_REASON_CORE_DEEP_SLEEP);
   }

   TEST_CASE_MULTIPLE_STAGES("reset reason check for deepsleep", "[{IDF_TARGET_PATH_NAME}]", trigger_deepsleep, check_deepsleep_reset_reason);

多阶段测试用例向用户呈现了一组测试函数，它需要用户进行交互（选择用例并选择不同的阶段）来运行。

应用于不同芯片的单元测试
------------------------

某些测试（尤其与硬件相关的）不支持在所有的芯片上执行。请参照本节，让你的单元测试只在其中一部分芯片上执行。

1. 使用宏 ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` 包装你的测试代码，并将其放于原始的测试文件中，或将代码分成按功能分组的文件。但请确保所有这些文件都会由编译器处理。例::

      #if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266)
      TEST_CASE("a test that is not ready for esp32 and esp8266 yet", "[]")
      {
      }
      #endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266)

如果需要将其中某个测试在特定芯片上编译，只需要修改禁止的芯片列表。推荐使用一些能在 ``soc_caps.h`` 中被清楚描述的通用概念来禁止某些单元测试。如果已经进行上述操作，但一些测试在芯片中的调试暂未通过，请同时使用上述两种方法，当调试完成后再移除 ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` 。例： ::

      #if SOC_SDIO_SLAVE_SUPPORTED
      #if !TEMPORARY_DISABLED_FOR_TARGETS(ESP64)
      TEST_CASE("a sdio slave tests that is not ready for esp64 yet", "[sdio_slave]")
      {
          //available for esp32 now, and will be available for esp64 in the future
      }
      #endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP64)
      #endif //SOC_SDIO_SLAVE_SUPPORTED

2. 对于某些你确定不会支持的测试（例如，芯片根本没有该外设），使用 ``DISABLED_FOR_TARGETS`` 来禁止该测试；对于其他只是临时性需要关闭的（例如，没有 runner 资源等），使用 ``TEMPORARY_DISABLED_FOR_TARGETS`` 来暂时关闭该测试。

一些禁用目标芯片测试用例的旧方法，由于它们具有明显的缺陷，已经被废弃，请勿继续使用：

- 请勿将测试代码放在 ``test/target`` 目录下并用 CMakeLists.txt 来选择其中一个进行编译。这是因为测试代码比实现代码更容易被复用。如果你将一些代码放在 ``test/esp32`` 目录下来避免 esp32s2 芯片执行它，一旦你需要在新的芯片（比如 esp32s3）中启用该测试，这种结构很难保持代码整洁。

- 请勿继续使用 ``CONFIG_IDF_TARGET_xxx`` 宏来禁用测试。这种方法会让被禁用的测试项目难以追踪和重新打开。并且，相比于白名单式的 ``#if CONFIG_IDF_TARGET_xxx``，黑名单式的 ``#if !disabled`` 不会导致在新芯片引入时这些测试被自动禁用。但对于测试实现，仍可使用 ``#if CONFIG_IDF_TARGET_xxx`` 给不同芯片版本选择实现代码。测试项目和测试实现区分如下：

  - 测试项目：那些会在一些芯片上执行，而在另外一些上跳过的项目，例如：

    有三个测试项目 SD 1-bit、SD 4-bit 和 SDSPI。对于不支持 SD Host 外设的 ESP32-S2 芯片，只有 SDSPI 一个项目需要被执行。

  - 测试实现：一些始终会发生的代码，但采取的实现方式不同。例如：

    ESP8266 芯片没有 SDIO_PKT_LEN 寄存器。如果在测试过程中需要从 slave 设备的数据长度，你可以用不同方式读取的 ``#if CONFIG_IDF_TARGET_`` 宏来保护不同的实现代码。

    但请注意避免使用 ``#else`` 宏。这样当新芯片被引入时，测试就会在编译阶段失败，提示维护者去显示选择一个正确的测试实现。

编译单元测试程序
----------------

按照 esp-idf 顶层目录的 README 文件中的说明进行操作，请确保 ``IDF_PATH`` 环境变量已经被设置指向了 esp-idf 的顶层目录。

切换到 ``tools/unit-test-app`` 目录下进行配置和编译：

* ``idf.py menuconfig`` - 配置单元测试程序。
* ``idf.py -T all build`` - 编译单元测试程序，测试每个组件 ``test`` 子目录下的用例。
* ``idf.py -T "xxx yyy" build`` - 编译单元测试程序，对以空格分隔的特定组件进行测试（如 ``idf.py -T heap build`` - 仅对 ``heap`` 组件目录下的单元测试程序进行编译）。
* ``idf.py -T all -E "xxx yyy" build`` - 编译单元测试程序，测试除指定组件之外的所有组件（例如 ``idf.py -T all -E "ulp mbedtls" build`` - 编译所有的单元测试，不包括 ``ulp`` 和 ``mbedtls`` 组件。）。

.. note::

    由于 Windows 命令提示符固有限制，需使用以下语法来编译多个组件的单元测试程序：``idf.py -T xxx -T yyy build`` 或者在 PowerShell 中使用 ``idf.py -T \`"xxx yyy\`" build``，在 Windows 命令提示符中使用 ``idf.py -T \^"ssd1306 hts221\^" build``。

当编译完成时，它会打印出烧写芯片的指令。你只需要运行 ``idf.py flash`` 即可烧写所有编译输出的文件。

你还可以运行 ``idf.py -T all flash`` 或者 ``idf.py -T xxx flash`` 来编译并烧写，所有需要的文件都会在烧写之前自动重新编译。

使用 ``menuconfig`` 可以设置烧写测试程序所使用的串口。更多信息，见 :idf_file:`tools/unit-test-app/README.md`。

运行单元测试
--------------

.. note::

    我们还提供基于 pytest 的框架 `pytest-embedded <https://github.com/espressif/pytest-embedded>`_，以便更方便、高效地运行单元测试。如需在 CI 中运行测试或连续运行多个测试，不妨尝试这一框架。了解更多信息，请查看 `pytest-embedded 文档 <https://docs.espressif.com/projects/pytest-embedded/en/latest/>`_ 和 :doc:`/contribute/esp-idf-tests-with-pytest`。


烧写完成后重启 {IDF_TARGET_NAME}， 它将启动单元测试程序。

当单元测试应用程序空闲时，输入回车键，它会打印出测试菜单，其中包含所有的测试项目::

   Here is the test menu, pick your combo:
   (1)     "esp_ota_begin() verifies arguments" [ota]
   (2)     "esp_ota_get_next_update_partition logic" [ota]
   (3)     "Verify bootloader image in flash" [bootloader_support]
   (4)     "Verify unit test app image" [bootloader_support]
   (5)     "can use new and delete" [cxx]
   (6)     "can call virtual functions" [cxx]
   (7)     "can use static initializers for non-POD types" [cxx]
   (8)     "can use std::vector" [cxx]
   (9)     "static initialization guards work as expected" [cxx]
   (10)    "global initializers run in the correct order" [cxx]
   (11)    "before scheduler has started, static initializers work correctly" [cxx]
   (12)    "adc2 work with wifi" [adc]
   (13)    "gpio master/slave test example" [ignore][misc][test_env=UT_T2_1][multi_device]
           (1)     "gpio_master_test"
           (2)     "gpio_slave_test"
   (14)    "SPI Master clockdiv calculation routines" [spi]
   (15)    "SPI Master test" [spi][ignore]
   (16)    "SPI Master test, interaction of multiple devs" [spi][ignore]
   (17)    "SPI Master no response when switch from host1 (SPI2) to host2 (SPI3)" [spi]
   (18)    "SPI Master DMA test, TX and RX in different regions" [spi]
   (19)    "SPI Master DMA test: length, start, not aligned" [spi]
   (20)    "reset reason check for deepsleep" [{IDF_TARGET_PATH_NAME}][test_env=UT_T2_1][multi_stage]
           (1)     "trigger_deepsleep"
           (2)     "check_deepsleep_reset_reason"

常规测试用例会打印用例名字和描述，主从测试用例还会打印子菜单（已注册的测试函数的名字）。

可以输入以下任意一项来运行测试用例：

-  引号中写入测试用例的名字，运行单个测试用例。
-  测试用例的序号，运行单个测试用例。
-  方括号中的模块名字，运行指定模块所有的测试用例。
-  星号，运行所有测试用例。

``[multi_device]`` 和 ``[multi_stage]`` 标签告诉测试运行者该用例是多设备测试还是多阶段测试。这些标签由 ``TEST_CASE_MULTIPLE_STAGES`` 和 ``TEST_CASE_MULTIPLE_DEVICES`` 宏自动生成。

一旦选择了多设备测试用例，它会打印一个子菜单::

    Running gpio master/slave test example...
    gpio master/slave test example
            (1)     "gpio_master_test"
            (2)     "gpio_slave_test"

你需要输入数字以选择在 DUT 上运行的测试。

与多设备测试用例相似，多阶段测试用例也会打印子菜单::

    Running reset reason check for deepsleep...
    reset reason check for deepsleep
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

第一次执行此用例时，输入 ``1`` 来运行第一阶段（触发深度睡眠）。在重启 DUT 并再次选择运行此用例后，输入 ``2`` 来运行第二阶段。只有在最后一个阶段通过并且之前所有的阶段都成功触发了复位的情况下，该测试才算通过。


.. _cache-compensated-timer:

带缓存补偿定时器的定时代码
-----------------------------------------

存储在外部存储器（如 SPI flash 和 SPI RAM）中的指令和数据是通过 CPU 的统一指令和数据缓存来访问的。当代码或数据在缓存中时，访问速度会非常快（即缓存命中）。

然而，如果指令或数据不在缓存中，则需要从外部存储器中获取（即缓存缺失）。访问外部存储器的速度明显较慢，因为 CPU 在等待从外部存储器获取指令或数据时会陷入停滞，从而导致整体代码执行速度会依据缓存命中或缓存缺失的次数而变化。

在不同的编译中，代码和数据的位置可能会有所不同，一些可能会更有利于缓存访问（即最大限度地减少缓存缺失）。理论上，这会影响执行速度，但这些因素通常无关紧要，因为它们的影响会在设备的运行过程中“平均化”。

然而，高速缓存对执行速度的影响可能与基准测试场景（尤其是微基准测试）有关。每次运行时间和构建时的测量时间可能会有所差异，减少差异的方法之一是将代码和数据分别放在指令或数据 RAM (IRAM/DRAM) 中。CPU 可以直接访问 IRAM 和 DRAM，从而消除了高速缓存的影响因素。然而，由于 IRAM 和 DRAM 容量有限，该方法并不总是可行。

缓存补偿定时器是上述方法的替代方法，该计时器使用处理器的内部事件计数器来确定在发生高速缓存未命中时等待代码/数据所花费的时间，然后从记录的实时时间中减去该时间。

  .. code-block:: c

    // Start the timer
    ccomp_timer_start();

    // Function to time
    func_code_to_time();

    // Stop the timer, and return the elapsed time in microseconds relative to
    // ccomp_timer_start
    int64_t t = ccomp_timer_stop();


缓存补偿定时器的限制之一是基准功能必须固定在一个内核上。这是由于每个内核都有自己的事件计数器，这些事件计数器彼此独立。例如，如果在一个内核上调用 ``ccomp_timer_start``，使调度器进入睡眠状态，唤醒并在在另一个内核上重新调度，那么对应的 ``ccomp_timer_stop`` 将无效。

.. _mocks:

Mocks
----------

.. note::
    目前，只有一些特定的组件在 Linux 主机上运行时才能 Mock。未来我们计划，无论是在 Linux 主机上运行还是在目标芯片 {IDF_TARGET_NAME} 上运行，ESP-IDF 所有重要的组件都可以实现 Mock。

嵌入式系统中单元测试的最大问题之一是对硬件依赖性极强。直接在 {IDF_TARGET_NAME} 上运行单元测试对于上层组件来说存在极大的困难，原因如下：

- 受下层组件和/或硬件设置的影响，测试可靠性降低。
- 由于下层组件和/或硬件设置的限制，测试边缘案例的难度提高。
- 由于数量庞大的依赖关系影响了行为，识别根本原因的难度提高。

当测试一个特定的组件（即被测组件）时，通过软件进行 Mock 能让所有被测组件的依赖在软件中被完全替换（即 Mock）。为了实现该功能，ESP-IDF 集成了 `CMock <https://www.throwtheswitch.org/cmock>`_ 的 Mock 框架作为组件。通过在 ESP-IDF 的构建系统中添加一些 CMake 函数，可以方便地 Mock 整个（或部分）ESP-IDF 组件。

理想情况下，被测组件所依赖的所有组件都应该被 Mock，从而让测试环境完全控制与被测组件之间的所有交互。然而，如果 Mock 所有的组件过于复杂或冗长（例如需要模拟过多的函数调用），以下做法可能会有帮助：

.. list::
    - 在测试代码中包含更多“真正”（非模拟）代码。这样做可能有效，但同时也会增加对“真正”代码行为的依赖。此外，一旦测试失败，很难判断失败原因是因为实际测试代码还是“真正”的 ESP-IDF 代码。
    - 重新评估被测代码的设计，尝试将被测代码划分为更易于管理的组件来减少其依赖性。这可能看起来很麻烦，但众所周知，单元测试经常暴露软件设计的弱点。修复设计上的弱点不仅在短期内有助于进行单元测试，而且还有助于长期的代码维护。

请参考 :component_file:`cmock/CMock/docs/CMock_Summary.md` 了解 CMock 工作原理以及如何创建和使用 Mock。

要求
^^^^^^^^^^^^
目前 Mock 只支持基于 Linux 主机的单元测试。生成 Mock 需要满足如下要求：

.. include:: inc/linux-host-requirements.rst


对组件进行 Mock
^^^^^^^^^^^^^^^^

如果 ESP-IDF 中已对组件进行 Mock（也称为 *组件模拟*），那么只要满足要求，该版本即可立即投入使用。已进行 Mock 的组件列表，可参考 :ref:`component-linux-mock-support`。具体组件模拟的使用方法，请参考 :ref:`adjustments_for_mocks`。

如果 ESP-IDF 尚未提供任何组件模拟，则需要创建组件的 Mock 版本，以特定方式覆盖组件。覆盖组件时，需要创建一个与原始组件名称完全相同的组件，让构建系统先发现原始组件，再发现这个具有相同名称的新组件。具体可参考 :ref:`同名组件 <cmake-components-same-name>`。

在组件模拟中需要指定如下部分：

.. list::
     - 头文件，头文件中提供了需要生成模拟的函数
     - 上述头文件的路径
     - 模拟组件的依赖（如果头文件中包含了其他组件的文件，那么这点非常必要）

以上这些部分都需要使用 ESP-IDF 构建系统函数 ``idf_component_mock`` 指定。你可以使用 ESP-IDF 构建系统函数 ``idf_component_get_property``，并加上标签 ``COMPONENT_OVERRIDEN_DIR`` 来访问原始组件的组件目录，然后使用 ``idf_component_mock`` 注册模拟组件。

.. code:: none

    idf_component_get_property(original_component_dir <original-component-name> COMPONENT_OVERRIDEN_DIR)
    ...
    idf_component_mock(INCLUDE_DIRS "${original_component_dir}/include"
        REQUIRES freertos
        MOCK_HEADER_FILES ${original_component_dir}/include/header_containing_functions_to_mock.h)

组件模拟还需要一个单独的 ``mock`` 目录，里面包含一个 ``mock_config.yaml`` 文件用于配置 CMock。以下是一份简单的 ``mock_config.yaml`` 文件：

  .. code-block:: yaml

    :cmock:
      :plugins:
        - expect
        - expect_any_args

更多关于 CMock yaml 类型配置文件的详细信息，请查看 :component_file:`cmock/CMock/docs/CMock_Summary.md`。

请注意，组件模拟不一定要对原始组件进行整体模拟。只要组件模拟满足测试项目的依赖以及其他代码对原始组件的依赖，部分模拟就足够了。事实上，ESP-IDF 中 ``tools/mocks`` 中的大多数组件模拟都只是部分地模拟了原始组件。

可在 ESP-IDF 目录的 :idf:`tools/mocks` 下找到组件模拟的示例。有关如何 *覆盖 ESP-IDF 组件*，可查看 :ref:`同名组件 <cmake-components-same-name>`。

- :component_file:`NVS 页面类的单元测试 <nvs_flash/host_test/nvs_page_test/README.md>`。
- :component_file:`esp_event 的单元测试 <esp_event/host_test/esp_event_unit_test/main/esp_event_test.cpp>`。
- :component_file:`mqtt 的单元测试 <mqtt/esp-mqtt/host_test/README.md>`。

.. _adjustments_for_mocks:

修改单元测试文件
^^^^^^^^^^^^^^^^^^^^^^^^

单元测试需要通知 cmake 构建系统对依赖的组件进行模拟（即用模拟组件来覆盖原始组件）。这可以通过将组件模拟放到项目的 ``components`` 目录，或者在项目的根目录 ``CMakeLists.txt`` 文件中使用以下代码来添加模拟组件的目录来实现：

  .. code:: cmake

    list(APPEND EXTRA_COMPONENT_DIRS "<mock_component_dir>")

这两种方法都会让组件模拟覆盖 ESP-IDF 中的现有组件。如果你使用的是 ESP-IDF 提供的组件模拟，则第二个方法更加方便。

可参考 ``esp_event`` 基于主机的单元测试及其 :component_file:`esp_event/host_test/esp_event_unit_test/CMakeLists.txt` 作为组件模拟的示例。
