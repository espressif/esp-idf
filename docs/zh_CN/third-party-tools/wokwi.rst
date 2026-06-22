.. _wokwi:

Wokwi
#####

:link_to_translation:`en:[English]`

Wokwi 是什么？
~~~~~~~~~~~~~~~~~~~

`Wokwi <https://wokwi.com/esp32>`__ 是一款在线电子仿真器。它支持仿真多数乐鑫芯片以及一系列外围设备和传感器。

通过基于浏览器的操作界面和 IDE 集成，Wokwi 能够帮助你快速开启 IoT 项目编程，支持 ESP-IDF 项目、Wi-Fi 仿真、虚拟逻辑分析仪、高级 GDB 调试以及自动化测试用的屏幕截图捕获等功能。

核心功能
~~~~~~~~~~~~

Wokwi 提供一系列面向嵌入式开发的功能：

- **Wi-Fi 仿真**：无需物理硬件即可测试 IoT 项目。
- **虚拟逻辑分析仪**：调试数字信号和时序。
- **高级 GDB 调试**：设置断点和查看变量。
- ``idf.py`` **集成**：使用熟悉的 ``idf.py`` 界面控制 Wokwi（参见 :ref:`使用 idf-wokwi 设置项目 <idf-wokwi-setup>`）。
- **VS Code 集成**：直接在 VS Code 中进行开发和仿真。
- **CLion 插件**：通过 `CLion 插件 <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__ 实现专业开发流程。
- **屏幕截图捕获**：支持 CI/CD 自动化视觉测试。
- **自定义芯片 API**：除主 MCU 之外，还可构建自定义虚拟芯片。

.. note::

   仿真结果可能与实际硬件不同。在部署之前，请务必在实际硬件上测试你的项目。

安装
~~~~

Wokwi 提供以下使用方式：

**基于浏览器（在线使用）**
   访问 `wokwi.com <https://wokwi.com/esp32>`__ 即可直接在浏览器中开始仿真，无需安装任何软件。

**通过** ``idf.py`` **使用** ``idf-wokwi`` **包**
   安装 `idf-wokwi <https://pypi.org/project/idf-wokwi/>`__ 包，可将 Wokwi 控制直接集成到 ``idf.py`` 中。

**VS Code 扩展**
   安装 `Wokwi for VS Code 扩展 <https://docs.wokwi.com/vscode/getting-started>`__，可在开发环境中直接进行仿真。

**CLion 插件**
   安装 `Wokwi Simulator 插件 <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__，可在 CLion 中结合专业 IDE 功能进行嵌入式开发。

配置说明
~~~~~~~~

**使用 wokwi-cli 设置项目**

若要进行本地开发或 CI/CD 集成，可使用 ``wokwi-cli`` 为 ESP-IDF 项目配置 Wokwi 仿真。

安装 ``wokwi-cli`` 请参阅 `Wokwi CLI 官方安装指南 <https://docs.wokwi.com/wokwi-ci/cli-installation>`__。

安装完成后，在 ESP-IDF 项目目录下执行：

.. code-block:: bash

   wokwi-cli init

该命令会引导你完成若干设置，并自动在项目目录中生成必要的配置文件。

Wokwi 项目主要通过两个文件进行配置：

- ``wokwi.toml``：配置文件，用于指定固件路径、调试用 ELF 文件及仿真器设置。
- ``diagram.json``：电路图文件，用于描述开发板、连接组件及其布线关系。

有关配置文件的详细信息，请参阅 `Wokwi 项目配置指南 <https://docs.wokwi.com/vscode/project-config>`__。

建议同时阅读官方 `ESP32 仿真指南 <https://docs.wokwi.com/guides/esp32>`__，了解 Wokwi 支持的具体开发板、编程语言及相关功能。

**使用** ``idf-wokwi`` **设置项目**

.. _idf-wokwi-setup:

与 ``wokwi-cli`` 类似，你可以使用 ``idf-wokwi`` 进行本地开发和 CI/CD 集成。使用 ``idf-wokwi`` 的优势在于，工作流完全保留在 ``idf.py`` 中，且 Wokwi 能够自动从 ESP-IDF 获取信息。这样就无需存在 ``wokwi.toml`` 和 ``diagram.json`` 文件 —— Wokwi 会隐式生成这些文件。

**如何选择** ``wokwi-cli`` **和** ``idf-wokwi``

.. list-table:: Wokwi 集成方案对比
   :header-rows: 1
   :widths: 25 38 37

   * - 功能
     - ``wokwi-cli``
     - ``idf-wokwi``
   * - **配置文件**
     - 必需（``wokwi.toml`` 和 ``diagram.json``）
     - 由 ESP-IDF 自动生成
   * - **构建集成**
     - 手动（需先构建，再仿真）
     - 通过 ``idf.py`` 自动完成
   * - **ESP-IDF 版本**
     - 任意版本
     - 仅支持 6.0 及以上
   * - **适用场景**
     - 非 ESP-IDF 项目、自定义工作流
     - ESP-IDF 6.0+ 项目、原生工作流

前置条件
~~~~~~~~

使用 ``idf-wokwi`` 之前，请确保：

- 使用 ESP-IDF 6.0 或更高版本（``idf.py`` 模块扩展需要此版本）
- 已有 Wokwi API 令牌（在 `Wokwi CI Dashboard <https://wokwi.com/dashboard/ci>`__ 创建）

快速开始
~~~~~~~~

安装并配置 ``idf-wokwi``：

.. code-block:: bash

   # 安装软件包
   pip install idf-wokwi

   # 设置 API 令牌
   export WOKWI_CLI_TOKEN=your_token_here

   # 运行仿真
   idf.py wokwi

.. important::

   ``idf.py`` 的模块扩展仅支持 ESP-IDF 6.0 及更高版本。

.. tip::

   ``idf.py wokwi`` 会在仿真前自动构建项目。若固件已存在，可使用 ``idf.py wokwi --no-build`` 跳过构建。

可用 CLI 选项
~~~~~~~~~~~~~

.. list-table:: ``idf.py wokwi`` 命令选项
   :header-rows: 1
   :widths: 30 70

   * - 选项
     - 说明
   * - ``--diagram-file``
     - ``diagram.json`` 文件路径（默认为项目根目录）
   * - ``--timeout``
     - 仿真超时时间，单位毫秒（超时退出码为 42）
   * - ``--expect-text``
     - 当串口输出中出现此文本时成功退出
   * - ``--fail-text``
     - 当串口输出中出现此文本时错误退出
   * - ``--expect-regex``
     - 当串口输出行匹配此正则表达式时成功退出
   * - ``--fail-regex``
     - 当串口输出行匹配此正则表达式时错误退出

示例输出
~~~~~~~~

运行 ``idf.py wokwi`` 会产生类似以下的输出：

.. code-block:: none

   $ idf.py wokwi
   Running Wokwi simulation...
   Firmware: build/your_project.bin
   ELF: build/your_project.elf
   Simulator ready at: https://wokwi.com/...
   Press Ctrl+C to stop...

   I (123) main: Hello, World!
   I (145) main: System initialized

故障排除
~~~~~~~~

- ``Module not found`` 或 ``No module named 'idf_wokwi'``

   检查 ``idf-wokwi`` 是否安装在与 ESP-IDF 相同的 Python 环境中：

   .. code-block:: bash

      pip show idf-wokwi
      xtensa-esp32-elf-gdb --version  # 验证 ESP-IDF 环境

- ``idf.py: error: no such option: wokwi``

   你的 ESP-IDF 版本低于 6.0，不支持模块扩展功能。请更新 ESP-IDF 或改用 ``wokwi-cli``。

- ``Invalid token`` 或 ``401 Unauthorized``

   检查 ``WOKWI_CLI_TOKEN`` 是否设置正确：

   .. code-block:: bash

      echo $WOKWI_CLI_TOKEN  # 应显示你的令牌，而非为空

   如需要，可在 `Wokwi CI Dashboard <https://wokwi.com/dashboard/ci>`__ 获取新令牌。

CI/CD 集成
~~~~~~~~~~

在 CI/CD 流水线中，``idf-wokwi`` 可与 GitHub Actions 无缝集成：

.. code-block:: yaml

   - name: Simulate with Wokwi
     run: |
       export WOKWI_CLI_TOKEN=${{ secrets.WOKWI_CLI_TOKEN }}
       idf.py wokwi --timeout 30000 --expect-text "Tests passed"

.. note::

   请将 ``WOKWI_CLI_TOKEN`` 作为密钥存储在 CI/CD 平台（如 GitHub Secrets）中。切勿将令牌提交到代码仓库。

如需了解自动化测试框架的相关信息，可参阅下文的 :ref:`wokwi-pytest-embedded` 章节。

更多文档请参阅官方 `Wokwi ESP-IDF 仿真扩展使用指南 <https://docs.wokwi.com/wokwi-ci/idf-wokwi-usage>`__。

IDE 集成
~~~~~~~~~~~~~~~

**VS Code**

在 VS Code 中使用 Wokwi 的步骤：

1. 安装 `Wokwi for VS Code 扩展 <https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode>`__。
2. 在项目中创建 ``wokwi.toml`` 和 ``diagram.json`` 文件。
3. 按下 ``F1``，选择 ``Wokwi: Start Simulator`` 即可开始仿真。

**CLion**

`CLion 的 Wokwi Simulator 插件 <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__ 提供以下功能：

- 与 CLion 嵌入式开发工具集成。
- 专业的调试工作流。
- 支持 ESP-IDF 项目。
- 可从 IDE 内无缝访问仿真器。

**Espressif IDE**

`Espressif IDE <https://developer.espressif.com/blog/how-to-use-wokwi-simulator-with-espressif-ide/>`__ 2.9.0 及以上版本已内置 Wokwi 集成：

- 可在 IDE 中构建应用程序。
- 支持直接烧录到 Wokwi 仿真器。
- 可在 IDE 控制台中查看串口监视器输出，并与仿真器进行通信。

.. _wokwi-pytest-embedded:

使用 pytest-embedded 进行测试
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Wokwi 通过 `pytest-embedded-wokwi <https://pypi.org/project/pytest-embedded-wokwi/>`__ 与 ESP-IDF 测试框架集成。支持以下功能：

- 自动化单元测试和集成测试。
- 与 GitHub Actions 的 CI/CD 流水线集成。
- 支持屏幕截图验证的视觉测试。
- 无需物理硬件的回归测试。

**基本用法**

使用 ``wokwi-cli`` 从命令行运行 Wokwi 仿真：

.. code-block:: bash

   # 运行仿真并期望输出中包含特定文本
   wokwi-cli --timeout 5000 --expect-text "Hello World"

   # 在 4.5 秒后截取屏幕截图
   wokwi-cli --screenshot-part esp --screenshot-time 4500 --screenshot-file screenshot.png

   # 将串口输出保存到文件
   wokwi-cli --serial-log-file output.log --timeout 10000

有关包含 CI/CD 配置的完整项目模板，请参阅 `ESP-IDF Project Template with CI and Wokwi <https://github.com/wokwi/wokwi-esp-test-template>`__。

**相关资源**

- `pytest-embedded-wokwi 文档 <https://pypi.org/project/pytest-embedded-wokwi/>`__
- `ESP-IDF pytest 指南 <https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.2/esp32/contribute/esp-idf-tests-with-pytest.html>`__
- `pytest-embedded 2.x 文档 <https://docs.espressif.com/projects/pytest-embedded>`__

高级功能
~~~~~~~~~~~~~~~~~

**调试**

Wokwi 支持内置 GDB 服务器，可用于高级调试：

1. 在 ``wokwi.toml`` 中添加以下配置：

   .. code-block:: toml

      [wokwi]
      version = 1
      firmware = 'build/your_app.bin'
      elf = 'build/your_app.elf'
      gdbServerPort = 3333

2. 创建 VS Code 调试配置（``.vscode/launch.json``）：

   .. code-block:: json

      {
        "version": "0.2.0",
        "configurations": [
          {
            "name": "Wokwi GDB",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/your_app.elf",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "miDebuggerPath": "xtensa-esp32-elf-gdb",
            "miDebuggerServerAddress": "localhost:3333"
          }
        ]
      }

3. 使用 ``F1`` → ``Wokwi: Start Simulator and Wait for Debugger`` 启动仿真器。
4. 在 VS Code 中按 ``F5`` 附加调试器。

更多详细信息请参阅 `Wokwi 调试指南 <https://docs.wokwi.com/guides/debugger>`__。

**Wi-Fi 网络**

Wokwi 支持 IoT 项目的 Wi-Fi 仿真。相关内容请参阅 `ESP32 Wi-Fi 网络指南 <https://docs.wokwi.com/guides/esp32-wifi>`__，包括：

- 连接 Wi-Fi 网络。
- MQTT、HTTP 和 HTTPS 协议。
- WebSocket 通信。
- 无需物理硬件的网络测试。

**CI/CD 集成**

使用 Wokwi 在 GitHub Actions 中自动化测试：

.. code-block:: yaml

   - name: Simulate & take screenshot
     run: |
       wokwi-cli \
         --screenshot-part "esp" \
         --screenshot-time 5000 \
         --screenshot-file "screenshot-${{ matrix.board }}.png" \
         "boards/${{ matrix.board }}"

   - name: Upload screenshot
     uses: actions/upload-artifact@v4
     with:
       name: screenshot-${{ matrix.board }}
       path: screenshot-${{ matrix.board }}.png

以上配置可实现自动化视觉回归测试，确保项目在不同提交之间表现一致。

更多资源
~~~~~~~~

**视频教程**

- `DevCon24 - Flash Less, Do More: The Magic of Virtual Hardware <https://youtu.be/8PTYnNFK1pw>`__：了解仿真在嵌入式开发中的强大功能

**文档**

- `Wokwi 文档 <https://docs.wokwi.com>`__：涵盖 Wokwi 全部功能的综合资源
- `ESP32 仿真指南 <https://docs.wokwi.com/guides/esp32>`__：介绍支持的开发板、语言和功能
- `ESP32 Wi-Fi 网络 <https://docs.wokwi.com/guides/esp32-wifi>`__：IoT 项目 Wi-Fi 仿真指南
- `VS Code 集成 <https://docs.wokwi.com/vscode/getting-started>`__：设置和配置指南
- `CLion 插件 <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__：CLion 集成详细信息
- `开发者门户上的 Wokwi 文章 <https://developer.espressif.com/tags/wokwi/>`__：汇集教程和用例

**获取帮助**

- `Wokwi 社区 <https://discord.gg/wokwi>`__：加入 Wokwi Discord 服务器，获取社区支持与交流

