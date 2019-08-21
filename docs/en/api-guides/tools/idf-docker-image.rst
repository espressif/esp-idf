****************
IDF Docker Image
****************

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

To build with a specific docker image tag, specify it as ``espressif/idf:TAG``::

    docker run --rm -v $PWD:/project -w /project espressif/idf:v4.0 idf.py build

.. note::

    At the time of writing, v4.0 release of ESP-IDF does not exist, yet, so the above command will not work. You can check the up-to-date list of available tags at https://hub.docker.com/r/espressif/idf/tags.


Building a project with GNU Make
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Same as for CMake, except that the build command is different::

    docker run --rm -v $PWD:/project -w /project espressif/idf make defconfig all -j4


.. note::

    If the ``sdkconfig`` file does not exist, the default behavior of GNU Make build system is to open the menuconfig UI. This may be not desired in automated build environments. To ensure that the ``sdkconfig`` file exists, ``defconfig`` target is added before ``all``.

Using the image interactively
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to do builds interactively, to debug build issues or test the automated build scripts. Start the container with `-i -t` flags::

    docker run --rm -v $PWD:/project -w /project -it espressif/idf


Then inside the container, use ``idf.py`` as usual::

    idf.py menuconfig
    idf.py build

.. note::

    Commands which communicate with the development board, such as ``idf.py flash`` and ``idf.py monitor`` will not work in the container unless the serial port is passed through into the container. However currently this is not possible with Docker for Windows (https://github.com/docker/for-win/issues/1018) and Docker for Mac (https://github.com/docker/for-mac/issues/900).
