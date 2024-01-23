========================
ESP-IDF pytest 指南
========================

:link_to_translation:`en:[English]`

ESP-IDF 有多种类型的测试需在 ESP 芯片上执行（即 **目标测试**）。目标测试通常作为 IDF 测试项目（即 **测试应用程序**）的一部分进行编译，在这个过程中，测试应用程序和其他标准 IDF 项目遵循同样的构建、烧写和监控流程。

通常，目标测试需要连接一台主机（如个人电脑），负责触发特定的测试用例、提供测试数据、检查测试结果。

ESP-IDF 在主机端使用 pytest 框架（以及一些 pytest 插件）来自动进行目标测试。本文档介绍 ESP-IDF 中的 pytest，并介绍以下内容：

1. ESP-IDF 中不同类型的测试应用程序。
2. 将 pytest 框架应用于 Python 测试脚本，进行自动化目标测试。
3. ESP-IDF CI (Continuous Integration) 板载测试流程。
4. 使用 pytest 在本地执行目标测试。
5. pytest 的使用技巧。

.. note::

  ESP-IDF 默认使用以下插件：

  -  `pytest-embedded <https://github.com/espressif/pytest-embedded>`__ 和默认服务 ``esp,idf``
  -  `pytest-rerunfailures <https://github.com/pytest-dev/pytest-rerunfailures>`__

  本文档介绍的所有概念和用法都基于 ESP-IDF 的默认配置，并非都适用于原生 pytest。

安装
============

所有依赖项都可以通过执行安装脚本的 ``--enable-pytest`` 进行安装:

.. code-block:: bash

  $ install.sh --enable-pytest


安装过程常见问题
----------------------------

No Package 'dbus-1' Found
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: text

  configure: error: Package requirements (dbus-1 >= 1.8) were not met:

  No package 'dbus-1' found

  Consider adjusting the PKG_CONFIG_PATH environment variable if you
  installed software in a non-standard prefix.

如果遇到上述错误信息，可能需要安装一些缺失的软件包。

如果使用 Ubuntu 系统，可能需要执行：

.. code:: shell

  sudo apt-get install libdbus-glib-1-dev

或

.. code:: shell

  sudo apt-get install libdbus-1-dev

如使用 Linux 其他发行版本，请在搜索引擎中查询上述错误信息，并查找对应发行版需安装哪些缺失的软件包。

Invalid command 'bdist_wheel'
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: text

  error: invalid command 'bdist_wheel'

如果遇到上述错误信息，可能需要安装一些缺失的 Python 包，例如：

.. code:: shell

  python -m pip install -U pip

或

.. code:: shell

  python -m pip install wheel

.. note::

  执行 pip 命令前，请确保使用的环境为 IDF Python 虚拟环境。


测试应用程序
==================

ESP-IDF 包含不同类型的测试应用程序，可用 pytest 自动完成。

组件测试
----------

ESP-IDF 组件通常包含针对特定组件的测试应用程序，执行针对特定组件的单元测试。推荐通过组件测试应用程序来测试组件。所有测试应用程序都应位于 ``${IDF_PATH}/components/<COMPONENT_NAME>/test_apps`` 下，例如：

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

例程测试
-------------

例程测试是为了向用户展示 ESP-IDF 的部分功能（要了解更多信息，请参考 :idf_file:`Examples Readme <examples/README.md>`）。

但是，要确保这些例程正确运行，可将例程看作测试应用，并用 pytest 自动执行。所有例程都应和已测试的例程，包括 Python 测试脚本一起放在 ``${IDF_PATH}/examples`` 路径下，例如：

.. code:: text

  examples/
  └── parent_folder/
      └── example_1/
          ├── main/
          │   └── ...
          ├── CMakeLists.txt
          └── pytest_example_1.py

自定义测试
--------------

自定义测试是为了测试 ESP-IDF 的一些任意功能，这些测试不是为了向用户展示 ESP-IDF 的功能。

所有自定义测试应用都位于 ``${IDF_PATH}/tools/test_apps`` 路径下。要了解更多信息，请参考 :idf_file:`Custom Test Readme <tools/test_apps/README.md>`。

在 ESP-IDF 中使用 pytest
============================

.. _pytest-execution-process:

pytest 执行步骤
---------------------

1. 引导阶段

创建会话缓存：

    - 端口目标缓存
    - 端口应用缓存

2. 数据获取阶段

    A. 获取所有前缀为 ``pytest_`` 的 Python 文件。
    B. 获取所有前缀为 ``test_`` 的测试函数。
    C. 应用 `params <https://docs.pytest.org/en/latest/how-to/parametrize.html>`__，并复制测试函数。
    D. 利用 CLI 选项筛选测试用例。详细用法请参考 :ref:`filter-the-test-cases`。

3. 运行阶段

    A. 创建 `fixture <https://docs.pytest.org/en/latest/how-to/fixtures.html>`__。在 ESP-IDF 中，常见 fixture 的初始化顺序如下：

        a. ``pexpect_proc``: `pexpect <https://github.com/pexpect/pexpect>`__ 实例

        b. ``app``: `IdfApp <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.app.IdfApp>`__ 实例

            此阶段会解析测试应用程序的相关信息，如 sdkconfig、flash_files、partition_table 等。

        c. ``serial``: `IdfSerial <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.serial.IdfSerial>`__ 实例

            此阶段会自动检测目标芯片所连接的主机端口。考虑到主机可能连接了多个目标芯片，应用程序会解析测试目标芯片的类型。测试程序的二进制文件会自动烧写到测试目标芯片上。

        d. ``dut``: `IdfDut <https://docs.espressif.com/projects/pytest-embedded/en/latest/api.html#pytest_embedded_idf.dut.IdfDut>`__ 实例

    B. 运行测试函数。

    C. 析构 fixture。析构顺序如下：

        a. ``dut``

            i. 关闭 ``serial`` 端口。
            ii. （仅适用于使用了 `Unity 测试框架 <https://github.com/ThrowTheSwitch/Unity>`__ 的应用程序）生成 Unity 测试用例的 JUnit 报告。

        b. ``serial``
        c. ``app``
        d. ``pexpect_proc``：关闭文件描述符

    D.（仅适用于使用了 `Unity 测试框架 <https://github.com/ThrowTheSwitch/Unity>`__ 的应用程序）

        如果调用了 ``dut.expect_from_unity_output()``，那么检测到 Unity 测试失败时会触发 ``AssertionError``。

4. 报告阶段

    A. 为测试函数生成 Junit 报告。
    B. 将 JUnit 报告中的测试用例名修改为 ESP-IDF 测试用例 ID 格式： ``<target>.<config>.<test function name>``。

5. 完成阶段（仅适用于使用了 `Unity 测试框架 <https://github.com/ThrowTheSwitch/Unity>`__ 的应用程序）

    如果生成了 Unity 测试用例的 JUnit 报告，这些报告会被合并。

入门示例
-------------

以下 Python 测试脚本示例来自 :idf_file:`pytest_console_basic.py <examples/system/console/basic/pytest_console_basic.py>`。

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

下面的小节对这个简单的测试脚本进行了逐行讲解，以说明 pytest 在 ESP-IDF 测试脚本中的典型使用方法。

目标芯片 marker
^^^^^^^^^^^^^^^^^^

使用 Pytest marker 可以指出某个特定测试用例应在哪个目标芯片（即 ESP 芯片）上运行。例如：

.. code:: python

  @pytest.mark.esp32     # <-- support esp32
  @pytest.mark.esp32c3   # <-- support esp32c3
  @pytest.mark.generic   # <-- test env "generic"

上例表明，某一测试用例可以在 ESP32 和 ESP32-C3 上运行。此外，目标芯片的类型应为 ``generic``。要了解有关 ``generic`` 类型，运行 ``pytest --markers`` 以获取所有 marker 的详细信息。

.. note::

  如果测试用例可以在 ESP-IDF 官方支持的所有目标芯片上运行（调用 ``idf.py --list-targets`` 了解详情），则可以使用特殊 marker ``supported_targets`` 指定所有目标芯片。

参数化 marker
^^^^^^^^^^^^^^^^

可使用 ``pytest.mark.parametrize`` 和 ``config`` 参数对包含不同 sdkconfig 文件的不同应用程序进行相同的测试。如需了解关于 ``sdkconfig.ci.xxx`` 文件的更多信息，请参考 :idf_file:`readme <tools/test_apps/README.md>` 下的 Configuration Files 章节。

.. code:: python

  @pytest.mark.parametrize('config', [
      'history',     # <-- run with app built by sdkconfig.ci.history
      'nohistory',   # <-- run with app built by sdkconfig.ci.nohistory
  ], indirect=True)  # <-- `indirect=True` is required

总体而言，这一测试函数会复制为 4 个测试用例：

-  ``esp32.history.test_console_advanced``
-  ``esp32.nohistory.test_console_advanced``
-  ``esp32c3.history.test_console_advanced``
-  ``esp32c3.nohistory.test_console_advanced``

测试串行输出
^^^^^^^^^^^^^^^^

为确保测试在目标芯片上顺利执行，测试脚本可使用 ``dut.expect()`` 函数来测试目标芯片上的串行输出：

.. code:: python

  def test_console_advanced(config: str, dut: IdfDut) -> None:  # The value of argument ``config`` is assigned by the parameterization.
      if config == 'history':
          dut.expect('Command history enabled')
      elif config == 'nohistory':
          dut.expect('Command history disabled')

在执行 ``dut.expect(...)`` 时，首先会将预期字符串编译成正则表达式用于搜索串行输出结果，直到找到与该编译后的正则表达式匹配的结果或运行超时。

如果预期字符串中包含正则表达式关键字（如括号或方括号），则需格外注意。或者，也可以使用 ``dut.expect_exact(...)``，它会尝试直接匹配字符串，而不将其转换为正则表达式。

如需了解关于 ``expect`` 函数类型的更多信息，请参考 `pytest-embedded Expecting documentation <https://docs.espressif.com/projects/pytest-embedded/en/latest/expecting.html>`__。

进阶示例
----------------

用同一应用程序进行多个 DUT 测试
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

有时，一个测试可能涉及多个运行同一测试程序的目标芯片。在这种情况下，可以使用 ``parameterize`` 将多个 DUT 实例化，例如：

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

将参数 ``count`` 设置为 2 后，所有 fixture 都会改为元组。

用不同应用程序进行多个 DUT 测试
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

有时（特别是协议测试），一个测试可能涉及多个运行不同测试程序的目标芯片（例如不同目标芯片作为主机和从机）。在这种情况下，可以使用 ``parameterize`` 将针对不同测试应用程序的多个 DUT 实例化。

以下代码示例来自 :idf_file:`pytest_wifi_getting_started.py <examples/wifi/getting_started/pytest_wifi_getting_started.py>`。

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

以上示例中，第一个 DUT 用 :idf_file:`softAP <examples/wifi/getting_started/softAP/main/softap_example_main.c>` 应用程序烧录，第二个 DUT 用 :idf_file:`station <examples/wifi/getting_started/station/main/station_example_main.c>` 应用程序烧录。

.. note::

    这里的 ``app_path`` 应设置为绝对路径。Python 中的 ``__file__`` 宏会返回测试脚本自身的绝对路径。

用不同应用程序和目标芯片进行多目标测试
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下代码示例来自 :idf_file:`pytest_wifi_getting_started.py <examples/wifi/getting_started/pytest_wifi_getting_started.py>`。如注释所述，该代码目前尚未在 ESP-IDF CI 中运行。

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

总体而言，此测试函数会被复制为 2 个测试用例：

- 在 ESP32 上烧录 softAP，在 ESP32-S2 上烧录 station
- 在 ESP32-S2 上烧录 softAP，在 ESP32 上烧录 station

支持对不同 sdkconfig 文件及目标芯片的组合测试
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下进阶代码示例来自 :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>`。

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

自定义类
^^^^^^^^^^^^

通常，可能会在下列情况下编写自定义类：

1. 向一定数量的 DUT 添加更多可复用功能。
2. 为不同阶段添加自定义的前置和后置函数，请参考章节 :ref:`pytest-execution-process`。

以下代码示例来自 :idf_file:`panic/conftest.py <tools/test_apps/system/panic/conftest.py>`。

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

``monkeypatch_module`` 提供了一个 `基于模块 <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ 的 `monkeypatch <https://docs.pytest.org/en/latest/how-to/monkeypatch.html>`__ fixture。

``replace_dut_class`` 是一个 `基于模块 <https://docs.pytest.org/en/latest/how-to/fixtures.html#scope-sharing-fixtures-across-classes-modules-packages-or-session>`__ 的 `自动执行 <https://docs.pytest.org/en/latest/how-to/fixtures.html#autouse-fixtures-fixtures-you-don-t-have-to-request>`__ fixture。 该函数会用你的自定义类替换 ``IdfDut`` 类。

标记不稳定测试
^^^^^^^^^^^^^^^^^^^^^^^^^^

某些测试用例基于以太网或 Wi-Fi。然而由于网络问题，测试可能会不稳定。此时，可以将某个测试用例标记为不稳定的测试用例。

以下代码示例来自 :idf_file:`pytest_esp_eth.py <components/esp_eth/test_apps/pytest_esp_eth.py>`。

.. code:: python

  @pytest.mark.flaky(reruns=3, reruns_delay=5)
  def test_esp_eth_ip101(dut: IdfDut) -> None:
      ...

这一 marker 表示，如果该测试函数失败，其测试用例会每隔 5 秒钟再运行一次，最多运行三次。

标记已知失败
^^^^^^^^^^^^^^^^^^^^^^^^

有时，测试会因以下原因而持续失败：

- 测试的功能（或测试本身）存在错误。
- 测试环境不稳定（例如网络问题），导致失败率较高。

可使用 `xfail <https://docs.pytest.org/en/latest/how-to/skipping.html#xfail-mark-test-functions-as-expected-to-fail>`__ marker 来标记此测试用例，并写出原因。

以下代码来自 :idf_file:`pytest_panic.py <tools/test_apps/system/panic/pytest_panic.py>`。

.. code:: python

  @pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
  def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:

这一 marker 表示该测试在 ESP32-S2 上是一个已知失败。

标记夜间运行的测试用例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在缺少 runner 时，一些测试用例仅在夜间运行的管道中触发。

.. code:: python

    @pytest.mark.nightly_run

这一 marker 表示，此测试用例仅在环境变量为 ``NIGHTLY_RUN`` 或 ``INCLUDE_NIGHTLY_RUN`` 时运行。

标记在 CI 中暂时禁用的测试用例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在缺少 runner 时，可以在 CI 中禁用一些本地能够通过测试的测试用例。

.. code:: python

   @pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2'], reason='lack of runners')

这一 marker 表明，此测试用例仍可以在本地用 ``pytest --target esp32`` 执行，但不会在 CI 中执行。

运行 Unity 测试用例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

对基于组件的单元测试应用程序，以下代码即可执行所有的单板测试用例，包括普通测试用例和多阶段测试用例：

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.run_all_single_board_cases()

此代码还会跳过所有 tag 为 ``[ignore]`` 的测试用例。

如需按组执行测试用例，可运行：

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.run_all_single_board_cases(group='psram')

此代码会触发模块包含 ``[psram]`` tag 的所有测试用例。

你可能还会看到一些包含以下语句的测试脚本，这些脚本已被弃用。请使用上述建议的方法。

.. code:: python

  def test_component_ut(dut: IdfDut):
      dut.expect_exact('Press ENTER to see the list of tests')
      dut.write('*')
      dut.expect_unity_test_output()

如需了解关于 ESP-IDF 单元测试的更多内容，请参考 :doc:`../api-guides/unit-tests`。

在 CI 中执行测试
======================

CI 的工作流程很简单，即 编译任务 -> 板载测试任务。

编译任务
--------------

编译任务命名
^^^^^^^^^^^^^^^^^

- 基于组件的单元测试： ``build_pytest_components_<target>``
- 例程测试： ``build_pytest_examples_<target>``
- 自定义测试： ``build_pytest_test_apps_<target>``

编译任务命令
^^^^^^^^^^^^^^^^^

CI 用于创建所有相关测试的命令为： ``python $IDF_PATH/tools/ci/ci_build_apps.py <parent_dir> --target <target> -vv --pytest-apps``

所有支持指定目标芯片的应用程序都使用 ``build_<target>_<config>`` 下支持的 sdkconfig 文件创建。

例如，如果运行 ``python $IDF_PATH/tools/ci/ci_build_apps.py $IDF_PATH/examples/system/console/basic --target esp32 --pytest-apps`` 指令，文件夹结构将如下所示：

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

所有编译文件的文件夹都会上传到同一路径。

板载测试任务
----------------

板载测试任务命名
^^^^^^^^^^^^^^^^^^^

-  基于部件的单元测试： ``component_ut_pytest_<target>_<test_env>``
-  例程测试： ``example_test_pytest_<target>_<test_env>``
-  自定义测试： ``test_app_test_pytest_<target>_<test_env>``

板载测试任务命令
^^^^^^^^^^^^^^^^^^^^^^^^^

CI 用于执行所有相关测试的命令为： ``pytest <parent_dir> --target <target> -m <test_env_marker>``

这一命令将执行父文件夹下所有具有指定目标芯片 marker 和测试环境 marker 的测试用例。

板载测试任务中的二进制文件是从编译任务中下载的，相应文件会放在同一路径下。

本地测试
==========

首先，你需为 ESP-IDF 安装 Python 依赖：

.. code-block:: shell

  $ cd $IDF_PATH
  $ bash install.sh --enable-pytest
  $ . ./export.sh

默认情况下，pytest 脚本会按照以下顺序查找编译目录：

- ``build_<target>_<sdkconfig>``
- ``build_<target>``
- ``build_<sdkconfig>``
- ``build``

因此，运行 pytest 最简单的方式是调用 ``idf.py build``。

例如，如果你要执行 ``$IDF_PATH/examples/get-started/hello_world`` 文件夹下的所有 ESP32 测试，你可执行：

.. code-block:: shell

  $ cd examples/get-started/hello_world
  $ idf.py build
  $ pytest --target esp32

如果你的测试应用程序中有多个 sdkconfig 文件，例如那些 ``sdkconfig.ci.*`` 文件， 仅使用 ``idf.py build`` 命令并不能调用这些额外的 sdkconfig 文件。下文以 ``$IDF_PATH/examples/system/console/basic`` 为例进行说明。

如果要用 ``history`` 配置测试此应用程序，并用 ``idf.py build`` 进行编译，你需运行：

.. code-block:: shell

  $ cd examples/system/console/basic
  $ idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.history" build
  $ pytest --target esp32 --sdkconfig history

如果你想同时编译测试所有 sdkconfig 文件，则需运行我们的 CI 脚本 (ci_build_apps.py) 作为辅助脚本：

.. code-block:: shell

  $ cd examples/system/console/basic
  $ python $IDF_PATH/tools/ci/ci_build_apps.py . --target esp32 -vv --pytest-apps
  $ pytest --target esp32

包含 ``sdkconfig.ci.history`` 配置的应用程序会编译到 ``build_esp32_history`` 中，而包含 ``sdkconfig.ci.nohistory`` 配置的应用程序会编译到 ``build_esp32_nohistory`` 中。 ``pytest --target esp32`` 命令会在这两个应用程序上运行测试。

使用技巧
============

.. _filter-the-test-cases:

筛选测试用例
---------------------

- 根据目标芯片筛选： ``pytest --target <target>``

   pytest 会执行所有支持指定目标芯片的测试用例。

- 根据 sdkconfig 文件筛选： ``pytest --sdkconfig <sdkconfig>``

   如果 ``<sdkconfig>`` 为 ``default``，pytest 会执行所有 sdkconfig 文件包含 ``sdkconfig.defaults`` 的测试用例。

   如果是其他情况，pytest 会执行所有 sdkconfig 文件包含 ``sdkconfig.ci.<sdkconfig>`` 的测试用例。

- 使用 ``pytest -k <test-case name>`` 按测试用例名称筛选，可以运行单个测试用例，例如 ``pytest -k test_int_wdt_cache_disabled``。

添加新 marker
----------------

我们目前使用两种自定义 marker。target marker 是指测试用例支持此目标芯片，env marker 是指测试用例应分配到 CI 中具有相应 tag 的 runner 上。

你可以在 ``${IDF_PATH}/conftest.py`` 文件后添加一行新的 marker。如果该 marker 是 target marker，应将其添加到 ``TARGET_MARKERS`` 中。如果该 marker 指定了一类测试环境，应将其添加到 ``ENV_MARKERS`` 中。自定义 marker 格式： ``<marker_name>: <marker_description>``。

生成 JUnit 报告
---------------------

你可调用 pytest 执行 ``--junitxml <filepath>`` 生成 JUnit 报告。在 ESP-IDF 中，测试用例命名会统一为 ``<target>.<config>.<function_name>``。

跳过自动烧录二进制文件
-------------------------------------

调试测试脚本时最好跳过自动烧录二进制文件。

调用 pytest 执行 ``--skip-autoflash y`` 即可实现。

记录数据
--------------

在执行测试时，你有时需要记录一些数据，例如性能测试数据。

在测试脚本中使用 `record_xml_attribute <https://docs.pytest.org/en/latest/how-to/output.html?highlight=junit#record-xml-attribute>`__ fixture，数据就会记录在 JUnit 报告的属性中。

日志系统
------------

在执行测试用例时，你有时可能需要添加一些额外的日志行。

这可通过使用 `Python 日志模块 <https://docs.python.org/3/library/logging.html>`__ 实现。

其他日志函数（作为 fixture）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``log_performance``
"""""""""""""""""""

.. code:: python

    def test_hello_world(
        dut: IdfDut,
        log_performance: Callable[[str, object], None],
    ) -> None:
        log_performance('test', 1)

以上示例可实现用预定义格式 ``[performance][test]: 1`` 记录性能数据，并在指定 ``--junitxml <filepath>`` 的情况下将其记录在 JUnit 报告的 ``properties`` tag 下。相应的 JUnit 测试用例节点如下所示：

.. code:: html

    <testcase classname="examples.get-started.hello_world.pytest_hello_world" file="examples/get-started/hello_world/pytest_hello_world.py" line="13" name="esp32.default.test_hello_world" time="8.389">
        <properties>
            <property name="test" value="1"/>
        </properties>
    </testcase>

``check_performance``
"""""""""""""""""""""

我们提供了 ``TEST_PERFORMANCE_LESS_THAN`` 和 ``TEST_PERFORMANCE_GREATER_THAN`` 宏来记录性能项，并检测性能项的数值是否在有效范围内。有时 C 宏无法检测一些性能项的值，为此，我们提供了 Python 函数实现相同的目的。注意，由于该 Python 函数不能很好地识别不同的 ifdef 块下同一性能项的阈值，请尽量使用 C 宏。

.. code:: python

    def test_hello_world(
        dut: IdfDut,
        check_performance: Callable[[str, float, str], None],
    ) -> None:
        check_performance('RSA_2048KEY_PUBLIC_OP', 123, 'esp32')
        check_performance('RSA_2048KEY_PUBLIC_OP', 19001, 'esp32')

以上示例会首先从 :idf_file:`components/idf_test/include/idf_performance.h` 和指定目标芯片的 :idf_file:`components/idf_test/include/esp32/idf_performance_target.h` 头文件中获取性能项 ``RSA_2048KEY_PUBLIC_OP`` 的阈值，然后检查该值是否达到了最小值或超过了最大值。

例如，假设 ``IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP`` 的值为 19000，则上例中第一行 ``check_performance`` 会通过测试，第二行会失败并警告： ``[Performance] RSA_2048KEY_PUBLIC_OP value is 19001, doesn\'t meet pass standard 19000.0``。

扩展阅读
=============

-  pytest：https://docs.pytest.org/en/latest/contents.html
-  pytest-embedded：https://docs.espressif.com/projects/pytest-embedded/en/latest/
