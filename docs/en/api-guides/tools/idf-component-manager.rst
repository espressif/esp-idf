*********************
IDF Component Manager
*********************

:link_to_translation:`zh_CN:[中文]`

The IDF Component Manager is a tool that downloads dependencies for any ESP-IDF CMake project. The download happens automatically during a run of CMake. It can source components either from the `ESP Component Registry <https://components.espressif.com>`__ or from a Git repository.

A list of components can be found on `<https://components.espressif.com/>`__.

For detailed information about the IDF Component Manager, see the `IDF Component Manager and ESP Component Registry Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/>`__.

Using with a Project
====================

Dependencies for each component in the project are defined in a separate manifest file named ``idf_component.yml`` placed in the root of the component. The manifest file template can be created by running ``idf.py create-manifest``. By default, a manifest file is created for the main component. You can explicitly either specify the directory where the manifest should be created using the ``--path`` option or specify the component in your ``components`` folder using ``--component=my_component``. The ``create-manifest`` command can be run in the following ways:

- ``idf.py create-manifest`` creates a manifest file for the main component
- ``idf.py create-manifest --component=my_component`` creates a manifest file for the component **my_component** in the ``components`` directory
- ``idf.py create-manifest --path="../../my_component"`` creates a manifest file for the component **my_component** in the ``my_component`` directory

When a new manifest is added to one of the components in the project, it is necessary to reconfigure the project manually by running ``idf.py reconfigure``. The build will then track changes in ``idf_component.yml`` manifests and automatically trigger CMake when necessary.

To add a dependency to a component (e.g., ``my_component``) in your ESP-IDF project, you can run the command ``idf.py add-dependency DEPENDENCY``. The ``DEPENDENCY`` argument represents an additional component managed by the IDF Component Manager that ``my_component`` depends on. It is defined in the format ``namespace/name=1.0.0``, where `namespace/name` is the name of the component and `=1.0.0` is a version range of the component, see the `Versioning Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`__. By default, dependencies are added to the main component. You can either explicitly specify a directory where the manifest is located using the ``--path`` option, or specify the component in your ``components`` folder using ``--component=my_component``. The ``add-dependency`` command can be run in the following ways:

- ``idf.py add-dependency example/cmp`` adds a dependency on the most recent version of ``example/cmp`` to the main component
- ``idf.py add-dependency --component=my_component example/cmp<=3.3.3`` adds a dependency on the version ``<=3.3.3`` of ``example/cmp`` to the component ``my_component`` in the ``components`` directory
- ``idf.py add-dependency --path="../../my_component" example/cmp^3.3.3`` adds a dependency on the version ``^3.3.3`` of ``example/cmp`` to the component ``my_component`` in the ``my_component`` directory

.. note::

    The command ``add-dependency`` adds dependencies to your project explicitly from the `ESP Component Registry <https://components.espressif.com/>`__.

To update dependencies of the ESP-IDF project, you can run the command ``idf.py update-dependencies``. You can also specify the path to the project directory using ``--project-dir PATH``.

:example:`build_system/cmake/component_manager` demonstrates how to use the IDF Component Manager to download dependencies from the ESP Component Registry.

It is not necessary to have a manifest for components that do not need any managed dependencies.

When CMake configures the project (e.g., ``idf.py reconfigure``) component manager does a few things:

- Processes ``idf_component.yml`` manifests for every component in the project and recursively solves dependencies.
- Creates a ``dependencies.lock`` file in the root of the project with a full list of dependencies.
- Downloads all dependencies to the ``managed_components`` directory.

The lock file ``dependencies.lock`` and the content of the ``managed_components`` directory are not supposed to be modified by a user. When the component manager runs, it always makes sure they are up to date. If these files were accidentally modified, it is possible to re-run the component manager by triggering CMake with ``idf.py reconfigure``.

You may set the build property ``DEPENDENCIES_LOCK`` to specify the lock-file path in the top-level CMakeLists.txt. For example, adding ``idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})`` before ``project(PROJECT_NAME)`` could help generate different lock files for different targets.

Creating a Project From an Example
==================================

Some components in the ESP Component Registry contain example projects. To create a new project from an example you can run the command ``idf.py create-project-from-example EXAMPLE``. The ``EXAMPLE`` argument should be in the format ``namespace/name=1.0.0:example`` where `namespace/name` is the name of the component, `=1.0.0` is a version range of the component (see the `Versioning Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html>`__) and `example` is the example's name. You can find the list of examples for every component and the command to start a project for it in the `ESP Component Registry <https://components.espressif.com/>`__.

Defining Dependencies in the Manifest
=====================================

You can easily define dependencies in the manifest file ``idf_component.yml`` by editing it directly in the text editor. Below are some basic examples that demonstrate how to define dependencies.

You can define a dependency from the ESP Component Registry by specifying the component name and the version range:

.. code-block:: yaml

    dependencies:
      # Define a dependency from the ESP Component Registry (https://components.espressif.com/component/example/cmp)
      example/cmp: ">=1.0.0"

To define a dependency from a Git repository, provide the path to the component within the repository and the repository's URL:

.. code-block:: yaml

    dependencies:
      # Define a dependency from a Git repository
      test_component:
        path: test_component
        git: ssh://git@gitlab.com/user/components.git

During the development of components, you can use components from a local directory by specifying either a relative or an absolute path:

.. code-block:: yaml

      dependencies:
        # Define local dependency with relative path
        some_local_component:
          path: ../../projects/component

For detailed information about the manifest file format, see `Manifest File Format Documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html>`__.

Disabling the Component Manager
===============================

The component manager can be explicitly disabled by setting the ``IDF_COMPONENT_MANAGER`` environment variable to ``0``.
