ESP-IDF Tests with Pytest Guide
===============================

This documentation is a guide that introduces the following aspects:

1. The basic idea of different test types in ESP-IDF
2. How to apply the pytest framework to the test python scripts to make sure the apps are working as expected.
3. ESP-IDF CI target test process
4. Run ESP-IDF tests with pytest locally
5. Tips and tricks on pytest

Disclaimer
----------

In ESP-IDF, we use the following plugins by default:

-  `pytest-embedded <https://github.com/espressif/pytest-embedded>`__ with default services ``esp,idf``
-  `pytest-rerunfailures <https://github.com/pytest-dev/pytest-rerunfailures>`__

All the introduced concepts and usages are based on the default behavior in ESP-IDF. Not all of them are available in vanilla pytest.

Basic Concepts
--------------

Component-based Unit Tests
~~~~~~~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~

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
~~~~~~~~~~~~

Custom Tests are tests that aim to run some arbitrary test internally. They are not intended to demonstrate the ESP-IDF functionality to our customers in any way.

All the test apps should be located under ``${IDF_PATH}/tools/test_apps``. For more information please refer to the :idf_file:`Custom Test Readme <tools/test_apps/README.md>`.

Pytest in ESP-IDF
-----------------

Pytest Execution Process
~~~~~~~~~~~~~~~~~~~~~~~~

1. Bootstrapping Phase

   Create session-scoped caches:

   -  port-target cache
   -  port-app cache

2. Collection Phase

   1. Get all the python files with the prefix ``pytest_``
   2. Get all the test functions with the prefix ``test_``
   3. Apply the `params <https://docs.pytest.org/en/latest/how-to/parametrize.html>`__, duplicate the test functions.
   4. Filter the test cases with CLI options. Introduced detail usages `here <#filter-the-test-cases>`__

3. Test Running Phase

   1. Construct the `fixtures <https://docs.pytest.org/en/latest/how-to/fixtures.html>`__. In ESP-IDF, the common fixtures are initialized with this order:

      1. ``pexpect_proc``: `pexpect <https://github.com/pexpect/pexpect>`__ instance

      2. ``app``: `IdfApp <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.app.IdfApp>`__ instance

         The information of the app, like sdkconfig, flash_files, partition_table, etc., would be parsed at this phase.

      3. ``serial``: `IdfSerial <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.serial.IdfSerial>`__ instance

         The port of the host which connected to the target type parsed from the app would be auto-detected. The flash files would be auto flashed.

      4. ``dut``: `IdfDut <https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.dut.IdfDut>`__ instance

   2. Run the real test function

   3. Deconstruct the fixtures with this order:

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

Example Code
~~~~~~~~~~~~

This code example is taken from :idf_file:`pytest_console_basic.py <examples/system/console/basic/pytest_console_basic.py>`.

.. code:: python

   @pytest.mark.esp32
   @pytest.mark.esp32c3
   @pytest.mark.generic
   @pytest.mark.parametrize('config', [
       'history',
       'nohistory',
   ], indirect=True)
   def test_console_advanced(config: str, dut: Dut) -> None:
       if config == 'history':
           dut.expect('Command history enabled')
       elif config == 'nohistory':
           dut.expect('Command history disabled')

.. note:: 

   Using ``expect_exact`` is better here. For further reading about the different types of ``expect`` functions, please refer to the `pytest-embedded Expecting documentation <https://docs.espressif.com/projects/pytest-embedded/en/latest/expecting>`__.

Use Markers to Specify the Supported Targets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use markers to specify the supported targets and the test env in CI. You can run ``pytest --markers`` to get more details about different markers.

.. code:: python

   @pytest.mark.esp32     # <-- support esp32
   @pytest.mark.esp32c3   # <-- support esp32c3
   @pytest.mark.generic   # <-- test env `generic, would assign to runner with tag `generic`

Besides, if the test case supports all officially ESP-IDF-supported targets, like esp32, esp32s2, esp32s3, esp32c3 for now (2022.2), you can use a special marker ``supported_targets`` to apply them all in one line.

This code example is taken from :idf_file:`pytest_gptimer_example.py <examples/peripherals/timer_group/gptimer/pytest_gptimer_example.py>`.

.. code:: python

   @pytest.mark.supported_targets
   @pytest.mark.generic
   def test_gptimer_example(dut: Dut) -> None:
       ...

Use Params to Specify the sdkconfig Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use ``pytest.mark.parametrize`` with “config” to apply the same test to different apps with different sdkconfig files. For more information about ``sdkconfig.ci.xxx`` files, please refer to the Configuration Files section under :idf_file:`this readme <tools/test_apps/README.md>`.

.. code:: python

   @pytest.mark.parametrize('config', [
       'history',     # <-- run with app built by sdkconfig.ci.history
       'nohistory',   # <-- run with app built by sdkconfig.ci.nohistory
   ], indirect=True)  # <-- `indirect=True` is required

Overall, this test case would be duplicated to 4 test functions:

-  esp32.history.test_console_advanced
-  esp32.nohistory.test_console_advanced
-  esp32c3.history.test_console_advanced
-  esp32c3.nohistory.test_console_advanced

Advanced Examples
~~~~~~~~~~~~~~~~~

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
   def test_esp_eth_ip101(dut: Dut) -> None:
       ...

This flaky marker means that if the test function failed, the test case would rerun for a maximum of 3 times with 5 seconds delay.

Mark Known Failure Cases
^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes a test couldn't pass for the following reasons:

- Has a bug
- Success ratio too low because of environment issue, such as network issue. Retry couldn't help

Now you may mark this test case with marker `xfail <https://docs.pytest.org/en/latest/how-to/skipping.html#xfail-mark-test-functions-as-expected-to-fail>`__ with a user-friendly readable reason.

This code example is taken from :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>`

.. code:: python


   @pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
   def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:

This marker means that if the test would be a known failure one on esp32s2.


Run the Tests in CI
-------------------

The workflow in CI is simple, build jobs -> target test jobs.

Build Jobs
~~~~~~~~~~

Build Job Names
^^^^^^^^^^^^^^^

-  Component-based Unit Tests: ``build_pytest_components_<target>``
-  Example Tests: ``build_pytest_examples_<target>``
-  Custom Tests: ``build_pytest_test_apps_<target>``

Build Job Command
^^^^^^^^^^^^^^^^^

The command used by CI to build all the relevant tests is: ``python $IDF_PATH/tools/ci/build_pytest_apps.py <parent_dir> --target <target> -vv``

All apps which supported the specified target would be built with all supported sdkconfig files under ``build_<target>_<config>``.

For example, If you run ``python $IDF_PATH/tools/ci/build_pytest_apps.py $IDF_PATH/examples/system/console/basic --target esp32``, the folder structure would be like this:

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
~~~~~~~~~~~~~~~~

Target Test Job Names
^^^^^^^^^^^^^^^^^^^^^

-  Component-based Unit Tests: ``component_ut_pytest_<target>_<test_env>``
-  Example Tests: ``example_test_pytest_<target>_<test_env>``
-  Custom Tests: ``test_app_test_pytest_<target>_<test_env>``

Target Test Job Command
^^^^^^^^^^^^^^^^^^^^^^^

The command used by CI to run all the relevant tests is: ``pytest <parent_dir> --target <target> -m <test_env_marker>``

All test cases with the specified target marker and the test env marker under the parent folder would be executed.

The binaries in the target test jobs are downloaded from build jobs, the artifacts would be placed under the same directories.

Run the Tests Locally
---------------------

The local executing process is the same as the CI process.

For example, if you want to run all the esp32 tests under the ``$IDF_PATH/examples/system/console/basic`` folder, you may:

.. code:: shell

   $ pip install pytest-embedded-serial-esp pytest-embedded-idf
   $ cd $IDF_PATH
   $ . ./export.sh
   $ cd examples/system/console/basic
   $ python $IDF_PATH/tools/ci/build_pytest_apps.py . --target esp32 -vv
   $ pytest --target esp32

Tips and Tricks
---------------

Filter the Test Cases
~~~~~~~~~~~~~~~~~~~~~

-  filter by target with ``pytest --target <target>`` 

   pytest would run all the test cases that support specified target.

-  filter by sdkconfig file with ``pytest --sdkconfig <sdkconfig>``

   if ``<sdkconfig>`` is ``default``, pytest would run all the test cases with the sdkconfig file ``sdkconfig.defaults``.

   In other cases, pytest would run all the test cases with sdkconfig file ``sdkconfig.ci.<sdkconfig>``.

Add New Markers
~~~~~~~~~~~~~~~

We’re using two types of custom markers, target markers which indicate that the test cases should support this target, and env markers which indicate that the test case should be assigned to runners with these tags in CI.

You can add new markers by adding one line under the ``${IDF_PATH}/pytest.ini`` ``markers =`` section. The grammar should be: ``<marker_name>: <marker_description>``

Generate JUnit Report
~~~~~~~~~~~~~~~~~~~~~

You can call pytest with ``--junitxml <filepath>`` to generate the JUnit report. In ESP-IDF, the test case name would be unified as "<target>.<config>.<function_name>". 

Skip Auto Flash Binary
~~~~~~~~~~~~~~~~~~~~~~

Skipping auto-flash binary everytime would be useful when you're debugging your test script.

You can call pytest with ``--skip-autoflash y`` to achieve it.

Record Statistics
~~~~~~~~~~~~~~~~~

Sometimes you may need to record some statistics while running the tests, like the performance test statistics.

You can use `record_xml_attribute <https://docs.pytest.org/en/latest/how-to/output.html?highlight=junit#record-xml-attribute>`__ fixture in your test script, and the statistics would be recorded as attributes in the JUnit report.

Logging System
~~~~~~~~~~~~~~

Sometimes you may need to add some extra logging lines while running the test cases.

You can use `python logging module <https://docs.python.org/3/library/logging.html>`__ to achieve this.

Known Limitations and Workarounds
---------------------------------

Avoid Using ``Thread`` for Performance Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``pytest-embedded`` is using some threads internally to help gather all stdout to the pexpect process. Due to the limitation of `Global Interpreter Lock <https://en.wikipedia.org/wiki/Global_interpreter_lock>`__, if you're using threads to do performance tests, these threads would block each other and there would be great performance loss.

**workaround**

Use `Process <https://docs.python.org/3/library/multiprocessing.html#the-process-class>`__ instead, the APIs should be almost the same as ``Thread``.

Further Readings
----------------

-  pytest documentation: https://docs.pytest.org/en/latest/contents.html
-  pytest-embedded documentation: https://docs.espressif.com/projects/pytest-embedded/en/latest/
