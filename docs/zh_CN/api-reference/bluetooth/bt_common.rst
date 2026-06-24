蓝牙\ :sup:`®` 通用 API
=========================

:link_to_translation:`en:[English]`

**蓝牙通用 API** 提供经典蓝牙和低功耗蓝牙共用的定义和 API。它为各蓝牙组件提供基础功能，提供统一的接口用于蓝牙初始化、配置和设备管理。

蓝牙通用 API 包含以下部分：

.. toctree::
   :maxdepth: 1

   Bluetooth Define <esp_bt_defs>

提供经典蓝牙和低功耗蓝牙共用的定义和数据结构。

.. toctree::
   :maxdepth: 1

   Bluetooth Main <esp_bt_main>

提供核心 API，用于初始化、启用/禁用和管理蓝牙主机栈。

.. toctree::
   :maxdepth: 1

   Bluetooth Device <esp_bt_device>

提供设备级 API，用于管理设备属性，例如地址、名称、可见性和共存设置。

每个部分通常包含以下内容：

- **概述**： 主要用途、核心功能和关键接口
- **应用示例**：展示典型使用场景的示例
- **API 参考**：详细 API 文档，包括头文件、函数、结构体、宏、类型定义和枚举
