构建系统
===================================

:link_to_translation:`en:[English]`

链接器孤立段处理行为变更为报错
-------------------------------

从 ESP-IDF v6.0 开始，构建系统不再允许最终 ELF 文件中存在孤立段。如果链接过程中发现任何孤立段，链接器将报错。

.. note::

    *孤立段* 是指未被链接脚本显式放置到任何输出段，且在链接过程中未被丢弃的段。

如何解决孤立段错误
~~~~~~~~~~~~~~~~~~~~

如果在链接时遇到孤立段错误，可通过以下任一方式解决：

1. 移除导致孤立段的代码或数据（若未使用或不必要）。
2. 使用 :ref:`链接器片段文件 <ldgen-linker-fragment-files>` 显式放置孤立段。
3. 通过设置 :ref:`CONFIG_COMPILER_ORPHAN_SECTIONS` 为 ``warning`` 或 ``place`` 来抑制错误。

.. warning::

    **不推荐使用** 方案 3，因为孤立段可能意味着内存映射配置存在问题，或应用程序中存在非预期行为。

全局构造函数顺序变更
---------------------

最初，ESP-IDF 使用内部函数 ``do_global_ctors()`` 执行全局构造函数，以兼容 Xtensa 架构，其编译器会生成按 **降序** 排列的 ``.ctors.*`` 段。

在 RISC-V 架构中，工具链会生成 ``.init_array.*`` 段，并采用标准的 **升序** 排列。虽然优先级构造函数（位于 ``.init_array.*``）能正确执行，但此前未指定优先级的 ``.init_array`` 段却是按 **降序** 方式处理，从而与 Xtensa 的 ``.ctors`` 行为保持一致。

从 ESP-IDF v6.0 起，启动代码改用标准工具链函数 ``__libc_init_array()``。该函数会按 **升序** 执行带优先级和不带优先级的构造函数。

因此，我们引入了以下重大变更：

- 对于 Xtensa 架构，``.ctors.*`` 项现在会被转换为升序，以确保与 ``__libc_init_array()`` 的行为兼容。
- 非优先级的 ``.init_array`` 和旧版 ``.ctors`` 段的执行顺序从 ``降序`` 改为 ``升序``。

这些变更使 ESP-IDF 的行为与工具链的预期保持一致，并提升了跨架构的兼容性。

若应用程序依赖原有构造函数的执行顺序（降序），并因此受到影响，可参考以下解决方案。

更新构造函数注册逻辑
~~~~~~~~~~~~~~~~~~~~

有些数据结构是基于“构建函数按相反顺序运行”的假设来构建的。现在为了不改变原有行为，可将注册新项的逻辑从“插入头部”改为“插入尾部”。

示例（来自 ``components/unity/unity_runner.c``）：

.. code-block:: diff

    -    // 插入到链表头部
    -    desc->next = s_unity_tests_first;
    -    s_unity_tests_first = desc;
    +    // 插入到链表尾部
    +    _unity_tests_last->next = desc;
    +    s_unity_tests_last = desc;

.. note::

    此方案仅适用于特定场景。

使用构造函数优先级
~~~~~~~~~~~~~~~~~~~~~~~

若需显式控制构造函数执行顺序，可使用带数值优先级的 ``constructor()`` 函数属性：

.. code-block:: c

    __attribute__((constructor(PRIO)))
    void foo(void);

将 ``PRIO`` 替换为整数值。值越小，执行越早。当对执行顺序有特定要求时，这种方式是首选。

配置文件语法变更
------------------

ESP-IDF v6 使用了 esp-idf-kconfig v3，对配置文件 ``Kconfig`` 的语法进行了若干变更。完整变更列表请参阅 `Migration Guide From esp-idf-kconfig v2.x to v3.x <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/developer-guide/migration-guide.html>`_。
