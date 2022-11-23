.. _xip_from_psram:

在 PSRAM 中执行代码
----------------------

启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` 选项后，flash 中 ``.text`` 部分的数据（用于指令）将被放入 PSRAM。

启用 :ref:`CONFIG_SPIRAM_RODATA` 选项后，flash 中 ``.rodata`` 部分的数据（用于只读数据）将被放入 PSRAM。

相应的虚拟内存地址将被重新映射到 PSRAM。

如果同时启用以上两个选项，则在 SPI1 flash 操作期间 cache 不会被禁用，无需确保 ISR、ISR 回调及相关数据放置在内部 RAM 中。
