为 ESP-IDF 安装 pre-commit 钩子
==========================================

:link_to_translation:`en:[English]`

环境依赖
---------------

我们向 IDF 开发人员推荐 Python 3.8.* 及以上版本。

如果你已安装了不兼容的 Python 版本，应在安装 pre-commit 工具前进行更新。

安装 ``pre-commit`` 工具
---------------------------

运行 ``pip install pre-commit`` 。

安装 ``pre-commit`` 钩子
----------------------------

1. 切换到 IDF 项目路径。

2. 运行 ``pre-commit install --allow-missing-config -t pre-commit -t commit-msg`` 命令。 使用这种方式安装钩子后，即使在没有 ``.pre-commit-config.yaml`` 的分支中也能成功提交。

3. 在运行 ``git commit`` 命令时，pre-commit 钩子会自动运行。

卸载 ``pre-commit`` 钩子
---------------------------

运行 ``pre-commit uninstall`` 。

更多
---------

更多详细使用方法，请参考 pre-commit_ 文档。

.. _pre-commit: https://pre-commit.com/

Windows 用户常见问题
--------------------------------

``/usr/bin/env: python: Permission denied.``

    如果使用 Git Bash，请运行 ``which python`` 检查 Python 的可执行位置。

    如果该可执行文件位于 ``~/AppData/Local/Microsoft/WindowsApps/``，这其实是一个到 Windows 应用商店的链接，而不是真正的可执行文件。

    请手动安装 Python，并在 ``PATH`` 环境变量中进行更新。


你的 ``USERPROFILE`` 中包含非 ASCII 字符

    如果 ``pre-commit`` 的缓存路径包含非 ASCII 字符， 用 ``pre-commit`` 为特定钩子初始化环境时可能会失败。解决方案是，将 ``PRE_COMMIT_HOME`` 设置为一个仅包含标准字符的路径，然后再运行 pre-commit。

    - CMD: ``set PRE_COMMIT_HOME=C:\somepath\pre-commit``
    - PowerShell: ``$Env:PRE_COMMIT_HOME = "C:\somepath\pre-commit"``
    - git bash: ``export PRE_COMMIT_HOME="/c/somepath/pre-commit"``
