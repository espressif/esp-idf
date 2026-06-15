Updating an Existing Project
****************************

Converting an existing v1 project to v2 usually requires only a change to the top-level ``CMakeLists.txt``. The project layout, the components, and the application code stay the same. The structure of a v2 project is described in :doc:`creating-project`.

Switching the Project to v2
===========================

Change the project's ``CMakeLists.txt`` from the v1 form:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(my_project)

to the v2 form:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(my_project C CXX ASM)
    idf_project_default()

The change is the included file, ``tools/cmakev2/idf.cmake`` instead of ``tools/cmake/project.cmake``, the explicit list of project languages, and the call to :cmakev2:ref:`idf_project_default`. The meaning of each line is described in :doc:`creating-project`.

This is sufficient for most projects. If a component does not build under v2, see :doc:`breaking-changes` for the differences that may require a change, and :doc:`managing-compatibility` for keeping a component working under both v1 and v2.
