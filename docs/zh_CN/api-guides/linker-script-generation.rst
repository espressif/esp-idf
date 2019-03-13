链接脚本生成机制
================
:link_to_translation:`en:[English]`

概述
----

ESP32 的代码和数据可以存放在多个 :ref:`内存区域 <memory-layout>`。通常，代码和只读数据存放在 flash 区域，可写数据存放在内存中。我们经常需要更改代码或者数据的默认映射区域，例如为了提高性能，将关键部分的代码和只读数据放置到内存中，或者将代码、数据和只读数据存放到 RTC 内存中以便在 :doc:`唤醒桩 <deep-sleep-stub>` 和 :doc:`ULP 协处理器 <ulp>` 中使用。

IDF 的链接脚本生成机制允许用户在组件级别定义代码和数据的存放区域。组件通过 :ref:`链接片段文件 <ldgen-fragment-files>` 描述如何映射目标文件的输入段（甚至可以是某个具体的函数或者数据）。在构建应用程序时，链接片段文件会被收集、解析并处理，然后扩充到 :ref:`链接脚本模板 <ldgen-script-templates>` 中形成最终的链接脚本文件，该链接脚本会被用于链接最终的二进制应用程序。

快速上手
--------

本节将指导如何快速将代码和数据放入 RAM 和 RTC 内存中，并演示如何使这些放置规则依赖于项目的配置。本节内容重在指导快速入门，因此并未在使用前详细介绍所有涉及的术语和概念，但在首次提及此类术语或概念时，均提供了相应链接，以帮助您的理解。

.. _ldgen-add-fragment-file :

准备工作
^^^^^^^^

Make
""""

在组件目录中新建一个链接片段文件，该文件是一个扩展名为 ``.lf`` 的文本文件。为了能够让构建系统收集到此片段文件，需要为组件添加一个条目，在调用 ``register_component`` 之前设置 ``COMPONENT_ADD_LDFRAGMENTS`` 变量的值，使其指向刚才的链接片段文件。

.. code-block:: make

    # 文件路径相对于组件的 Makefile
    COMPONENT_ADD_LDFRAGMENTS += "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf"

CMake
"""""

对于 CMake 构建系统来说，需要在调用 ``register_component`` 之前设置 ``COMPONENT_ADD_LDFRAGMENTS`` 变量的值，使其指向链接片段文件。

.. code-block:: cmake

    # 文件路径相对于组件的 CMakeLists.txt
    set(COMPONENT_ADD_LDFRAGMENTS "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf")

    register_component()

也可以使用函数 ``ldgen_add_fragment_files`` 在项目的 CMakeLists.txt 文件或者组件的 project_include.cmake 文件中指定该片段文件 ::

    ldgen_add_fragment_files(target files ...)


指定放置区域
^^^^^^^^^^^^

链接脚本生成机制允许指定以下条目的存放位置：

    - 组件中的一个或多个目标文件
    - 一个或多个函数/变量（使用它们的名字来指定）
    - 整个组件库

在继续讲解之前，假设我们的组件包含以下内容：

    - 一个名为 ``component`` 的组件，在构建期间被归档为 ``libcomponent.a`` 库文件
    - 该库中有三个存档的目标文件： ``object1.o``，``object2.o`` 和 ``object3.o``
    - ``object1.o`` 中定义了 ``function1`` 函数，``object2.o`` 中定义了 ``function2`` 函数
    - 在其中的一个 IDF KConfig 文件中存在 ``PERFORMANCE_MODE`` 和 ``PERFORMANCE_LEVEL`` 两个配置，相应地，项目的 sdkconfig 文件会通过 ``CONFIG_PERFORMANCE_MODE`` 和 ``CONFIG_PERFORMANCE_LEVEL`` 这两个宏来指示当前设置的值

在新建的链接片段文件中输入以下内容：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:

这会创建一个空的 :ref:`mapping 片段 <ldgen-mapping-fragment>`，它并不会执行任何操作。在链接期间，会使用 :ref:`默认的存放规则 <ldgen-default-placements>` 来映射 ``libcomponent.a``，除非填充了 ``entries`` 字段。

.. _ldgen-placing-object-files :

放置目标文件
""""""""""""

假设整个 ``object1.o`` 目标文件对性能至关重要，所以最好把它放在 RAM 中。另一方面，假设``object2.o`` 目标文件包含有从深度睡眠唤醒所需的数据，因此需要将它存放到 RTC 内存中。可以在链接片段文件中写入以下内容：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        object1 (noflash)     # 将所有代码和只读数据放置在 IRAM 和 DRAM 中
        object2 (rtc)         # 将所有代码、数据和只读数据放置到 RTC 快速内存和 RTC 慢速内存中

那么 ``object3.o`` 放在哪里呢？由于未指定放置规则，它会被存放到默认区域。

放置函数和数据
""""""""""""""

假设在 ``object1.o`` 目标文件中只有 ``function1`` 是与性能密切相关，且在 ``object2.o`` 目标文件中只有 ``function2`` 需要在深度睡眠唤醒后执行。可以在链接片段文件中写入以下内容：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        object1:function1 (noflash) 
        object2:function2 (rtc) 

``object1.o`` 和 ``object2.o`` 的剩余函数以及整个 ``object3.o`` 目标文件会被存放到默认区域。指定数据存放区域的方法很类似，仅需将 ``：`` 之后的函数名，替换为变量名即可。

.. warning::

    使用符号名来指定放置区域有一定的 :ref:`局限 <ldgen-type1-limitations>`。因此，您也可以将相关代码和数据集中在源文件中，然后根据 :ref:`使用目标文件的放置规则 <ldgen-placing-object-files>` 进行放置。

放置整个组件
""""""""""""

在这个例子中，假设我们需要将整个组件存放到 RAM 中，可以这样写：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        * (noflash)

类似的，下面的写法可以将整个组件存放到 RTC 内存中：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        * (rtc)

依赖于具体配置的存放方式
""""""""""""""""""""""""

假设只有当 sdkconfig 文件中存在 ``CONFIG_PERFORMANCE_MODE == y`` 时，整个组件才会被放置到指定区域，可以这样写：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        : PERFORMANCE_MODE = y
        * (noflash)

其含义可以通过如下伪代码来表述：

.. code-block:: none

    if PERFORMANCE_MODE = y
        place entire libcomponent.a in RAM
    else
        use default placements

此外，您还可以设置多个判断条件。假设有如下需求：当 ``CONFIG_PERFORMANCE_LEVEL == 1`` 时，只有 ``object1.o`` 存放到 RAM 中；当 ``CONFIG_PERFORMANCE_LEVEL == 2`` 时，``object1.o`` 和 ``object2.o`` 会被存放到 RAM 中；当 ``CONFIG_PERFORMANCE_LEVEL == 3`` 时，归档中的所有目标文件都会被存放到 RAM 中；当这三个条件都不满足时，将整个组件库存放到 RTC 内存中。虽然这种使用场景很罕见，不过，还是可以通过以下方式实现：

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        : PERFORMANCE_LEVEL = 3
        * (noflash)
        : PERFORMANCE_LEVEL = 2
        object1 (noflash)
        object2 (noflash)
        : PERFORMANCE_LEVEL = 1
        object1 (noflash)
        : default
        * (rtc)

用伪代码可以表述为：

.. code-block:: none

    if CONFIG_PERFORMANCE_LEVEL == 3
        place entire libcomponent.a in RAM
    else if CONFIG_PERFORMANCE_LEVEL == 2
        only place object1.o and object2.o in RAM
    else if CONFIG_PERFORMANCE_LEVEL == 1
        only place object1.o in RAM
    else
        place entire libcomponent.a in RTC memory 

条件测试还支持 :ref:`其他操作 <ldgen-condition-entries>`。

.. _ldgen-default-placements:

默认的存放规则
^^^^^^^^^^^^^^

到目前为止，“默认存放规则”一直作为未指定 ``rtc`` 和 ``noflash`` 存放规则时的备选放置方式。``noflash`` 或者 ``rtc`` 标记不仅仅是链接脚本生成机制中的关键字，实际上还是由用户指定且被称为 :ref:`scheme 片段 <ldgen-scheme-fragment>` 的对象。由于这些存放规则非常常用，所以 IDF 中已经预定义了这些规则。

类似地，还有一个名为 ``default`` 的 scheme 片段，它定义了默认的存放规则，详情请见 :ref:`默认 scheme <ldgen-default-scheme>`。

.. note::
    有关使用此功能的 IDF 组件的示例，请参阅 :component_file:`freertos/CMakeLists.txt`。为了提高性能，``freertos`` 组件通过该机制将所有目标文件中的代码、字面量和只读数据存放到 IRAM 中。

快速入门指南到此结束，下面的文章将进一步详细讨论这个机制，例如它的组件、基本概念、语法、如何集成到构建系统中等等。以下部分有助于创建自定义的映射或者修改默认行为。

组件
----

.. _ldgen-fragment-files :

链接片段文件
^^^^^^^^^^^^

“链接片段文件”包含称为“片段”的对象，每个片段含有多条信息，放在一起时即可形成寻访规则，共同描述目标文件各个段在二进制输出文件中的存放位置。

换言之，处理“链接片段文件”也就是在 GNU LD 的 ``SECTIONS`` 命令中，创建段的存放规则，并将其放在一个内部 ``target`` token 中。

下面讨论三种类型的片段。

.. note::

    片段具有名称属性（mapping 片段除外）并且是全局可见的。片段的命名遵循 C 语言的基本变量命名规则，即区分大小写；必须以字母或者下划线开头；允许非初始字符使用字母、数字和下划线；不能使用空格等特殊字符。此外，每种片段都有自己的独立命名空间，如果多个片段的类型和名称相同，就会引发异常。

.. _ldgen-sections-fragment :

I. sections 片段
""""""""""""""""

sections 片段定义了 GCC 编译器输出的目标文件段的列表，可以是默认的段（比如 ``.text`` 段、``.data`` 段），也可以是用户通过 ``__attribute__`` 关键字自定义的段。 

此外，用户还可以在某类段后增加一个 ``+``，表示囊括列表中的“所有这类段”和“所有以这类段开头的段”。相较于显式地罗列所有的段，我们更推荐使用这种方式。

**语法**

.. code-block:: none

    [sections:name]
    entries:
        .section+
        .section
        ...

**示例**

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

II. scheme 片段
"""""""""""""""

scheme 片段定义了为每个 sections 指定的 ``target``。 

**语法**

.. code-block:: none

    [scheme:name]
    entries:
        sections -> target
        sections -> target
        ...

**示例**

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text          # 名为 text 的 sections 片段下的所有条目均归入 iram0_text
        rodata -> dram0_data        # 名为 rodata 的 sections 片段下的所有条目均归入 dram0_data

.. _ldgen-default-scheme:

**default scheme**

注意，有一个名为 ``default`` 的 scheme 很特殊，特殊在于 catch-all 存放规则都是从这个 scheme 中的条目生成的。这意味着，如果该 scheme 有一条 ``text -> flash_text`` 条目，则将为目标 ``flash_text`` 生成如下的存放规则 :

.. code-block:: none

    *(.literal .literal.* .text .text.*)

此后，这些生成的 catch-all 规则将用于未指定映射规则的情况。 

.. note::

    ``default`` scheme 是在 :component:`esp32/ld/esp32_fragments.lf` 文件中定义的，此外，快速上手指南中提到的内置 ``noflash`` scheme 片段和 ``rtc`` scheme 片段也是在这个文件中定义的。

.. _ldgen-mapping-fragment :

III. mapping 片段
"""""""""""""""""

mapping 片段定义了可映射实体（即目标文件、函数名、变量名）对应的 scheme 片段。具体来说，mapping 片段有两种类型的条目，分别为映射条目和条件条目。

.. note::

    mapping 片段没有具体的名称属性，内部会根据归档条目的值构造其名称。

**语法**

.. code-block:: none

    [mapping]
    archive: archive                # 构建后输出的存档文件的名称（即 libxxx.a）
    entries:
        : condition                 # 条件条目，非默认
        object:symbol (scheme)      # 映射条目，Type I
        object (scheme)             # 映射条目，Type II
        * (scheme)                  # 映射条目，Type III

        # 为了提高可读性，可以适当增加分隔行或注释，非必须

        : default                   # 条件条目，默认
        * (scheme)                  # 映射条目，Type III

.. _ldgen-mapping-entries :

**映射条目**

mapping 片段的映射条目共有三种类型，分别为：

    ``Type I``
        同时指定了目标文件名和符号名。其中，符号名可以是函数名或者变量名。

    ``Type II``
        仅指定了目标文件名。

    ``Type III``
        指定了 ``*``，也就是指定了归档文件中所有目标文件。

接下来，让我们通过展开一个 ``Type II`` 映射条目，更好地理解映射条目的含义。最初： 

.. code-block:: none

    object (scheme)

接着，让我们根据条目定义，将这个 scheme 片段展开：

.. code-block:: none

    object (sections -> target, 
            sections -> target, 
            ...)

然后再根据条目定义，将这个 sections 片段展开：

.. code-block:: none

    object (.section,
            .section,
            ... -> target, # 根据目标文件将这里所列出的所有段放在该目标位置
            
            .section,
            .section,
            ... -> target, # 同样的方法指定其他段 
            
            ...)           # 直至所有段均已展开

.. _ldgen-type1-limitations :

**有关 Type I 映射条目的局限性**

``Type I`` 映射条目可以工作的大前提是编译器必须支持 ``-ffunction-sections`` 和 ``-ffdata-sections`` 选项。因此，如果用户主动禁用了这两个选项，``Type I`` 映射条目就无法工作。此外，值得注意的是，``Type I`` 映射条目的实现还与输出段有关。因此，有时及时用户在编译时没有选择禁用这两个选项，也有可能无法使用 ``Type I`` 映射条目。

例如，当使用 ``-ffunction-sections`` 选项时，编译器会给每个函数都输出一个单独的段，根据段名的构造规则，这些段的名称应该类似 ``.text.{func_name}`` 或 ``.literal.{func_name}``。然而，对于函数中的字符串文字，情况并非如此，因为它们会使用池化后或者新创建的段名。

当使用 ``-fdata-sections`` 选项时，编译器会给每一个全局可见的数据输出一个单独的段，名字类似于 ``.data.{var_name}``、 ``.rodata.{var_name}`` 或者 ``.bss.{var_name}``。这种情况下，``Type I`` 映射条目可以使用。然而，对于在函数作用域中声明的静态数据，编译器在为其生成段名时会同时使用其变量名和其他信息，因此当涉及在函数作用域中定义的静态数据时就会出现问题。

.. _ldgen-condition-entries :

**条件条目**

条件条目允许根据具体项目配置生成不同的链接脚本。也就是说，可以根据一些配置表达式的值，选择使用一套不同的映射条目。由于检查配置的过程是通过 :idf_file:`tools/kconfig_new/kconfiglib.py` 文件中的 ``eval_string`` 完成的，因此条件表达式也必须遵循 ``eval_string`` 的语法和限制。

在一个 mapping 片段中，跟着一个条件条目后定义的所有映射条目均属于该条件条目，直至下一个条件条目的出现或者是该 mapping 片段的结束。在检查配置时，编译器将逐条检查这个 mapping 片段中的所有条件条目，直至找到一个满足条件的条件条目（即表达式为 ``TRUE``），然后使用该条件条目下定义的映射条目。另外，尽管每个映射都已包含一个隐式的空映射，但用户还是可以自定义一个默认条件，即所有条件条目均不满足时（即没有表达式为 ``TRUE``）使用的映射条目。

**示例**

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text
        rodata -> dram0_data

    [mapping:lwip]
    archive: liblwip.a
    entries:
        : LWIP_IRAM_OPTIMIZATION = y         # 如果 CONFIG_LWIP_IRAM_OPTIMIZATION 在 sdkconfig 中被定义为 'y' 
        ip4:ip4_route_src_hook (noflash)     # 将 ip4.o:ip4_route_src_hook，ip4.o:ip4_route_src 和
        ip4:ip4_route_src (noflash)          # ip4.o:ip4_route 映射到 noflash scheme
        ip4:ip4_route (noflash)              # 该 scheme 会将他们存放到 RAM 中
        
        : default                            # 否则不使用特殊的映射规则

.. _ldgen-script-templates :

链接脚本模板
^^^^^^^^^^^^

链接脚本模板与其他链接脚本没有本质区别，但带有特定的标记语法，可以指示放置生成的存放规则的位置，是指定存放规则的放置位置的框架。

**语法**

如需引用一个 ``target`` token 下的所有存放规则，请使用以下语法：

.. code-block:: none

    mapping[target]

**示例**

以下示例是某个链接脚本模板的摘录。该链接脚定义了一个输出段 ``.iram0.text``，里面包含一个引用目标 ``iram0_text`` 的标记。

.. code-block:: none

    .iram0.text :
    {
        /* 标记 IRAM 的边界 */
        _iram_text_start = ABSOLUTE(.);

        /* 引用 iram0_text */
        mapping[iram0_text]

        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

下面，让我们更具体一点。假设某个链接脚本生成器收集到了以下片段：

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
        /* 标记 IRAM 的边界 */
        _iram_text_start = ABSOLUTE(.);

        /* 将链接片段处理生成的存放规则放置在模板标记的位置处 */
        *(.iram1 .iram1.*)
        *libfreertos.a:(.literal .text .literal.* .text.*)

        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

``*libfreertos.a:(.literal .text .literal.* .text.*)``

    这是从 ``freertos`` mapping 片段的 ``* (noflash)`` 条目中生成的规则。``libfreertos.a`` 归档文件下的所有目标文件的 ``text`` 段会被收集到 ``iram0_text`` 目标下（假设采用 ``noflash`` scheme），并放在模板中被 ``iram0_text`` 标记的地方。

``*(.iram1 .iram1.*)``

    这是从 ``default`` scheme 的 ``iram -> iram0_text`` 条目生成的规则，因为 ``default`` scheme 指定了一个 ``iram -> iram0_text`` 条目，因此生成的规则也将放在被 ``iram0_text`` 标记的地方。值得注意的是，由于该规则是从 ``default`` scheme 中生成的，因此在同一目标下收集的所有规则下排在第一位。


与构建系统的集成
----------------

链接脚本是在应用程序的构建过程中生成的，此时尚未链接形成最终的二进制文件。实现该机制的工具位于 ``$(IDF_PATH)/tools/ldgen`` 目录下。

链接脚本模板
^^^^^^^^^^^^
目前使用的链接脚本模板是 :component:`esp32/ld/esp32.project.ld.in`，仅用于应用程序的构建，生成的链接脚本文件将放在同一组件的构建目录下。值得注意的是，修改此链接描述文件模板会触发应用程序的二进制文件的重新链接。

链接片段文件
^^^^^^^^^^^^
任何组件都可以将片段文件添加到构建系统中，方法有两种：设置 ``COMPONENT_ADD_LDFRAGMENTS`` 变量或者使用 ``ldgen_add_fragment_files`` 函数（仅限 CMake），具体可以参考 :ref:`添加片段文件 <ldgen-add-fragment-file>` 小节中的介绍。值得注意的是，修改构建系统中的任何片段文件都会触发应用程序的二进制文件的重新链接。
