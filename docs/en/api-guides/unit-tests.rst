Unit Testing in ESP32
=============================
:link_to_translation:`zh_CN:[中文]`

ESP-IDF comes with a unit test app based on Unity - unit test framework. Unit tests are integrated in the ESP-IDF repository and are placed in ``test`` subdirectory of each component respectively.

Add normal test cases
---------------------

Unit tests are added in the ``test`` subdirectory of the respective component.
Tests are added in C files, a single C file can include multiple test cases.
Test files start with the word "test".

The test file should include unity.h and the header for the C module to be tested.

Tests are added in a function in the C file as follows::

    TEST_CASE("test name", "[module name]"
    {
            // Add test here
    }

First argument is a descriptive name for the test, second argument is an identifier in square brackets.
Identifiers are used to group related test, or tests with specific properties.

There is no need to add a main function with ``UNITY_BEGIN()`` and ``​UNITY_END()`` in each test case.
``unity_platform.c`` will run ``UNITY_BEGIN()``, run the tests cases, and then call ``​UNITY_END()``.

The ``test`` subdirectory should contain a :ref:`component CMakeLists.txt <component-directories>`, since they are themselves,
components. ESP-IDF uses the test framework ``unity`` and should be specified as a requirement for the component. Normally, components 
:ref:`should list their sources manually <cmake-file-globbing>`; for component tests however, this requirement is relaxed and the 
use of ``SRC_DIRS`` argument to ``idf_component_register`` is advised.

Overall, the minimal ``test`` subdirectory CMakeLists.txt file may look like as follows:

.. code:: cmake

    idf_component_register(SRC_DIRS "."
                           INCLUDE_DIRS "."
                           REQUIRES unity)

See http://www.throwtheswitch.org/unity for more information about writing tests in Unity.


Add multiple devices test cases
-------------------------------

The normal test cases will be executed on one DUT (Device Under Test). Components need to communicate with each other (like GPIO, SPI ...) can't be tested with normal test cases.
Multiple devices test cases support writing and running test with multiple DUTs.

Here's an example of multiple devices test case::

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


The macro ``TEST_CASE_MULTIPLE_DEVICES`` is used to declare multiple devices test cases.
First argument is test case name, second argument is test case description.
From the third argument, upto 5 test functions can be defined, each function will be the entry point of tests running on each DUT.

Running test cases from different DUTs could require synchronizing between DUTs. We provide ``unity_wait_for_signal`` and ``unity_send_signal`` to support synchronizing with UART.
As the secnario in the above example, slave should get GPIO level after master set level. DUT UART console will prompt and requires user interaction:

DUT1 (master) console::

    Waiting for signal: [output high level]!
    Please press "Enter" key to once any board send this signal.

DUT2 (slave) console::

    Send signal: [output high level]!

Once the signal is set from DUT2, you need to press "Enter" on DUT1, then DUT1 unblocks from ``unity_wait_for_signal`` and starts to change GPIO level.


Add multiple stages test cases
-------------------------------

The normal test cases are expected to finish without reset (or only need to check if reset happens). Sometimes we want to run some specific test after certain kinds of reset. 
For example, we want to test if reset reason is correct after wakeup from deep sleep. We need to create deep sleep reset first and then check the reset reason.
To support this, we can define multiple stages test case, to group a set of test functions together::

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

    TEST_CASE_MULTIPLE_STAGES("reset reason check for deepsleep", "[esp32]", trigger_deepsleep, check_deepsleep_reset_reason);

Multiple stages test cases present a group of test functions to users. It need user interactions (select case and select different stages) to run the case.


Building unit test app
----------------------

Follow the setup instructions in the top-level esp-idf README.
Make sure that IDF_PATH environment variable is set to point to the path of esp-idf top-level directory.

Change into tools/unit-test-app directory to configure and build it:

* `idf.py menuconfig` - configure unit test app.

* `idf.py -T all build` - build unit test app with tests for each component having tests in the ``test`` subdirectory.
* `idf.py -T xxx build` - build unit test app with tests for specific components. 
* `idf.py -T all -E xxxbuild` - build unit test app with all unit tests, except for unit tests of some components. (For instance: `idf.py -T all -E ulp mbedtls build` - build all unit tests exludes ulp and mbedtls components).

When the build finishes, it will print instructions for flashing the chip. You can simply run ``idf.py flash`` to flash all build output.

You can also run ``idf.py -T all flash`` or ``idf.py -T xxx flash`` to build and flash. Everything needed will be rebuilt automatically before flashing. 

Use menuconfig to set the serial port for flashing.

Running unit tests
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
    (17)    "SPI Master no response when switch from host1 (HSPI) to host2 (VSPI)" [spi]
    (18)    "SPI Master DMA test, TX and RX in different regions" [spi]
    (19)    "SPI Master DMA test: length, start, not aligned" [spi]
    (20)    "reset reason check for deepsleep" [esp32][test_env=UT_T2_1][multi_stage]
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

Normal case will print the case name and description. Master slave cases will also print the sub-menu (the registered test function names).

Test cases can be run by inputting one of the following:

- Test case name in quotation marks to run a single test case 

- Test case index to run a single test case

- Module name in square brackets to run all test cases for a specific module

- An asterisk to run all test cases

``[multi_device]`` and ``[multi_stage]`` tags tell the test runner whether a test case is a multiple devices or multiple stages test case.
These tags are automatically added by ```TEST_CASE_MULTIPLE_STAGES`` and ``TEST_CASE_MULTIPLE_DEVICES`` macros.

After you select a multiple devices test case, it will print sub menu::

    Running gpio master/slave test example...
    gpio master/slave test example
            (1)     "gpio_master_test"
            (2)     "gpio_slave_test"

You need to input number to select the test running on the DUT.

Similar to multiple devices test cases, multiple stages test cases will also print sub menu::

    Running reset reason check for deepsleep...
    reset reason check for deepsleep
            (1)     "trigger_deepsleep"
            (2)     "check_deepsleep_reset_reason"

First time you execute this case, input ``1`` to run first stage (trigger deepsleep).
After DUT is rebooted and able to run test cases, select this case again and input ``2`` to run the second stage.
The case only passes if the last stage passes and all previous stages trigger reset.
