安全性
======

:link_to_translation:`en:[English]`

内存保护
--------

从 **ESP-IDF v6.2** 开始，在 ESP32-C5、ESP32-C6、ESP32-C61、ESP32-H2 和 ESP32-P4 上，二级引导加载程序不再配置或锁定任何 RISC-V PMP 表项。完整的 PMP 配置改由应用程序在启动早期编程并锁定。

被引导加载程序锁定的 PMP 表项在下一次 CPU 复位前无法重新配置。由于已部署设备上的引导加载程序通常不会更新，旧版引导加载程序锁定的表项可能与使用较新 ESP-IDF 版本构建的应用程序所期望的 PMP 布局发生冲突，导致启动失败，或使内存保护在没有任何提示的情况下失效。

使用 ESP-IDF 构建的应用程序不受影响，因为应用程序会在启动阶段、任何应用代码运行之前，自行编程并锁定完整的 PMP 配置。

由 ESP-IDF 二级引导加载程序启动的自定义（非 ESP-IDF）应用程序不得假定交接时任何 PMP 表项已被预先配置或锁定。此前，引导加载程序会配置并锁定覆盖 ROM 和外设地址空间等区域的表项；此类应用程序现在必须自行完成 PMP 配置。


镜像校验功能从 ``bootloader_support`` 中拆分
--------------------------------------------

镜像校验与安全启动签名校验已从 ``bootloader_support`` 移至新的
``esp_image_verify`` 组件。``bootloader_support`` 不再依赖 ``mbedtls``。

- 此前通过 ``bootloader_support`` 间接获得 ``mbedtls`` 或 ``app_update`` 依赖的组件，现在必须在 ``PRIV_REQUIRES`` / ``REQUIRES`` 中显式声明。
- :cpp:func:`bootloader_common_get_sha256_of_partition` 已弃用，请改用 :cpp:func:`esp_partition_get_sha256`。
- 在未包含 ``esp_image_verify`` 组件的工程中（未启用 OTA 或签名镜像功能），调用 :cpp:func:`esp_partition_get_sha256` 或上述弃用函数会在链接时因 ``esp_image_get_metadata`` / ``bootloader_sha256_flash_contents`` 未定义引用而失败。请将 ``esp_image_verify`` （或 ``app_update``，它包含该组件并提供 OTA API）添加到调用组件的 ``PRIV_REQUIRES`` 或工程的 ``COMPONENTS`` 列表中。
- 启用 :ref:`CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT` 的工程必须包含 ``esp_image_verify`` 组件，该组件提供启动时校验当前应用签名的检查。
  使用 OTA 的应用会通过 ``app_update`` 自动引入该组件；裁剪组件依赖图的应用（如启用 ``MINIMAL_BUILD``）必须将 ``esp_image_verify`` （或 ``app_update``）添加到 ``PRIV_REQUIRES`` 或工程的 ``COMPONENTS`` 中。否则构建将以 ``#error`` 失败，而不会静默跳过已配置的安全检查。
- Kconfig 选项 ``CONFIG_SECURE_SIGNED_DATA_PARTITION`` 已重命名为 ``CONFIG_APP_UPDATE_SECURE_SIGNED_DATA_PARTITION`` （通过 ``sdkconfig.rename`` 兼容旧名称）。
