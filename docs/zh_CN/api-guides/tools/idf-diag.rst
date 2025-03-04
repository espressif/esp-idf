*****************
IDF Diag 诊断工具
*****************

:link_to_translation:`en:[English]`

IDF Diag 是一个诊断工具，用于加速和辅助排查与 ESP-IDF 环境及项目相关的问题。它将 esp-idf-diag_ Python 包集成到了 :ref:`idf.py` 中，能够收集已安装的 IDF 工具、Python 环境、项目构建产物、日志及其他相关数据，并将其存储到一个报告目录下。可以轻松查看该报告目录的内容，并可在其中补充未被自动收集但与问题相关的文件。报告目录整理完成后，可以将其压缩为 zip 文件并进行分享，例如，将其添加到 GitHub Issue 的附件中。

创建报告目录
^^^^^^^^^^^^^

可使用默认配置生成诊断信息报告目录：

.. code-block:: bash

    $ idf.py diag

报告目录将创建在在当前目录下，命名为 ``idf-diag-UUID``。其中， ``UUID`` 是随机生成的全局唯一标识符 (Universally Unique Identifier)，类似 ``idf-diag-5aaa949b-40dc-4d53-96f1-1280c801585a``。使用 ``--output`` 选项可以指定不同的输出目录。如果指定的输出目录已存在，则 ``idf.py diag`` 命令不会执行，除非提供 ``--force`` 选项。

诊断数据通过 YAML 配置文件（名为 ``recipes``）收集。该文件定义了需包含在报告中的命令输出及需要复制的文件。可通过 ``idf.py --list`` 命令查看可用的 recipes 文件。默认情况下，所有 ``builtin`` recipes 均会使用，但可以通过 ``--recipe`` 选项修改，或使用 ``--tag`` 选项选择带有特定 ``tags`` 的 recipes 文件。更多信息，请参考 ``idf.py diag --help`` 以及 recipes_ 的详细描述。

默认情况下，报告目录中的诊断数据会进行脱敏处理，以移除可能包含的敏感信息，例如 URL 中的密码和令牌。此处理过程基于名为 ``purge`` 的 YAML 配置文件，其中包含由正则表达式和替换字符串组成的清理规则。可以使用 ``--purge`` 选项指定自定义的 purge 文件。详细信息请参考 purge_ 文件描述。

.. note::

    在共享报告目录前，务必仔细检查目录中是否包含敏感信息，删除不希望共享的文件。此外，请手动添加未自动收集但与问题相关的其他文件。

创建 Zip 归档文件
^^^^^^^^^^^^^^^^^^^^

检查并确认报告目录无误后，可将其归档为 zip 文件：

.. code-block:: bash

    $ idf.py diag --zip <REPORT_DIRECTORY>

其中，``REPORT_DIRECTORY`` 是由 ``idf.py diag`` 命令创建的报告目录。默认情况下，zip 归档文件的名称与 ``REPORT_DIRECTORY`` 相同，并添加 ``.zip`` 扩展名，如 ``idf-diag-5aaa949b-40dc-4d53-96f1-1280c801585a.zip``。可以使用 ``--output`` 选项为其指定不同的文件名。

报告归档文件可用于分享，例如，将其附加到 GitHub issue，提供有关项目环境的重要信息，协助问题排查。


其他信息
^^^^^^^^

更多信息请参考 esp-idf-diag_ 项目，或通过 ``idf.py diag --help`` 查看帮助信息。

.. _esp-idf-diag: https://github.com/espressif/esp-idf-diag
.. _recipes: https://github.com/espressif/esp-idf-diag/blob/master/esp_idf_diag/data/recipes/README.md
.. _purge: https://github.com/espressif/esp-idf-diag/tree/master/esp_idf_diag/data/purge
