===============================
ESP-IDF Tests with Pytest Guide
===============================

This documentation is a guide that introduces the following aspects:

1. The basic idea of different test types in ESP-IDF
2. How to apply the pytest framework to the test python scripts to make sure the apps are working as expected.
3. ESP-IDF CI target test process
4. Run ESP-IDF tests with pytest locally
5. Tips and tricks on pytest

Disclaimer
==========

In ESP-IDF, we use the following plugins by default:

-  `pytest-embedded <https://github.com/espressif/pytest-embedded>`__ with default services ``esp,idf``
-  `pytest-rerunfailures <https://github.com/pytest-dev/pytest-rerunfailures>`__

All the introduced concepts and usages are based on the default behavior in ESP-IDF. Not all of them are available in vanilla pytest.

Installation
============

All dependencies could be installed by running the install script with the ``--enable-pytest`` argument, e.g., ``$ install.sh --enable-pytest``.

Common Issues During Installation
---------------------------------

No Package 'dbus-1' found
^^^^^^^^^^^^^^^^^^^^^^^^^

If you're facing an error message like:

.. code:: text

   configure: error: Package requirements (dbus-1 >= 1.8) were not met:
  
   No package 'dbus-1' found
  
   Consider adjusting the PKG_CONFIG_PATH environment variable if you
   installed software in a non-standard prefix.

If you're running under ubuntu system, you may need to run:

.. code:: shell

   sudo apt-get install libdbus-glib-1-dev

or 

.. code:: shell

   sudo apt-get install libdbus-1-dev

For other linux distros, you may Google the error message and find the solution. This issue could be solved by installing the related header files.

Invalid command 'bdist_wheel'
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you're facing an error message like:

.. code:: text

   error: invalid command 'bdist_wheel'        

You may need to run:

.. code:: shell

   python -m pip install -U pip

Or

.. code:: shell

   python -m pip install wheel

Before running the pip commands, please make sure you're using the IDF python virtual environment.

Basic Concepts
==============

Component-based Unit Tests
--------------------------

Component-based unit tests are our recommended way to test your component. All the test apps should be located under ``${IDF_PATH}/components/<COMPONENT_NAME>/test_apps``.

For example:

.. code:: text

   components/
   └── my_component/
       ├── include/
       │   └── ...
       ├── test_apps/
       │   ├── test_app_1
       │   │   ├── main/
       │   │   │   └── ...
       │   │   ├── CMakeLists.txt
       │   │   └── pytest_my_component_app_1.py
       │   ├── test_app_2
       │   │   ├── ...
       │   │   └── pytest_my_component_app_2.py
       │   └── parent_folder
       │       ├── test_app_3
       │       │   ├── ...
       │       │   └── pytest_my_component_app_3.py
       │       └── ...
       ├── my_component.c
       └── CMakeLists.txt

Example Tests
-------------

Example Tests are tests for examples that are intended to demonstrate parts of the ESP-IDF functionality to our customers.

All the test apps should be located under ``${IDF_PATH}/examples``. For more information please refer to the :idf_file:`Examples Readme <examples/README.md>`.

For example:

.. code:: text

   examples/
   └── parent_folder/
       └── example_1/
           ├── main/
           │   └── ...
           ├── CMakeLists.txt
           └── pytest_example_1.py

Custom Tests
------------

Custom Tests are tests that aim to run some arbitrary test internally. They are not intended to demonstrate the ESP-IDF functionality to our customers in any way.

All the test apps should be located under ``${IDF_PATH}/tools/test_apps``. For more information please refer to the :idf_file:`Custom Test Readme <tools/test_apps/README.md>`.

Pytest in ESP-IDF
=================

Pytest Execution Process
------------------------

1. Bootstrapping Phase

   Create session-scoped caches:

   -  port-target cache
   -  port-app cache

2. Collection Phase

   1. Get all the python files with the prefix ``pytest_``
   2. Get all the test functions with the prefix ``test_``
   3. Apply the `params <https://docs.pytest.org/en/latest/how-to/parametrize.html>`__, and duplicate the test functions.
   4. Filter the test cases with CLI options. Introduced detailed usages `here <#filter-the-test-cases>`__

3. Test Running Phase

   1. Construct the `fixtures <https://docs.pytest.org/en/latest/how-to/fixtures.html>`__. In ESP-IDF, the common fixtures are initialized in this order:

      1. ``pexpect_proc``: `pexpect <https://github.com/pexpect/pexpect>`__ instance

      2. ``app``: `IdfApp <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.app.IdfApp>`__ instance

         The information of the app, like sdkconfig, flash_files, partition_table, etc., would be parsed at this phase.

      3. ``serial``: `IdfSerial <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.serial.IdfSerial>`__ instance

         The port of the host which connected to the target type parsed from the app would be auto-detected. The flash files would be auto flashed.

      4. ``dut``: `IdfDut <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.dut.IdfDut>`__ instance

   2. Run the real test function

   3. Deconstruct the fixtures in this order:

      1. ``dut``

         1. close the ``serial`` port
         2. (Only for apps with `unity test framework <https://github.com/ThrowTheSwitch/Unity>`__) generate junit report of the unity test cases

      2. ``serial``
      3. ``app``
      4. ``pexpect_proc``: Close the file descriptor

   4. (Only for apps with `unity test framework <https://github.com/ThrowTheSwitch/Unity>`__)

      Raise ``AssertionError`` when detected unity test failed if you call ``dut.expect_from_unity_output()`` in the test function.

4. Reporting Phase

   1. Generate junit report of the test functions
   2. Modify the junit report test case name into ESP-IDF test case ID format: ``<target>.<config>.<test function name>``

5. Finalizing Phase (Only for apps with `unity test framework <https://github.com/ThrowTheSwitch/Unity>`__)

   Combine the junit reports if the junit reports of the unity test cases are generated.

Getting Started Example
-----------------------

This code example is taken from :idf_file:`pytest_console_basic.py <examples/system/console/basic/pytest_console_basic.py>`.

.. code:: python

   @pytest.mark.esp32
   @pytest.mark.esp32c3
   @pytest.mark.generic
   @pytest.mark.parametrize('config', [
       'history',
       'nohistory',
   ], indirect=True)
   def test_console_advanced(config: str, dut: IdfDut) -> None:
       if config == 'history':
           dut.expect('Command history enabled')
       elif config == 'nohistory':
           dut.expect('Command history disabled')

Let's go through this simple test case line by line in the following subsections.

Use Markers to Specify the Supported Targets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: python

   @pytest.mark.esp32     # <-- support esp32
   @pytest.mark.esp32c3   # <-- support esp32c3
   @pytest.mark.generic   # <-- test env "generic"

The above lines indicate that this test case supports target esp32 and esp32c3, the target board type should be "generic". If you want to know what is the "generic" type refers to, you may run ``pytest --markers`` to get the detailed information of all markers.

.. note::

   If the test case supports all officially ESP-IDF supported targets (You may check the value via "idf.py --list-targets"), you can use a special marker ``supported_targets`` to apply all of them in one line.

Use Params to Specify the sdkconfig Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use ``pytest.mark.parametrize`` with “config” to apply the same test to different apps with different sdkconfig files. For more information about ``sdkconfig.ci.xxx`` files, please refer to the Configuration Files section under :idf_file:`this readme <tools/test_apps/README.md>`.

.. code:: python

   @pytest.mark.parametrize('config', [
       'history',     # <-- run with app built by sdkconfig.ci.history
       'nohistory',   # <-- run with app built by sdkconfig.ci.nohistory
   ], indirect=True)  # <-- `indirect=True` is required

Overall, this test function would be replicated to 4 test cases:

-  esp32.history.test_console_advanced
-  esp32.nohistory.test_console_advanced
-  esp32c3.history.test_console_advanced
-  esp32c3.nohistory.test_console_advanced

Expect From the Serial output
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: python
    
   def test_console_advanced(config: str, dut: IdfDut) -> None:  # The value of argument ``config`` is assigned by the parametrization.
       if config == 'history':
           dut.expect('Command history enabled')
       elif config == 'nohistory':
           dut.expect('Command history disabled')

When we're using ``dut.expect(...)``, the string would be compiled into regex at first, and then seeks through the serial output until the compiled regex is matched, or a timeout is exceeded. You may have to pay extra attention when the string contains regex keyword characters, like parentheses, or square brackets.

Actually using ``dut.expect_exact(...)`` here is better, since it would seek until the string is matched. For further reading about the different types of ``expect`` functions, please refer to the `pytest-embedded Expecting documentation <https://docs.espressif.com/projects/pytest-embedded/en/latest/expecting>`__.

Advanced Examples
-----------------

Multi Dut Tests with the Same App
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: python

    @pytest.mark.esp32s2
    @pytest.mark.esp32s3
    @pytest.mark.usb_host
    @pytest.mark.parametrize('count', [
        2,
    ], indirect=True)
    def test_usb_host(dut: Tuple[IdfDut, IdfDut]) -> None:
        device = dut[0]  # <-- assume the first dut is the device
        host = dut[1]    # <-- and the second dut is the host
        ...

After setting the param ``count`` to 2, all these fixtures are changed into tuples.

Multi Dut Tests with Different Apps
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This code example is taken from :idf_file:`pytest_wifi_getting_started.py <examples/wifi/getting_started/pytest_wifi_getting_started.py>`.

.. code:: python

    @pytest.mark.esp32
    @pytest.mark.multi_dut_generic
    @pytest.mark.parametrize(
        'count, app_path', [
            (2,
             f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}'),
        ], indirect=True
    )
    def test_wifi_getting_started(dut: Tuple[IdfDut, IdfDut]) -> None:
        softap = dut[0]
        station = dut[1]
        ...

Here the first dut was flashed with the app :idf_file:`softap <examples/wifi/getting_started/softAP/main/softap_example_main.c>`, and the second dut was flashed with the app :idf_file:`station <examples/wifi/getting_started/station/main/station_example_main.c>`.

.. note::

   Here the ``app_path`` should be set with absolute path. the ``__file__`` macro in python would return the absolute path of the test script itself.

Multi Dut Tests with Different Apps, and Targets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This code example is taken from :idf_file:`pytest_wifi_getting_started.py <examples/wifi/getting_started/pytest_wifi_getting_started.py>`. As the comment says, for now it's not running in the ESP-IDF CI.

.. code:: python

    @pytest.mark.parametrize(
        'count, app_path, target', [
            (2,
             f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}',
             'esp32|esp32s2'),
            (2,
             f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}',
             'esp32s2|esp32'),
        ],
        indirect=True,
    )
    def test_wifi_getting_started(dut: Tuple[IdfDut, IdfDut]) -> None:
        softap = dut[0]
        station = dut[1]
        ...

Overall, this test function would be replicated to 2 test cases:

- softap with esp32 target, and station with esp32s2 target
- softap with esp32s2 target, and station with esp32 target

Support different targets with different sdkconfig files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This code example is taken from :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>` as an advanced example.

.. code:: python

   CONFIGS = [
       pytest.param('coredump_flash_bin_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
       pytest.param('coredump_flash_elf_sha', marks=[pytest.mark.esp32]),  # sha256 only supported on esp32
       pytest.param('coredump_uart_bin_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
       pytest.param('coredump_uart_elf_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
       pytest.param('gdbstub', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
       pytest.param('panic', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
   ]

   @pytest.mark.parametrize('config', CONFIGS, indirect=True)
   ...

Use Custom Class
^^^^^^^^^^^^^^^^

Usually, you can write a custom class in these conditions:

1. Add more reusable functions for a certain number of DUTs
2. Add custom setup and teardown functions in different phases described `here <#pytest-execution-process>`__ 

This code example is taken from :idf_file:`panic/conftest.py <tools/test_apps/system/panic/conftest.py>`

.. code:: python

   class PanicTestDut(IdfDut):
       ...

   @pytest.fixture(scope='module')
   def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
       mp = MonkeyPatch()
       request.addfinalizer(mp.undo)
       return mp


   @pytest.fixture(scope='module', autouse=True)
   def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
       monkeypatch_module.setattr('pytest_embedded_idf.dut.IdfDut', PanicTestDut)

``monkeypatch_module`` provide a `module-scoped <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ `monkeypatch <https://docs.pytest.org/en/latest/how-to/monkeypatch.html>`__ fixture.

``replace_dut_class`` is a `module-scoped <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ `autouse <https://docs.pytest.org/en/latest/how-to/fixtures.html#autouse-fixtures-fixtures-you-don-t-have-to-request>`__ fixture. This function replaces the ``IdfDut`` class with your custom class.

Mark Flaky Tests
^^^^^^^^^^^^^^^^

Sometimes, our test is based on ethernet or wifi. The network may cause the test flaky. We could mark the single test case within the code repo.

This code example is taken from :idf_file:`pytest_esp_eth.py <components/esp_eth/test_apps/pytest_esp_eth.py>`

.. code:: python

   @pytest.mark.flaky(reruns=3, reruns_delay=5)
   def test_esp_eth_ip101(dut: IdfDut) -> None:
       ...

This flaky marker means that if the test function failed, the test case would rerun for a maximum of 3 times with 5 seconds delay.

Mark Known Failure Cases
^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes a test couldn't pass for the following reasons:

- Has a bug
- The success ratio is too low because of environment issue, such as network issue. Retry couldn't help

Now you may mark this test case with marker `xfail <https://docs.pytest.org/en/latest/how-to/skipping.html#xfail-mark-test-functions-as-expected-to-fail>`__ with a user-friendly readable reason.

This code example is taken from :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>`

.. code:: python

   @pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
   def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:

This marker means that if the test would be a known failure one on esp32s2.

Mark Nightly Run Test Cases
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some test cases are only triggered in nightly run pipelines due to a lack of runners.

.. code:: python

    @pytest.mark.nightly_run

This marker means that the test case would only be run with env var ``NIGHTLY_RUN`` or ``INCLUDE_NIGHTLY_RUN``.

Mark Temp Disabled in CI
^^^^^^^^^^^^^^^^^^^^^^^^

Some test cases which can pass locally may need to be temporarily disabled in CI due to a lack of runners.

.. code:: python

   @pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2'], reason='lack of runners')

This marker means that the test case could still be run locally with ``pytest --target esp32``, but will not run in CI.

Run Unity Test Cases
^^^^^^^^^^^^^^^^^^^^

For component-based unit test apps, one line could do the trick to run all single-board test cases, including normal test cases and multi-stage test cases:

.. code:: python

   def test_component_ut(dut: IdfDut):
       dut.run_all_single_board_cases()

It would also skip all the test cases with ``[ignore]`` mark. 

If you need to run a group of test cases, you may run:

.. code:: python

   def test_component_ut(dut: IdfDut):
       dut.run_all_single_board_cases(group='psram')

It would trigger all test cases with module name ``[psram]``.

You may also see that there are some test scripts with the following statements, which are deprecated. Please use the suggested one as above.

.. code:: python

   def test_component_ut(dut: IdfDut):
       dut.expect_exact('Press ENTER to see the list of tests')
       dut.write('*')
       dut.expect_unity_test_output()

For further reading about our unit testing in ESP-IDF, please refer to :doc:`our unit testing guide <../api-guides/unit-tests>`.

Run the Tests in CI
===================

The workflow in CI is simple, build jobs -> target test jobs.

Build Jobs
----------

Build Job Names
^^^^^^^^^^^^^^^

-  Component-based Unit Tests: ``build_pytest_components_<target>``
-  Example Tests: ``build_pytest_examples_<target>``
-  Custom Tests: ``build_pytest_test_apps_<target>``

Build Job Commands
^^^^^^^^^^^^^^^^^^

The command used by CI to build all the relevant tests is: ``python $IDF_PATH/tools/ci/ci_build_apps.py <parent_dir> --target <target> -vv --pytest-apps``

All apps which supported the specified target would be built with all supported sdkconfig files under ``build_<target>_<config>``.

For example, If you run ``python $IDF_PATH/tools/ci/ci_build_apps.py $IDF_PATH/examples/system/console/basic --target esp32 --pytest-apps``, the folder structure would be like this:

.. code:: text

   basic
   ├── build_esp32_history/
   │   └── ...
   ├── build_esp32_nohistory/
   │   └── ...
   ├── main/
   ├── CMakeLists.txt
   ├── pytest_console_basic.py
   └── ...

All the binaries folders would be uploaded as artifacts under the same directories.

Target Test Jobs
----------------

Target Test Job Names
^^^^^^^^^^^^^^^^^^^^^

-  Component-based Unit Tests: ``component_ut_pytest_<target>_<test_env>``
-  Example Tests: ``example_test_pytest_<target>_<test_env>``
-  Custom Tests: ``test_app_test_pytest_<target>_<test_env>``

Target Test Job Commands
^^^^^^^^^^^^^^^^^^^^^^^^

The command used by CI to run all the relevant tests is: ``pytest <parent_dir> --target <target> -m <test_env_marker>``

All test cases with the specified target marker and the test env marker under the parent folder would be executed.

The binaries in the target test jobs are downloaded from build jobs, the artifacts would be placed under the same directories.

Run the Tests Locally
=====================

First you need to install ESP-IDF with additional python requirements:

.. code-block:: shell

   $ cd $IDF_PATH
   $ bash install.sh --enable-pytest
   $ . ./export.sh

By default, the pytest script will look for the build directory in this order:

- ``build_<target>_<sdkconfig>``
- ``build_<target>``
- ``build_<sdkconfig>``
- ``build``

Which means, the simplest way to run pytest is calling ``idf.py build``.

For example, if you want to run all the esp32 tests under the ``$IDF_PATH/examples/get-started/hello_world`` folder, you should run:

.. code-block:: shell

   $ cd examples/get-started/hello_world
   $ idf.py build
   $ pytest --target esp32

If you have multiple sdkconfig files in your test app, like those ``sdkconfig.ci.*`` files, the simple ``idf.py build`` won't apply the extra sdkconfig files. Let's take ``$IDF_PATH/examples/system/console/basic`` as an example.

If you want to test this app with config ``history``, and build with ``idf.py build``, you should run

.. code-block:: shell

   $ cd examples/system/console/basic
   $ idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.history" build
   $ pytest --target esp32 --sdkconfig history

If you want to build and test with all sdkconfig files at the same time, you should use our CI script as an helper script:

.. code-block:: shell

   $ cd examples/system/console/basic
   $ python $IDF_PATH/tools/ci/ci_build_apps.py . --target esp32 -vv --pytest-apps
   $ pytest --target esp32

The app with ``sdkconfig.ci.history`` will be built in ``build_esp32_history``, and the app with ``sdkconfig.ci.nohistory`` will be built in ``build_esp32_nohistory``. ``pytest --target esp32`` will run tests on both apps.

Tips and Tricks
===============

Filter the Test Cases
---------------------

-  filter by target with ``pytest --target <target>`` 

   pytest would run all the test cases that support specified target.

-  filter by sdkconfig file with ``pytest --sdkconfig <sdkconfig>``

   if ``<sdkconfig>`` is ``default``, pytest would run all the test cases with the sdkconfig file ``sdkconfig.defaults``.

   In other cases, pytest would run all the test cases with sdkconfig file ``sdkconfig.ci.<sdkconfig>``.

Add New Markers
---------------

We’re using two types of custom markers, target markers which indicate that the test cases should support this target, and env markers which indicate that the test case should be assigned to runners with these tags in CI.

You can add new markers by adding one line under the ``${IDF_PATH}/conftest.py``. If it's a target marker, it should be added into ``TARGET_MARKERS``. If it's a marker that specifies a type of test environment, it should be added into ``ENV_MARKERS``. The grammar should be: ``<marker_name>: <marker_description>``.

Generate JUnit Report
---------------------

You can call pytest with ``--junitxml <filepath>`` to generate the JUnit report. In ESP-IDF, the test case name would be unified as "<target>.<config>.<function_name>". 

Skip Auto Flash Binary
----------------------

Skipping auto-flash binary every time would be useful when you're debugging your test script.

You can call pytest with ``--skip-autoflash y`` to achieve it.

Record Statistics
-----------------

Sometimes you may need to record some statistics while running the tests, like the performance test statistics.

You can use `record_xml_attribute <https://docs.pytest.org/en/latest/how-to/output.html?highlight=junit#record-xml-attribute>`__ fixture in your test script, and the statistics would be recorded as attributes in the JUnit report.

Logging System
--------------

Sometimes you may need to add some extra logging lines while running the test cases.

You can use `python logging module <https://docs.python.org/3/library/logging.html>`__ to achieve this.

Useful Logging Functions (as Fixture)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``log_performance``
"""""""""""""""""""

.. code:: python

    def test_hello_world(
        dut: IdfDut,
        log_performance: Callable[[str, object], None],
    ) -> None:
        log_performance('test', 1)

The above example would log the performance item with pre-defined format: "[performance][test]: 1" and record it under the ``properties`` tag in the junit report if ``--junitxml <filepath>`` is specified. The junit test case node would look like:

.. code:: html

    <testcase classname="examples.get-started.hello_world.pytest_hello_world" file="examples/get-started/hello_world/pytest_hello_world.py" line="13" name="esp32.default.test_hello_world" time="8.389">
        <properties>
            <property name="test" value="1"/>
        </properties>
    </testcase>

``check_performance``
"""""""""""""""""""""

We provide C macros ``TEST_PERFORMANCE_LESS_THAN`` and ``TEST_PERFORMANCE_GREATER_THAN`` to log the performance item and check if the value is in the valid range. Sometimes the performance item value could not be measured in C code, so we also provide a python function for the same purpose. Please note that using C macros is the preferred approach, since the python function couldn't recognize the threshold values of the same performance item under different ifdef blocks well.

.. code:: python

    def test_hello_world(
        dut: IdfDut,
        check_performance: Callable[[str, float, str], None],
    ) -> None:
        check_performance('RSA_2048KEY_PUBLIC_OP', 123, 'esp32')
        check_performance('RSA_2048KEY_PUBLIC_OP', 19001, 'esp32')

The above example would first get the threshold values of the performance item ``RSA_2048KEY_PUBLIC_OP`` from :idf_file:`components/idf_test/include/idf_performance.h` and the target-specific one :idf_file:`components/idf_test/include/esp32/idf_performance_target.h`, then check if the value reached the minimum limit or exceeded the maximum limit.

Let's assume the value of ``IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP`` is 19000. so the first ``check_performance`` line would pass and the second one would fail with warning: ``[Performance] RSA_2048KEY_PUBLIC_OP value is 19001, doesn\'t meet pass standard 19000.0``

Further Readings
================

-  pytest documentation: https://docs.pytest.org/en/latest/contents.html
-  pytest-embedded documentation: https://docs.espressif.com/projects/pytest-embedded/en/latest/
