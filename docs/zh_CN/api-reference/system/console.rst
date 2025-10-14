控制台终端
==========

:link_to_translation:`en:[English]`

ESP-IDF 提供了 ``console`` 组件，它包含了开发基于串口的交互式控制终端所需要的所有模块，主要支持以下功能：

-  行编辑，由 `linenoise <https://github.com/antirez/linenoise>`_ 库具体实现，它支持处理退格键和方向键，支持回看命令的历史记录，支持命令的自动补全和参数提示。
-  将命令行拆分为参数列表。
-  参数解析，由 `argtable3 <https://www.argtable.org/>`_ 库具体实现，该库提供解析 GNU 样式的命令行参数的 API。
-  用于注册和调度命令的函数。
-  帮助创建 REPL (Read-Evaluate-Print-Loop) 环境的函数。

.. note::

  这些功能模块可以一起使用，也可以独立使用，例如仅使用行编辑和命令注册的功能，然后使用 ``getopt`` 函数或者自定义的函数来实现参数解析，而不是直接使用 `argtable3 <https://www.argtable.org/>`_ 库。同样地，还可以使用更简单的命令输入方法（比如 ``fgets`` 函数）和其他用于命令分割和参数解析的方法。

.. note::

  当在支持硬件 USB 串行接口的芯片上使用控制台应用程序时，建议禁用次级串口控制台输出。次级输出仅用于显示，对于交互式应用程序没有任何作用。

行编辑
------

行编辑功能允许用户通过按键输入来编辑命令，使用退格键删除符号，使用左/右键在命令中移动光标，使用上/下键导航到之前输入的命令，使用制表键（“Tab”）来自动补全命令。

.. note::

  此功能依赖于终端应用程序对 ANSI 转义符的支持。因此，显示原始 UART 数据的串口监视器不能与行编辑库一同使用。如果运行 :example:`system/console` 示例程序的时候看到的输出结果是 ``[6n`` 或者类似的转义字符而不是命令行提示符 ``esp>`` 时，就表明当前的串口监视器不支持 ANSI 转义字符。已知可用的串口监视程序有 GNU screen、minicom 和 esp-idf-monitor（可以通过在项目目录下执行 ``idf.py monitor`` 来调用）。

前往这里可以查看 `linenoise <https://github.com/antirez/linenoise>`_ 库提供的所有函数的描述。

配置
^^^^

Linenoise 库不需要显式地初始化，但是在调用行编辑函数之前，可能需要对某些配置的默认值稍作修改。

- :cpp:func:`linenoiseClearScreen`

  使用转义字符清除终端屏幕，并将光标定位在左上角。

- :cpp:func:`linenoiseSetMultiLine`

  在单行和多行编辑模式之间进行切换。单行模式下，如果命令的长度超过终端的宽度，会在行内滚动命令文本以显示文本的结尾，在这种情况下，文本的开头部分会被隐藏。单行模式在每次按下按键时发送给屏幕刷新的数据比较少，与多行模式相比更不容易发生故障。另一方面，在单行模式下编辑命令和复制命令将变得更加困难。默认情况下开启的是单行模式。

- :cpp:func:`linenoiseAllowEmpty`

  设置 linenoise 库收到空行的解析行为，设置为 ``true`` 时返回长度为零的字符串 (``""``) ，设置为 ``false`` 时返回 ``NULL``。默认情况下，将返回长度为零的字符串。

- :cpp:func:`linenoiseSetMaxLineLen`

  设置 linenoise 库中每行的最大长度，默认长度为 4096 字节，可以通过更新该默认值来优化 RAM 内存的使用。

- :cpp:func:`linenoiseSetReadFunction`

  设置 linenoise 使用的读取函数。

- :cpp:func:`linenoiseSetReadCharacteristics`

  设置用于读取的文件描述符的特性（例如，阻塞模式或非阻塞模式）。该函数在 ``linenoise.c`` 中具有一个弱定义，可通过强定义进行覆盖。

主循环
^^^^^^

- :cpp:func:`linenoise`

  在大多数情况下，控制台应用程序都会具有相同的工作形式——在某个循环中不断读取输入的内容，然后解析再处理。:cpp:func:`linenoise` 是专门用来获取用户按键输入的函数，当回车键被按下后会便返回完整的一行内容。因此可以用它来完成前面循环中的“读取”任务。

- :cpp:func:`linenoiseFree`

  必须调用此函数才能释放从 :cpp:func:`linenoise` 函数获取的命令行缓冲区。


提示和补全
^^^^^^^^^^

- :cpp:func:`linenoiseSetCompletionCallback`

  当用户按下制表键时，linenoise 会调用 **补全回调函数** ，该回调函数会检查当前已经输入的内容，然后调用 :cpp:func:`linenoiseAddCompletion` 函数来提供所有可能的补全后的命令列表。启用补全功能，需要事先调用 :cpp:func:`linenoiseSetCompletionCallback` 函数来注册补全回调函数。

  ``console`` 组件提供了一个现成的函数来为注册的命令提供补全功能 :cpp:func:`esp_console_get_completion` （见下文）。

- :cpp:func:`linenoiseAddCompletion`

  补全回调函数会通过调用此函数来通知 linenoise 库当前键入命令所有可能的补全结果。

- :cpp:func:`linenoiseSetHintsCallback`

  每当用户的输入改变时，linenoise 就会调用此回调函数，检查到目前为止输入的命令行内容，然后提供带有提示信息的字符串（例如命令参数列表），然后会在同一行上用不同的颜色显示出该文本。

- :cpp:func:`linenoiseSetFreeHintsCallback`

  如果 **提示回调函数** 返回的提示字符串是动态分配的或者需要以其它方式回收，就需要使用 :cpp:func:`linenoiseSetFreeHintsCallback` 注册具体的清理函数。


历史记录
^^^^^^^^

- :cpp:func:`linenoiseHistorySetMaxLen`

  该函数设置要保留在内存中的最近输入的命令的数量。用户通过使用向上/向下箭头来导航历史记录。

- :cpp:func:`linenoiseHistoryAdd`

  Linenoise 不会自动向历史记录中添加命令，应用程序需要调用此函数来将命令字符串添加到历史记录中。

- :cpp:func:`linenoiseHistorySave`

  该函数将命令的历史记录从 RAM 中保存为文本文件，例如保存到 SD 卡或者 flash 的文件系统中。

- :cpp:func:`linenoiseHistoryLoad`

  与 ``linenoiseHistorySave`` 相对应，从文件中加载历史记录。

- :cpp:func:`linenoiseHistoryFree`

  释放用于存储命令历史记录的内存。当使用完 linenoise 库后需要调用此函数。


将命令行拆分成参数列表
----------------------

``console`` 组件提供 :cpp:func:`esp_console_split_argv` 函数来将命令行字符串拆分为参数列表。该函数会返回参数的数量（``argc``）和一个指针数组，该指针数组可以作为 ``argv`` 参数传递给任何接受 ``argc，argv`` 格式参数的函数。

根据以下规则来将命令行拆分成参数列表：

-  参数由空格分隔
-  如果参数本身需要使用空格，可以使用 ``\`` （反斜杠）对它们进行转义
-  其它能被识别的转义字符有 ``\\`` （显示反斜杠本身）和 ``\"`` （显示双引号）
-  可以使用双引号来引用参数，引号只可能出现在参数的开头和结尾。参数中的引号必须如上所述进行转义。参数周围的引号会被 :cpp:func:`esp_console_split_argv` 函数删除

示例：

-  ``abc def 1 20 .3`` > [ ``abc``, ``def``, ``1``, ``20``, ``.3`` ]
-  ``abc "123 456" def`` > [ ``abc``, ``123 456``, ``def`` ]
-  ```a\ b\\c\"`` > [ ``a b\c"`` ]


参数解析
--------

对于参数解析，``console`` 组件使用 `argtable3 <https://www.argtable.org/>`_ 库。有关 `argtable3 <https://www.argtable.org/>`_ 的介绍请查看 `教程 <https://www.argtable.org/tutorial/>`_ 或者 Github 仓库中的 `示例代码 <https://github.com/argtable/argtable3/tree/master/examples>`_ 。


命令的注册与调度
----------------

``console`` 组件包含了一些工具函数，用来注册命令，将用户输入的命令和已经注册的命令进行匹配，使用命令行输入的参数调用命令。

应用程序首先调用 :cpp:func:`esp_console_init` 来初始化命令注册模块，然后调用 :cpp:func:`esp_console_cmd_register` 函数注册命令处理程序。

对于每个命令，应用程序需要提供以下信息（需要以 ``esp_console_cmd_t`` 结构体的形式给出）：

-  命令名字（不含空格的字符串）
-  帮助文档，解释该命令的用途
-  可选的提示文本，列出命令的参数。如果应用程序使用 ``Argtable3`` 库来解析参数，则可以通过提供指向 argtable 参数定义结构体的指针来自动生成提示文本
- 命令处理函数（无上下文），或
- 命令处理函数（有上下文）。如要使用此函数，则必须在调用其他命令 **之前** 调用 :cpp:func:`esp_console_cmd_set_context` 初始化上下文。

.. note::

  使用接受上下文的命令处理函数或者不接受上下文的命令处理函数均可，但两者不能同时使用。如果使用接受上下文的命令处理程序函数，则必须调用 :cpp:func:`esp_console_cmd_set_context` 初始化上下文，否则该函数可能会访问未初始化的上下文。

命令注册模块还提供了其它函数：

- :cpp:func:`esp_console_run`

  该函数接受命令行字符串，使用 :cpp:func:`esp_console_split_argv` 函数将其拆分为 argc/argv 形式的参数列表，在已经注册的组件列表中查找命令，如果找到，则执行其对应的处理程序。

- :cpp:func:`esp_console_register_help_command`

  将 ``help`` 命令添加到已注册命令列表中，此命令将会以列表的方式打印所有注册的命令及其参数和帮助文本。

- :cpp:func:`esp_console_get_completion`

  与 linenoise 库中的 :cpp:func:`linenoiseSetCompletionCallback` 一同使用的回调函数，根据已经注册的命令列表为 linenoise 提供补全功能。

- :cpp:func:`esp_console_get_hint`

  与 linenoise 库中 :cpp:func:`linenoiseSetHintsCallback` 一同使用的回调函数，为 linenoise 提供已经注册的命令的参数提示功能。


初始化 REPL 环境
----------------

除了上述的各种函数，``console`` 组件还提供了一些 API 来帮助创建一个基本的 REPL 环境。

在一个典型的 console 应用中，你只需要调用 :cpp:func:`esp_console_new_repl_uart`，它会为你初始化好构建在 UART 基础上的 REPL 环境，其中包括安装 UART 驱动，基本的 console 配置，创建一个新的线程来执行 REPL 任务，注册一些基本的命令（比如 `help` 命令）。

之后你可以使用 :cpp:func:`esp_console_cmd_register` 来注册其它命令。REPL 环境在初始化后需要再调用 :cpp:func:`esp_console_start_repl` 函数才能开始运行。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    同样，如果 REPL 环境是构建在 USB_SERIAL_JTAG 设备基础上，你只需要先调用 :cpp:func:`esp_console_new_repl_usb_serial_jtag` 函数进行初始化，然后再照常调用其它函数。

应用示例
------------

- :example:`system/console/basic` 演示了如何使用控制台组件的 REPL（读-评估-打印循环）API 在 {IDF_TARGET_NAME} 上创建一个交互式 Shell，该 Shell 可以通过串行接口控制，支持 UART 和 USB 接口，并可以作为需要命令行接口的应用程序的基础。

- :example:`system/console/advanced` 演示了如何使用控制台组件在 {IDF_TARGET_NAME} 上创建一个交互式 Shell，该 Shell 可以通过串行接口控制，支持 UART 和 USB 接口，可作为需要命令行接口的应用程序的基础。

API 参考
--------

.. include-build-file:: inc/esp_console.inc
