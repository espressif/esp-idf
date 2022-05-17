*********************
IDF Component Manager
*********************

The IDF Component manager is a tool that downloads dependencies for any ESP-IDF CMake project. The download happens automatically during a run of CMake. It can source components either from `the component registry <https://components.espressif.com>`_ or from a git repository.

A list of components can be found on `<https://components.espressif.com/>`_

Using with a project
====================

Dependencies for each component in the project are defined in a separate manifest file named ``idf_component.yml`` placed in the root of the component. The manifest file template can be created for a component by running ``idf.py create-manifest --component=my_component``. When a new manifest is added to one of the components in the project it's necessary to reconfigure it manually by running ``idf.py reconfigure``. Then build will track changes in ``idf_component.yml`` manifests and automatically triggers CMake when necessary.

There is an example application: example:`build_system/cmake/component_manager` that uses components installed by the component manager.

It's not necessary to have a manifest for components that don't need any managed dependencies.

When CMake configures the project (e.g. ``idf.py reconfigure``) component manager does a few things:

- Processes ``idf_component.yml`` manifests for every component in the project and recursively solves dependencies
- Creates a ``dependencies.lock`` file in the root of the project with a full list of dependencies
- Downloads all dependencies to the ``managed_components`` directory

The lock-file ``dependencies.lock`` and content of ``managed_components`` directory is not supposed to be modified by a user. When the component manager runs it always make sure they are up to date. If these files were accidentally modified it's possible to re-run the component manager by triggering CMake with ``idf.py reconfigure``

Defining dependencies in the manifest
=====================================

.. code-block:: yaml

    dependencies:
      # Required IDF version
      idf: ">=4.1"
      # Defining a dependency from the registry:
      # https://components.espressif.com/component/example/cmp
      example/cmp: ">=1.0.0"

      # # Other ways to define dependencies
      #
      # # For components maintained by Espressif only name can be used.
      # # Same as `espressif/cmp`
      # component: "~1.0.0"
      #
      # # Or in a longer form with extra parameters
      # component2:
      #   version: ">=2.0.0"
      #
      #   # For transient dependencies `public` flag can be set.
      #   # `public` flag doesn't affect the `main` component.
      #   # All dependencies of `main` are public by default.
      #   public: true
      #
      #   # For components hosted on non-default registry:
      #   service_url: "https://componentregistry.company.com"
      #
      # # For components in git repository:
      # test_component:
      #   path: test_component
      #   git: ssh://git@gitlab.com/user/components.git
      #
      # # For test projects during component development
      # # components can be used from a local directory
      # # with relative or absolute path
      # some_local_component:
      #   path: ../../projects/component

Disabling the Component Manager
===============================

The component manager can be explicitly disabled by setting ``IDF_COMPONENT_MANAGER`` environment variable to ``0``.
