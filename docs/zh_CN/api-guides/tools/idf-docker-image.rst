********************
IDF Docker 镜像
********************

:link_to_translation:`en:[English]`

..
    When changing this page, please keep tools/docker/README.md in sync.

.. highlight:: bash

IDF Docker 镜像 (``espressif/idf``) 为使用特定版本的 ESP-IDF 自动化构建应用程序和库而设计。

该镜像包含以下内容：

- 常见的实用工具，如 ``git``、``wget``、``curl`` 和 ``zip``。
- Python 3.8 或更高版本。
- 特定版本 ESP-IDF 的副本。有关版本信息，请参阅下文。该副本中设置了 ``IDF_PATH`` 环境变量，并指向容器中 ESP-IDF 的位置。
- 构建特定版本 ESP-IDF 所需工具：CMake、Ninja、交叉编译器工具链等。
- ESP-IDF 需要的所有 Python 软件包。这些软件包均已安装在虚拟环境中。

镜像 ``ENTRYPOINT`` 会设置 ``PATH`` 环境变量，指向正确版本的工具，并激活 Python 虚拟环境。此时，环境已经准备好，可以使用 ESP-IDF 构建系统。

如需使用其他工具，可用该镜像作为基础自定义镜像。

标签
====

该镜像维护了以下多个标签：

- ``latest``：跟踪 ESP-IDF 的 ``master`` 分支
- ``vX.Y``：对应 ESP-IDF 的版本 ``vX.Y``
- ``release-vX.Y``：跟踪 ESP-IDF 的 ``release/vX.Y`` 分支

.. note::

    在引入镜像功能前发布的 ESP-IDF 版本没有对应的 Docker 镜像版本。要查找最新可用标签列表，请参阅 https://hub.docker.com/r/espressif/idf/tags。

使用 Docker
===========

设置 Docker
~~~~~~~~~~~~~~~~~

在本地使用 ``espressif/idf`` Docker 镜像前，请确保已安装 Docker。如果本地未安装 Docker，请按 https://docs.docker.com/install/ 提供的说明完成安装。

如果在 CI 环境中使用该镜像，请参阅 CI 服务说明文档，了解如何指定用于构建的镜像。

使用 CMake 构建项目
~~~~~~~~~~~~~~~~~~~~~~~~~~

在项目目录下，运行以下命令：

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project -u $UID -e HOME=/tmp espressif/idf idf.py build

该命令具体内容如下：

- ``docker run``：运行 Docker 镜像。此为 ``docker container run`` 命令的缩写形式。
- ``--rm``：构建完成后删除相应容器。
- ``-v $PWD:/project``：将主机当前目录 (``$PWD``) 挂载为容器中的 ``/project`` 目录。
- ``-w /project``：使 ``/project`` 成为当前命令的工作目录。
- ``-u $UID``：以当前用户的 ID 运行命令，使文件以当前用户而非 root 用户的身份创建。
- ``-e HOME=/tmp``：为用户提供一个主目录，用于将 ``idf.py`` 创建的临时文件保存在 ``~/.cache`` 中。
- ``espressif/idf``：使用标签为 ``latest`` 的 Docker 镜像 ``espressif/idf``。未指定标签时，Docker 会隐式添加 ``latest`` 标签。
- ``idf.py build``：在容器内运行此命令。

.. note::

   如果挂载目录 ``/project`` 包含的 git 仓库的用户 (``UID``) 不同于运行 Docker 容器的用户，在 ``/project`` 中执行 git 命令可能会失败，并显示错误信息 ``fatal: detected dubious ownership in repository at '/project'``。如需解决此问题，可以在启动 Docker 容器时设置 IDF_GIT_SAFE_DIR 环境变量，将 ``/project`` 目录指定为安全目录。例如，可以将 ``-e IDF_GIT_SAFE_DIR='/project'`` 作为参数包含，还可以使用分隔符 ``:`` 指定多个目录，或使用 ``*`` 完全禁用此项 git 安全检查。

要以特定 Docker 镜像标签进行构建，请将其指定为 ``espressif/idf:TAG``，示例如下：

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project -u $UID -e HOME=/tmp espressif/idf:release-v4.4 idf.py build

要查看最新可用标签列表，请参阅 https://hub.docker.com/r/espressif/idf/tags。

交互使用镜像
~~~~~~~~~~~~~~~~~~~~~~

Docker 也支持以交互方式进行构建，以调试构建问题或测试自动构建脚本。请使用 ``-i -t`` 标志启动容器，示例如下：

.. code-block:: bash

    docker run --rm -v $PWD:/project -w /project -u $UID -e HOME=/tmp -it espressif/idf

接着在容器内部照常使用 ``idf.py``：

.. code-block:: bash

    idf.py menuconfig
    idf.py build

.. note::

    若未将串行接口传递到容器中，则 ``idf.py flash`` 和 ``idf.py monitor`` 等与开发板通信的命令在容器中无法正常工作。对于 Linux 系统，可以使用 `设备选项`_ 将串行接口传递到容器中。然而，目前 Windows 系统 (https://github.com/docker/for-win/issues/1018) 和 Mac 系统 (https://github.com/docker/for-mac/issues/900) 中 Docker 不支持此功能。可以使用 `远程串行接口`_ 克服此限制。有关如何执行此操作，请参阅以下 `使用远程串行接口`_ 章节。

.. note::

    对于 Linux 系统，如果使用 ``--device`` 或 ``--privileged`` 等选项将主机的串行接口设备添加到容器，并使用 ``-u $UID`` 以特定用户启动容器时，请确保此用户对设备具有读/写访问权限。使用 ``--group-add`` 选项可以将容器用户添加到分配给主机设备的组 ID 中。例如，如果主机设备被分配到 ``dialout`` 组，你可以使用 ``--group-add $(getent group dialout | cut -d':' -f3)`` 将容器用户添加到主机的 ``dialout`` 组。


.. _using remote serial port:

使用远程串行接口
~~~~~~~~~~~~~~~~~~~~~~~~

`RFC2217`_ (Telnet) 协议可用于远程连接到串行接口，详情请参阅 ESP 工具项目的 `远程串行接口`_ 文档。如果无法直接访问 Docker 容器内的串行接口，也可使用该协议进行访问。以下示例展示了如何从 Docker 容器内部使用烧写命令。

在主机上安装并启动 ``esp_rfc2217_server``：

* 在 Windows 系统中，该软件包以一个文件的形式提供，这个文件是由 ``pyinstaller`` 创建的可执行文件，可以从 `ESP 工具版本`_ 页面以 ZIP 压缩文件的形式与其他 ESP 工具一起下载：

.. code-block:: bash

    esp_rfc2217_server -v -p 4000 COM3

* 在 Linux 或 macOS 系统中，该软件包是 ``esptool`` 的组成部分，可以在 ESP-IDF 环境中找到，或使用以下 ``pip`` 命令安装：

.. code-block:: bash

    pip install esptool

随后执行以下命令启动服务器：

.. code-block:: bash

    esp_rfc2217_server.py -v -p 4000 /dev/ttyUSB0

此时，便可使用以下命令，从 Docker 容器内部烧写连接到主机的设备：

.. code-block:: bash

    docker run --rm -v <host_path>:/<container_path> -w /<container_path> espressif/idf idf.py --port 'rfc2217://host.docker.internal:4000?ign_set_control' flash

请确保将 ``<host_path>`` 正确设置为主机上的项目路径，并使用 ``-w`` 选项将 ``<container_path>`` 设置为容器内的工作目录。``host.docker.internal`` 为特殊的 Docker DNS 名称，用于访问主机。如有需要，可以将其替换为主机的 IP 地址。


构建自定义镜像
======================

ESP-IDF 库中的 Docker 文件提供了以下构建参数，可用于构建自定义 Docker 镜像：

- ``IDF_CLONE_URL``：克隆 ESP-IDF 存储库的 URL。在使用 ESP-IDF 分支时，可以将该参数设置为自定义 URL，默认值为 ``https://github.com/espressif/esp-idf.git``。
- ``IDF_CLONE_BRANCH_OR_TAG``：克隆 ESP-IDF 时使用的 git 分支或标签的名称。该参数将作为 ``git clone`` 命令的 ``--branch`` 参数传递，默认值为 ``master``。
- ``IDF_CHECKOUT_REF``：如果将此参数设置为非空值，在克隆之后会执行 ``git checkout $IDF_CHECKOUT_REF`` 命令。可以将此参数设置为特定 commit 的 SHA 值，以便切换到所需的版本分支或 commit。例如，在希望使用特定版本分支上的某个 commit 时，就可以将此参数设置为该 commit 的 SHA 值。
- ``IDF_CLONE_SHALLOW``：如果将此参数设置为非空值，则会在执行 ``git clone`` 时使用 ``--depth=1 --shallow-submodules`` 参数。浅克隆的深度可以使用 ``IDF_CLONE_SHALLOW_DEPTH`` 设置。浅克隆可以极大减少下载的数据量及生成的 Docker 镜像大小。然而，如果需要切换到此类“浅层”存储库中的其他分支，必须先执行额外的 ``git fetch origin <branch>`` 命令。
- ``IDF_CLONE_SHALLOW_DEPTH``：此参数指定进行浅克隆时要使用的深度值。如未设置，将使用 ``--depth=1``。此参数仅在使用 ``IDF_CLONE_SHALLOW`` 时有效。如果要为分支构建 Docker 镜像，并且该镜像必须包含该分支上的最新标签，则需使用此参数。要确定所需的深度，请在特定的分支运行 ``git describe`` 命令，并注意偏移值。将偏移值加 1 后即可将其用作 ``IDF_CLONE_SHALLOW_DEPTH`` 参数的值。此过程将确保生成的镜像包含分支上的最新标签，且 Docker 镜像内部的 ``git describe`` 命令也会按预期工作。
- ``IDF_INSTALL_TARGETS``：以逗号分隔的 ESP-IDF 目标列表，用于安装工具链，或者使用 ``all`` 安装所有目标的工具链。选择特定目标可以减少下载的数据量和生成的 Docker 镜像的大小。该参数默认值为 ``all``。

要使用以上参数，请通过 ``--build-arg`` 命令行选项传递。例如，以下命令使用 ESP-IDF v4.4.1 的浅克隆以及仅适用于 ESP32-C3 的工具链构建了 Docker 镜像：

.. code-block:: bash

    docker build -t idf-custom:v4.4.1-esp32c3 \
        --build-arg IDF_CLONE_BRANCH_OR_TAG=v4.4.1 \
        --build-arg IDF_CLONE_SHALLOW=1 \
        --build-arg IDF_INSTALL_TARGETS=esp32c3 \
        tools/docker

.. _远程串行接口: https://docs.espressif.com/projects/esptool/en/latest/esptool/remote-serial-ports.html
.. _RFC2217: http://www.ietf.org/rfc/rfc2217.txt
.. _ESP 工具版本: https://github.com/espressif/esptool/releases
.. _设备选项: https://docs.docker.com/engine/reference/run/#runtime-privilege-and-linux-capabilities
