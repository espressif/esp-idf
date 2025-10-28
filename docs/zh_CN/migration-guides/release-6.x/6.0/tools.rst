工具
====

:link_to_translation:`en:[English]`

安装 gdbgui
-----------

安装脚本中已移除对 ``--enable-gdbgui`` 参数的支持，因此不再能通过该方式安装 `gdbgui <https://www.gdbgui.com>`_。请参考 `gdbgui 安装指南 <https://www.gdbgui.com/installation/>`_ 使用 ``pipx`` 方式进行安装。

根据操作系统和 Python 版本的不同，可能需要参考 `已知问题列表 <https://github.com/cs01/gdbgui/issues>`_。

例如，目前所有操作系统均 `不支持 Python 3.13 <https://github.com/cs01/gdbgui/issues/494>`_。

gdbgui 自 v0.14 起不再支持 Windows 操作系统。由于其他兼容性问题，Windows 下需使用 Python 3.10，并配合特定依赖版本。可通过以下命令安装已知可用的 gdbgui 及其所需版本的依赖：

.. code-block:: bash

    pipx install "gdbgui==0.13.2.0" "pygdbmi<=0.9.0.2" "python-socketio<5" "jinja2<3.1" "itsdangerous<2.1"

如果操作系统为 Linux 或 macOS，可以使用 Python 3.11 或 3.12 以及 gdbgui v0.15.2.0。

请注意，以上推荐的版本可能会发生变化，请按需查阅 `官方问题追踪页面 <https://github.com/cs01/gdbgui/issues>`_ 查看最新问题列表。

如果安装过程中遇到问题，建议使用 ``idf.py gdb`` 代替 ``idf.py gdbgui``，或使用 Eclipse 或 VSCode 进行调试。

``idf.py efuse*`` 命令需要指定串口
-----------------------------------

所有 eFuse 相关命令现在都必须指定串口。这样做是为了防止意外使用错误的端口，因为这些操作是不可逆的。

对所有 ``idf.py efuse*`` 命令，需使用 ``--port`` 参数（或设置 ``ESPPORT`` 环境变量）明确指定串口。未指定时，命令将报错并终止执行。

Python 3.9 弃用说明
----------------------

Python 3.9 已不再受支持。现在要求的最低 Python 版本为 3.10。请将你的 Python 升级至 3.10 或更高版本，或者使用 Docker 镜像：:ref:`idf-docker-image`。

参考 `Python 官方文档 <https://docs.python.org/3/using/index.html>`_ 获取不同操作系统的 Python 升级指南。

对于 Linux 用户，建议升级到包含受支持 Python 版本的较新发行版。

配置文件语法变更
----------------

ESP-IDF v6 使用了 esp-idf-kconfig v3，对配置文件 ``Kconfig`` 的语法进行了若干变更。完整变更列表请参阅 `Migration Guide From esp-idf-kconfig v2.x to v3.x <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/developer-guide/migration-guide.html>`_。

CMake 版本升级
---------------------

最低支持的 CMake 版本已升级至 3.22.1。如果你在构建过程中遇到与此相关的问题，建议将操作系统升级到包含 CMake 3.22 或更高版本。

如果无法升级操作系统，可以使用以下命令安装推荐的 CMake 版本：``./tools/idf_tools.py install cmake``。

此变更影响所有使用系统自带 CMake 或自定义 CMake 的 ESP-IDF 用户。

移除 ``idf.py size --legacy`` 选项
-------------------------------------

``idf.py size`` 命令的 ``--legacy`` 参数已被移除，因为旧版实现已不再受支持。``ESP_IDF_SIZE_LEGACY`` 环境变量也随之失效。如需继续使用旧版选项，请使用 ESP-IDF 5.5 或更早版本。对于 ESP-IDF v6.0 及更高版本，请直接将 ``idf.py size --legacy`` 替换为 ``idf.py size``。若遇到 esp-idf-size 版本问题，请运行安装脚本更新至正确版本。

更改 ``idf.py size --format json`` 为 ``--format json2``
------------------------------------------------------------

``--format json`` 选项已替换为 ``--format json2``。``json2`` 格式提供了更完善的结构，为每个内存区域提供明确的 ``total``、``used`` 和 ``free`` 字段，并在 ``parts`` 部分提供详细分类。迁移时，请将 ``idf.py size --format json`` 替换为 ``idf.py size --format json2``。

- **旧格式 (json)**：扁平结构，内存类型字段直接列出，如 ``"dram_data": 9192, "iram_text": 43295``。
- **新格式 (json2)**：分层结构，包含一个 ``layout`` 数组来表示内存区域：

    .. code-block:: json

        {
            "version": "1.1",
            "layout": [
                {
                    "name": "DRAM",
                    "total": 180736,
                    "used": 11344,
                    "free": 169392,
                    "parts": {
                        ".data": {"size": 9192},
                        ".bss": {"size": 2152}
                    }
                }
            ]
        }

