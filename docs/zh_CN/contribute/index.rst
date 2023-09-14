贡献指南
===================

:link_to_translation:`en:[English]`

欢迎为 ESP-IDF 项目贡献内容！

如何贡献
--------------

欢迎为 ESP-IDF 贡献内容，如修复问题、新增功能、添加文档等。你可通过 `Github Pull Requests <https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests>`_ 提交你的贡献内容。

准备工作
--------------

在提交 Pull Request 前，请检查以下要点：

* 贡献内容是否完全是自己的成果，或已获得与 Apache License 2.0 兼容的开源许可？如果不是，我们不能接受该内容。了解更多信息，请见 :doc:`copyright-guide`。

* 要提交的代码是否符合 ESP-IDF :doc:`style-guide`？

* 是否安装了 ESP-IDF :doc:`pre-commit 钩子 <install-pre-commit-hook>`？

* 代码文档是否符合 :doc:`documenting-code` 的要求？

* 代码是否注释充分，便于读者理解其结构？

* 是否为贡献的代码提供文档或示例？要写出好的示例，请参考 :idf:`examples` readme。

* 注释或文档是否以英语书写并表达清晰，不存在拼写或语法错误？

* 欢迎贡献新的代码示例。了解更多信息，请参考 :doc:`creating-examples`。

* 如果需提交多个内容，是否将所有内容按照改动的类型（每个 pull request 对应一个主要改动）进行分组？是否有命名类似 “fixed typo” 的提交 `压缩到了此前的提交中 <https://eli.thegreenplace.net/2014/02/19/squashing-github-pull-requests-into-a-single-commit/>`_？

* 如不能确定上述任意内容，请提交 Pull Request，并向我们寻求反馈。

Pull Request 提交流程
--------------------------

创建 Pull Request 后，PR 评论区中可能有一些关于该请求的讨论。

Pull Request 准备好待合并时，首先会合并到我们的内部 git 系统中进行内部自动化测试。

测试流程通过后，你贡献的内容将合并到公共 GitHub 库。

法律规范
------------

在提交贡献内容前，你需签署 :doc:`contributor-agreement`。该协议将在 Pull Request 过程中自动推送。

相关文档
------------

.. toctree::
    :maxdepth: 1

    style-guide
    install-pre-commit-hook
    documenting-code
    creating-examples
    ../api-reference/template
    contributor-agreement
    copyright-guide
    esp-idf-tests-with-pytest
