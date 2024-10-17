# Espressif 物联网开发框架

* [English Version](./README.md)

ESP-IDF 是乐鑫官方推出的物联网开发框架，支持 Windows、Linux 和 macOS 操作系统。

# ESP-IDF 版本支持期限

![支持期限](https://dl.espressif.com/dl/esp-idf/support-periods.svg?v=1)

- 请参考 [ESP-IDF 支持政策](SUPPORT_POLICY_CN.md) 以及 [相关文档](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/versions.html) 了解更多关于 ESP-IDF 版本的信息。
- 请参考 [ESP-IDF 版本停止维护 (EOL) 公告](https://www.espressif.com/zh-hans/support/documents/advisories?keys=&field_type_of_advisory_tid%5B%5D=817)。

# ESP-IDF 与乐鑫芯片

下表总结了乐鑫芯片在 ESP-IDF 各版本中的支持状态，其中 ![alt text][supported] 代表已支持，![alt text][preview] 代表目前处于预览支持状态。预览支持状态通常有时间限制，而且仅适用于测试版芯片。请确保使用与芯片相匹配的 ESP-IDF 版本。

|芯片         |          v5.0          |          v5.1          |          v5.2          |          v5.3          |                                                                 |
|:----------- | :---------------------:| :--------------------: | :--------------------: | :--------------------: | :-------------------------------------------------------------- |
|ESP32        | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |                                                                 |
|ESP32-S2     | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |                                                                 |
|ESP32-C3     | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |                                                                 |
|ESP32-S3     | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32_S3) |
|ESP32-C2     | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32-C2) |
|ESP32-C6     |                        | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32_C6) |
|ESP32-H2     |                        | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32_H2) |
|ESP32-P4     |                        |                        |                        | ![alt text][supported] | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32-P4) |
|ESP32-C5     |                        |                        |                        | ![alt text][preview]   | [芯片发布公告](https://www.espressif.com/zh-hans/news/ESP32-C5) |

[supported]: https://img.shields.io/badge/-%E6%94%AF%E6%8C%81-green "supported"
[preview]: https://img.shields.io/badge/-%E9%A2%84%E8%A7%88-orange "preview"

每款乐鑫芯片都可能有不同版本。建议参考 [ESP-IDF 版本与乐鑫芯片版本兼容性](https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY_CN.md)，了解 ESP-IDF 版本与各芯片版本之间的兼容性。

对于 2016 年之前发布的乐鑫芯片（包括 ESP8266 和 ESP8285），请参考 [RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK)。

# 使用 ESP-IDF 进行开发

## 搭建 ESP-IDF 开发环境

关于不同芯片如何搭建 ESP-IDF 的开发环境，请参考 https://idf.espressif.com/ 。

**注意：** 不同系列芯片和不同 ESP-IDF 版本都有其对应的文档。请参阅[版本](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/versions.html)部分，获得关于如何查找文档以及如何检出 ESP-IDF 的特定发行版的详细信息。

### 非 GitHub 分叉的 ESP-IDF 项目

ESP-IDF 中的子模块采用相对路径（[详见 .gitmodules 文件](.gitmodules)），所以它们会指向 GitHub。 如果 ESP-IDF 被分叉到的仓库不在 GitHub 上，那么你需要在克隆结束后运行该脚本 [tools/set-submodules-to-github.sh](tools/set-submodules-to-github.sh)。

这个脚本会为所有的子模块设置绝对路径，接着可以通过 `git submodule update --init --recursive` 完成子模块的更新。如果 ESP-IDF 是从 GitHub 上克隆得到，则不需要此步骤。

## 寻找项目

除了入门指南中提到的 [esp-idf 模板项目](https://github.com/espressif/esp-idf-template)，ESP-IDF 的 [examples](examples) 目录下还带有很多其它示例项目。

一旦找到了需要的项目，便可以进入该目录，执行配置和构建操作。

如果要基于示例工程开始你自己的项目，请将示例工程复制到 ESP-IDF 目录之外。

# 快速参考

详细的使用方法请参考上面入门指南的链接，这里仅仅列举一些 ESP-IDF 项目开发中常用的命令：

## 设置构建环境

请参考入门指南中列出的详细步骤。

* 在主机中安装入门指南中提到的构建所依赖的工具。
* 运行安装脚本来设置构建环境。可为 Windows shell 选择 `install.bat` 或 `install.ps1`，为 Unix shell 选择 `install.sh` 和 `install.fish`。
* 在使用 ESP-IDF 之前，需要在 shell 中运行导出脚本。Windows 下可运行 `export.bat`，Unix 下可运行 `source export.sh`。

## 配置项目

* `idf.py set-target <chip_name>` 可将项目的目标芯片设置为 `<chip_name>`。运行 `idf.py set-target`，不用带任何参数，可查看所有支持的目标芯片列表。
* `idf.py menuconfig` 可打开一个基于文本的配置菜单，可以用来对项目进行配置。

## 编译项目

`idf.py build`

编译应用程序，引导程序，并根据配置生成分区表。

## 烧写项目

当构建结束，终端会打印出一条命令行，告知如何使用 esptool.py 工具烧写项目到芯片中。但你也可以运行下面这条命令来自动烧写：

`idf.py -p PORT flash`

将其中的 PORT 替换为系统中实际串口的名字（比如 Windows 下的 `COM3`，Linux 下的 `/dev/ttyUSB0`，或者 macOS 下的 `/dev/cu.usbserial-X`。如果省略 `-p` 选项，`idf.py flash` 会尝试使用第一个可用的串口进行烧写。

这会烧写整个项目（包括应用程序，引导程序和分区表）到芯片中，此外还可以使用 `idf.py menuconfig` 来调整串口烧写相关的配置。

不必先运行 `idf.py build` 再运行 `idf.py flash`，`idf.py flash` 会根据需要自动重新构建项目。

## 观察串口输入

`idf.py monitor` 会调用 [esp-idf-monitor 工具](https://github.com/espressif/esp-idf-monitor)来显示乐鑫芯片的串口输出。esp-idf-monitor 还包含一系列的功能来解析程序崩溃后的输出结果并与设备进行交互。更多详细内容，请参阅[文档](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html).

输入 `Ctrl-]` 可退出监视器。

想要一次性执行构建、烧写和监视，可以运行如下命令：

`idf.py flash monitor`

## 仅编译并烧写应用程序

在第一次烧写过后，你可能只想构建并烧写你的应用程序，不包括引导程序和分区表：

* `idf.py app` - 仅构建应用程序。
* `idf.py app-flash` - 仅烧写应用程序。

`idf.py app-flash` 会自动判断是否有源文件发生了改变然后重新构建应用程序。

（在正常的开发中，即使引导程序和分区表没有发生变化，每次都重新烧写它们并不会带来什么危害。）

## 擦除 Flash

`idf.py flash` 并不会擦除 flash 上所有的内容，但是有时候我们需要设备恢复到完全擦除的状态，尤其是分区表发生了变化或者 OTA 应用升级时。要擦除整块 flash 请运行 `idf.py erase-flash`。

这条命令还可以和其余命令整合在一起，`idf.py -p PORT erase-flash flash` 会擦除一切然后重新烧写新的应用程序、引导程序和分区表。

# 其它参考资源

* 最新版的文档：https://docs.espressif.com/projects/esp-idf/ ，该文档是由本仓库 [docs 目录](docs) 构建得到。

* [初学者指南：主要概念和资源](https://www.bilibili.com/video/BV1114y1r7du/)

* 可以前往 [esp32.com 论坛](https://esp32.com/) 提问，挖掘社区资源。

* 如果你在使用中发现了错误或者需要新的功能，请先[查看 GitHub Issues](https://github.com/espressif/esp-idf/issues)，确保该问题没有重复提交。

* 如果你有兴趣为 ESP-IDF 作贡献，请先阅读[贡献指南](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/index.html)。
