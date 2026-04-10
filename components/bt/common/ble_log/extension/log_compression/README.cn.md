## BLE 日志压缩方案 （Preview）

### 一、概述

本方案通过在编译阶段扫描 BLE 协议栈相关组件的代码，将日志语句中的格式化字符串和参数转换为纯二进制数据，从而提升日志输出效率并减少协议栈所占用的 Flash 空间。

目前，该方案已支持对 BLE-MESH 和 BLE-HOST-BLUEDROID 组件的日志压缩。

### 二、使用方法
该功能依赖额外的 Python 库，请按以下步骤安装所需环境。

#### 步骤一：验证 ESP-IDF 虚拟环境
请确保在 ESP-IDF 的 Python 虚拟环境中执行后续操作。可通过以下命令验证当前环境是否已激活：
```bash
idf.py --version
```
若提示 `command not found`，则表示未激活 ESP-IDF 虚拟环境。

请参考官方文档配置并激活环境：
[ESP-IDF 环境设置指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/linux-macos-setup.html#get-started-linux-macos-first-steps)

激活后再次执行 `idf.py --version`，若显示版本信息则表明环境已就绪。

#### 步骤二：清理缓存文件
安装日志压缩所需的依赖后，删除之前构建生成的 build 文件夹（如有）。之后重新构建应用程序即可。


### 步骤三：配置 Menuconfig

使用命令`idf.py menuconfig`打开menuconfig, 以开启`BLE-MESH`组件的日志压缩为例：

通过路径`(Top) → Component config → Bluetooth → Common Options → BLE Log → Enable BLE Log Module (Experimental) → Settings of BLE Log Compression->Enable BLE Mesh log compression(Preview)`开启`BLE-MESH`组件的日志压缩功能。

在`Enable BLE Mesh log compression(Preview)`目录中有三个配置项目可供设置：
1. `BLE Mesh log buffer length` 通过该配置项来配置单条日志可能出现的最大长度。
2. `Select the stack log tag to be compressed` : 选择期待压缩的BLE-Mesh协议栈日志级别。
3. `Select the net buf log tag to be compressed`: 选择期待压缩的BLE-Mesh协议栈中Net_buf相关日志级别。

在 BLE-Mesh 组件中，日志按级别分为四类：`BT_ERR`、`BT_WARN`、`BT_INFO`、`BT_DBG`.

  若启用`Compress ERROR log of ESP-BLE-MESH`选项，系统将对 `BT_ERR` 级别日志进行压缩传输。
  若同时勾选“保留原始错误日志语句”，则 `BT_ERR` 日志会同时通过压缩通道和原串口通道输出；双通道会增加 bin 体积，并延长单次日志的总输出时间（压缩传输耗时 + 串口耗时）。

默认策略：
- ERROR 与 WARN → 压缩通道 + 串口通道（双通道）
- INFO 与 DEBUG → 仅压缩通道（串口不再输出）

因此，在默认配置下，即使开启 BLE-Mesh 的 INFO 级日志，终端也不会打印任何 INFO 内容——它们已被重定向到压缩接口，不再经过串口。

### 步骤四：构建应用程序

当开启配置项后，使用命令`idf.py build`构建应用程序，请注意构建过程中是否会有警告日志出现，比如出现以下日志时：
```txt
CMake Warning at esp/esp-idf/components/bt/common/ble_log/log_compression/CMakeLists.txt:46 (message):
  tree_sitter import failed, please check whether the package is installed
  correctly,Please refer to the
  file: esp/esp-idf/components/bt/common/ble_log/log_compression/README
  for installation instructions.
```
该警告表示依赖未正确安装，日志压缩构建失败，系统将自动回退至普通编译模式。请重新执行步骤一至步骤四。

若日志压缩成功，终端将显示如下信息：
```
[0/1285] Log compression is being performed, please wait...
Log compression underway, please wait...
Found module BLE_MESH for compression
Found 111 source files in module BLE_MESH requiring compression
3055 ble log(s) compressed
Header file for compressed logs generated
```
出现该信息表明压缩日志构建成功，其中输出的文件数目和日志数目可能会随版本变更而略有不同。

构建成功后，将在 `build/ble_log/` 目录下生成如下结构：：
```
build/ble_log/
├── ble_log_database
│   └── BLE_MESH_logs.json
├── ble_script_log_{timestamp}.log
├── .compressed_srcs
│   └── esp_ble_mesh
├── include
│   └── mesh_log_index.h
└── module_info.yml
```
- `.compressed_srcs`: 压缩处理后的C代码文件。
- `mesh_log_index.h`: 生成的日志宏头文件。
- `BLE_MESH_logs.json`: 每条日志的详细信息。
- `ble_script_log_{timestamp}.log`: 压缩日志在运行过程中产生的日志。
- `module_info.yml`: 压缩日志对各个模块的配置文件。

**注意：这些为自动生成文件，请勿手动修改。**

### 步骤五、接收日志
开启日志压缩后， 被压缩组件在默认配置下， 除`ERR`、`WARN`级别的日志外，其余级别的日志将会被重定向到压缩日志接口进行输出，接收日志的方法请参考BLE Log模块的说明文档：[BLE Log Module](../../README.md)

### 步骤六、解析压缩的日志
使用日志压缩方案的目的是为了更快的帮助用户解决协议栈的问题， 解析脚本当前还未公布，因此出现问题后，请将生成的日志文件(.bin)和当前IDF的commit提交给 Espressif BLE 团队，由 Espressif BLE 团队负责解析。

## 常见问题解决
1. 如果出现编码后的日志导致编译错误或者找不到宏定义，请删除`build`文件夹后重新构建，如果问题持续存在，请将问题反馈给 Espressif BLE 团队。