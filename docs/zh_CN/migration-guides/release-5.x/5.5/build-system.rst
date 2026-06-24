构建系统
========

:link_to_translation:`en:[English]`

启用 MINIMAL_BUILD 属性的示例
------------------------------------

目前 ESP-IDF 中的大多数示例均已在其项目的 ``CMakeLists.txt`` 中通过 ``idf_build_set_property(MINIMAL_BUILD ON)`` 启用 ``MINIMAL_BUILD`` 属性。通过仅将 ``main`` 组件及其传递依赖项包含在构建中，缩短了示例的构建时间。

带来的影响是：如 ``Components config`` 菜单中所述，只有这些组件会出现在 menuconfig 中。启用 ``MINIMAL_BUILD`` 后，若要使其他组件在 menuconfig 中可见并参与编译，需将其添加为 ``main`` 组件的依赖项或传递依赖项。更多信息请参阅 :ref:`在构建中导入组件 <including-components-in-the-build>`。
