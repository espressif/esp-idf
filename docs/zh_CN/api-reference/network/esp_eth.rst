以太网
=========

:link_to_translation:`en:[英文]`

应用示例
-------------------

- 以太网基本示例：:example:`ethernet/basic`.
- 以太网 iperf 示例：:example:`ethernet/iperf`.

以太网驱动程序模型
---------------------

    * :component_file:`esp_eth/include/esp_eth.h`

以太网通用接口
-------------------------

    * :component_file:`esp_eth/include/esp_eth_com.h`

以太网 MAC 接口
----------------------

    * :component_file:`esp_eth/include/esp_eth_mac.h`

以太网 PHY 接口
----------------------

    * :component_file:`esp_eth/include/esp_eth_phy.h`

以太网 PHY 公共寄存器
-----------------------------

    * :component_file:`esp_eth/include/eth_phy_802_3_regs.h`

API 参考 -- 驱动程序模型
----------------------------

.. include-build-file:: inc/esp_eth.inc

API 参考 -- 通用接口
--------------------------------

.. include-build-file:: inc/esp_eth_com.inc

API 参考 -- MAC 接口
-----------------------------

.. include-build-file:: inc/esp_eth_mac.inc

API 参考 -- PHY 接口
-----------------------------

.. include-build-file:: inc/esp_eth_phy.inc

API 参考 -- esp_netif 相关使用
----------------------------------

.. include-build-file:: inc/esp_eth_netif_glue.inc

