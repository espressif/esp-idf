Unit Testing on Linux
=====================

.. note::
    Host testing with IDF is experimental for now. We try our best to keep interfaces stable but can't guarantee it for now. Feedback via github or the forum on esp32.com is highly welcome, though and may influence the future design of the host-based tests.

This article provides an overview of unit tests with IDF on Linux. For using unit tests on the target, please refer to :doc:`target based unit testing <unit-tests>`.

Embedded Software Tests
-----------------------

Embedded software tests are challenging due to the following factors:

- Difficulties running tests efficiently.
- Lack of many operating system abstractions when interfacing with hardware, making it difficult to isolate code under test.

To solve these two problems, Linux host-based tests with `CMock <https://www.throwtheswitch.org/cmock>`_ are introduced. Linux host-based tests are more efficient than unit tests on the target since they:

- Compile the necessary code only
- Don't need time to upload to a target
- Run much faster on a host-computer, compared to an ESP

Using the `CMock <https://www.throwtheswitch.org/cmock>`_ framework also solves the problem of hardware dependencies. Through mocking, hardware details are emulated and specified at run time, but only if necessary.

Of course, using code on the host and using mocks does not fully represent the target device. Thus, two kinds of tests are recommended:

1. Unit tests which test program logic on a Linux machine, isolated through mocks.
2. System/Integration tests which test the interaction of components and the whole system. They run on the target, where irrelevant components and code may as well be emulated via mocks.

This documentation is about the first kind of tests. Refer to :doc:`target based unit testing <unit-tests>` for more information on target tests (the second kind of tests).

IDF Unit Tests on Linux Host
----------------------------

The current focus of the Linux host tests is on creating isolated unit tests of components, while mocking the component's dependencies with CMock.

A complete implementation of IDF to run on Linux does not exist currently.

There are currently two examples for running IDF-built code on Linux host: 

- An example :example_file:`hello-world application <build_system/cmake/linux_host_app/README.md>` 
- A :component_file:`unit test for NVS <nvs_flash/host_test/nvs_page_test/README.md>`.

Inside the component which should be tested, there is a separate directory ``host_test``, besides the "traditional" ``test`` directory or the ``test_apps`` directory. It has one or more subdirectories::

    - host_test/
    
                 - fixtures/ 
                     contains test fixtures (structs/functions to do test case set-up and tear-down).
                     If there are no fixtures, this can be ommitted.
                 - <test_name>/ 
                     IDF applications which run the tests
                 - <test_name2>/ 
                     Further tests are possible.

The IDF applications inside ``host_test`` set the mocking configuration as described in the :doc:`IDF unit test documentation <unit-tests>`.

The :component_file:`NVS page unit test <nvs_flash/host_test/nvs_page_test/main/nvs_page_test.cpp>` provides some illustration of how to control the mocks.

Requirements
^^^^^^^^^^^^

- Installed IDF including all IDF requirements
- CMock requirements (``Ruby``)
- ``libbsd``

The host tests have been tested on Ubuntu 20.04 with ``GCC`` version 9 and 10.
