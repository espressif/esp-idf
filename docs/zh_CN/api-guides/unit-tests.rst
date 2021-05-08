{IDF_TARGET_NAME} 中的单元测试
============================
:link_to_translation:`en:[English]`

ESP-IDF 中附带了一个基于 ``Unity`` 的单元测试应用程序框架，且所有的单元测试用例分别保存在 ESP-IDF 仓库中每个组件的 ``test`` 子目录中。

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
-  第二个参数用方括号中的标识符来表示，标识符用来对相关测试或具有特定属性的测试进行分组。

.. note::
    没有必要在每个测试用例中使用 ``UNITY_BEGIN()`` 和 ``UNITY_END()`` 来声明主函数的区域， ``unity_platform.c`` 会自动调用 ``UNITY_BEGIN()``， 然后运行测试用例，最后调用 ``UNITY_END()``。

``test`` 子目录应包含 :ref: `组件 CMakeLists.txt <component-directories>`，因为他们本身就是一种组件。ESP-IDF 使用了 ``unity`` 测试框架，需要将其指定为组件的依赖项。通常，组件 :ref: `需要手动指定待编译的源文件 <cmake-file-globbing>；但是，对于测试组件来说，这个要求被放宽为仅建议将参数 ``SRC_DIRS`` 用于 ``idf_component_register``。

总的来说，``test`` 子目录下最小的 CMakeLists.txt 文件可能如下所示:

.. code:: cmake

    idf_component_register(SRC_DIRS "."
                           INCLUDE_DIRS "."
                           REQUIRES unity)

更多关于如何在 Unity 下编写测试用例的信息，请查阅 http://www.throwtheswitch.org/unity。


添加多设备测试用例
------------------

常规测试用例会在一个 DUT（Device Under Test，在试设备）上执行。但是，那些需要互相通信的组件（比如 GPIO、SPI）需要其他设备与其通信，因此不能使用常规测试用例进行测试。多设备测试用例包括写入多个测试函数，并在多个 DUT 进行运行测试。

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

宏 ``TEST_CASE_MULTIPLE_DEVICES`` 用来声明多设备测试用例，

-  第一个参数指定测试用例的名字。
-  第二个参数是测试用例的描述。
-  从第三个参数开始，可以指定最多 5 个测试函数，每个函数都是单独运行在一个 DUT 上的测试入口点。

在不同的 DUT 上运行的测试用例，通常会要求它们之间进行同步。我们提供 ``unity_wait_for_signal`` 和 ``unity_send_signal`` 这两个函数来使用 UART 去支持同步操作。如上例中的场景，slave 应该在 master 设置好 GPIO 电平后再去读取 GPIO 电平，DUT 的 UART 终端会打印提示信息，并要求用户进行交互。

DUT1（master）终端::

   Waiting for signal: [output high level]!
   Please press "Enter" key once any board send this signal.

DUT2（slave）终端::

   Send signal: [output high level]!

一旦 DUT2 发送了该信号，您需要在 DUT1 的终端按回车键，然后 DUT1 会从 ``unity_wait_for_signal`` 函数中解除阻塞，并开始更改 GPIO 的电平。


添加多阶段测试用例
------------------

常规的测试用例无需重启就会结束（或者仅需要检查是否发生了重启），可有些时候我们想在某些特定类型的重启事件后运行指定的测试代码，例如，我们想在深度睡眠唤醒后检查复位的原因是否正确。首先我们需要触发深度睡眠复位事件，然后检查复位的原因。为了实现这一点，我们可以定义多阶段测试用例来将这些测试函数组合在一起::

   static void trigger_deepsleep(void)
   {
       esp_sleep_enable_timer_wakeup(2000);
       esp_deep_sleep_start();
   }

   void check_deepsleep_reset_reason()
   {
       RESET_REASON reason = rtc_get_reset_reason(0);
       TEST_ASSERT(reason == DEEPSLEEP_RESET);
   }

   TEST_CASE_MULTIPLE_STAGES("reset reason check for deepsleep", "[{IDF_TARGET_PATH_NAME}]", trigger_deepsleep, check_deepsleep_reset_reason);

多阶段测试用例向用户呈现了一组测试函数，它需要用户进行交互（选择用例并选择不同的阶段）来运行。

应用于不同芯片的单元测试
------------------------

某些测试（尤其与硬件相关的）无法在所有的芯片上执行。请参照本节让你的单元测试只在其中一部分芯片上执行。

1. 使用宏 ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` 包装你的测试代码，并将其放于原始的测试文件中，或将代码分成按功能分组的文件。但请确保所有这些文件都会由编译器处理。例::

      #if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266) 
      TEST_CASE("a test that is not ready for esp32 and esp8266 yet", "[]")
      {
      }
      #endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266)

一旦你需要其中一个测试在某个芯片上被编译，只需要修改禁止的芯片列表。我们更鼓励使用一些通用的概念（能在 ``soc_caps.h`` 中被清楚描述）来禁止某些单元测试。如果你已经这样做，但有一些测试还没有在新的芯片版本中被调试通过，请同时使用上述两种方法，当调试完成后再移除 ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` 。例： ::

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

- 请勿将测试代码放在 ``test/芯片版本`` 目录下面，然后用 CMakeLists.txt 来选择其中一个进行编译。这是因为测试代码比实现代码更容易被复用。如果你将一些代码放在 ``test/esp32`` 目录下来避免 esp32s2 芯片执行它，一旦你需要在新的芯片（比如 esp32s3 ）中启用该测试，你会发现这种结构非常难以保持代码的整洁。

- 请勿继续使用 ``CONFIG_IDF_TARGET_xxx`` 宏来禁止某些测试在一些芯片上编译。这种 方法会让被禁止的测试项目难以追踪和重新打开。并且，相比于白名单式的 ``#if CONFIG_IDF_TARGET_xxx`` ，黑名单式的 ``#if !disabled`` 能避免新芯片引入时，这些测试被自动关闭。但对于用于测试的一些实现， ``#if CONFIG_IDF_TARGET_xxx`` 仍可用于给不同芯片版本选择实现代码。测试项目和测试实现区分如下：

  - 测试项目：某些你会在一些芯片上执行，而在另外一些上跳过的项目，例如：

    有三个测试项目 SD 1-bit 、 SD 4-bit 和 SDSPI 。对于不支持 SD Host 外设的ESP32-S2 芯片，只有 SDSPI 一个项目需要被执行。

  - 测试实现：某些代码始终会发生，但采取不同的做法。例如：

    ESP8266 芯片没有 SDIO_PKT_LEN 寄存器。如果在测试过程中需要获取从设备的数据长度，你可以用不同方式读取的 ``#if CONFIG_IDF_TARGET_`` 宏来保护不同的实现代码。

    但请注意避免使用 ``#else`` 宏。这样当新芯片被引入时，测试就会在编译阶段失败，提示维护者去显示选择一个正确的测试实现。

编译单元测试程序
----------------

按照 esp-idf 顶层目录的 README 文件中的说明进行操作，请确保 ``IDF_PATH`` 环境变量已经被设置指向了 esp-idf 的顶层目录。

切换到 ``tools/unit-test-app`` 目录下进行配置和编译：

* ``idf.py menuconfig`` - 配置单元测试程序。
* ``idf.py -T all build`` - 编译单元测试程序，测试每个组件 ``test`` 子目录下的用例。
* ``idf.py -T "xxx yyy" build`` - 编译单元测试程序，测试指定的组件。（如 ``idf.py -T heap build`` - 仅对 ``heap`` 组件目录下的单元测试程序进行编译）
* ``idf.py -T all -E "xxx yyy" build`` - 编译单元测试程序，测试除指定组件之外的所有组件。（例如 ``idf.py -T all -E "ulp mbedtls" build`` - 编译所有的单元测试，不包括 ``ulp`` 和 ``mbedtls`` 组件。）

当编译完成时，它会打印出烧写芯片的指令。您只需要运行 ``idf.py flash`` 即可烧写所有编译输出的文件。

您还可以运行 ``idf.py -T all flash`` 或者 ``idf.py -T xxx flash`` 来编译并烧写，所有需要的文件都会在烧写之前自动重新编译。

使用 ``menuconfig`` 可以设置烧写测试程序所使用的串口。

运行单元测试
--------------

烧写完成后重启 {IDF_TARGET_NAME}， 它将启动单元测试程序。

当单元测试应用程序空闲时，输入回车键，它会打印出测试菜单，其中包含所有的测试项目::

    Here's the test menu, pick your combo:
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
    (17)    "SPI Master no response when switch from host1 (HSPI) to host2 (VSPI)" [spi]
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

``[multi_device]`` 和 ``[multi_stage]``标签告诉测试运行者该用例是多设备测试还是多阶段测试。这些标签由 ``TEST_CASE_MULTIPLE_STAGES`` 和 ``TEST_CASE_MULTIPLE_DEVICES`` 宏自动生成。

一旦选择了多设备测试用例，它会打印一个子菜单::

    Running gpio master/slave test example...
    gpio master/slave test example
            (1)     "gpio_master_test"
            (2)     "gpio_slave_test"

您需要输入数字以选择在 DUT 上运行的测试。

与多设备测试用例相似，多阶段测试用例也会打印子菜单::

    Running reset reason check for deepsleep...
    reset reason check for deepsleep
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

第一次执行此用例时，输入 ``1`` 来运行第一阶段（触发深度睡眠）。在重启 DUT 并再次选择运行此用例后，输入 ``2`` 来运行第二阶段。只有在最后一个阶段通过并且之前所有的阶段都成功触发了复位的情况下，该测试才算通过。


带缓存补偿定时器的定时代码
-----------------------------------------

存储在外部存储器（如 SPI Flash 和 SPI RAM）中的指令和数据是通过 CPU 的统一指令和数据缓存来访问的。当代码或数据在缓存中时，访问速度会非常快（即缓存命中）。

然而，如果指令或数据不在缓存中，则需要从外部内存中获取（即缓存缺失）。访问外部存储器的速度明显较慢，因为 CPU 在等待从外部存储器获取指令或数据时会陷入停滞。这导致整体代码执行速度会依据缓存命中或缓存缺失的次数而变化。

在不同的编译中，代码和数据的位置可能会有所不同，一些可能会更有利于缓存访问（即，最大限度地减少缓存缺失）。理论上说这会影响执行速度，但这些因素通常却是无关紧要，因为它们的影响会在设备的运行过程中“平均化”。

然而，高速缓存对执行速度的影响可能与基准测试场景（尤其是微基准测试）有关。每次运行和构建时的测量时间可能会有所差异，消除部分差异的方法之一是将代码和数据分别放在指令或数据 RAM（IRAM/DRAM）中。CPU 可以直接访问 IRAM 和 DRAM，从而消除了高速缓存的影响因素。然而，由于 IRAM 和 DRAM 容量有限，该方法并不总是可行。

缓存补偿定时器是将要基准测试的代码/数据放置在 IRAM/DRAM 中的替代方法，该计时器使用处理器的内部事件计数器来确定在发生高速缓存未命中时等待代码/数据所花费的时间，然后从记录的实时时间中减去该时间。

  .. code-block:: c

    // Start the timer
    ccomp_timer_start();

    // Function to time
    func_code_to_time();

    // Stop the timer, and return the elapsed time in microseconds relative to
    // ccomp_timer_start
    int64_t t = ccomp_timer_stop();


缓存补偿定时器的限制之一是基准功能必须固定在一个内核上。这是由于每个内核都有自己的事件计数器，这些事件计数器彼此独立。例如，如果在一个内核上调用 ``ccomp_timer_start``，使调度器进入睡眠状态，唤醒并在在另一个内核上重新调度，那么对应的 ``ccomp_timer_stop`` 将无效。