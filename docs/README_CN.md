* [English Version](./README.md)

# 文档源文件夹

本文件夹包含 **ESP-IDF 文档**的源文件，提供[英文版](https://docs.espressif.com/projects/esp-idf/en/latest/)和[中文版](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/)。

这些源文件在 GitHub 上的渲染效果不佳，某些信息只有在构建文档后才能显示。

请使用每次提交后约 20 分钟内生成的实际文档：

# 在线文档

* 英文: https://docs.espressif.com/projects/esp-idf/en/latest/
* 中文: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/

在您点击任何链接访问 [ESP-IDF 编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/index.html)后，请前往侧边栏顶部，确保在下拉菜单中选择了正确的乐鑫芯片（目标）和 ESP-IDF 版本。您还可以在页面右下角找到下载 HTML 版本的链接。

# 构建文档

文档使用 Python 包 `esp-docs` 构建，可以通过运行以下命令安装：

```
pip install esp-docs
```

要查看可用选项的摘要，请运行：

```
build-docs --help
```

更多信息请参阅 `esp-docs` 文档：https://github.com/espressif/esp-docs/blob/master/README.md
