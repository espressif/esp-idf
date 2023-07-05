- Installed ESP-IDF including all ESP-IDF requirements
- System package requirements (``libbsd``, ``libbsd-dev``)
- A recent enough Linux or macOS version and GCC compiler
- All components the application depends on must be either supported on the Linux target (Linux/POSIX simulator) or mock-able

An application that runs on the Linux target has to set the ``COMPONENTS`` variable to ``main`` in the CMakeLists.txt of the application's root directory:

.. code-block:: cmake

  set(COMPONENTS main)


This prevents the automatic inclusion of all components from ESP-IDF to the build process which is otherwise done for convenience.
