*********************
IDF Component Manager
*********************

The IDF Component manager is a tool that downloads dependencies for any ESP-IDF CMake project. The download happens automatically during a run of CMake. It can source components either from `the component registry <https://components.espressif.com>`_ or from a git repository.

A list of components can be found on `<https://components.espressif.com/>`_

For detailed information about the IDF Component Manager, see the `IDF Component Manager and ESP Component Registry Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/>`_.

Using With a Project
====================

Dependencies for each component in the project are defined in a separate manifest file named ``idf_component.yml`` placed in the root of the component. The manifest file template can be created by running ``idf.py create-manifest``. By default, manifest file is created for the main component. You can explicitly either specify the directory where the manifest should be created using ``--path`` option or specify the component in your `components` folder using ``--component=my_component``. The ``create-manifest`` command can be run in the following ways:

- ``idf.py create-manifest`` creates a manifest file for the main component
- ``idf.py create-manifest --component=my_component`` creates a manifest file for the component *my_component* in the ``components`` directory
- ``idf.py create-manifest --path="../../my_component"`` creates a manifest file for the component *my_component* in the directory ``my_component``

When a new manifest is added to one of the components in the project, it is necessary to reconfigure the project manually by running ``idf.py reconfigure``. Then build will then track changes in ``idf_component.yml`` manifests and automatically triggers CMake when necessary.

To add a dependency to a component (e.g. ``my_component``) in your ESP-IDF project, you can run the command ``idf.py add-dependency DEPENDENCY``. "The ``DEPENDENCY`` argument represents an additional component managed by the IDF Component Manager that ``my_component`` depends on. It is defined in a format ``namespace/name=1.0.0``, where `namespace/name` is the name of the component and `=1.0.0` is a version range of the component (see the `Versioning Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`_). By default, dependencies are added to the main component. You can either explicitly specify a directory where the manifest is located using ``--path`` option, or specify the component in your `components` folder using ``--component=my_component``. The ``add-dependency`` command can be run in the following ways:

- ``idf.py add-dependency example/cmp`` adds a dependency on the most recent version of ``example/cmp`` to the main component
- ``idf.py add-dependency --component=my_component example/cmp<=3.3.3`` adds a dependency on the version ``<=3.3.3`` of ``example/cmp`` to the component ``my_component`` in the ``components`` directory
- ``idf.py add-dependency --path="../../my_component" example/cmp^3.3.3`` adds a dependency on the version ``^3.3.3`` of ``example/cmp`` to the component ``my_component`` in the directory ``my_component``

.. note::

    The command ``add-dependency`` adds dependencies to your project explicitly from the `Espressif Component Registry <https://components.espressif.com/>`_.

To update dependencies of the ESP-IDF project, you can run the command ``idf.py update-dependencies``. You can also specify the path to the project directory using ``--project-dir PATH``. 

There is an example application: example:`build_system/cmake/component_manager` that uses components installed by the component manager.

It's not necessary to have a manifest for components that don't need any managed dependencies.

When CMake configures the project (e.g. ``idf.py reconfigure``) component manager does a few things:

- Processes ``idf_component.yml`` manifests for every component in the project and recursively solves dependencies
- Creates a ``dependencies.lock`` file in the root of the project with a full list of dependencies
- Downloads all dependencies to the ``managed_components`` directory

The lock-file ``dependencies.lock`` and content of ``managed_components`` directory is not supposed to be modified by a user. When the component manager runs it always make sure they are up to date. If these files were accidentally modified it's possible to re-run the component manager by triggering CMake with ``idf.py reconfigure``

You may set build property ``DEPENDENCIES_LOCK`` to specify the lock-file path in the top-level CMakeLists.txt. For example, adding ``idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})`` before ``project(PROJECT_NAME)`` could help generate different lock files for different targets.

Creating Project From an Example
================================

Some components on the registry contain example projects. To create a new project from an example you can run the command ``idf.py create-project-from-example EXAMPLE``. The ``EXAMPLE`` argument should be in the format ``namespace/name=1.0.0:example`` where `namespace/name` is the name of the component, `=1.0.0` is a version range of the component (see the `Versioning Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`_) and `example` is the example's name. You can find the list of examples for every component and the command to start a project for it in the `Espressif Component Registry <https://components.espressif.com/>`_.

Defining Dependencies in the Manifest
=====================================

You can easily define dependencies in the manifest file (idf_component.yml) by editing it directly in the text editor. Below are some basic examples that demonstrate how to define dependencies.

You can define a dependency from the registry by specifying the component name and the version range:

.. code-block:: yaml

    dependencies:
      # Define a dependency from the registry (https://components.espressif.com/component/example/cmp)
      example/cmp: ">=1.0.0"

To define a dependency from a git repository, provide the path to the component within the repository and the repository's URL:

.. code-block:: yaml

    dependencies:
      # Define a dependency from a git repository
      test_component:
        path: test_component
        git: ssh://git@gitlab.com/user/components.git

During the development of components, you can use components from a local directory by specifying either a relative or an absolute path:

.. code-block:: yaml

      dependencies:
        # Define local dependency with relative path
        some_local_component:
          path: ../../projects/component

For detailed information about the manifest file format, see the `Manifest File Format Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html>`_.

Disabling the Component Manager
===============================

The component manager can be explicitly disabled by setting ``IDF_COMPONENT_MANAGER`` environment variable to ``0``.
