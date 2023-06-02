其他库和开发框架
=============================
:link_to_translation:`en:[English]`

本文展示了一系列乐鑫官方发布的库和框架。

ESP-ADF
-------------------------------------

ESP-ADF 是一个全方位的音频应用程序框架，该框架支持：

* CODEC 的 HAL
* 音乐播放器和录音机
* 音频处理
* 蓝牙扬声器
* 互联网收音机
* 免提设备
* 语音识别

该框架对应的 GitHub 仓库为 `ESP-ADF <https://github.com/espressif/esp-adf>`_。

ESP-CSI
------------------------------------

ESP-CSI 是一个具有实验性的框架，它利用 Wi-Fi 信道状态信息来检测人体存在。

该框架对应的 GitHub 仓库为 `ESP-CSI <https://github.com/espressif/esp-csi>`_。

ESP-DSP
------------------------------------

ESP-DSP 提供了针对数字信号处理应用优化的算法，该库支持：

* 矩阵乘法
* 点积
* 快速傅立叶变换 (FFT)
* 无限脉冲响应 (IIR)
* 有限脉冲响应 (FIR)
* 向量数学运算

该库对应的 GitHub 仓库为 `ESP-DSP 库 <https://github.com/espressif/esp-dsp>`_。

ESP-WIFI-MESH
------------------------------------------------------

ESP-WIFI-MESH 基于 ESP-WIFI-MESH 协议搭建，该框架支持：

* 快速网络配置
* 稳定升级
* 高效调试
* LAN 控制
* 多种应用示例

该框架对应的 GitHub 仓库为 `ESP-MDF <https://github.com/espressif/esp-mdf>`_。

ESP-WHO
------------------------------

ESP-WHO 框架利用 ESP32 及摄像头实现人脸检测及识别。

该框架对应的 GitHub 仓库为 `ESP-WHO <https://github.com/espressif/esp-who>`_。

ESP RainMaker
---------------------------------------------

`ESP RainMaker <https://rainmaker.espressif.com/>`_ 提供了一个快速 AIoT 开发的完整解决方案。使用 ESP RainMaker，用户可以创建多种 AIoT 设备，包括固件 AIoT 以及集成了语音助手、手机应用程序和云后端的 AIoT 等。

该解决方案对应的 GitHub 仓库为 `GitHub 上的 ESP RainMaker <https://github.com/espressif/esp-rainmaker>`_。

ESP-IoT-Solution
--------------------------------------------------

`ESP-IoT-Solution <https://docs.espressif.com/projects/espressif-esp-iot-solution/en/latest/>`_ 涵盖了开发 IoT 系统时常用的设备驱动程序及代码框架。在 ESP-IoT-Solution 中，设备驱动程序和代码框架以独立组件存在，可以轻松地集成到 ESP-IDF 项目中。

ESP-IoT-Solution 支持：

* 传感器、显示器、音频、GUI、输入、执行器等设备驱动程序
* 低功耗、安全、存储等框架和文档
* 从实际应用角度指导乐鑫开源解决方案

该解决方案对应的 GitHub 仓库为 `GitHub 上的 ESP-IoT-Solution <https://github.com/espressif/esp-iot-solution>`_。


ESP-Protocols
-----------------------------------------

`ESP-Protocols <https://github.com/espressif/esp-protocols>`_ 库包含 ESP-IDF 的协议组件集。ESP-Protocols 中的代码以独立组件存在，可以轻松地集成到 ESP-IDF 项目中。此外，每个组件都可以在 `ESP-IDF 组件注册表 <https://components.espressif.com/>`_ 中找到。

ESP-Protocols 组件：

* `esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_ 使用 AT 命令或 PPP 协议与 GSM/LTE 调制解调器连接，详情请参阅 `esp_modem 文档 <https://docs.espressif.com/projects/esp-protocols/esp_modem/docs/latest/index.html>`_。

* `mdns <https://components.espressif.com/component/espressif/mdns>`_ (mDNS) 是一种组播 UDP 服务，用于提供本地网络服务与主机发现，详情请参阅 `mdns 文档 <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/zh_CN/index.html>`_。

* `esp_websocket_client <https://components.espressif.com/component/espressif/esp_websocket_client>`_ 是 ESP-IDF 的托管组件，可在 ESP32 上实现 WebSocket 协议客户端，详情请参阅 `esp_websocket_client 文档 <https://docs.espressif.com/projects/esp-protocols/esp_websocket_client/docs/latest/index.html>`_。有关 WebSocket 协议客户端，请参阅 `WebSocket_protocol_client <https://datatracker.ietf.org/doc/html/rfc6455>`_。

* `asio <https://components.espressif.com/component/espressif/asio>`_ 是一个跨平台的 C++ 库，请参阅 `<https://think-async.com/Asio/>`_。该库基于现代 C++ 提供一致的异步模型，请参阅 `asio 文档 <https://docs.espressif.com/projects/esp-protocols/asio/docs/latest/index.html>`_。

ESP-BSP
----------------------------------------

`ESP-BSP <https://github.com/espressif/esp-bsp>`_ 库包含了各种乐鑫和第三方开发板的板级支持包 (BSP)，可以帮助快速上手特定的开发板。它们通常包含管脚定义和辅助函数，这些函数可用于初始化特定开发板的外设。此外，BSP 还提供了一些驱动程序，可用于开发版上的外部芯片，如传感器、显示屏、音频编解码器等。

ESP-IDF-CXX
----------------------------------------------------------

`ESP-IDF-CXX <https://github.com/espressif/esp-idf-cxx>`_ 包含了 ESP-IDF 的部分 C++ 封装，重点在实现易用性、安全性、自动资源管理，以及将错误检查转移到编译过程中，以避免运行时失败。它还提供了 ESP 定时器、I2C、SPI、GPIO 等外设或 ESP-IDF 其他功能的 C++ 类。ESP-IDF-CXX 作为组件可以从 `组件注册表 <https://components.espressif.com/components/espressif/esp-idf-cxx>`_ 中获取。详情请参阅 `README.md <https://github.com/espressif/esp-idf-cxx/blob/main/README.md>`_。
