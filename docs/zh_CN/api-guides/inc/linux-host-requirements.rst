- 已安装 ESP-IDF 及使用 ESP-IDF 的所有依赖项
- 满足系统软件包需求 (``libbsd``、``libbsd-dev``)
- Linux 或 macOS 和 GCC 编译器已更新至足够新的版本
- 应用程序所依赖的所有组件必须受 Linux 目标（Linux/POSIX 模拟器）支持，或可进行模拟

对于在 Linux 目标上运行的应用程序，需要在应用程序根目录的 CMakeLists.txt 文件中，设置 ``COMPONENTS`` 变量为 ``main``，具体操作如下：

.. code-block:: cmake

  set(COMPONENTS main)


为方便起见，应用程序会在构建过程中，自动包含 ESP-IDF 的所有组件，执行上述代码则可以防止此类情况。
