工具
====

:link_to_translation:`en:[English]`

ESP-IDF 监视器
---------------

ESP-IDF 监视器在波特率方面的改动如下：

- 目前，ESP-IDF 监视器默认遵循自定义的控制台波特率 (:ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`)，而非 115200。
- ESP-IDF v5.0 不再支持通过 menuconfig 自定义波特率。
- 支持通过设置环境变量或在命令行中使用 ``idf.py monitor -b <baud>`` 命令自定义波特率。
- 注意，为了与全局波特率 ``idf.py -b <baud>`` 保持一致，波特率参数已从 ``-B`` 改名为 ``-b``。请运行 ``idf.py monitor --help`` 获取更多信息。

废弃指令
-------------------

ESP-IDF v5.0 已将 ``idf.py`` 子命令和 ``cmake`` 目标名中的下划线 (``_``) 统一为连字符 (``-``)。使用废弃的子命令及目标名将会触发警告，建议使用更新后的版本。具体改动如下：

.. list-table:: 废弃子命令及目标名
   :widths: 50 50
   :header-rows: 1

   * - 废弃名
     - 现用名
   * - efuse_common_table
     - efuse-common-table
   * - efuse_custom_table
     - efuse-custom-table
   * - erase_flash
     - erase-flash
   * - partition_table
     - partition-table
   * - partition_table-flash
     - partition-table-flash
   * - post_debug
     - post-debug
   * - show_efuse_table
     - show-efuse-table
   * - erase_otadata
     - erase-otadata
   * - read_otadata
     - read-otadata

Esptool
-------

``CONFIG_ESPTOOLPY_FLASHSIZE_DETECT`` 选项已重命名为 :ref:`CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE`，且默认禁用。迁移到 ESP-IDF v5.0 的新项目和现有项目必须设置 :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`。若因编译时 flash 大小未知而无法设置，可启用 :ref:`CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE`。但需注意的是，启用该项后，为在烧录期间使用 flash 大小更新二进制头时不会导致摘要无效，映像后将不再附加 SHA256 摘要。

Windows 环境
-------------

基于 MSYS/MinGW 的 Windows 环境支持已在 ESP-IDF v4.0 中弃用，v5.0 则完全移除了该项服务。请使用 :ref:`get-started-windows-tools-installer` 设置 Windows 兼容环境。目前支持 Windows 命令行、Power Shell 和基于 Eclipse IDE 的图形用户界面等选项。此外，还可以使用 `支持的插件 <https://github.com/espressif/vscode-esp-idf-extension>`_，设置基于 VSCode 的环境。
