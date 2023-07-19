********************
ESP-IDF Docker Image
********************

:link_to_translation:`zh_CN:[中文]`

..
    When changing this page, please keep tools/docker/README.md in sync.

.. highlight:: bash

ESP-IDF Docker image (``espressif/idf``) is intended for building applications and libraries with specific versions of ESP-IDF when doing automated builds.

The image contains:

- Common utilities such as ``git``, ``wget``, ``curl``, and ``zip``.
- Python 3.8 or newer.
- A copy of a specific version of ESP-IDF. See below for information about versions. ``IDF_PATH`` environment variable is set and points to the ESP-IDF location in the container.
- All the build tools required for the specific version of ESP-IDF: CMake, Ninja, cross-compiler toolchains, etc.
- All Python packages required by ESP-IDF are installed in a virtual environment.

The image ``ENTRYPOINT`` sets up the ``PATH`` environment variable to point to the correct version of tools, and activates the Python virtual environment. As a result, the environment is ready to use the ESP-IDF build system.

The image can also be used as a base for custom images, if additional utilities are required.

Tags
====

Multiple tags of this image are maintained:

- ``latest``: tracks ``master`` branch of ESP-IDF
- ``vX.Y``: corresponds to ESP-IDF release ``vX.Y``
- ``release-vX.Y``: tracks ``release/vX.Y`` branch of ESP-IDF

.. note::

    Versions of ESP-IDF released before this feature was introduced do not have corresponding Docker image versions. You can check the up-to-date list of available tags at https://hub.docker.com/r/espressif/idf/tags.

Usage
=====

Setting up Docker
~~~~~~~~~~~~~~~~~

Before using the ``espressif/idf`` Docker image locally, make sure you have Docker installed. Follow the instructions at https://docs.docker.com/install/, if it is not installed yet.

If using the image in a CI environment, consult the documentation of your CI service on how to specify the image used for the build process.

Building a Project with CMake
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the project directory, run:

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project espressif/idf idf.py build

The above command explained:

- ``docker run``: runs a Docker image. It is a shorter form of the command ``docker container run``.
- ``--rm``: removes the container when the build is finished.
- ``-v $PWD:/project``: mounts the current directory on the host (``$PWD``) as ``/project`` directory in the container.
- ``espressif/idf``: uses Docker image ``espressif/idf`` with tag ``latest``. The ``latest`` tag is implicitly added by Docker when no tag is specified.
- ``idf.py build``: runs this command inside the container.

To build with a specific Docker image tag, specify it as ``espressif/idf:TAG``, for example:

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project espressif/idf:release-v4.4 idf.py build

You can check the up-to-date list of available tags at https://hub.docker.com/r/espressif/idf/tags.

Using the Image Interactively
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to do builds interactively, to debug build issues or test the automated build scripts. Start the container with ``-i -t`` flags:

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project -it espressif/idf

Then inside the container, use ``idf.py`` as usual:

.. code-block:: bash

    idf.py menuconfig
    idf.py build

.. note::

    Commands which communicate with the development board, such as ``idf.py flash`` and ``idf.py monitor`` does not work in the container, unless the serial port is passed through into the container. This can be done with Docker for Linux with the `device option`_. However, currently, this is not possible with Docker for Windows (https://github.com/docker/for-win/issues/1018) and Docker for Mac (https://github.com/docker/for-mac/issues/900). This limitation may be overcome by using `remote serial ports`_. An example of how to do this can be found in the following `using remote serial port`_ section.


.. _using remote serial port:

Using Remote Serial Port
~~~~~~~~~~~~~~~~~~~~~~~~

The `RFC2217`_ (Telnet) protocol can be used to remotely connect to a serial port. For more information please see the `remote serial ports`_ documentation in the ESP tool project. This method can also be used to access the serial port inside a Docker container if it cannot be accessed directly. Following is an example of how to use the Flash command from within a Docker container.

On host install and start ``esp_rfc2217_server``:

* On Windows, the package is available as a one-file bundled executable created by ``pyinstaller`` and it can be downloaded from the `esptool releases`_ page in a ZIP archive along with other ESP tool utilities:

.. code-block:: bash

    esp_rfc2217_server -v -p 4000 COM3

* On Linux or macOS, the package is available as part of ``esptool``, which can be found in the ESP-IDF environment or by installing using ``pip``:

.. code-block:: bash

    pip install esptool

And then starting the server by executing

.. code-block:: bash

    esp_rfc2217_server.py -v -p 4000 /dev/ttyUSB0

Now the device attached to the host can be flashed from inside a Docker container by using:

.. code-block:: bash

    docker run --rm -v <host_path>:/<container_path> -w /<container_path> espressif/idf idf.py --port rfc2217://host.docker.internal:4000?ign_set_control flash

Please make sure that ``<host_path>`` is properly set to your project path on the host, and ``<container_path>`` is set as a working directory inside the container with the ``-w`` option. The ``host.docker.internal`` is a special Docker DNS name to access the host. This can be replaced with a host IP if necessary.


Building Custom Images
======================

The Docker file in ESP-IDF repository provides several build arguments which can be used to customize the Docker image:

- ``IDF_CLONE_URL``: URL of the repository to clone ESP-IDF from. Can be set to a custom URL when working with a fork of ESP-IDF. The default is ``https://github.com/espressif/esp-idf.git``.
- ``IDF_CLONE_BRANCH_OR_TAG``: Name of a git branch or tag used when cloning ESP-IDF. This value is passed to the ``git clone`` command using the ``--branch`` argument. The default is ``master``.
- ``IDF_CHECKOUT_REF``: If this argument is set to a non-empty value, ``git checkout $IDF_CHECKOUT_REF`` command performs after cloning. This argument can be set to the SHA of the specific commit to check out, for example, if some specific commit on a release branch is desired.
- ``IDF_CLONE_SHALLOW``: If this argument is set to a non-empty value, ``--depth=1 --shallow-submodules`` arguments are be used when performing ``git clone``. This significantly reduces the amount of data downloaded and the size of the resulting Docker image. However, if switching to a different branch in such a "shallow" repository is necessary, an additional ``git fetch origin <branch>`` command must be executed first.
- ``IDF_INSTALL_TARGETS``: Comma-separated list of ESP-IDF targets to install toolchains for, or ``all`` to install toolchains for all targets. Selecting specific targets reduces the amount of data downloaded and the size of the resulting Docker image. The default is ``all``.

To use these arguments, pass them via the ``--build-arg`` command line option. For example, the following command builds a Docker image with a shallow clone of ESP-IDF v4.4.1 and tools for ESP32-C3 only：

.. code-block:: bash

    docker build -t idf-custom:v4.4.1-esp32c3 \
        --build-arg IDF_CLONE_BRANCH_OR_TAG=v4.4.1 \
        --build-arg IDF_CLONE_SHALLOW=1 \
        --build-arg IDF_INSTALL_TARGETS=esp32c3 \
        tools/docker

.. _remote serial ports: https://docs.espressif.com/projects/esptool/en/latest/esptool/remote-serial-ports.html
.. _RFC2217: http://www.ietf.org/rfc/rfc2217.txt
.. _esptool releases: https://github.com/espressif/esptool/releases
.. _device option: https://docs.docker.com/engine/reference/run/#runtime-privilege-and-linux-capabilities
