semihosting 功能
----------------

借助 semihosting 机制，在目标设备上运行的程序可以使用调试器所在主机上的 I/O 功能。该机制适用于嵌入式应用的调试与测试，而无需在目标端实现特定硬件相关的 I/O 功能。

OpenOCD 为乐鑫目标芯片实现了扩展的 semihosting 协议，其功能超出了标准 ARM semihosting 规范。这使嵌入式应用能够与主机系统进行交互，以执行文件操作、目录管理以及其他系统调用。

.. warning::

    每个 semihosting 调用都通过包含软件断点指令的序列实现。如果包含 semihosting 调用的程序在 **未连接调试器** 的情况下运行，则会触发异常。

.. note::

    每个 semihosting 调用都会暂停 CPU，直到主机返回结果。因此，semihosting 不适用于对延迟敏感或对实时性要求较高的代码路径。


.. _jtag-debugging-semihosting-available-operations:

支持的操作
^^^^^^^^^^

头文件 :idf_file:`components/vfs/openocd_semihosting.h` 声明了所有可用的 semihosting 操作。常见操作包括：

* **文件操作**：``open``、``close``、``read``、``write``、``lseek``、``fsync``、``link``、``unlink``
* **目录操作**：``opendir``、``readdir``、``seekdir``、``telldir``、``closedir``、``mkdir``、``rmdir``
* **文件属性操作**：``rename``、``truncate``、``fstat``、``stat``、``utime``、``access``

此外，目标端还可以使用调试钩子触发由 OpenOCD 直接处理的事件：

* ``panic_reason``：直接在调试器控制台中向用户输出详细的 panic 信息。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

   * ``breakpoint_set``、``watchpoint_set``：允许在目标端配置断点和观察点，而无需用户手动操作。


.. _jtag-debugging-semihosting-using-from-app:

在应用中使用 semihosting
^^^^^^^^^^^^^^^^^^^^^^^^

在应用代码中使用 semihosting 最便捷的方法是通过虚拟文件系统 (VFS) 驱动。调用 :cpp:func:`esp_vfs_semihost_register` 可将主机目录挂载为普通 VFS 路径，从而无需额外适配即可使用 ``fopen``、``read``、``write`` 等标准接口：

.. code-block:: c

    #include "esp_vfs_semihost.h"

    esp_vfs_semihost_register("/host");
    FILE *f = fopen("/host/log.txt", "w");

完整使用流程请参考 :doc:`虚拟文件系统组件 API 参考 <../../api-reference/storage/vfs>` 以及 :example:`storage/semihost_vfs` 示例。

也可以参阅 `OpenOCD semihosting 测试应用 <https://github.com/espressif/openocd-esp32/blob/master/testing/esp/test_apps/gen_ut_app/main/semihost_tests.c>`_。

.. _jtag-debugging-semihosting-configuration:

配置
^^^^

默认情况下，semihosting 文件操作会使用当前目录（即启动 OpenOCD 时所在的目录）作为基础目录。若需指定其他基础目录，请在 OpenOCD 启动命令开头添加额外参数 ``-c 'set ESP_SEMIHOST_BASEDIR /path/to/semihost/root'``，详见 :ref:`jtag-debugging-tip-openocd-config-vars`。

.. _jtag-debugging-semihosting-gdb-semihosting:

GDB semihosting
^^^^^^^^^^^^^^^

GDB 也提供了内置的 semihosting 支持，可作为 OpenOCD 实现的补充。当 GDB 以远程方式连接 OpenOCD，并运行在另一台主机上时，这一功能尤其有用。因为此时 semihosting 文件操作会基于 GDB 所在主机，而不是 OpenOCD 所在主机进行解析。

若要将 semihosting 请求重定向到 GDB，请在 GDB 中输入 ``mon arm semihosting_fileio enable``。对于多核目标，该命令仅会为当前核心启用 semihosting；如有需要，可针对每个核心分别执行 ``mon <target name> arm semihosting_fileio enable`` （可通过 ``mon targets`` 查看目标列表）。

启用文件 I/O 功能后，OpenOCD 在截获系统调用后不会自行处理该操作，而是向 GDB 发送文件 I/O 请求数据包，并保持目标暂停状态，直到 GDB 返回结果。对于运行在目标端的代码而言，这一过程是完全透明的。
