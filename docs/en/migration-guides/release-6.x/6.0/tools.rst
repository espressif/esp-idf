Tools
=====

:link_to_translation:`zh_CN:[中文]`

gdbgui Installation
-------------------

The support for the ``--enable-gdbgui`` argument has been removed from the install scripts, and `gdbgui <https://www.gdbgui.com>`_ cannot be installed this way anymore. Please use the ``pipx`` method from the `gdbgui installation guide <https://www.gdbgui.com/installation/>`_ in order to set up this tool.

Depending on your operating system and Python version, you may need to consult the `list of known issues <https://github.com/cs01/gdbgui/issues>`_.

For example, `Python 3.13 is not supported <https://github.com/cs01/gdbgui/issues/494>`_ on any operating systems at the moment.

The Windows operating system is not supported since gdbgui version 0.14. Because of the existence of other issues, you will need Python 3.10 with some specific versions of the dependencies. The last known working gdbgui and dependency versions can be installed with the following command:

.. code-block:: bash

    pipx install "gdbgui==0.13.2.0" "pygdbmi<=0.9.0.2" "python-socketio<5" "jinja2<3.1" "itsdangerous<2.1"

On Linux or macOS, you can use Python 3.11 or 3.12 and gdbgui version 0.15.2.0.

Please be aware that these recommendations may change over time. For an up-to-date list of issues, please refer to `the official issue tracker <https://github.com/cs01/gdbgui/issues>`_.

We recommend to use ``idf.py gdb`` instead of ``idf.py gdbgui``, or debug in Eclipse/Vscode if you encounter an issue with the installation.

``idf.py efuse*`` Commands Require Port
---------------------------------------

All commands with eFuse functionality now require a serial port to be specified. This was done to prevent accidental use of the wrong port, as these operations are irreversible.

For all ``idf.py efuse*`` commands, you now need to specify the serial port with the ``--port`` argument (or ``ESPPORT`` environment variable). If the port is not specified, the command will fail with an error message.

Python 3.9 Deprecation
----------------------

Python 3.9 is no longer supported. The minimum required Python version is now 3.10. Please upgrade your Python installation to version 3.10 or later. Alternatively, you can use the provided Docker images. For more information, see :ref:`idf-docker-image`.

Refer to the official `Python documentation <https://docs.python.org/3/using/index.html>`_ for instructions on upgrading Python for your operating system.

For Linux users, it is recommended to upgrade to a newer version of your distribution that includes a supported Python version.

Changes in Configuration Files Syntax
-------------------------------------

ESP-IDF v6 uses esp-idf-kconfig v3, which introduces several changes in the configuration (``Kconfig``) files. For the full list of changes, please refer to the `esp-idf-kconfig migration guide from v2.x to v3.x <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/developer-guide/migration-guide.html>`_.

CMake version upgrade
---------------------

The minimal supported CMake version has been upgraded to 3.22.1. In case you encounter build issues regarding this change, we recommend updating your operating system to a version that includes CMake 3.22 or newer.

If updating the OS is not possible, you can install a recommended CMake version using the following command: ``./tools/idf_tools.py install cmake``.

This affects ESP-IDF users who use system-provided CMake and custom CMake.

Unit-Test-App
-------------

The legacy unit-test-app has been moved out of ESP-IDF repository as no ESP-IDF component longer uses it for running tests. It is now available at the `component registry <https://components.espressif.com/components/espressif/unit-test-app>`_ and a test app can be created using the ``idf.py create-project-from-example`` command:

.. code-block:: bash

    idf.py create-project-from-example espressif/unit-test-app:unit-test-app

Catch
-----

The header-only copy of Catch2 unit testing library previously located in tools/catch directory has been removed. To continue using Catch2 in your project, migrate to Catch2 3.x, available from the `ESP component registry <https://components.espressif.com/components/espressif/catch2>`_. For an example of migrating from Catch2 2.x to Catch2 3.x, see commit 79a2c15477dc327550ff46a64ee0f8b4679cc417.

Dropped ``idf.py size --legacy`` option
---------------------------------------

The ``--legacy`` argument for ``idf.py size`` has been removed, as the legacy implementation is no longer supported. The ``ESP_IDF_SIZE_LEGACY`` environment variable also no longer has any effect. To continue using the legacy option, use ESP-IDF version 5.5 or lower. For ESP-IDF v6.0 and later, simply replace ``idf.py size --legacy`` with ``idf.py size``. If you encounter esp-idf-size version problems, please run the install script to update to the correct version.

Changed ``idf.py size --format json`` to ``--format json2``
-----------------------------------------------------------

The ``--format json`` option has been replaced with ``--format json2``. The ``json2`` format provides better structure with explicit ``total``, ``used``, and ``free`` fields for each memory region, and detailed breakdown in the ``parts`` section. To migrate, replace ``idf.py size --format json`` with ``idf.py size --format json2``.

- **Old format (json)**: Flat structure with direct memory type fields like ``"dram_data": 9192, "iram_text": 43295``.
- **New format (json2)**: Hierarchical structure with a ``layout`` array containing memory regions:

    .. code-block:: json

        {
            "version": "1.1",
            "layout": [
                {
                    "name": "DRAM",
                    "total": 180736,
                    "used": 11344,
                    "free": 169392,
                    "parts": {
                        ".data": {"size": 9192},
                        ".bss": {"size": 2152}
                    }
                }
            ]
        }

