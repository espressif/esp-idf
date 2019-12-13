编写代码文档
============

:link_to_translation:`en:[English]`

本文简要介绍了 `espressif/esp-idf`_ 项目库采用的文件风格以及如何在项目库中添加新文件。

概述
----

在项目库内编写代码文档时，请遵循 `Doxygen 代码注释风格 <https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#specialblock>`_。要采用这一风格，您可以将 ``@param`` 等特殊命令插入到标准注释块中，比如： ::

    /**
     * @param ratio this is oxygen to air ratio
     */

Doxygen 会解析代码，提取命令和后续文本，生成代码文档。

注释块通常包含对功能的记述，如下所示。

.. image:: ../../_static/doc-code-documentation-inline.png
    :align: center
    :alt: 内联代码样本文档
 
Doxygen 支持多种排版风格，对于文档中可以包含的细节非常灵活。请参考数据丰富、条理清晰的 `Doxygen 手册 <https://www.stack.nl/~dimitri/doxygen/manual/index.html>`_ 熟悉 Doxygen 特性。


为什么需要 Doxygen?
--------------------

使用 Doxygen 的最终目的是确保所有代码编写风格一致，以便在代码变更时使用 `Sphinx <http://www.sphinx-doc.org/>`_ 和 `Breathe <https://breathe.readthedocs.io/>`_ 等工具协助筹备、自动更新 API 文档。 

使用这类工具时，上文代码渲染后呈现效果如下：

.. image:: ../../_static/doc-code-documentation-rendered.png
    :align: center
    :alt: 渲染后的内联代码样本文档


尝试一下！
----------

在本项目库编写代码文档时，请遵守下列准则。

1. 写明代码的基本内容：函数、结构、类型定义、枚举、宏等。请详细说明代码的用途、功能和限制，因为在阅读他人的文档时你也想看到这些信息。

2. 函数文档需简述该函数的功能，并解释输入参数和返回值的含义。

3. 请不要在参数或除空格外的其他字符前面添加数据类型。所有空格和换行符都会压缩为一个空格。如需换行，请执行换行操作两次。

    .. image:: ../../_static/doc-code-function.png
        :align: center
        :alt: 内联函数样本文档及渲染后的效果

4. 如果函数没有输入参数或返回值，请跳过 ``@param`` 或 ``@return``。

    .. image:: ../../_static/doc-code-void-function.png
        :align: center
        :alt: 隐式内联函数样本文档及渲染后的效果
 
5. 为 ``define``、``struct`` 和 ``enum`` 的成员编写文档时，请在每一项后添加注释，如下所示。

    .. image:: ../../_static/doc-code-member.png
        :align: center
        :alt: 内联函数成员样本文档及渲染后的效果

6. 请在命令后换行（如下文中的 ``@return`` ），呈现排版精美的列表。 ::

    *
    * @return
    *    - ESP_OK if erase operation was successful
    *    - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
    *    - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
    *    - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
    *    - other error codes from the underlying storage driver
    *
 
7. 头文件的功能概览和库文件应当存在同一个项目库之下，放入单独的 ``README.rst`` 文件。如果目录下包含不同 API 的头文件，应将文件命名为 ``apiname-readme.rst``。


进阶
----

以下小贴士可以帮助你进一步提高文档质量，增强可读性。

1. 添加代码片段举例说明。请在片段前后添加 ``@code{c}`` 和 ``@endcode`` 命令。 ::

    *
    * @code{c}
    * // Example of using nvs_get_i32:
    * int32_t max_buffer_size = 4096; // default value
    * esp_err_t err = nvs_get_i32(my_handle, "max_buffer_size", &max_buffer_size);
    * assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
    * // if ESP_ERR_NVS_NOT_FOUND was returned, max_buffer_size will still
    * // have its default value.
    * @endcode
    *

   代码片段应放入所介绍功能的注释块中。

2. 使用 ``@attention`` 或 ``@note`` 命令高亮显示重要信息。 ::

    *
    * @attention
    *     1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
    *     2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
    *

   上述例子介绍了如何使用编号列表。

3. 给相似的函数编写文档时，可在前后使用 ``/**@{*/`` 和 ``/**@}*/`` 标记命令。 ::

    /**@{*/
    /**
     * @brief common description of similar functions
     *
     */
    void  first_similar_function (void);
    void second_similar_function (void);
    /**@}*/ 

   示例请参照 :component_file:`nvs_flash/include/nvs.h`。

4. 如果想跳过重复的宏定义、枚举项等代码，不添加描述，请在代码前后添加 ``/** @cond */`` 和 ``/** @endcond */`` 命令。示例请参照 :component_file:`driver/include/driver/gpio.h`。

5. 使用 markdown 增强文档可读性，添加页眉、链接、表格及更多内容。 ::

    *
    * [ESP32 技术参考手册](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf)
    *

.. note::

    代码片段、注释、链接等内容如没有附在所述对象对应的注释块中，将不会添加到文档中。

6. 准备一个或更多完整的代码示例和描述，将描述放入单独的 ``README.md`` 文件中，置于 :idf:`examples` 目录的特定文件夹中。 

.. _link-custom-roles:

链接到示例
----------

链接到 GitHub 上的示例时，请不要使用绝对 URLs 或硬编码 URLs。请使用 Docutils 自定义角色生成链接。自动生成的链接指向项目库中 git commit 编号（或标记）的 tree 或 blob。这种做法可以确保 master 分支上的文件移动或删除时，链接不会失效。

有如下角色可以选择：

- ``:idf:`path``` - 指向 ESP-IDF 内的目录
- ``:idf_file:`path``` - 指向 ESP-IDF 内的文件
- ``:idf_raw:`path``` - 指向 ESP-IDF 内的原始格式文件
- ``:component:`path``` - 指向 ESP-IDF components 内的文件夹
- ``:component_file:`path``` - 指向 ESP-IDF components 内的文件
- ``:component_raw:`path``` - 指向 ESP-IDF components 内的原始格式文件
- ``:example:`path``` - 指向 ESP-IDF examples 内的文件夹
- ``:example_file:`path``` - 指向 ESP-IDF examples 内的文件
- ``:example_raw:`path``` - 指向 inside ESP-IDF examples 内的原始格式文件

示例::

    * :example:`get-started/hello_world`
    * :example:`Hello World! <get-started/hello_world>`

渲染效果：

* :example:`get-started/hello_world`
* :example:`Hello World! <get-started/hello_world>`

CI build 脚本中添加了检查功能，查找 RST 文件中的硬编码链接（通过 URL 的 tree/master、blob/master 或 raw/master 部分识别）。该功能可通过 ``cd docs`` 和 ``make gh-linkcheck`` 命令手动运行。


.. _link-language-versions:

链接到其他语言文档
------------------

要切换不同语言的文档，可使用 ``:link_to_translation:`` 自定义角色。文档页面中的角色提供其他语言版本的链接。下文的例子说明了如何在文档中添加中英文版本的链接::

    :link_to_translation:`zh_CN:中文版`
    :link_to_translation:`en:English`

语言用 ``en`` 或 ``zh_CN`` 等标准简写表示。最后一个分号后的文本非标准化内容，可根据链接的位置自行输入，如::

    :link_to_translation:`en:see description in English`


.. _add-illustrations:

添加图例
--------

请考虑使用图表和图片解释表述的概念。

相比于长篇的表述，图例有时可以更好地描述复杂的理念、数据结构或算法。本项目库使用`blockdiag <http://blockdiag.com/en/index.html>`_ 工具包由简单的文本文件生成图表。

工具包支持下列图表类型：

* `框图 <http://blockdiag.com/en/blockdiag/index.html>`_
* `时序图 <http://blockdiag.com/en/seqdiag/index.html>`_
* `活动图 <http://blockdiag.com/en/actdiag/index.html>`_
* `逻辑网络图 <http://blockdiag.com/en/nwdiag/index.html>`_

使用该工具包，可以将简单的文本（与 graphviz 的 DOT 格式类似）转换成美观的图片。图中内容自动排版。图标代码之后会转换为 ".png" 图片，在后台添加进 **Sphinx** 文档中。 

要查看图表的渲染效果，可使用线上的 `interactive shell`_ 即时显示生成的图片。

下面是一些图表示例：

* 简单的 **框图** / ``blockdiag`` - `Wi-Fi Buffer 配置 <http://interactive.blockdiag.com/?compression=deflate&src=eJylUk1rwkAQvfsrBntpIUKiRQqSgK0VSj0EtCi0EjbJxCyuuyG7QW3pf-9m06hJeyg0t33zmHkfCZmItjElGwiLJME8IEwjRFHBA3WAj04H9HcFGyZCwoAoldOwUCgNzkWMwZ7GKgUXnKE9gjOcIt2kSuN39sigMiP8jDqX6GmF_Y3GmJCCqUCmJEM9yEXBY4xDcWjOE8GVpO9oztdaGQmRSRAJlMZysjOCKsVj358Fi_H8GV4Nze2Os4zRyvEbB0XktrseQWVktn_ym-wS-UFb0ilt0pa0N6Vn3i_KUEY5zcqrbXWTx_nDaZHjwYvEHGKiSNeC2q_r3FpQZekObAtMTi4XCi2IBBO5e0Rd5L7ppLG574GvO__PUuO7sXTgweTIyY5GcD1XOtToBhYruDf_VvuUad3tD-0_Xq1TLPPSI84xKvNrF9vzLnrTj1M7rYhrXv24cCPVkZUaOK47n1-lOvbk>`_
* 稍复杂的 **框图** - `Wi-Fi 编程模型 <http://interactive.blockdiag.com/?compression=deflate&src=eJyFk09P40AMxe98CqscIVILq72UIFX8kSoQWy0RHABFTuImFtOZaGYKuyC-O840bagaRI7Pfs7Pz0mmTP5cMJbwynNOa2tKi4sF6zJdmIIUvO_tgTz7UCqToQL03nK29OSCrqUpfeXCVxDD6Gg47tSKuKy8yL9b1dWov1E3E4atWtAcl8qnrsKapGDNUhdUZObfdr2UQp3mRhkrXdpoGq-BGwhQmJFaoSZns_Q2mZxdwUNQ44Eojxqcx_x5cAhzo73jN4pHv55WL7m4u0nSZHLbOeiFtBePR9dvmcxm19sWrGvFOXo2utd4CGH5eHQ8bGfcTy-n6fnfO9jMuOfoksV9bvmFbO-Lr27-JPAQ4oqbGJ62c8iN1pQ3EA4O-lOJTncXDvvupCGdu3vmqFQmSQqm3CIYBx0EWou6pADjQJbw3Bj-h3I4onxpsHrCQLnmoD0yVKgLJXuP1x3GsowPmUpfbay3yH5T7khPoi7NnpU-1nisPdkFyY_gV4x9XB3Y0pHdpfoJ60toURQOtqbYuvpJ1B6zDXYym0qmTVpNnh-fpWcbRA>`_
* **时序图** / ``seqdiag`` - `在所有信道中扫描特定 AP <http://interactive.blockdiag.com/seqdiag/?compression=deflate&src=eJyVkU1PwzAMhu_7FdburUgQXMomTaPcKIdOIIRQlDVuG1EloUknPsR_J2s2rRsT2nKJ9drvY8ex-C4kr8AWXLFSt8waLBg38D0Cf3jh5Io7qRVMQGmFSS-jqJA1qCpXe51cXwTZGg-pUVa1W8tXQRVY8q5xzNbcoNdb3SmBYqk_9vOlVs7Kr3UJoQmMwgDGMMftWwK4QuU28ZOM7uQm3q_zYTQd5OGl4UtsJmMSE5jCXKtSVl2LUPgpXPvpb4Hj1-RUCPWQ3O_K-wKpX84WMLAcB9B-igCouVLYADnDTA_N9GRzHMdnNMoOG2Vb8-4b4CY6Zr4MT3zOF-k9Sx_TbMHy-Sxjtw9Z-mfRHjEA7hD0X8TPLxU91AQ>`_
* **包图** / ``packetdiag`` - `NVS 页面结构 <http://interactive.blockdiag.com/packetdiag/?compression=deflate&src=eJxFkMFOwzAQRO_9ij2mh63idRKaSj1V_ACIE6DIxG4StTgh3oCg6r_j2JTs8c3szNqDqk-GdacasJ-uGlRjKsfjVPM0GriswE_dn786zS3sQRJAYLbXprpRkS-sNV3TcrAGqM1RTWeujr1l1_2Y2U6rIKUod_DIis2LTbJ1YBneeWY-Nj5ts-AtkudPdnJGQ0JppLRFKXZweDhIWrySsPDB95bHb3BzPLx1_K4GSCSt_-4vMizzmykNSuBlgWKuioJYBOHLROnbEBGe_ZfEh-7pNcolIdF_raA8rl5_AaqqWyE>`_

尝试修改源代码，看看图表会发生什么变化。

.. note::

     `interactive shell`_ 使用的字体和 esp-idf 文档使用的字体略有不同。


添加注释
--------

写文档时，您可能需要：

- 留下建议，说明之后需添加会修改哪些内容。
- 提醒自己或其他人跟进。

这时，您可以使用 ``.. todo::`` 命令在 reST 文件中添加待做事项。如：

.. code-block:: none

   .. todo::

      Add a package diagram.

如果在 reST 文件中添加 ``.. todolist::`` 命令，整篇文档中的所有待做事项将会罗列成表。

默认情况下，文档生成器会忽视 ``.. todo::`` 和 ``.. todolist::`` 命令。如果您想在本地生成的文档中显示注释和注释列表，请执行下列步骤：

1. 打开本地的 ``conf_common.py`` 文件。
2. 找到 ``todo_include_todos`` 参数。
3. 将该参数的值由 ``False`` 改为 ``True``。

将改动推送到远端分支之前，请把 ``todo_include_todos`` 的值重置为 ``False``。

更多关于扩展的信息，请参阅 `sphinx.ext.todo <https://www.sphinx-doc.org/en/master/usage/extensions/todo.html#directive-todolist>`_ 的相关文档。



汇总文档
--------

文档准备好后，请参照 :doc:`../api-reference/template` 的要求创建一个文件，汇总所有准备好的文档。最后，在文件中添加链接指向 ``/docs`` 文件夹或子文件夹下 ``index.rst`` 文件的 ``.. toctree::``。


Sphinx 新手怎么办
------------------

1. 不要担心。所有需要的软件均有详细文档，并且开源、免费。您可以先查看 `Sphinx <http://www.sphinx-doc.org/>`_ 文档。如果您不清楚如何用 rst markup 语言写作，请查看  `reStructuredText Primer <http://www.sphinx-doc.org/en/stable/rest.html>`_。您也可以使用  markdown (.md) 文件，查找更多在 `Recommonmark parser' 文档页面 <https://recommonmark.readthedocs.io/en/latest/>`_ 使用的特定 markdown 句法信息。

2. 查看本文档的源文件，了解本文档使用的代码。源文件存储于 GitHub `espressif/esp-idf`_ 项目库的 :idf:`docs` 文件夹下。您可以滑动到页面上方，点击右上角的链接，直接查看本页面的源文件。您也可以通过点击 ``Raw`` 按键打开源文件，在 GitHub 上查看文件的代码。

3. 想要查看在上传至 GitHub 前文档如何生成、呈现，有两种方式：

    * 安装`Sphinx <http://www.sphinx-doc.org/>`_、 `Breathe <https://breathe.readthedocs.io/>`_、 `Blockdiag <http://blockdiag.com/en/index.html>`_ 和 `Doxygen <https://www.stack.nl/~dimitri/doxygen/>`_ 本地生成文档，具体可查看下文。
   
    * 在 `Read the Docs <https://readthedocs.org/>`_ 建立账号，在云端生成文档。 Read the Docs 免费提供文档生成和存储，且速度快、质量高。

4. 在生成文档前预览，可使用 `Sublime Text <https://www.sublimetext.com/>`_ 编辑器和 `OmniMarkupPreviewer <https://github.com/timonwong/OmniMarkupPreviewer>`_ 插件。 


.. _setup-for-building-documentation:

搭建环境本地生成文档
--------------------

您可以安装下列包，通过搭建环境在电脑上本地生成文档：

1. Doxygen - https://www.stack.nl/~dimitri/doxygen/
2. Sphinx - https://github.com/sphinx-doc/sphinx/#readme-for-sphinx
3. Breathe - https://github.com/michaeljones/breathe#breathe
4. Document theme "sphinx_rtd_theme" - https://github.com/rtfd/sphinx_rtd_theme
5. Custom 404 page "sphinx-notfound-page" - https://github.com/rtfd/sphinx-notfound-page
6. Blockdiag - http://blockdiag.com/en/index.html
7. Recommonmark - https://github.com/rtfd/recommonmark

添加 "sphinx_rtd_theme" 包之后，文档将与 `ESP32 编程指南 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/index.html>`_ 的风格保持一致，与 "Read the Docs" 网站的显示效果一样.

不用担心需要安装太多包。除 Doxygen 之外，其他包均使用 Python 语言，可一键安装。

Doxygen 的安装取决于操作系统：

**Linux**

::  

	sudo apt-get install doxygen

**Windows** - 在 MSYS2 控制台中安装

::

	pacman -S doxygen

**MacOS**

::

	brew install doxygen

.. note::

    如果您是在 Windows 系统上安装（Linux 和 MacOS 用户可以跳过此说明），在安装 **之前**，请完成以下两步。这是安装 :ref:`add-illustrations` 提到的 "blockdiag" 依赖项的必须步骤。

    1.  更新所有系统包：

        ::

            $ pacman -Syu

        该过程可能需要重启 MSYS2 MINGW32 控制台重复上述命令，直至更新完成。

    2.  安装 *blockdiag* 的依赖项之一 *pillow*：

        ::

            $ pacman -S mingw32/mingw-w64-i686-python2-pillow

        查看屏幕上的记录，确定 ``mingw-w64-i686-python2-pillow-4.3.0-1`` 已安装。旧版本 *pillow* 无法运行。

    Windows 安装 Doxygen 的缺点是 `blockdiag pictures <add-illustrations>` 字体不能正确加载，可能会存在乱码。在此问题解决之前，您可以使用 `interactive shell`_ 查看完整图片。

其他所有应用都是 `Python <https://www.python.org/>`_ 包，可以按照下列步骤一键安装：

::

	cd ~/esp/esp-idf/docs
	pip install --user -r requirements.txt

.. note::

	安装步骤设定将 ESP-IDF 放在 ``~/esp/esp-idf`` 目录下，这是文档中使用的 ESP-IDF 默认地址。

更换到特定语言文件所在的目录::

    cd en

现在可以调用如下命令生成文档::

    make html

这一步骤需要几分钟时间。完成后，文档会放置在 ``~/esp/esp-idf/docs/en/_build/html`` 文件夹下。您可以在网页浏览器中打开 ``index.html`` 查看。 


大功告成
--------

我们喜欢可以做酷炫事情的好代码。
但我们更喜欢有清晰文档的好代码，可以让读者快速上手，做酷炫的事情。

尝试一下，贡献你的代码和文档！


相关文档
--------

* :doc:`../api-reference/template`
* :doc:`add-ons-reference`


.. _espressif/esp-idf: https://github.com/espressif/esp-idf/

.. _interactive shell: http://interactive.blockdiag.com/?compression=deflate&src=eJxlUMFOwzAMvecrrO3aITYQQirlAIIzEseJQ5q4TUSIq8TVGIh_J2m7jbKc7Ge_5_dSO1Lv2soWvoVYgieNoMh7VGzJR9FJtugZ7lYQ0UcKEbYNOY36rRQHZHUPT68vV5tceGLbWCUzPfeaFFMoBZzecVc56vWwJFnWMmJ59CCZg617xpOFbTSyw0pmvT_HJ7hxtFNGBr6wvuu5SCkchcrZ1vAeXZomznh5YgTqfcpR02cBO6vZVDeXBRjMjKEcFRbLh8f18-Z2UUBDnqP9wmp9ncRmSSfND2ldGo2h_zse407g0Mxc1q7HzJ3-4jzYYTJjtQH3iSV-fgFzx50J

