乐鑫 IoT 开发框架风格指南
=========================

:link_to_translation:`en:[English]`

关于本指南
----------

本风格指南旨在鼓励遵循 ESP-IDF 中通用的编码规范。

风格指南提供了一系列规则，从而帮助创建可读性强、可维护性好且稳健的代码。编写与代码库风格一致的代码，可以促进阅读和理解；遵循相同的空格和换行规范，后期更改就不太会产生影响阅读的巨大差异；使用模块结构的通用模板并使用一致的编程语言特性，可以更好地理解代码行为。

本指南尽可能使规则保持简单，因此无法涵盖所有情况。某些时候，这些规则需要为代码的可读性、可维护性或稳健性让步。

对 ESP-IDF 中使用的第三方代码进行修改时，请遵循该特定项目的编写方式，提出有用的更改，以合并到上游项目中。

C 语言代码格式化
----------------

.. highlight:: c

.. _style-guide-naming:

命名
^^^^

* 任何仅在单个源文件中使用的变量或函数应声明为 ``static``。
* 公共名称（非静态变量和函数）应使用每个组件或每个单元的前缀进行命名，以避免产生命名冲突，例如 ``esp_vfs_register()`` 或 ``esp_console_run()``。可以选择 ``esp_`` 作为乐鑫特定名称的前缀，但应与同一组件中的其他名称保持一致。
* 为了便于识别，静态变量应以 ``s_`` 为前缀。例如，``static bool s_invert``。
* 除非名称会非常长，否则避免使用不必要的缩写（例如，将 ``data`` 缩写为 ``dat``）。

缩进
^^^^

每个缩进层级使用四个空格。不要使用制表符进行缩进。配置编辑器，确保在每次按下制表键时能打出四个空格。

垂直空格
^^^^^^^^

函数之间放置一行空行。不要在函数的开头或结尾处放置空行：

.. code-block:: c

    void function1()
    {
        do_one_thing();
        do_another_thing();
                                    // 错误，请勿在此处使用空行
    }
                                    // 请在此处使用空行
    void function2()
    {
                                    // 错误，请勿在此处使用空行
        int var = 0;
        while (var < SOME_CONSTANT) {
            do_stuff(&var);
        }
    }

每行最多包含 120 个字符，只要不会严重影响可读性即可。

水平空格
^^^^^^^^

- 记得在条件和循环关键字后加一个空格：

.. code-block:: c

    if (condition) {    // 正确
        // ...
    }

    switch (n) {        // 正确
        case 0:
            // ...
    }

    for(int i = 0; i < CONST; ++i) {    // 错误
        // ...
    }

- 在二元运算符前后加一个空格，一元运算符则不需要空格。乘法和除法运算符前后可省略空格：

.. code-block:: c

    const int y = y0 + (x - x0) * (y1 - y0) / (x1 - x0);    // 正确

    const int y = y0 + (x - x0)*(y1 - y0)/(x1 - x0);        // 正确

    int y_cur = -y;                                         // 正确
    ++y_cur;

    const int y = y0+(x-x0)*(y1-y0)/(x1-x0);                // 错误

``.`` 和 ``->`` 运算符前后不需要加空格。

- 有时在一行中添加水平空格可以提高代码的可读性。例如，可以添加空格以对齐函数参数：

.. code-block:: c

    esp_rom_gpio_connect_in_signal(PIN_CAM_D6,   I2S0I_DATA_IN14_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_D7,   I2S0I_DATA_IN15_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_HREF, I2S0I_H_ENABLE_IDX,  false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_PCLK, I2S0I_DATA_IN15_IDX, false);

但是请注意，如果新写一行，将较长的标识符作为第一个参数（例如，``PIN_CAM_VSYNC``），这条规则将不再适用，其他行也不得不重新进行对齐，提交记录中将出现无意义的更改。

因此，请谨慎使用水平对齐，尤其是在后期可能会向列表中添加新行的情况下。

切勿使用制表符进行水平对齐。

切勿在行末以空格结尾。

大括号
^^^^^^

- 函数定义应将大括号放在单独的一行：

.. code-block:: c

    // 这是正确的：
    void function(int arg)
    {

    }

    // 这是错误的：
    void function(int arg) {

    }

- 在函数内部，请将左大括号、条件及循环语句放在同一行：

.. code-block:: c

    if (condition) {
        do_one();
    } else if (other_condition) {
        do_two();
    }


注释
^^^^

使用 ``//`` 进行单行注释；多行注释既可以在每行使用 ``//``，也可以使用 ``/* */`` 块。

尽管与格式无直接关系，但以下是一些关于如何有效使用注释的注意事项。

- 不要使用单行注释来禁用某些功能：

.. code-block:: c

    void init_something()
    {
        setup_dma();
        // load_resources();                // 读者会疑惑为什么此处被置于注释中。
        start_timer();
    }

- 如果不再需要某些代码，请将其完全删除。这些代码之后可以随时在相关文件的 git 历史记录中查到。如果出于某些临时原因禁用了部分函数调用，并且打算之后将其恢复，请在相邻行添加解释：

.. code-block:: c

    void init_something()
    {
        setup_dma();
        // TODO: 此处应加载资源，但加载器尚未完全集成。
        // load_resources();
        start_timer();
    }

- 上述规则同样适用于 ``#if 0 ... #endif`` 块。如果不再需要代码块，请将其完全删除。否则，请添加注释以说明禁用该代码块的原因。请不要使用 ``#if 0 ... #endif`` 或注释来存储将来可能需要的代码片段。

- 不要添加有关作者和更改日期的琐碎注释，可以直接使用 git 来查找修改人及修改内容等相关信息。例如，类似下文的注释不仅没添加任何有用的信息，还会使代码变得杂乱：

.. code-block:: c

    void init_something()
    {
        setup_dma();
        // XXX 于 2016-09-01 添加
        init_dma_list();
        fill_dma_item(0);
        // XXX 结束添加
        start_timer();
    }


行结束符
^^^^^^^^

提交内容只能包含以 LF（Unix 风格）为行结束符的文件。

Windows 用户可以通过设置 ``core.autocrlf``，将 git 配置为在本地检出以 CRLF（Windows 风格）为行结束符的文件，但提交时转换为以 LF 为行结束符。Github 中有一个 `文档 <https://docs.github.com/cn/get-started/getting-started-with-git/configuring-git-to-handle-line-endings?platform=windows>`_ 介绍了如何配置此选项。

如果分支提交的信息在无意中带有 LF 行结束符，可以在 MSYS2 或 Unix 终端中运行以下命令，将其转换为 Unix 风格（请先切换到 IDF 工作目录并检查当前所在分支是否正确）：

.. code-block:: bash

    git rebase --exec 'git diff-tree --no-commit-id --name-only -r HEAD | xargs dos2unix && git commit -a --amend --no-edit --allow-empty' master

请注意，上述命令基于 master 进行 rebase，若要基于其他分支进行 rebase，请更改命令结尾的分支名称。

要更新单次提交中的文件行结束符，可以先运行 ``dos2unix FILENAME``，再运行 ``git commit --amend``。

代码格式化
^^^^^^^^^^

ESP-IDF 使用 Astyle 来格式化源代码。配置存储在 :project_file:`tools/ci/astyle-rules.yml` 文件中。

所有组件都默认排除在格式化检查之外。可以将组件从 ``components_not_formatted_temporary`` 列表中删除，从而启用对该组件的格式化检查，然后运行：

.. code-block:: bash

    pre-commit run --files <path_to_files> astyle_py

或者，也可以手动运行 ``astyle_py``。通过 ``pip install astyle_py==VERSION`` 安装该工具，确保安装的版本与 :project_file:`.pre-commit-config.yaml` 文件中指定的版本相同，然后运行：

.. code-block:: bash

    astyle_py --rules=$IDF_PATH/tools/ci/astyle-rules.yml <path-to-file>


类型定义
^^^^^^^^

应使用 ``蛇形命名法`` (snake_case)，并以 ``_t`` 后缀结尾：

.. code-block:: c

    typedef int signed_32_bit_t;

枚举
^^^^

枚举应通过 `typedef` 定义，并使用命名空间：

.. code-block:: c

    typedef enum
    {
        MODULE_FOO_ONE,
        MODULE_FOO_TWO,
        MODULE_FOO_THREE
    } module_foo_t;


.. _assertions:

断言
^^^^

使用 ``assert.h`` 中定义的标准 C 函数 ``assert()`` 来检查源代码中应该为真的条件。在默认配置中，若断言条件返回 ``false`` 或 ``0``，则将调用 ``abort()`` 并触发 :doc:`严重错误 </api-guides/fatal-errors>`。

``assert()`` 只用于检测那些无法修复的错误，这些错误因严重的内部逻辑漏洞或数据损坏而产生，导致程序无法继续运行。对于可修复的错误（如因无效外部输入而产生的错误），:doc:`应返回一个错误值 </api-guides/error-handling>`。

.. note::

    当断言一个类型为 ``esp_err_t`` 的值等于 ``ESP_OK`` 时，应使用 :ref:`esp-error-check-macro` 而不是 ``assert()``。

可以将 ESP-IDF 项目配置为禁用断言（详见 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL`）。因此，在 ``assert()`` 语句中调用的函数不应有副作用。

还需要使用特定技术来避免禁用断言时出现“变量定义但未使用”的警告，这种警告通常由以下代码模式引起：

.. code-block:: c

    int res = do_something();
    assert(res == 0);

一旦 ``assert`` 被优化掉，将不再使用 ``res`` 值，编译器会对此发出警告。但即使禁用了断言，仍必须调用 ``do_something()`` 函数。

当变量在单个语句中声明并初始化时，最好在新的一行上将其转换为 ``void``。编译器将不再发出警告，且变量仍可在最终的二进制文件中被优化掉：

.. code-block:: c

    int res = do_something();
    assert(res == 0);
    (void)res;

如果变量是单独声明的，例如该变量被用于多个断言，则可以使用 GCC 属性 ``__attribute__((unused))`` 来声明它。编译器将不再发出任何有关未使用变量的警告，且变量仍可被优化掉：

.. code-block:: c

    int res __attribute__((unused));

    res = do_something();
    assert(res == 0);

    res = do_something_else();
    assert(res != 0);


头文件保护
----------

所有面向公众的头文件应具有预处理器保护。推荐使用 pragma：

.. code-block:: c

    #pragma once

最好不要用以下模式：

.. code-block:: c

    #ifndef FILE_NAME_H
    #define FILE_NAME_H
    ...
    #endif // FILE_NAME_H

除了保护宏，所有 C 语言头文件应具有 ``extern "C"`` 保护，使头文件能够在 C++ 代码中使用。头文件结构应遵循以下顺序：先是 ``pragma once``，再是 ``#include`` 语句，然后是 ``extern "C"`` 保护：

.. code-block:: c

    #pragma once

    #include <stdint.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    /* declarations go here */

    #ifdef __cplusplus
    }
    #endif


include 语句
------------

编写 ``#include`` 语句时，请尝试保持以下顺序：

* C 标准库头文件。
* 其他 POSIX 标准头文件及其常见扩展（如 ``sys/queue.h``）。
* 常见的 IDF 头文件（``esp_log.h``、``esp_system.h``、``esp_timer.h`` 以及 ``esp_sleep.h`` 等）。
* 其他组件的头文件，如 FreeRTOS。
* 当前组件的公共头文件。
* 私有头文件。

对于 C 标准库头文件和其他 POSIX 头文件，请使用尖括号 (``#include <stdio.h>``)。

对于所有其他头文件，请使用双引号 (``#include "esp_log.h"``)。


C++ 代码格式化
--------------

前文提到的 C 语言代码格式化规则同样适用于 C++ 代码格式化。如果这些规则不够，请遵循以下补充规则。

文件命名
^^^^^^^^

C++ 头文件的扩展名为 ``.hpp``。C++ 源文件的扩展名为 ``.cpp``。后者可帮助编译器将 C++ 源文件与 C 源文件区分开来。

命名
^^^^

* **类和结构体** 名称应使用首字母大写的 ``驼峰命名法`` (CamelCase)。成员变量和方法应使用 ``蛇形命名法`` (snake_case)。若 ``驼峰命名法`` 严重降低了可读性（例如命名 ``GPIOOutput`` 时），可允许使用下划线 ``_`` （例如，``GPIO_Output``），从而增强可读性。
* **命名空间** 应使用小写的 ``蛇形命名法``。
* **模板** 应在函数声明的上一行进行指定。
* 面向对象编程 (OOP) 中的接口命名不应使用后缀 ``...Interface``。这种命名方式使得将来从普通类中提取接口或将接口转换为类时，更加容易实现而不会造成破坏性变更。

类成员顺序
^^^^^^^^^^

按照优先顺序：

* 先放置公共成员，然后是受保护成员，最后是私有成员。没有成员的公共、受保护或私有部分应省略。
* 先放置构造函数/析构函数，然后是成员函数，最后是成员变量。

例如：

.. code-block:: cpp

    class ForExample {
    public:
        // 首先是构造函数，接着是默认构造函数，然后是析构函数
        ForExample(double example_factor_arg);
        ForExample();
        ~ForExample();

        // 剩余的公共方法
        set_example_factor(double example_factor_arg);

        // 公共成员变量
        uint32_t public_data_member;

    private:
        // 首先是私有方法
        void internal_method();

        // 然后是私有成员变量
        double example_factor;
    };

空格
^^^^

* 在命名空间内部不要缩进。
* 将 ``public``、``protected`` 和 ``private`` 标签的缩进级别与相应 ``class`` 标签的缩进级别保持一致。

简单示例
^^^^^^^^

.. code-block:: cpp

    // spaceship.h 文件
    #ifndef SPACESHIP_H_
    #define SPACESHIP_H_
    #include <cstdlib>

    namespace spaceships {

    class SpaceShip {
    public:
        SpaceShip(size_t crew);
        size_t get_crew_size() const;

    private:
        const size_t crew;
    };

    class SpaceShuttle : public SpaceShip {
    public:
        SpaceShuttle();
    };

    class Sojuz : public SpaceShip {
    public:
        Sojuz();
    };

    template <typename T>
    class CargoShip {
    public:
        CargoShip(const T &cargo);

    private:
        T cargo;
    };

    } // 命名空间 spaceships

    #endif // SPACESHIP_H_

    // spaceship.cpp 文件
    #include "spaceship.h"

    namespace spaceships {

    // 如果仅在初始化列表中初始化值
    // 则可以将大括号放置在同一行
    SpaceShip::SpaceShip(size_t crew) : crew(crew) { }

    size_t SpaceShip::get_crew_size() const
    {
        return crew;
    }

    SpaceShuttle::SpaceShuttle() : SpaceShip(7)
    {
        // 进一步初始化
    }

    Sojuz::Sojuz() : SpaceShip(3)
    {
        // 进一步初始化
    }

    template <typename T>
    CargoShip<T>::CargoShip(const T &cargo) : cargo(cargo) { }

    } // 命名空间 spaceships


CMake 代码风格
--------------

- 使用四个空格缩进。
- 每行最多包含 120 个字符。当分割行时，尽量注重可读性（例如，在单独的行上进行关键字匹配或参数匹配）。
- 在 ``endforeach()``、``endif()`` 等可选括号中不要添加任何内容。
- 使用小写 (``with_underscores``) 来命名指令、函数和宏。
- 对于局部变量，使用小写 (``with_underscores``)。
- 对于全局变量，使用大写 (``WITH_UNDERSCORES``)。
- 其他方面遵循 cmake-lint_ 项目的默认设置。

使用 EditorConfig 配置项目代码风格
----------------------------------

EditorConfig 可以帮助开发者在不同的编辑器和集成开发环境 (IDE) 之间定义和维护一致的代码风格。EditorConfig 项目包含定义代码风格的文件格式，以及一些文本编辑器插件，可以帮助编辑器读取文件格式并遵循定义风格。EditorConfig 文件易于阅读，并且与版本控制系统配合良好。

详情请浏览 `EditorConfig <https://editorconfig.org>`_ 网站。

第三方组件代码风格
------------------

ESP-IDF 集成了许多第三方组件，这些组件可能具有不同的代码风格。

FreeRTOS
^^^^^^^^

有关FreeRTOS 采用的代码风格，请参阅 `FreeRTOS 风格指南 <https://www.freertos.org/zh-cn-cmn-s/FreeRTOS-Coding-Standard-and-Style-Guide.html>`_。FreeRTOS 源代码的格式化是通过 `Uncrustify <https://github.com/uncrustify/uncrustify>`_ 自动完成的，因此 ESP-IDF FreeRTOS 组件中存有 FreeRTOS 代码风格的 Uncrustify 配置 (``uncrustify.cfg``) 副本 。

如果修改了 FreeRTOS 源文件，可以通过以下步骤重新格式化更新后的文件：

1. 确保系统安装了 Uncrustify (v0.69.0)。
2. 在更新后的 FreeRTOS 源文件上运行以下命令（其中 ``source.c`` 是需要格式化的源文件的路径）。

.. code-block:: bash

    uncrustify -c $IDF_PATH/components/freertos/FreeRTOS-Kernel/uncrustify.cfg --replace source.c --no-backup

代码文档
--------

请参阅此处的指南：:doc:`documenting-code`。

结构体
------

待编写。


语言特性
--------

待编写。

.. _cmake-lint: https://github.com/richq/cmake-lint
