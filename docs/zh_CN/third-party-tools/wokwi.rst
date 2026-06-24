.. _wokwi:

Wokwi
#####

:link_to_translation:`en:[English]`

Wokwi 是什么？
~~~~~~~~~~~~~~~~~~~

`Wokwi <https://wokwi.com/esp32>`__ 是一款在线电子仿真器。它支持仿真多数乐鑫芯片以及一系列外围设备和传感器。

通过基于浏览器的操作界面和 IDE 集成，Wokwi 能够帮助你快速开启 IoT 项目编程，支持 Wi-Fi 仿真、虚拟逻辑分析仪、高级 GDB 调试以及自动化测试用的屏幕截图捕捉等功能。

核心功能
~~~~~~~~~~~~

Wokwi 提供一系列面向嵌入式开发的功能：

- **Wi-Fi 仿真**：无需物理硬件即可测试 IoT 项目。
- **虚拟逻辑分析仪**：调试数字信号和时序。
- **高级 GDB 调试**：设置断点和查看变量。
- **VS Code 集成**：直接在 VS Code 中进行开发和仿真。
- **CLion 插件**：通过 `CLion 插件 <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__ 实现专业开发流程。
- **屏幕截图捕获**：支持 CI/CD 自动化视觉测试。
- **自定义芯片 API**：可在主 MCU 外构建自定义虚拟芯片。

.. note::

   仿真结果可能与实际硬件不同。在部署之前，请务必在实际硬件上测试你的项目。

安装
~~~~

Wokwi 提供三种使用方式：

**基于浏览器（在线使用）**
   访问 `wokwi.com <https://wokwi.com/esp32>`__ 即可直接在浏览器中开始仿真，无需安装任何软件。

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

- **wokwi.toml**：配置文件，用于指定固件路径、调试用 ELF 文件及仿真器设置。
- **diagram.json**：电路图文件，用于描述开发板、连接组件及其布线关系。

有关配置文件的详细信息，请参阅 `Wokwi 项目配置指南 <https://docs.wokwi.com/vscode/project-config>`__。

建议同时阅读官方 `ESP32 仿真指南 <https://docs.wokwi.com/guides/esp32>`__，了解 Wokwi 支持的具体开发板、编程语言及相关功能。

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

使用 pytest-embedded 进行测试
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Wokwi 通过 `pytest-embedded-wokwi <https://pypi.org/project/pytest-embedded-wokwi/>`__ 与 ESP-IDF 测试框架集成。实现以下功能：

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

