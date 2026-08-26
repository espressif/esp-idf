Build System v2
***************

.. attention::

    Build System v2 is currently available as a **Technical Preview** intended for **testing and evaluation**. Features, functionality, and performance are **subject to change without notice**, and **production use is not recommended** at this stage.

Introduction
============

The ESP-IDF CMake-based build system v2, referred to throughout this documentation simply as v2, is the next generation of the ESP-IDF build system. It is the successor to the :doc:`CMake-based build system v1 </api-guides/build-system>`, referred to as v1, which has been the default ESP-IDF build system since ESP-IDF v4.0, when CMake superseded the earlier GNU Make based build system. Like v1, v2 builds and links an ESP-IDF project from reusable units called components, while resolving several structural limitations of the original CMake-based design.

The most significant changes introduced by v2 are:

- **Configuration-driven component dependencies.** Component dependencies can be expressed in terms of Kconfig configuration options, so the set of components built into a project can follow its configuration. See :doc:`component-dependencies`.
- **Single-pass component evaluation.** v2 removes the early component evaluation that v1 performed in CMake script mode. Each component is evaluated once, as ordinary CMake code, which makes the build easier to reason about. The model is described in :doc:`design`.
- **Native CMake components.** A component can be written as a plain CMake static library, without the ``idf_component_register`` wrapper, giving full access to native CMake functionality. See :ref:`cmakev2-component-native`.

v2 is designed to stay as backward compatible with v1 as possible. Most projects and components written for v1 build under v2 without modification. Where a design difference does require a change to a v1 component, it is documented in :doc:`breaking-changes`. To convert an existing project to v2, see :doc:`updating-project`.

Runnable applications that accompany the guides below are provided in the :example:`Build System v2 examples <build_system/cmakev2>`. Terminology used throughout this guide is collected in the :doc:`glossary`.

Guides
======

.. toctree::
   :maxdepth: 1

   creating-project
   updating-project
   creating-component
   updating-component
   managing-compatibility
   component-dependencies
   third-party-libraries
   idf-as-library
   multiple-binaries
   multiple-configurations
   build-event-callbacks

Reference
=========

.. toctree::
   :maxdepth: 1

   design
   breaking-changes
   api
   glossary
   Build System v1 </api-guides/build-system>
