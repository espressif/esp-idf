Unit Testing (Legacy GNU Make)
==============================
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst

ESP-IDF comes with a unit test application that is based on the Unity - unit test framework. Unit tests are integrated in the ESP-IDF repository and are placed in the ``test`` subdirectories of each component respectively.

Normal Test Cases
------------------

Unit tests are located in the ``test`` subdirectory of a component.
Tests are added in C files, a single C file can include multiple test cases.
Test files start with the word "test".

Each test file should include the ``unity.h`` header and the header for the C module to be tested.

Tests are added in a function in the C file as follows:

.. code-block:: c

    TEST_CASE("test name", "[module name]"
    {
            // Add test here
    }

The first argument is a descriptive name for the test, the second argument is an identifier in square brackets.
Identifiers are used to group related test, or tests with specific properties.

.. note::
    There is no need to add a main function with ``UNITY_BEGIN()`` and ``​UNITY_END()`` in each test case. ``unity_platform.c`` will run ``UNITY_BEGIN()`` autonomously, and run the test cases, then call ``​UNITY_END()``.

Each ``test`` subdirectory needs to include a ``component.mk`` file with the following line of code::

    COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

See http://www.throwtheswitch.org/unity for more information about writing tests in Unity.


Multi-device Test Cases
------------------------

The normal test cases will be executed on one DUT (Device Under Test). However, components that require some form of communication (e.g., GPIO, SPI) require another device to communicate with, thus cannot be tested normal test cases.
Multi-device test cases involve writing multiple test functions, and running them on multiple DUTs.

The following is an example of a Multi-device test case:

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
The first argument is test case name, the second argument is test case description.
From the third argument, up to 5 test functions can be defined, each function will be the entry point of tests running on each DUT.

Running test cases from different DUTs could require synchronizing between DUTs. We provide ``unity_wait_for_signal`` and ``unity_send_signal`` to support synchronizing with UART.
As the scenario in the above example, the slave should get GPIO level after master set level. DUT UART console will prompt and requires user interaction:

DUT1 (master) console::

    Waiting for signal: [output high level]!
    Please press "Enter" key once any board send this signal.

DUT2 (slave) console::

    Send signal: [output high level]!

Once the signal is sent from DUT2, you need to press "Enter" on DUT1, then DUT1 unblocks from ``unity_wait_for_signal`` and starts to change GPIO level.

Signals can also be used to pass parameters between multiple devices. For example, DUT1 want to know the MAC address of DUT2, so it can connect to DUT2.
In this case, ``unity_wait_for_signal_param`` and ``unity_send_signal_param`` can be used:

DUT1 console::

    Waiting for signal: [dut2 mac address]!
    Please input parameter value from any board send this signal and press "Enter" key.

DUT2 console::

    Send signal: [dut2 mac address][10:20:30:40:50:60]!

Once the signal is sent from DUT2, you need to input ``10:20:30:40:50:60`` on DUT1 and press "Enter". Then DUT1 will get the MAC address string of DUT2 and unblock from ``unity_wait_for_signal_param``, then start to connect to DUT2.


Multi-stage Test Cases
-----------------------

The normal test cases are expected to finish without reset (or only need to check if reset happens). Sometimes we expect to run some specific tests after certain kinds of reset.
For example, we expect to test if reset reason is correct after a wakeup from deep sleep. We need to create a deep-sleep reset first and then check the reset reason.
To support this, we can define multi-stage test cases, to group a set of test functions::

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

    TEST_CASE_MULTIPLE_STAGES("reset reason check for deepsleep", "[esp32]", trigger_deepsleep, check_deepsleep_reset_reason);

Multi-stage test cases present a group of test functions to users. It need user interactions (select cases and select different stages) to run the case.


Building Unit Test App
----------------------

Follow the setup instructions in the top-level esp-idf README.
Make sure that ``IDF_PATH`` environment variable is set to point to the path of esp-idf top-level directory.

Change into ``tools/unit-test-app`` directory to configure and build it:

* ``make menuconfig`` - configure unit test app.

* ``make TESTS_ALL=1`` - build unit test app with tests for each component having tests in the ``test`` subdirectory.
* ``make TEST_COMPONENTS='xxx'`` - build unit test app with tests for specific components.
* ``make TESTS_ALL=1 TEST_EXCLUDE_COMPONENTS='xxx'`` - build unit test app with all unit tests, except for unit tests of some components. (For instance: ``make TESTS_ALL=1 TEST_EXCLUDE_COMPONENTS='ulp mbedtls'`` - build all unit tests exludes ``ulp`` and ``mbedtls`` components).

When the build finishes, it will print instructions for flashing the chip. You can simply run ``make flash`` to flash all build output.

You can also run ``make flash TESTS_ALL=1`` or ``make TEST_COMPONENTS='xxx'`` to build and flash. Everything needed will be rebuilt automatically before flashing.

Use menuconfig to set the serial port for flashing.

Running Unit Tests
------------------

After flashing reset the ESP32 and it will boot the unit test app.

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
    (20)    "reset reason check for deepsleep" [esp32][test_env=UT_T2_1][multi_stage]
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

The normal case will print the case name and description. Master-slave cases will also print the sub-menu (the registered test function names).

Test cases can be run by inputting one of the following:

- Test case name in quotation marks (for example, ``"esp_ota_begin() verifies arguments"``) to run a single test case.

- Test case index (for example, ``1``) to run a single test case.

- Module name in square brackets (for example, ``[cxx]``) to run all test cases for a specific module.

- An asterisk (``*``) to run all test cases

``[multi_device]`` and ``[multi_stage]`` tags tell the test runner whether a test case is a multi-device or multi-stage test case.
These tags are automatically added by ```TEST_CASE_MULTIPLE_STAGES`` and ``TEST_CASE_MULTIPLE_DEVICES`` macros.

After you select a multi-device test case, it will print sub menu::

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

For the first time you execute this case, please input ``1`` to run the first stage (trigger deep-sleep).
After DUT is rebooted and test cases are available to run, select this case again and input ``2`` to run the second stage.
The case will only pass if the last stage passes and all previous stages trigger reset.
