网络
=====

:link_to_translation:`en:[English]`

以太网
******

``esp_eth_phy_802_3_reset_hw()`` API 变更
------------------------------------------

此变更仅适用于维护自己的以太网 PHY 驱动程序，该驱动程序基于 :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` 通用函数。现在 :cpp:func:`esp_eth_phy_802_3_reset_hw` API 仅接受一个参数，并使用初始化期间定义的内部时序配置重置以太网 PHY。以前，该 API 需要一个 ``reset_assert_us`` 参数来指定复位引脚断言时间。此参数已删除。

使用示例：

.. code-block:: c

    esp_eth_phy_802_3_reset_hw(phy_802_3);
