# Espressif 物联网开发框架

* [English Version](./README.md)

[![Documentation Status](https://readthedocs.com/projects/espressif-esp-idf/badge/?version=latest)](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/?badge=latest)

ESP-IDF 是由乐鑫官方推出的针对 [ESP32](https://espressif.com/en/products/hardware/esp32/overview) 系列芯片的开发框架。

# 使用 ESP-IDF 进行开发

## 搭建 ESP-IDF 开发环境

请参阅如下指南搭建 ESP-IDF 的开发环境：

* [ESP-IDF 稳定版本的入门指南](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/)
* [ESP-IDF 开发版本（master 分支）的入门指南](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/)

### 非 GitHub 分叉的 ESP-IDF 项目

ESP-IDF 中的子模块采用相对路径（[详见 .gitmodules 文件](.gitmodules)），所以它们会指向 GitHub。
如果 ESP-IDF 被分叉到的仓库不在 GitHub 上，那么你需要在克隆结束后运行该[脚本](tools/set-submodules-to-github.sh)。它会为所有的子模块设置绝对路径，接着可以通过 `git submodule update --init --recursive` 完成子模块的更新。
如果 ESP-IDF 是从 GitHub 上克隆得到，则不需要此步骤。

## 寻找项目

除了入门指南中提到的 [esp-idf 模板项目](https://github.com/espressif/esp-idf-template)，ESP-IDF 的 [examples](examples) 目录下还带有很多其它示例项目。

一旦找到了需要的项目，便可以进入该目录，执行配置和构建操作。

如果要基于示例工程开始你自己的项目，请将示例工程复制到 ESP-IDF 目录之外。

# 快速参考

详细的使用方法请参考上面入门指南的链接，这里仅仅列举一些 ESP-IDF 项目开发中常用的命令：

## 设置构建环境

（请参考入门指南中列出的详细步骤。）
* 在主机中安装入门指南中提到的构建所依赖的工具。
* 将 ESP-IDF 中的 `tools/` 目录加入 PATH 环境变量中。
* 运行 `python -m pip install -r requirements.txt` 安装 Python 依赖库。

## 配置项目

`idf.py menuconfig`

* 打开项目的文本配置菜单。
* 使用上下键浏览菜单。
* 使用回车键进入子菜单，退出键返回上一级菜单或者退出配置。
* 输入 `?` 查看帮助界面，按下回车键可以退出帮助界面。
* 使用空格键或者 `Y` 和 `N` 按键来启用和禁用带复选框“`[*]`”的配置项。
* 高亮某个配置项的同时按下 `?` 键可以显示该选项的帮助文档。
* 输入 `/` 可以搜索指定的配置项。

一旦配置完成，请按下退出键多次以退出配置界面，当提示是否保存新的的配置时，选择 “Yes”。

## 编译项目

`idf.py build`

编译应用程序，引导程序，并根据配置生成分区表。

## 烧写项目

当构建结束，终端会打印出一条命令行，告知如何使用 esptool.py 工具烧写项目到芯片中。但是你还可以运行下面这条命令来自动烧写：

`idf.py -p PORT flash`

将其中的 PORT 替换为系统中实际串口的名字（比如 Windows 下的 `COM3`，Linux 下的 `/dev/ttyUSB0`，或者 MacOS 下的 `/dev/cu.usbserial-X`。如果省略 `-p` 选项，`idf.py flash` 会尝试使用第一个可用的串口进行烧写。

这会烧写整个项目（包括应用程序，引导程序和分区表）到芯片中，此外还可以使用 `idf.py menuconfig` 来调整串口烧写相关的配置。

你也不必先运行 `idf.py build`，再运行 `idf.py flash`，`idf.py flash` 会根据需要自动重新构建项目。

## 观察串口输入

`idf.py monitor` 会调用 [idf_monitor 工具](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html)来显示 ESP32 的串口输出。`idf_monitor` 还包含一系列的功能来解析程序崩溃后的输出结果并与设备进行交互。更多详细内容，请参阅[文档](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html).

输入 `Ctrl-]` 可退出监视器。

想要一次性执行构建，烧写和监视，可以运行如下命令：

`idf.py flash monitor`

## 仅编译并烧写应用程序

在第一次烧写过后，你可能只想构建并烧写你的应用程序，不包括引导程序和分区表：

* `idf.py app` - 仅构建应用程序。
* `idf.py app-flash` - 仅烧写应用程序。

`idf.py app-flash` 会自动判断是否有源文件发生了改变而后重新构建应用程序。

（在正常的开发中，即使引导程序和分区表没有发生变化，每次都重新烧写它们并不会带来什么危害。）

## 擦除 Flash

`idf.py flash` 并不会擦除 Flash 上所有的内容，但是有时候我们需要设备恢复到完全擦除的状态，尤其是分区表发生了变化或者 OTA 应用升级。要擦除整块 Flash 请运行 `idf.py erase_flash`。

这条命令还可以和其余命令整合在一起，`idf.py -p PORT erase_flash flash` 会擦除一切然后重新烧写新的应用程序，引导程序和分区表。

# 其它参考资源

* 最新版的文档：https://docs.espressif.com/projects/esp-idf/ ，该文档是由本仓库 [docs 目录](docs) 构建得到。

* 可以前往 [esp32.com 论坛](https://esp32.com/) 提问，挖掘社区资源。

* 如果你在使用中发现了错误或者需要新的功能，请先[查看 GitHub Issues](https://github.com/espressif/esp-idf/issues)，确保该问题不会被重复提交。

* 如果你有兴趣为 ESP-IDF 作贡献，请先阅读[贡献指南](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/index.html)。


