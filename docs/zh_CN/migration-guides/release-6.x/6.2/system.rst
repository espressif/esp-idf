系统
====

:link_to_translation:`en:[English]`

OTA 更新
--------

默认启用应用程序回滚
^^^^^^^^^^^^^^^^^^^^

:ref:`app_rollback` 现在默认启用，并使用自动确认模式 (:ref:`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP <CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP>`)。此前，回滚功能 (``CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE``) 默认是禁用的，需要应用程序自行调用 :cpp:func:`esp_ota_mark_app_valid_cancel_rollback`。

新的默认设置适用于新建项目，以及配置来自 ``sdkconfig.defaults`` 的项目。对于使用固定的、已提交版本控制的 ``sdkconfig`` 文件的项目，新默认设置同样适用，除非该文件中已经记录了明确的回滚配置：

- 若已设置 ``CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y``，该配置会自动迁移为 :ref:`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_BY_APP <CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_BY_APP>`，此前的应用程序控制确认行为保持不变。
- 若项目此前将回滚保留为默认值（禁用），由于不存在可迁移的旧配置项，该项目会采用新的默认设置，与新建项目一样启用自动确认的回滚功能。

如需保持应用程序回滚为禁用状态，请在 *Bootloader config* > *Application Rollback* > *Enable application rollback* 中显式禁用 :ref:`CONFIG_BOOTLOADER_APP_ROLLBACK`。

对已部署引导加载程序设备的影响
""""""""""""""""""""""""""""""

应用程序回滚功能由第二阶段引导加载程序和应用程序共同实现。如果设备已部署的引导加载程序本身并未支持回滚功能，仅在应用程序中启用回滚是不够的。

在此类设备上，更新为已启用回滚功能的应用程序并不能恢复该安全机制：引导加载程序不会将 OTA 状态从 ``ESP_OTA_IMG_NEW`` 转换为 ``ESP_OTA_IMG_PENDING_VERIFY``，因此更新后首次启动时发生的崩溃不会被检测到，设备也不会回滚至此前正常运行的应用程序。应用程序会在启动时检测到这种情况，并将自身标记为有效，以保持 OTA 状态的一致性，但这本身并不能提供回滚保护。

要为已部署到现场的设备提供该安全机制，必须重新构建支持回滚功能的引导加载程序并将其重新烧录到设备中。

代码大小影响
""""""""""""

与此前的默认配置（禁用回滚）相比，默认启用自动确认的应用程序回滚功能会略微增加代码大小。在 ESP32 上测试 :example:`system/ota/native_ota_example` 示例，引导加载程序大约增加 80 字节，应用程序大约增加 350 字节。具体数值因目标芯片和应用配置而异。
