Updating an Existing Component
******************************

Most components written for v1 build under v2 without modification. This page describes what to do when one does not, and how to keep a component working under both build systems.

Workflow
========

#. Build the component under v2 by building a project that uses it with Build System v2; see :doc:`updating-project`.
#. If the component builds and links correctly, no change is needed. v2 is designed to stay as backward compatible with v1 as possible.
#. If the build fails, find the relevant difference in :doc:`breaking-changes`. Each entry explains how v2 differs from v1 and how to adapt the component.
#. If the component must continue to build under v1 as well, apply the change in a way that works for both build systems, using the techniques in :doc:`managing-compatibility`.

Most adaptations are small and local. The techniques for building under both v1 and v2 are only needed when a component must still build with v1; a component that targets v2 only can be changed directly to the v2 behavior, or rewritten as a :ref:`native CMake component <cmakev2-component-native>`.
