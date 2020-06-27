链接脚本生成机制
================
:link_to_translation:`en:[English]`

概述
----

用于存放代码和数据的 :ref:` 内存区域 <memory-layout>`有多个。代码和只读数据默认存放在 flash 中，可写数据存放在 RAM 中。不过有时，我们必须更改默认存放区域，例如为了提高性能，将关键代码存放到 RAM 中，或者将代码存放到 RTC 存储器中以便在 :doc:`唤醒桩 <deep-sleep-stub>` 和 :doc:`ULP 协处理器 <ulp>` 中使用。

链接脚本生成机制可以让用户指定代码和数据在 ESP-IDF 组件中的存放区域。组件包含如何存放符号、目标或完整库的信息。在构建应用程序时，组件中的这些信息会被收集、解析并处理；生成的存放规则用于链接应用程序。

快速上手
--------

本段将指导如何使用 ESP-IDF 的即用方案，快速将代码和数据放入 RAM 和 RTC 存储器中。

假设我们有::

    - components/
                    - my_component/
                                    - CMakeLists.txt
                                    - component.mk
                                    - Kconfig
                                    - src/
                                          - my_src1.c
                                          - my_src2.c
                                          - my_src3.c
                                    - my_linker_fragment_file.lf

- 名为 ``my_component`` 的组件，在构建过程中存储为 ``libmy_component.a`` 库文件
- 库文件包含的三个源文件：``my_src1.c``、``my_src2.c`` 和 ``my_src3.c``，编译后分别为 ``my_src1.o``、``my_src2.o`` 和 ``my_src3.o``,
- 在 ``my_src1.o`` 定义的 ``my_function1`` 功能；在 ``my_src2.o`` 定义的 ``my_function2`` 功能
- 存储在 ``my_component`` 下 Kconfig 中的布尔类型配置 ``PERFORMANCE_MODE`` (y/n) 和整数类型配置 ``PERFORMANCE_LEVEL`` （范围是 0-3）                                    

创建和指定链接片段文件
^^^^^^^^^^^^^^^^^^^^^^

首先，我们需要创建链接片段文件。链接片段文件是一个拓展名为 ``.lf`` 的文本文件，文件内写有想要存放的位置。文件创建成功后，需要将其呈现在构建系统中。ESP-IDF 支持的构建系统指南如下：

Make
""""

在组件目录的 ``component.mk`` 文件中设置 ``COMPONENT_ADD_LDFRAGMENTS`` 变量的值，使其指向已创建的链接片段文件。路径可以为绝对路径，也可以为组件目录的相对路径。

.. code-block:: make

    COMPONENT_ADD_LDFRAGMENTS += my_linker_fragment_file.lf

CMake
"""""

在组件目录的 ``CMakeLists.txt`` 文件中，指定 ``idf_component_register`` 调用引数 ``LDFRAGMENTS`` 的值。``LDFRAGMENTS`` 可以为绝对路径，也可为组件目录的相对路径，指向刚才创建的链接片段文件。

.. code-block:: cmake

    # 相对于组件的 CMakeLists.txt 的文件路径
    idf_component_register(...
                           LDFRAGMENTS "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf"
                           ...
                           )


指定存放区域
^^^^^^^^^^^^

可以按照下列粒度指定存放区域：

    - 目标文件（``.obj`` 或 ``.o`` 文件）
    - 符号（函数/变量）
    - 库（``.a`` 文件）

.. _ldgen-placing-object-files :

存放目标文件
""""""""""""

假设整个 ``my_src1.o`` 目标文件对性能至关重要，所以最好把该文件放在 RAM 中。另外，``my_src2.o`` 目标文件包含从深度睡眠唤醒所需的符号，因此需要将其存放到 RTC 存储器中。
在链接片段文件中可以写入以下内容：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        my_src1 (noflash)     # 将所有 my_src1 代码和只读数据存放在 IRAM 和 DRAM 中
        my_src2 (rtc)         # 将所有 my_src2 代码、数据和只读数据存放到 RTC 快速 RAM 和 RTC 慢速 RAM 中

那么 ``my_src3.o`` 放在哪里呢？由于未指定存放区域，``my_src3.o`` 会存放到默认区域。更多关于默认存放区域的信息，请查看 :ref:`这里<ldgen-default-placements>`。


存放符号
""""""""

继续上文的例子，假设 ``object1.o`` 目标文件定义的功能中，只有 ``function1`` 影响到性能；``object2.o`` 目标文件中只有 ``function2`` 需要在芯片从深度睡眠中唤醒后运行。可以在链接片段文件中写入以下内容实现：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        my_src1:my_function1 (noflash)
        my_src2:my_function2 (rtc)

``my_src1.o`` 和 ``my_src2.o`` 中的其他函数以及整个 ``object3.o`` 目标文件会存放到默认区域。
要指定数据的存放区域，仅需将上文的函数名替换为变量名即可，如::

       my_src1:my_variable (noflash)

.. 注意::

    按照符号粒度存放代码和数据有一定的 :ref:`局限 <ldgen-symbol-granularity-placements>`。为确保存放区域合适，您也可以将相关代码和数据集中在源文件中，参考 :ref:`使用目标文件的存放规则 <ldgen-placing-object-files>`。

存放整个库
""""""""""

在这个例子中，假设整个组件库都需存放到 RAM 中，可以写入以下内容实现：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        * (noflash)

类似的，写入以下内容可以将整个组件存放到 RTC 存储器中：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        * (rtc)

根据具体配置存放
""""""""""""""""""""

假设只有在某个条件为真时，比如 ``CONFIG_PERFORMANCE_MODE == y`` 时，整个组件库才有特定存放区域，可以写入以下内容实现：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_MODE = y:
            * (noflash)
        else:
            * (default)

来看一种更复杂的情况。假设``CONFIG_PERFORMANCE_LEVEL == 1`` 时，只有 ``object1.o`` 存放到 RAM 中；``CONFIG_PERFORMANCE_LEVEL == 2`` 时，``object1.o`` 和 ``object2.o`` 会存放到 RAM 中；``CONFIG_PERFORMANCE_LEVEL == 3`` 时，库中的所有目标文件都会存放到 RAM 中。以上三个条件为假时，整个库会存放到 RTC 存储器中。虽然这种使用场景很罕见，不过，还是可以通过以下方式实现：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_LEVEL = 1:
            my_src1 (noflash)
        elif PERFORMANCE_LEVEL = 2:
            my_src1 (noflash)
            my_src2 (noflash)
        elif PERFORMANCE_LEVEL = 3:
            my_src1 (noflash)
            my_src2 (noflash)
            my_src3 (noflash)
        else:
            * (rtc)

也可以嵌套条件检查。以下内容与上述片段等效：

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_LEVEL <= 3 && PERFORMANCE_LEVEL > 0:
            if PERFORMANCE_LEVEL >= 1:
                object1 (noflash)
                if PERFORMANCE_LEVEL >= 2:
                    object2 (noflash)
                    if PERFORMANCE_LEVEL >= 3:
                        object2 (noflash)
        else:
            * (rtc)

.. _ldgen-default-placements:

默认存放区域
^^^^^^^^^^^^

到目前为止，“默认存放区域”在未指定 ``rtc`` 和 ``noflash`` 存放规则时才会使用，作为备选方案。需要注意的是，``noflash`` 或者 ``rtc`` 标记不仅仅是关键字，实际上还是被称作片段的实体，确切地说是 :ref:`协议 <ldgen-scheme-fragment>`。

与 ``rtc`` 和 ``noflash`` 类似，还有一个 ``默认`` 协议，定义了默认存放规则。顾名思义，该协议规定了代码和数据通常存放的区域，即代码和恒量存放在 flash 中，变量存放在 RAM 中。更多关于默认协议的信息，请见 :ref:`这里<ldgen-default-scheme>`。

.. note::
    使用链接脚本生成机制的 IDF 组件示例，请参阅 :component_file:`freertos/CMakeLists.txt`。为了提高性能，``freertos`` 使用链接脚本生成机制，将其目标文件存放到 RAM 中。

快速入门指南到此结束，下文将详述这个机制的内核，有助于创建自定义存放区域或修改默认方式。

链接脚本生成机制内核
--------------------

链接是将 C/C++ 源文件转换成可执行文件的最后一步。链接由工具链的链接器完成，接受指定代码和数据存放区域等信息的链接脚本。链接脚本生成机制的转换过程类似，区别在于传输给链接器的链接脚本根据(1) 收集的 :ref:`链接片段文件<ldgen-linker-fragment-files>` 和 (2) :ref:`链接脚本模板<ldgen-linker-script-template>` 动态生成。

.. note::

    执行链接脚本生成机制的工具存放在 :idf:`tools/ldgen` 之下。

.. _ldgen-linker-fragment-files :

链接片段文件
^^^^^^^^^^^^

如快速入门指南所述，片段文件是拓展名为 ``.lf`` 的简单文本文件，内含想要存放区域的信息。不过，这是对片段文件所包含内容的简化版描述。实际上，片段文件内包含的是“片段”。片段是实体，包含多条信息，这些信息放在一起组成了存放规则，说明目标文件各个段在二进制输出文件中的存放位置。片段一共有三种，分别是 :ref:`段<ldgen-sections-fragment>`、
:ref:`协议<ldgen-scheme-fragment>` 和 :ref:`映射<ldgen-mapping-fragment>`。

语法
""""

三种片段类型使用同一种语法：

.. code-block:: none

    [type:name]
    key: value
    key:
        value
        value
        value
        ...

- 类型：片段类型，可以为 ``段``、``协议`` 或 ``映射``。
- 名称：片段名称，指定片段类型的片段名称应唯一。 
- 键值：片段内容。每个片段类型可支持不同的键值和不同的键值语法。

.. note::

    多个片段的类型和名称相同时会引发异常。

.. note::

    片段名称和键值只能使用字母、数字和下划线。

.. _ldgen-condition-checking :

**条件检查**

条件检查使得链接脚本生成机制可以感知配置。含有配置值的表达式是否为真，决定了使用哪些特定键值。检查使用的是 :idf_file:`tools/kconfig_new/kconfiglib.py` 脚本的 ``eval_string``，遵循该脚本要求的语法和局限性，支持：

    - 比较
        - 小于 ``<``
        - 小于等于 ``<=``
        - 大于 ``>``
        - 大于等于 ``>=``
        - 等于 ``=``
        - 不等于 ``!=``
    - 逻辑
        - 或 ``||``
        - 和 ``&&``
        - 否定？取反？ ``!``
    - 分组
        - 圆括号 ``()``

条件检查和其他语言中的 ``if...elseif/elif...else`` 块作用一样。键值和完整片段都可以进行条件检查。以下两个示例效果相同：

.. code-block:: none

    # 键值取决于配置
    [type:name]
    key_1:
        if CONDITION = y:
            value_1
        else:
            value_2
    key_2:
        if CONDITION = y:
            value_a
        else:
            value_b

.. code-block:: none

    # 完整片段的定义取决于配置
    if CONDITION = y:
        [type:name]
        key_1:
            value_1
        key_2:
            value_b
    else:
        [type:name]
        key_1:
            value_2
        key_2:
            value_b


**注释**

链接片段文件中的注释以 ``#`` 开头。和在其他语言中一样，注释提供了有用的描述和资料，在处理过程中会被忽略。

与 ESP-IDF v3.x 链接脚本片段文件兼容
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

ESP-IDF v4.0 变更了链接脚本片段文件使用的一些语法：

- 必须缩进，缩进不当的文件会产生解析异常；旧版本不强制缩进，但之前的文档和示例均遵循了正确的缩进语法
- 条件改用 ``if...elif...else`` 结构，可以嵌套检查，将完整片段置于条件内
- 映射片段和其他片段类型一样，需有名称

链接脚本生成器可解析 ESP-IDF v3.x 版本中缩进正确的链接片段文件（如 ESP-IDF v3.x 版本中的本文件所示），依然可以向后兼容此前的映射片段语法（可选名称和条件的旧语法），但是会有弃用警告。用户应换成本文档介绍的新语法，因为旧语法将在未来停用。

请注意，ESP-IDF v3.x 不支持使用 ESP-IDF v4.0 新语法的链接片段文件。

类型
""""

.. _ldgen-sections-fragment :

**段**

段定义了 GCC 编译器输出的一系列目标文件段，可以是默认段（如 ``.text``、``.data``），也可以是用户通过 ``__attribute__`` 关键字定义的段。

'+' 表示段列表开始，且当前段为列表中的第一个段。这种表达方式更加推荐。

.. code-block:: none

    [sections:name]
    entries:
        .section+
        .section
        ...

示例：

.. code-block:: none

    # 不推荐的方式
    [sections:text]
    entries:
        .text
        .text.*
        .literal
        .literal.*

    # 推荐的方式，效果与上面等同
    [sections:text]
    entries:
        .text+              # 即 .text 和 .text.*
        .literal+           # 即 .literal 和 .literal.*

.. _ldgen-scheme-fragment :

**协议**

协议定义了每个段对应的 ``目标``。

.. code-block:: none

    [scheme:name]
    entries:
        sections -> target
        sections -> target
        ...

示例：

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text          # text 段下的所有条目均归入 iram0_text
        rodata -> dram0_data        # rodata 段下的所有条目均归入 dram0_data

.. _ldgen-default-scheme:

``默认`` 协议

注意，有一个 ``默认`` 的协议很特殊，特殊在于包罗存放规则都是根据这个协议中的条目生成的。这意味着，如果该协议有一条条目是 ``text -> flash_text``，则将为目标 ``flash_text`` 生成如下的存放规则:

.. code-block:: none

    *(.literal .literal.* .text .text.*)

这些生成的包罗规则将用于未指定映射规则的情况。

``默认`` 协议在 :component_file:`{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}_fragments.lf` 文件中定义，快速上手指南中提到的内置 ``noflash`` 协议和 ``rtc`` 协议也在该文件中定义。

.. _ldgen-mapping-fragment :

**映射**

映射定义了可映射实体（即目标文件、函数名、变量名和库）对应的协议。

.. code-block:: none

    [mapping]
    archive: archive                # 构建后输出的库文件名称（即 libxxx.a）
    entries:
        object:symbol (scheme)      # 符号
        object (scheme)             # 目标
        * (scheme)                  # 库

有三种存放粒度：

    - 符号：指定了目标文件名称和符号名称。符号名称可以是函数名或变量名。
    - 目标：只指定目标文件名称。
    - 库：指定 ``*``，即某个库下面所有目标文件的简化表达法。

为了更好地理解条目的含义，我们看一个按目标存放的例子。

.. code-block:: none

    object (scheme)

根据条目定义，将这个协议展开：

.. code-block:: none

    object (sections -> target,
            sections -> target,
            ...)

再根据条目定义，将这个段展开：

.. code-block:: none


    object (.section,
            .section,
            ... -> target, # 根据目标文件将这里所列出的所有段放在该目标位置

            .section,
            .section,
            ... -> target, # 同样的方法指定其他段

            ...)           # 直至所有段均已展开

示例：

.. code-block:: none

    [mapping:map]
    archive: libfreertos.a
    entries:
        * (noflash)

.. _ldgen-symbol-granularity-placements :

按符号存放
""""""""""

按符号存放可通过编译器标志 ``-ffunction-sections`` 和 ``-ffdata-sections`` 实现。ESP-IDF 默认用这些标志编译。
用户若选择移除标志，便不能按符号存放。另外，即便有标志，也会其他限制，具体取决于编译器输出的段。

比如，使用 ``-ffunction-sections``，针对每个功能会输出单独的段。段的名称可以预测，即 ``.text.{func_name}``
和 ``.literal.{func_name}``。但是功能内的字符串并非如此，因为字符串会进入字符串池，或者使用生成的段名称。

使用 ``-fdata-sections``，对全局数据来说编译器可输出 ``.data.{var_name}``、``.rodata.{var_name}`` 或 ``.bss.{var_name}``；因此 ``类型 I`` 映射词条可以适用。
但是，功能中声明的静态数据并非如此，生成的段名称是将变量名称和其他信息混合。

.. _ldgen-linker-script-template :

链接脚本模板
^^^^^^^^^^^^

链接脚本模板是指定存放规则的存放位置的框架，与其他链接脚本没有本质区别，但带有特定的标记语法，可以指示存放生成的存放规则的位置。

如需引用一个 ``目标`` 标记下的所有存放规则，请使用以下语法：

.. code-block:: none

    mapping[target]

示例：

以下示例是某个链接脚本模板的摘录，定义了输出段 ``.iram0.text``，该输出段包含一个引用目标 ``iram0_text`` 的标记。

.. code-block:: none

    .iram0.text :
    {
        /* 标记 IRAM 空间不足 */
        _iram_text_start = ABSOLUTE(.);

        /* 引用 iram0_text */
        mapping[iram0_text]

        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

假设链接脚本生成器收集到了以下片段定义：

.. code-block:: none

    [sections:text]
        .text+
        .literal+

    [sections:iram]
        .iram1+

    [scheme:default]
    entries:
        text -> flash_text
        iram -> iram0_text

    [scheme:noflash]
    entries:
        text -> iram0_text

    [mapping:freertos]
    archive: libfreertos.a
    entries:
        * (noflash)

则该脚本生成器生成的链接脚本文件，其摘录应如下所示：

.. code-block:: c

    .iram0.text :
    {
        /* 标记 IRAM 空间不足 */
        _iram_text_start = ABSOLUTE(.);

        /* 处理片段生成的存放规则，存放在模板标记的位置处 */
        *(.iram1 .iram1.*)
        *libfreertos.a:(.literal .text .literal.* .text.*)

        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

``*libfreertos.a:(.literal .text .literal.* .text.*)``

    这是根据 ``freertos`` 映射的 ``* (noflash)`` 条目生成的规则。``libfreertos.a`` 库下所有目标文件的所有 ``text`` 段会收集到 ``iram0_text`` 目标下（按照 ``noflash`` 协议），并放在模板中被 ``iram0_text`` 标记的地方。

``*(.iram1 .iram1.*)``

    这是根据默认协议条目 ``iram -> iram0_text`` 生成的规则。默认协议指定了 ``iram -> iram0_text`` 条目，因此生成的规则同样也放在被 ``iram0_text`` 标记的地方。由于该规则是根据默认协议生成的，因此在同一目标下收集的所有规则下排在第一位。

    目前使用的链接脚本模板是 :component_file:`{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}.project.ld.in`，由 ``{IDF_TARGET_PATH_NAME}`` 组件指定，生成的脚本存放在构建目录下。
    