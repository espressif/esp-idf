贡献指南
===================

:link_to_translation:`en:[English]`

欢迎为 ESP-IDF 项目贡献内容！

如何贡献
--------------

欢迎为 ESP-IDF 贡献内容，如 bug 修复、新增功能、完善文档等。你可通过 `GitHub Pull Requests <https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests>`_ 提交贡献内容。

准备工作
--------------

在提交 Pull Request 前，请先确认以下注意事项：

* 贡献内容是否完全由你独立创作，或已获得与 Apache License 2.0 兼容的开源许可？如不符合上述条件，我们将无法接受该贡献内容。如需了解更多信息，请参阅 :doc:`版权标头指南 <copyright-guide>`。

* 要提交的代码是否符合 :doc:`乐鑫 IoT 开发框架风格指南 <style-guide>`？

* 如果贡献内容新增或修改了 Python 工具，是否遵循 :ref:`Python 代码风格指南 <python-code-style>`，并在适当的场景下复用了 ``esp-pylib`` 提供的公共辅助代码？

* 是否安装了 ESP-IDF 项目的 :doc:`pre-commit 钩子 <install-pre-commit-hook>`？

* 代码文档是否符合 :doc:`编写代码文档 <documenting-code>` 的要求？

* 代码是否注释充分，便于读者理解其结构？

* 贡献代码是否附有文档或示例？关于如何编写优质示例，请参阅 :idf:`examples` 中的 readme 文件。

* 注释和文档是否以英语书写，且表意清晰，无拼写或语法错误？

* 欢迎贡献代码示例，具体请参阅 :doc:`创建示例项目 <creating-examples>`。

* 若某份贡献包含多次代码提交 (commit)，是否按照改动的内容分组处理（每个 pull request 对应一项主要改动）？对于修改错字一类的次要 commit，是否已 `压缩合并到之前的 commit 中 <https://eli.thegreenplace.net/2014/02/19/squashing-github-pull-requests-into-a-single-commit/>`_？

* 如不能确定上述任意内容，请提交 Pull Request 并在评论区寻求反馈。

Pull Request 提交流程
--------------------------

创建 Pull Request 后，PR 评论区中可能会有一些讨论。

Pull Request 准备好待合并时，首先会合并到乐鑫的内部 Git 系统中进行自动化测试。

测试流程通过后，你的贡献内容将合并到公开 GitHub 仓库。

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
