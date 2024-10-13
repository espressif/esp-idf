安全性
=======

:link_to_translation:`en:[English]`

.. only:: SOC_FLASH_ENC_SUPPORTED

    平台安全特性
    ------------

    当启用 flash 加密时，仅加密 app 分区中存在的应用程序镜像，而不是加密整个分区。这可以帮助优化在首次启动期间所需的加密时间。

    这可以通过配置 ``CONFIG_SECURE_FLASH_ENCRYPT_ONLY_IMAGE_LEN_IN_APP_PART`` 来实现，默认情况下从 ESP-IDF v5.3 开始启用，而在所有早期版本中均禁用，以避免任何破坏性行为。
