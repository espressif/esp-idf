Unit Testing in ESP32
=====================

ESP-IDF comes with a unit test app based on Unity - unit test framework. Unit tests are integrated in the ESP-IDF repository and are placed in ``test`` subdirectory of each component respectively.

Adding unit tests
-----------------

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

Each `test` subdirectory needs to include component.mk file with at least the following line of code::

    COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

See http://www.throwtheswitch.org/unity for more information about writing tests in Unity.

Building unit test app
----------------------

Follow the setup instructions in the top-level esp-idf README.
Make sure that IDF_PATH environment variable is set to point to the path of esp-idf top-level directory.

Change into tools/unit-test-app directory to configure and build it:

* `make menuconfig` - configure unit test app.

* `make TESTS_ALL=1` - build unit test app with tests for each component having tests in the ``test`` subdirectory.
* `make TEST_COMPONENTS='xxx'` - build unit test app with tests for specific components. 

When the build finishes, it will print instructions for flashing the chip. You can simply run ``make flash`` to flash all build output.

You can also run ``make flash TESTS_ALL=1`` or ``make TEST_COMPONENTS='xxx'`` to build and flash. Everything needed will be rebuilt automatically before flashing. 

Use menuconfig to set the serial port for flashing.

Running unit tests
------------------

After flashing reset the ESP32 and it will boot the unit test app.

Unit test app prints a test menu with all available tests.

Test cases can be run by inputting one of the following:

- Test case name in quotation marks to run a single test case 

- Test case index to run a single test case

- Module name in square brackets to run all test cases for a specific module

- An asterisk to run all test cases
