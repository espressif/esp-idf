编写文档
=================

:link_to_translation:`en:[English]`

本文档简要总结了 `espressif/esp-idf`_ 库中的文档风格，并介绍了如何添加新文档。

概述
-----

为本仓库代码编写文档时，请遵循 `Doxygen style <https://www.doxygen.nl/manual/docblocks.html#specialblock>`_，即在标准注释块中插入特殊命令，如 ``@param``： ::

    /**
     * @param ratio this is oxygen to air ratio
     */

Doxygen 能够解析代码，提取此类特殊命令及其后续文本，并基于提取的信息构建文档。

下图为一个包含函数文档的典型注释块：

.. image:: ../../_static/doc-code-documentation-inline.png
    :align: center
    :alt: 内联代码文档样例

Doxygen 支持多种格式，并支持文档内部的多个详情级别，具有很强的灵活性。要了解更多可用功能，请参考 `Doxygen Manual <https://www.doxygen.nl/manual/index.html>`_。


为什么要用 Doxygen?
-------------------------

我们的最终目的是保证代码文档的一致性。因此，我们可以使用 `Sphinx`_ 和 `Breathe`_ 等工具，在代码变更时自动更新 API 文档。

借助这些工具，以上代码的渲染效果如下：

.. image:: ../../_static/doc-code-documentation-rendered.png
    :align: center
    :alt: 内联文档的代码及渲染样例


试一试！
-------------

为 ESP-IDF 库添加代码时，请遵循以下标准：

1. 为代码的所有构件提供文档说明，包括函数、结构体、类型定义、枚举、宏等。对其目的、功能和局限性进行充分介绍，提供符合读者预期的良好体验。

2. 函数文档应说明该函数的作用。如果函数接受输入参数并返回值，也需要对输入参数和返回值进行解释。

3. 请勿在参数前添加数据类型或除空格外的任何字符。所有空格和换行符都会压缩为一个空格。如需换行，请换行两次。

    .. image:: ../../_static/doc-code-function.png
        :align: center
        :alt: 内联文档的函数及渲染样例

4. 在函数输入为空或不返回值时，无需插入 ``@param`` 或 ``@return`` 命令。

    .. image:: ../../_static/doc-code-void-function.png
        :align: center
        :alt: 无参函数内联文档及渲染样例

5. 为 ``define`` ``struct`` 或 ``enum`` 的成员撰写文档时，需遵循以下格式在每个成员后添加注释。

    .. image:: ../../_static/doc-code-member.png
        :align: center
        :alt: 内联的成员注释及渲染样例

6. 要渲染出整齐的列表，可在命令后换行（如以下代码示例中的 ``@return`` 命令）。 ::

    *
    * @return
    *    - ESP_OK if erase operation was successful
    *    - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
    *    - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
    *    - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
    *    - other error codes from the underlying storage driver


7. 头文件或一组文件库的功能概述需在同一路径下单独的 ``README.rst`` 文档中进行描述。如该路径下还有其他 API 的头文件，则应将此头文件对应的 README 文件命名为 ``apiname-readme.rst``。


进阶用法
-------------

也可以使用以下进阶技巧，产出更加优质使用的文档。

在编写代码时，请遵循以下标准：

1. 使用 ``@code{c}`` 和 ``@endcode`` 命令添加代码示例片段，阐述实现过程。 ::

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

   该代码片段应添加在其所说明的函数的注释区中。

2. 要高亮重点信息，可使用 ``@attention`` 或 ``@note`` 命令。 ::

    *
    * @attention
    *     1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
    *     2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
    *

   以上示例也展示了有序列表的用法。

3. 要描述一组具有相似功能的函数，可使用 ``/**@{*/`` 和 ``/**@}*/`` 标记命令。 ::

    /**@{*/
    /**
     * @brief common description of similar functions
     *
     */
    void  first_similar_function (void);
    void second_similar_function (void);
    /**@}*/

   如需更多应用示例，请参考 :component_file:`nvs_flash/include/nvs.h`。

4. 如需进一步跳过重复定义或枚举等代码，可使用 ``/** @cond */`` 和 ``/** @endcond */`` 命令附上该代码。相关应用实例，请参考 :component_file:`esp_driver_gpio/include/driver/gpio.h`。

5. 使用 markdown 添加标题、链接和表格等，增强文档的可读性。 ::

    *
    * [{IDF_TARGET_NAME} Technical Reference Manual]({IDF_TARGET_TRM_EN_URL})
    *

.. note::

    如果没有将代码片段、说明或链接等内容包含在一个文档对象相关联的注释块中，则文档中不会出现相关内容。

6. 为一个或多个完整代码示例提供说明。将说明写入单独的 ``README.md`` 文件中并放在 :idf:`examples` 路径下对应文件夹。

文档格式标准化
--------------------------

撰写代码文档文本时，请遵循以下标准，提供格式良好的 Markdown (.md) 或 reST (.rst) 文档。

1. 确保将一个段落写在同一行。通过换行加强可读性仅适用于代码，请勿在文本中以下图形式换行。为方便阅读，建议在段落之间空一行。

    .. figure:: ../../_static/doc-format1-recommend.png
        :align: center
        :scale: 20%
        :alt: 一个段落写在同一行 - 推荐（点击放大）

        一个段落写在同一行（点击放大）

    .. figure:: ../../_static/doc-format2-notrecommend.png
        :align: center
        :scale: 20%
        :alt: 一个段落写在同一行 - 不推荐（点击放大）

        同一段落中请勿换行（点击放大）

2. 中英文文档行号需对齐，如下所示。这样做的好处是为作者和译者节省时间。非双语作者更新文档时，仅需更新对应中文或英文文档的同一行。对译者来说，如果英文文档出现更新，可快速定位对应中文文档需更新的位置。另外，通过比较英文和中文文档的总行数，可以快速检查文档的中文版本是否落后于英文版本。

    .. figure:: ../../_static/doc-format3-recommend.png
        :align: center
        :scale: 40%
        :alt: 对齐中英文文档行号（点击放大）

        对齐中英文文档行号（点击放大）

构建文档
-----------

文档由基于 `Sphinx <https://www.sphinx-doc.org/>`_ 的 Python 包 `esp-docs` 进行构建。

安装命令::

    pip install esp-docs

安装成功后，使用如下命令在 docs 文件夹中构建文档::

    build-docs build

或使用以下命令指定目标芯片和语言::

    build-docs -t esp32 -l en build

如需深入了解 `esp-docs` 的功能，请参考 `esp-docs <https://docs.espressif.com/projects/esp-docs/en/latest/>`_。

小结
------

出色的代码可以实现令人赞叹的功能，精心编写的文档则让开发者们如虎添翼。

期待你的贡献！

相关文档
------------

* :doc:`../api-reference/template`


.. _espressif/esp-idf: https://github.com/espressif/esp-idf/

.. _interactive shell: http://interactive.blockdiag.com/?compression=deflate&src=eJxlUMFOwzAMvecrrO3aITYQQirlAIIzEseJQ5q4TUSIq8TVGIh_J2m7jbKc7Ge_5_dSO1Lv2soWvoVYgieNoMh7VGzJR9FJtugZ7lYQ0UcKEbYNOY36rRQHZHUPT68vV5tceGLbWCUzPfeaFFMoBZzecVc56vWwJFnWMmJ59CCZg617xpOFbTSyw0pmvT_HJ7hxtFNGBr6wvuu5SCkchcrZ1vAeXZomznh5YgTqfcpR02cBO6vZVDeXBRjMjKEcFRbLh8f18-Z2UUBDnqP9wmp9ncRmSSfND2ldGo2h_zse407g0Mxc1q7HzJ3-4jzYYTJjtQH3iSV-fgFzx50J

.. _Breathe: https://breathe.readthedocs.io/en/latest/2
