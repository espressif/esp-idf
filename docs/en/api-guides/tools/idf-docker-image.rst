****************
IDF Docker Image
****************

..
    When changing this page, please keep tools/docker/README.md in sync.

.. highlight:: bash

IDF Docker image (``espressif/idf``) is intended for building applications and libraries with specific versions of ESP-IDF, when doing automated builds.

The image contains:

- Common utilities such as git, wget, curl, zip.
- Python 3.6 or newer.
- A copy of a specific version of ESP-IDF (see below for information about versions). ``IDF_PATH`` environment variable is set, and points to ESP-IDF location in the container.
- All the build tools required for the specific version of ESP-IDF: CMake, make, ninja, cross-compiler toolchains, etc.
- All Python packages required by ESP-IDF are installed in a virtual environment.

The image entrypoint sets up ``PATH`` environment variable to point to the correct version of tools, and activates the Python virtual environment. As a result, the environment is ready to use the ESP-IDF build system.

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

If using the image in CI environment, consult the documentation of your CI service on how to specify the image used for the build process.

Building a project with CMake
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the project directory, run::

    docker run --rm -v $PWD:/project -w /project espressif/idf idf.py build


The above command explained:

- ``docker run``: runs a Docker image. It is a shorter form of the command ``docker container run``.
- ``--rm``: removes the container when the build is finished
- ``-v $PWD:/project``: mounts the current directory on the host (``$PWD``) as ``/project`` directory in the container
- ``espressif/idf``: uses Docker image ``espressif/idf`` with tag ``latest`` (implicitly added by Docker when no tag is specified)
- ``idf.py build``: runs this command inside the container

To build with a specific docker image tag, specify it as ``espressif/idf:TAG``, for example::

    docker run --rm -v $PWD:/project -w /project espressif/idf:release-v4.4 idf.py build

You can check the up-to-date list of available tags at https://hub.docker.com/r/espressif/idf/tags.


Building a project with GNU Make
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Same as for CMake, except that the build command is different::

    docker run --rm -v $PWD:/project -w /project espressif/idf make defconfig all -j4


.. note::

    If the ``sdkconfig`` file does not exist, the default behavior of GNU Make build system is to open the menuconfig UI. This may be not desired in automated build environments. To ensure that the ``sdkconfig`` file exists, ``defconfig`` target is added before ``all``.

If you intend to build the same project repeatedly, you may bind the ``tools/kconfig`` directory of ESP-IDF to a named volume. This will prevent Kconfig tools, located in ESP-IDF directory, from being rebuilt, causing a rebuild of the rest of the project::

    docker run --rm -v $PWD:/project -v kconfig:/opt/esp/idf/tools/kconfig -w /project espressif/idf make defconfig all -j4

If you need clean up the ``kconfig`` volume, run ``docker volume rm kconfig``.

Binding the ``tools/kconfig`` directory to a volume is not necessary when using the CMake build system.

Using the image interactively
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to do builds interactively, to debug build issues or test the automated build scripts. Start the container with `-i -t` flags::

    docker run --rm -v $PWD:/project -w /project -it espressif/idf


Then inside the container, use ``idf.py`` as usual::

    idf.py menuconfig
    idf.py build

.. note::

    Commands which communicate with the development board, such as ``idf.py flash`` and ``idf.py monitor`` will not work in the container unless the serial port is passed through into the container. However currently this is not possible with Docker for Windows (https://github.com/docker/for-win/issues/1018) and Docker for Mac (https://github.com/docker/for-mac/issues/900).

Building custom images
======================

The Dockerfile in ESP-IDF repository provides several build arguments which can be used to customize the Docker image:

- ``IDF_CLONE_URL``: URL of the repository to clone ESP-IDF from. Can be set to a custom URL when working with a fork of ESP-IDF. Default is ``https://github.com/espressif/esp-idf.git``.
- ``IDF_CLONE_BRANCH_OR_TAG``: Name of a git branch or tag use when cloning ESP-IDF. This value is passed to ``git clone`` command using the ``--branch`` argument. Default is ``master``.
- ``IDF_CHECKOUT_REF``: If this argument is set to a non-empty value, ``git checkout $IDF_CHECKOUT_REF`` command will be performed after cloning. This argument can be set to the SHA of the specific commit to check out, for example if some specific commit on a release branch is desired.
- ``IDF_CLONE_SHALLOW``: If this argument is set to a non-empty value, ``--depth=1 --shallow-submodules`` arguments will be used when performing ``git clone``. This significantly reduces the amount of data downloaded and the size of the resulting Docker image. However, if switching to a different branch in such a "shallow" repository is necessary, an additional ``git fetch origin <branch>`` command must be executed first.
- ``IDF_INSTALL_TARGETS``: Comma-separated list of IDF targets to install toolchains for, or ``all`` to install toolchains for all targets. Selecting specific targets reduces the amount of data downloaded and the size of the resulting Docker image. Default is ``all``.

To use these arguments, pass them via the ``--build-arg`` command line option. For example, the following command will build a Docker image with a shallow clone of ESP-IDF v4.4.1 and tools for ESP32-C3, only::

    docker build -t idf-custom:v4.4.1-esp32c3 \
        --build-arg IDF_CLONE_BRANCH_OR_TAG=v4.4.1 \
        --build-arg IDF_CLONE_SHALLOW=1 \
        --build-arg IDF_INSTALL_TARGETS=esp32c3 \
        tools/docker
