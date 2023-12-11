Unit Testing in {IDF_TARGET_NAME}
=================================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF provides the following methods to test software.

- Target based tests using a central unit test application which runs on the {IDF_TARGET_PATH_NAME}. These tests use the `Unity <https://www.throwtheswitch.org/unity>`_ unit test framework. They can be integrated into an ESP-IDF component by placing them in the component's ``test`` subdirectory. This document mainly introduces this target based tests.

- Linux-host based unit tests in which part of the hardware can be abstracted via mocks. Currently, Linux-host based tests are still under development and only a small fraction of IDF components support them. More information on running IDF applications on the host can be found here: :doc:`Running Applications on the Host Machine <host-apps>`.

Normal Test Cases
-----------------

Unit tests are located in the ``test`` subdirectory of a component. Tests are written in C, and a single C source file can contain multiple test cases. Test files start with the word "test".

Each test file should include the ``unity.h`` header and the header for the C module to be tested.

Tests are added in a function in the C file as follows:

.. code-block:: c

    TEST_CASE("test name", "[module name]")
    {
            // Add test here
    }

- The first argument is a descriptive name for the test.
- The second argument is an identifier in square brackets. Identifiers are used to group related test, or tests with specific properties.

.. note::
    There is no need to add a main function with ``UNITY_BEGIN()`` and ``​UNITY_END()`` in each test case. ``unity_platform.c`` will run ``UNITY_BEGIN()`` autonomously, and run the test cases, then call ``​UNITY_END()``.

The ``test`` subdirectory should contain a :ref:`component CMakeLists.txt <component-directories>`, since they are themselves components (i.e., a test component). ESP-IDF uses the Unity test framework located in the ``unity`` component. Thus, each test component should specify the ``unity`` component as a component requirement using the ``REQUIRES`` argument. Normally, components :ref:`should list their sources manually <cmake-file-globbing>`; for component tests however, this requirement is relaxed and the use of the ``SRC_DIRS`` argument in ``idf_component_register`` is advised.

Overall, the minimal ``test`` subdirectory ``CMakeLists.txt`` file should contain the following:

.. code:: cmake

    idf_component_register(SRC_DIRS "."
                           INCLUDE_DIRS "."
                           REQUIRES unity)

See http://www.throwtheswitch.org/unity for more information about writing tests in Unity.


Multi-device Test Cases
-----------------------

The normal test cases will be executed on one DUT (Device Under Test). However, components that require some form of communication (e.g., GPIO, SPI) require another device to communicate with, thus cannot be tested through normal test cases. Multi-device test cases involve writing multiple test functions, and running them on multiple DUTs.

The following is an example of a multi-device test case:

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

The macro ``TEST_CASE_MULTIPLE_DEVICES`` is used to declare a multi-device test case.

- The first argument is test case name.
- The second argument is test case description.
- From the third argument, up to 5 test functions can be defined, each function will be the entry point of tests running on each DUT.

Running test cases from different DUTs could require synchronizing between DUTs. We provide ``unity_wait_for_signal`` and ``unity_send_signal`` to support synchronizing with UART. As the scenario in the above example, the slave should get GPIO level after master set level. DUT UART console will prompt and user interaction is required:

DUT1 (master) console::

    Waiting for signal: [output high level]!
    Please press "Enter" key to once any board send this signal.

DUT2 (slave) console::

    Send signal: [output high level]!

Once the signal is sent from DUT2, you need to press "Enter" on DUT1, then DUT1 unblocks from ``unity_wait_for_signal`` and starts to change GPIO level.


Multi-stage Test Cases
----------------------

The normal test cases are expected to finish without reset (or only need to check if reset happens). Sometimes we expect to run some specific tests after certain kinds of reset. For example, we want to test if the reset reason is correct after a wake up from deep sleep. We need to create a deep-sleep reset first and then check the reset reason. To support this, we can define multi-stage test cases, to group a set of test functions::

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

Multi-stage test cases present a group of test functions to users. It needs user interactions (select cases and select different stages) to run the case.

Tests For Different Targets
---------------------------

Some tests (especially those related to hardware) cannot run on all targets. Below is a guide how to make your unit tests run on only specified targets.

1. Wrap your test code by ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` macros and place them either in the original test file, or separate the code into files grouped by functions, but make sure all these files will be processed by the compiler. E.g.::

      #if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266)
      TEST_CASE("a test that is not ready for esp32 and esp8266 yet", "[]")
      {
      }
      #endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32, ESP8266)

Once you need one of the tests to be compiled on a specified target, just modify the targets in the disabled list. It's more encouraged to use some general conception that can be described in ``soc_caps.h`` to control the disabling of tests. If this is done but some of the tests are not ready yet, use both of them (and remove ``!(TEMPORARY_)DISABLED_FOR_TARGETS()`` later). E.g.: ::

      #if SOC_SDIO_SLAVE_SUPPORTED
      #if !TEMPORARY_DISABLED_FOR_TARGETS(ESP64)
      TEST_CASE("a sdio slave tests that is not ready for esp64 yet", "[sdio_slave]")
      {
          //available for esp32 now, and will be available for esp64 in the future
      }
      #endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP64)
      #endif //SOC_SDIO_SLAVE_SUPPORTED

2. For test code that you are 100% for sure that will not be supported (e.g., no peripheral at all), use ``DISABLED_FOR_TARGETS``; for test code that should be disabled temporarily, or due to lack of runners, etc., use ``TEMPORARY_DISABLED_FOR_TARGETS``.

Some old ways of disabling unit tests for targets, that have obvious disadvantages, are deprecated:

- DON'T put the test code under ``test/target`` folder and use CMakeLists.txt to choose one of the target folder. This is prevented because test code is more likely to be reused than the implementations. If you put something into ``test/esp32`` just to avoid building it on esp32s2, it's hard to make the code tidy if you want to enable the test again on esp32s3.

- DON'T use ``CONFIG_IDF_TARGET_xxx`` macros to disable the test items any more. This makes it harder to track disabled tests and enable them again. Also, a black-list style ``#if !disabled`` is preferred to white-list style ``#if CONFIG_IDF_TARGET_xxx``, since you will not silently disable cases when new targets are added in the future. But for test implementations, it's allowed to use ``#if CONFIG_IDF_TARGET_xxx`` to pick one of the implementation code.

  - Test item: some items that will be performed on some targets, but skipped on other targets. E.g.

    There are three test items SD 1-bit, SD 4-bit and SDSPI. For ESP32-S2, which doesn't have SD host, among the tests only SDSPI is enabled on ESP32-S2.

  - Test implementation: some code will always happen, but in different ways. E.g.

    There is no SDIO PKT_LEN register on ESP8266. If you want to get the length from the slave as a step in the test process, you can have different implementation code protected by ``#if CONFIG_IDF_TARGET_`` reading in different ways.

    But please avoid using ``#else`` macro. When new target is added, the test case will fail at building stage, so that the maintainer will be aware of this, and choose one of the implementations explicitly.

Building Unit Test App
----------------------

Follow the setup instructions in the top-level esp-idf README. Make sure that ``IDF_PATH`` environment variable is set to point to the path of esp-idf top-level directory.

Change into ``tools/unit-test-app`` directory to configure and build it:

* ``idf.py menuconfig`` - configure unit test app.
* ``idf.py -T all build`` - build unit test app with tests for each component having tests in the ``test`` subdirectory.
* ``idf.py -T "xxx yyy" build`` - build unit test app with tests for some space-separated specific components (For instance: ``idf.py -T heap build`` - build unit tests only for ``heap`` component directory).
* ``idf.py -T all -E "xxx yyy" build`` - build unit test app with all unit tests, except for unit tests of some components (For instance: ``idf.py -T all -E "ulp mbedtls" build`` - build all unit tests excludes ``ulp`` and ``mbedtls`` components).

.. note::

    Due to inherent limitations of Windows command prompt, following syntax has to be used in order to build unit-test-app with multiple components: ``idf.py -T xxx -T yyy build`` or with escaped quotes: ``idf.py -T \`"xxx yyy\`" build`` in PowerShell or ``idf.py -T \^"ssd1306 hts221\^" build`` in Windows command prompt.

When the build finishes, it will print instructions for flashing the chip. You can simply run ``idf.py flash`` to flash all build output.

You can also run ``idf.py -T all flash`` or ``idf.py -T xxx flash`` to build and flash. Everything needed will be rebuilt automatically before flashing.

Use menuconfig to set the serial port for flashing. For more information, see :idf_file:`tools/unit-test-app/README.md`.

Running Unit Tests
------------------

.. note::

    We also provide the pytest-based framework `pytest-embedded <https://github.com/espressif/pytest-embedded>`_ to help make running unit-tests more convenient and efficient. If you need to run tests in CI or run multiple tests in a row we recommend checking out this project. For more information see `Pytest-embedded Docs <https://docs.espressif.com/projects/pytest-embedded/en/latest/>`_ and :doc:`/contribute/esp-idf-tests-with-pytest`.


After flashing reset the {IDF_TARGET_NAME} and it will boot the unit test app.

When unit test app is idle, press "Enter" will make it print test menu with all available tests::

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
    (17)    "SPI Master no response when switch from host1 (SPI2) to host2 (SPI3)" [spi]
    (18)    "SPI Master DMA test, TX and RX in different regions" [spi]
    (19)    "SPI Master DMA test: length, start, not aligned" [spi]
    (20)    "reset reason check for deepsleep" [{IDF_TARGET_PATH_NAME}][test_env=UT_T2_1][multi_stage]
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

The normal case will print the case name and description. Master-slave cases will also print the sub-menu (the registered test function names).

Test cases can be run by inputting one of the following:

- Test case name in quotation marks to run a single test case
- Test case index to run a single test case
- Module name in square brackets to run all test cases for a specific module
- An asterisk to run all test cases

``[multi_device]`` and ``[multi_stage]`` tags tell the test runner whether a test case is a multiple devices or multiple stages of test case. These tags are automatically added by ```TEST_CASE_MULTIPLE_STAGES`` and ``TEST_CASE_MULTIPLE_DEVICES`` macros.

After you select a multi-device test case, it will print sub-menu::

    Running gpio master/slave test example...
    gpio master/slave test example
            (1)     "gpio_master_test"
            (2)     "gpio_slave_test"

You need to input a number to select the test running on the DUT.

Similar to multi-device test cases, multi-stage test cases will also print sub-menu::

    Running reset reason check for deepsleep...
    reset reason check for deepsleep
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

First time you execute this case, input ``1`` to run first stage (trigger deepsleep). After DUT is rebooted and able to run test cases, select this case again and input ``2`` to run the second stage. The case only passes if the last stage passes and all previous stages trigger reset.


.. _cache-compensated-timer:

Timing Code with Cache Compensated Timer
----------------------------------------

Instructions and data stored in external memory (e.g., SPI Flash and SPI RAM) are accessed through the CPU's unified instruction and data cache. When code or data is in cache, access is very fast (i.e., a cache hit).

However, if the instruction or data is not in cache, it needs to be fetched from external memory (i.e., a cache miss). Access to external memory is significantly slower, as the CPU must execute stall cycles whilst waiting for the instruction or data to be retrieved from external memory. This can cause the overall code execution speed to vary depending on the number of cache hits or misses.

Code and data placements can vary between builds, and some arrangements may be more favorable with regards to cache access (i.e., minimizing cache misses). This can technically affect execution speed, however these factors are usually irrelevant as their effect 'average out' over the device's operation.

The effect of the cache on execution speed, however, can be relevant in benchmarking scenarios (especially micro benchmarks). There might be some variability in measured time between runs and between different builds. A technique for eliminating for some of the variability is to place code and data in instruction or data RAM (IRAM/DRAM), respectively. The CPU can access IRAM and DRAM directly, eliminating the cache out of the equation. However, this might not always be viable as the size of IRAM and DRAM is limited.

The cache compensated timer is an alternative to placing the code/data to be benchmarked in IRAM/DRAM. This timer uses the processor's internal event counters in order to determine the amount of time spent on waiting for code/data in case of a cache miss, then subtract that from the recorded wall time.

  .. code-block:: c

    // Start the timer
    ccomp_timer_start();

    // Function to time
    func_code_to_time();

    // Stop the timer, and return the elapsed time in microseconds relative to
    // ccomp_timer_start
    int64_t t = ccomp_timer_stop();


One limitation of the cache compensated timer is that the task that benchmarked functions should be pinned to a core. This is due to each core having its own event counters that are independent of each other. For example, if ``ccomp_timer_start`` gets called on one core, put to sleep by the scheduler, wakes up, and gets rescheduled on the other core, then the corresponding ``ccomp_timer_stop`` will be invalid.

.. _mocks:

Mocks
-----

.. note::
    Currently, mocking is only possible with some selected components when running on the Linux host. In the future, we plan to make essential components in IDF mock-able. This will also include mocking when running on the {IDF_TARGET_NAME}.

One of the biggest problems regarding unit testing on embedded systems are the strong hardware dependencies. Running unit tests directly on the {IDF_TARGET_NAME} can be especially difficult for higher layer components for the following reasons:

- Decreased test reliability due to lower layer components and/or hardware setup.
- Increased difficulty in testing edge cases due to limitations of lower layer components and/or hardware setup
- Increased difficulty in identifying the root cause due to the large number of dependencies influencing the behavior

When testing a particular component, (i.e., the component under test), mocking allows the dependencies of the component under test to be substituted (i.e., mocked) entirely in software. Through mocking, hardware details are emulated and specified at run time, but only if necessary. To allow mocking, ESP-IDF integrates the `CMock <https://www.throwtheswitch.org/cmock>`_ mocking framework as a component. With the addition of some CMake functions in the ESP-IDF build system, it is possible to conveniently mock the entirety (or a part) of an IDF component.

Ideally, all components that the component under test is dependent on should be mocked, thus allowing the test environment complete control over all interactions with the component under test. However, if mocking all dependent components becomes too complex or too tedious (e.g., because you need to mock too many function calls) you have the following options:

.. list::
    - Include more "real" IDF code in the tests. This may work but increases the dependency on the "real" code's behavior. Furthermore, once a test fails, you may not know if the failure is in your actual code under test or the "real" IDF code.
    - Re-evaluate the design of the code under test and attempt to reduce its dependencies by dividing the code under test into more manageable components. This may seem burdensome but it is quite common that unit tests expose software design weaknesses. Fixing design weaknesses will not only help with unit testing in the short term, but will help future code maintenance as well.

Refer to :component_file:`cmock/CMock/docs/CMock_Summary.md` for more details on how CMock works and how to create and use mocks.

Requirements
^^^^^^^^^^^^

Mocking with CMock requires ``Ruby`` on the host machine. Furthermore, since mocking currently only works on the Linux target, the requirements of the latter also need to be fulfilled:

.. include:: inc/linux-host-requirements.rst

Mock a Component
^^^^^^^^^^^^^^^^

If a mocked component, called a *component mock*, is already available in ESP-IDF, then it can be used right away as long as it satisfies the required functionality. Refer to :ref:`component-linux-mock-support` to see which components are mocked already. Then refer to :ref:`adjustments_for_mocks` in order to use the component mock.

It is necessary to create component mocks if they are not yet provided in ESP-IDF. To create a component mock, the component needs to be overwritten in a particular way. Overriding a component entails creating a component with the exact same name as the original component, then letting the build system discover it later than the original component (see :ref:`Multiple components with the same name <cmake-components-same-name>` for more details).

In the component mock, the following parts are specified:

.. list::
    - The headers providing the functions to generate mocks for
    - Include paths of the aforementioned headers
    - Dependencies of the mock component (this is necessary e.g. if the headers include files from other components)

All these parts have to be specified using the IDF build system function ``idf_component_mock``. You can use the IDF build system function ``idf_component_get_property`` with the tag ``COMPONENT_OVERRIDEN_DIR`` to access the component directory of the original component and then register the mock component parts using ``idf_component_mock``:

.. code:: none

    idf_component_get_property(original_component_dir <original-component-name> COMPONENT_OVERRIDEN_DIR)
    ...
    idf_component_mock(INCLUDE_DIRS "${original_component_dir}/include"
        REQUIRES freertos
        MOCK_HEADER_FILES ${original_component_dir}/include/header_containing_functions_to_mock.h)

The component mock also requires a separate ``mock`` directory containing a ``mock_config.yaml`` file that configures CMock. A simple ``mock_config.yaml`` could look like this:

  .. code-block:: yaml

    :cmock:
      :plugins:
        - expect
        - expect_any_args

For more details about the CMock configuration yaml file, have a look at :component_file:`cmock/CMock/docs/CMock_Summary.md`.

Note that the component mock does not have to mock the original component in its entirety. As long as the test project's dependencies and dependencies of other code to the original components are satisfied by the component mock, partial mocking is adequate. In fact, most of the component mocks in IDF in ``tools/mocks`` are only partially mocking the original component.

Examples of component mocks can be found under :idf:`tools/mocks` in the IDF directory. General information on how to *override an IDF component* can be found in :ref:`Multiple components with the same name <cmake-components-same-name>`. There are several examples for testing code while mocking dependencies with CMock (non-exhaustive list):

- :component_file:`unit test for the NVS Page class <nvs_flash/host_test/nvs_page_test/README.md>`.
- :component_file:`unit test for esp_event <esp_event/host_test/esp_event_unit_test/main/esp_event_test.cpp>`.
- :component_file:`unit test for mqtt <mqtt/esp-mqtt/host_test/README.md>`.

.. _adjustments_for_mocks:

Adjustments in Unit Test
^^^^^^^^^^^^^^^^^^^^^^^^

The unit test needs to inform the cmake build system to mock dependent components (i.e., it needs to override the original component with the mock component). This is done by either placing the component mock into the project's ``components`` directory or adding the mock component's directory using the following line in the project's root ``CMakeLists.txt``:

  .. code:: cmake

    list(APPEND EXTRA_COMPONENT_DIRS "<mock_component_dir>")

Both methods will override existing components in ESP-IDF with the component mock. The latter is particularly convenient if you use component mocks that are already supplied by IDF.

Users can refer to the ``esp_event`` host-based unit test and its :component_file:`esp_event/host_test/esp_event_unit_test/CMakeLists.txt` as an example of a component mock.
