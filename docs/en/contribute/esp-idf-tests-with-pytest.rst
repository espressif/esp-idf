=================
pytest in ESP-IDF
=================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF has numerous types of tests that are meant to be executed on an ESP chip (known as **on target testing**). Target tests are usually compiled as part of an IDF project used for testing (known as a **test app**), where test apps follows the same build, flash, and monitor process of any other standard IDF project.

Typically, on target testing will require a connected host (e.g., a PC) that is responsible for triggering a particular test case, providing test data, and inspecting test results.

ESP-IDF uses the pytest framework (and some pytest plugins) on the host side to automate on target testing. This guide introduces pytest in ESP-IDF and covers the following concepts:

1. The different types of test apps in ESP-IDF.
2. Using the pytest framework in Python scripts to automate target testing.
3. ESP-IDF Continuous Integration (CI) target testing process.
4. How to run target tests locally with pytest.
5. pytest tips and tricks.

.. note::

  In ESP-IDF, we use the following pytest plugins by default:

  -  `pytest-embedded <https://github.com/espressif/pytest-embedded>`__ with default services ``esp,idf``
  -  `pytest-rerunfailures <https://github.com/pytest-dev/pytest-rerunfailures>`__

  All the concepts and usages introduced in this guide are based on the default behavior of these plugins, thus may not be available in vanilla pytest.

Installation
============

All dependencies could be installed by running the install script with the ``--enable-pytest`` argument:

.. code-block:: bash

  $ install.sh --enable-pytest


Common Issues During Installation
---------------------------------

No Package 'dbus-1' Found
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: text

  configure: error: Package requirements (dbus-1 >= 1.8) were not met:

  No package 'dbus-1' found

  Consider adjusting the PKG_CONFIG_PATH environment variable if you
  installed software in a non-standard prefix.

If you encounter the error message above, you may need to install some missing packages.

If you are using Ubuntu, you may need to run:

.. code:: shell

  sudo apt-get install libdbus-glib-1-dev

or

.. code:: shell

  sudo apt-get install libdbus-1-dev

For other Linux distributions, please Google the error message above and find which missing packages need to be installed for your particular distribution.

Invalid Command 'bdist_wheel'
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: text

  error: invalid command 'bdist_wheel'

If you encounter the error message above, you may need to install some missing Python packages such as:

.. code:: shell

  python -m pip install -U pip

or

.. code:: shell

  python -m pip install wheel

.. note::

  Before running the pip commands, please make sure you are using the IDF Python virtual environment.


Test Apps
=========

ESP-IDF contains different types of test apps that can be automated using pytest.

Component Tests
---------------

ESP-IDF components typically contain component specific test apps that execute component specific unit tests. Component test apps are the recommended way to test components. All the test apps should be located under ``${IDF_PATH}/components/<COMPONENT_NAME>/test_apps``, for example:

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

The purpose of ESP-IDF examples is to demonstrate parts of ESP-IDF functionality to users (refer to :idf_file:`Examples Readme <examples/README.md>` for more information).

However, to ensure that these examples operate correctly, examples can be treated as test apps and executed automatically by using pytest. All examples should be located under ``${IDF_PATH}/examples``, with tested example including a Python test script, for example:

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

Custom Tests are tests that aim to test some arbitrary functionality of ESP-IDF, thus are not intended to demonstrate IDF functionality to users in any way.

All custom test apps are located under ``${IDF_PATH}/tools/test_apps``. For more information please refer to the :idf_file:`Custom Test Readme <tools/test_apps/README.md>`.

pytest in ESP-IDF
=================

.. _pytest-execution-process:

pytest Execution Process
------------------------

1. Bootstrapping Phase

  Create session-scoped caches:

  -  port-target cache
  -  port-app cache

2. Collection Phase

  A. Gather all Python files with the prefix ``pytest_``.
  B. Gather all test functions with the prefix ``test_``.
  C. Apply the `params <https://docs.pytest.org/en/latest/how-to/parametrize.html>`__, and duplicate the test functions.
  D. Filter the test cases with CLI options. For the detailed usages, see :ref:`filter-the-test-cases`.

3. Execution Phase

  A. Construct the `fixtures <https://docs.pytest.org/en/latest/how-to/fixtures.html>`__. In ESP-IDF, the common fixtures are initialized in this order:

    a. ``pexpect_proc``: `pexpect <https://github.com/pexpect/pexpect>`__ instance

    b. ``app``: `IdfApp <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.app.IdfApp>`__ instance

      The test app's information (e.g., sdkconfig, flash_files, partition_table, etc) would be parsed at this phase.

    c. ``serial``: `IdfSerial <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.serial.IdfSerial>`__ instance

      The port of the host to which the target is connected is auto-detected. In the case of multiple targets connected to the host, the test target's type is parsed from the app. The test app binary files are flashed to the test target automatically.

    d. ``dut``: `IdfDut <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.dut.IdfDut>`__ instance

  B. Run the real test function.

  C. Deconstruct the fixtures in this order:

    a. ``dut``

      i. close the ``serial`` port.
      ii. (Only for apps with `Unity test framework <https://github.com/ThrowTheSwitch/Unity>`__) generate JUnit report of the Unity test cases.

    b. ``serial``
    c. ``app``
    d. ``pexpect_proc``: Close the file descriptor

  D. (Only for apps with `Unity test framework <https://github.com/ThrowTheSwitch/Unity>`__)

    If ``dut.expect_from_unity_output()`` is called, an ``AssertionError`` is raised upon detection of a Unity test failure.

4. Reporting Phase

  A. Generate JUnit report of the test functions.
  B. Modify the JUnit report test case name into ESP-IDF test case ID format: ``<target>.<config>.<test function name>``.

5. Finalizatoin Phase (Only for apps with `Unity test framework <https://github.com/ThrowTheSwitch/Unity>`__)

  Combine the JUnit reports if the JUnit reports of the Unity test cases are generated.

Basic Example
-------------

This following Python test script example is taken from :idf_file:`pytest_console_basic.py <examples/system/console/basic/pytest_console_basic.py>`.

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

To demonstrate how pytest is typically used in an ESP-IDF test script, let us go through this simple test script line by line in the following subsections.

Target Markers
^^^^^^^^^^^^^^

Pytest markers can be used to indicate which targets (i.e., which ESP chip) a particular test case should should run on. For example:

.. code:: python

  @pytest.mark.esp32     # <-- support esp32
  @pytest.mark.esp32c3   # <-- support esp32c3
  @pytest.mark.generic   # <-- test env "generic"

The example above indicates that a particular test case is supported on the ESP32 and ESP32-C3. Furthermore, the target's board type should be ``generic``. For more details regarding the ``generic`` type, you may run ``pytest --markers`` to get detailed information regarding all markers.

.. note::

  If the test case can be run on all targets officially supported by ESP-IDF (call ``idf.py --list-targets`` for more details), you can use a special marker ``supported_targets`` to apply all of them in one line.

Parameterized Markers
^^^^^^^^^^^^^^^^^^^^^

You can use ``pytest.mark.parametrize`` with ``config`` to apply the same test to different apps with different sdkconfig files. For more information about ``sdkconfig.ci.xxx`` files, please refer to the Configuration Files section under :idf_file:`this readme <tools/test_apps/README.md>`.

.. code:: python

  @pytest.mark.parametrize('config', [
      'history',     # <-- run with app built by sdkconfig.ci.history
      'nohistory',   # <-- run with app built by sdkconfig.ci.nohistory
  ], indirect=True)  # <-- `indirect=True` is required

Overall, this test function would be replicated to 4 test cases:

-  ``esp32.history.test_console_advanced``
-  ``esp32.nohistory.test_console_advanced``
-  ``esp32c3.history.test_console_advanced``
-  ``esp32c3.nohistory.test_console_advanced``

Testing Serial Output
^^^^^^^^^^^^^^^^^^^^^

To ensure that test has executed successfully on target, the test script can test that serial output of the target using the ``dut.expect()`` function, for example:

.. code:: python

  def test_console_advanced(config: str, dut: IdfDut) -> None:  # The value of argument ``config`` is assigned by the parameterization.
      if config == 'history':
          dut.expect('Command history enabled')
      elif config == 'nohistory':
          dut.expect('Command history disabled')

The ``dut.expect(...)`` will first compile the expected string into regex, which in turn is then used to seek through the serial output until the compiled regex is matched, or until a timeout occurs.

Please pay extra attention to the expected string when it contains regex keyword characters (e.g., parentheses, square brackets). Alternatively, you may use ``dut.expect_exact(...)`` that will attempt to match the string without converting it into regex.

For more information regarding the different types of ``expect`` functions, please refer to the `pytest-embedded Expecting documentation <https://docs.espressif.com/projects/pytest-embedded/en/latest/expecting.html>`__.

Advanced Examples
-----------------

Multi-Target Tests with the Same App
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In some cases a test may involve multiple targets running the same test app. In this case, multiple DUTs can be instantiated using ``parameterize``, for example:

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

Multi-Target Tests with Different Apps
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In some cases (in particular protocol tests), a test may involve multiple targets running different test apps (e.g., separate targets to act as master and slave). In this case, multiple DUTs with different test apps can be instantiated using ``parameterize``.

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

Here the first DUT was flashed with the app :idf_file:`softAP <examples/wifi/getting_started/softAP/main/softap_example_main.c>`, and the second DUT was flashed with the app :idf_file:`station <examples/wifi/getting_started/station/main/station_example_main.c>`.

.. note::

   Here the ``app_path`` should be set with absolute path. The ``__file__`` macro in Python would return the absolute path of the test script itself.

Multi-Target Tests with Different Apps and Targets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This code example is taken from :idf_file:`pytest_wifi_getting_started.py <examples/wifi/getting_started/pytest_wifi_getting_started.py>`. As the comment says, for now it is not running in the ESP-IDF CI.

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

- softAP with ESP32 target, and station with ESP32-S2 target
- softAP with ESP32-S2 target, and station with ESP32 target

Support Different Targets with Different sdkconfig Files
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

Custom Classes
^^^^^^^^^^^^^^

Usually, you may want to write a custom class under these conditions:

1. Add more reusable functions for a certain number of DUTs.
2. Add custom setup and teardown functions in different phases described in Section :ref:`pytest-execution-process`.

This code example is taken from :idf_file:`panic/conftest.py <tools/test_apps/system/panic/conftest.py>`.

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

``monkeypatch_module`` provides a `module-scoped <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ `monkeypatch <https://docs.pytest.org/en/latest/how-to/monkeypatch.html>`__ fixture.

``replace_dut_class`` is a `module-scoped <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ `autouse <https://docs.pytest.org/en/latest/how-to/fixtures.html#autouse-fixtures-fixtures-you-don-t-have-to-request>`__ fixture. This function replaces the ``IdfDut`` class with your custom class.

Mark Flaky Tests
^^^^^^^^^^^^^^^^

Certain test cases are based on Ethernet or Wi-Fi. However, the test may be flaky due to networking issues. Thus, it is possible to mark a particular test case as flaky.

This code example is taken from :idf_file:`pytest_esp_eth.py <components/esp_eth/test_apps/pytest_esp_eth.py>`.

.. code:: python

  @pytest.mark.flaky(reruns=3, reruns_delay=5)
  def test_esp_eth_ip101(dut: IdfDut) -> None:
      ...

This flaky marker means that if the test function failed, the test case would rerun for a maximum of 3 times with 5 seconds delay.

Mark Known Failures
^^^^^^^^^^^^^^^^^^^

Sometimes, a test can consistently fail for the following reasons:

- The feature under test (or the test itself) has a bug.
- The test environment is unstable (e.g., due to network issues) leading to a high failure ratio.

Now you may mark this test case with marker `xfail <https://docs.pytest.org/en/latest/how-to/skipping.html#xfail-mark-test-functions-as-expected-to-fail>`__ with a user-friendly readable reason.

This code example is taken from :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>`

.. code:: python

  @pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
  def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:

This marker means that test is a known failure on the ESP32-S2.

Mark Nightly Run Test Cases
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some test cases are only triggered in nightly run pipelines due to a lack of runners.

.. code:: python

    @pytest.mark.nightly_run

This marker means that the test case would only be run with env var ``NIGHTLY_RUN`` or ``INCLUDE_NIGHTLY_RUN``.

Mark Temporarily Disabled in CI
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some test cases which can pass locally may need to be temporarily disabled in CI due to a lack of runners.

.. code:: python

   @pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2'], reason='lack of runners')

This marker means that the test case could still be run locally with ``pytest --target esp32``, but will not run in CI.

Run Unity Test Cases
^^^^^^^^^^^^^^^^^^^^

For component-based unit test apps, all single-board test cases (including normal test cases and multi-stage test cases) can be run using the following command:

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.run_all_single_board_cases()

Using this command will skip all the test cases containing the ``[ignore]`` tag.

If you need to run a group of test cases, you may run:

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.run_all_single_board_cases(group='psram')

It would trigger all test cases with the ``[psram]`` tag.

You may also see that there are some test scripts with the following statements, which are deprecated. Please use the suggested one as above.

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.expect_exact('Press ENTER to see the list of tests')
      dut.write('*')
      dut.expect_unity_test_output()

For further reading about our unit testing in ESP-IDF, please refer to :doc:`our unit testing guide <../api-guides/unit-tests>`.

Running Tests in CI
===================

The workflow in CI is simple, build jobs > target test jobs.

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

All the build folders would be uploaded as artifacts under the same directories.

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

The binaries in the target test jobs are downloaded from build jobs. the artifacts would be placed under the same directories.

Running Tests Locally
=====================

First you need to install ESP-IDF with additional Python requirements:

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

If you have multiple sdkconfig files in your test app, like those ``sdkconfig.ci.*`` files, the simple ``idf.py build`` won't apply the extra sdkconfig files. Let us take ``$IDF_PATH/examples/system/console/basic`` as an example.

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

.. _filter-the-test-cases:

Filter the Test Cases
---------------------

-  Filter by target with ``pytest --target <target>``

   pytest would run all the test cases that support specified target.

-  Filter by sdkconfig file with ``pytest --sdkconfig <sdkconfig>``

   If ``<sdkconfig>`` is ``default``, pytest would run all the test cases with the sdkconfig file ``sdkconfig.defaults``.

   In other cases, pytest would run all the test cases with sdkconfig file ``sdkconfig.ci.<sdkconfig>``.

Add New Markers
---------------

We are using two types of custom markers, target markers which indicate that the test cases should support this target, and env markers which indicate that the test cases should be assigned to runners with these tags in CI.

You can add new markers by adding one line under the ``${IDF_PATH}/conftest.py``. If it is a target marker, it should be added into ``TARGET_MARKERS``. If it is a marker that specifies a type of test environment, it should be added into ``ENV_MARKERS``. The syntax should be: ``<marker_name>: <marker_description>``.

Generate JUnit Report
---------------------

You can call pytest with ``--junitxml <filepath>`` to generate the JUnit report. In ESP-IDF, the test case name would be unified as ``<target>.<config>.<function_name``.

Skip Auto Flash Binary
----------------------

Skipping auto-flash binary every time would be useful when you are debugging your test script.

You can call pytest with ``--skip-autoflash y`` to achieve it.

Record Statistics
-----------------

Sometimes you may need to record some statistics while running the tests, like the performance test statistics.

You can use `record_xml_attribute <https://docs.pytest.org/en/latest/how-to/output.html?highlight=junit#record-xml-attribute>`__ fixture in your test script, and the statistics would be recorded as attributes in the JUnit report.

Logging System
--------------

Sometimes you may need to add some extra logging lines while running the test cases.

You can use `Python logging module <https://docs.python.org/3/library/logging.html>`__ to achieve this.

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

The above example would log the performance item with pre-defined format: ``[performance][test]: 1`` and record it under the ``properties`` tag in the JUnit report if ``--junitxml <filepath>`` is specified. The JUnit test case node would look like:

.. code:: html

    <testcase classname="examples.get-started.hello_world.pytest_hello_world" file="examples/get-started/hello_world/pytest_hello_world.py" line="13" name="esp32.default.test_hello_world" time="8.389">
        <properties>
            <property name="test" value="1"/>
        </properties>
    </testcase>

``check_performance``
"""""""""""""""""""""

We provide C macros ``TEST_PERFORMANCE_LESS_THAN`` and ``TEST_PERFORMANCE_GREATER_THAN`` to log the performance item and check if the value is in the valid range. Sometimes the performance item value could not be measured in C code, so we also provide a Python function for the same purpose. Please note that using C macros is the preferred approach, since the Python function could not recognize the threshold values of the same performance item under different ifdef blocks well.

.. code:: python

    def test_hello_world(
        dut: IdfDut,
        check_performance: Callable[[str, float, str], None],
    ) -> None:
        check_performance('RSA_2048KEY_PUBLIC_OP', 123, 'esp32')
        check_performance('RSA_2048KEY_PUBLIC_OP', 19001, 'esp32')

The above example would first get the threshold values of the performance item ``RSA_2048KEY_PUBLIC_OP`` from :idf_file:`components/idf_test/include/idf_performance.h` and the target-specific one :idf_file:`components/idf_test/include/esp32/idf_performance_target.h`, then check if the value reached the minimum limit or exceeded the maximum limit.

Let us assume the value of ``IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP`` is 19000. so the first ``check_performance`` line would pass and the second one would fail with warning: ``[Performance] RSA_2048KEY_PUBLIC_OP value is 19001, doesn\'t meet pass standard 19000.0``.

Further Readings
================

-  pytest documentation: https://docs.pytest.org/en/latest/contents.html
-  pytest-embedded documentation: https://docs.espressif.com/projects/pytest-embedded/en/latest/