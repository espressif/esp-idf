Tools
=====

:link_to_translation:`zh_CN:[中文]`

GDBGUI installation
-------------------

The support for the ``--enable-gdbgui`` argument  has been removed from the install scripts and `gdbgui <https://www.gdbgui.com>`_ cannot be installed this way anymore. Please use the ``pipx`` method from the `gdbgui installation guide <https://www.gdbgui.com/installation/>`_ in order to set up this tool.

Depending on your operating system and Python version, you may need to consult the `list of known issues <https://github.com/cs01/gdbgui/issues>`_.

For example, `Python 3.13 is not supported <https://github.com/cs01/gdbgui/issues/494>`_ on any operating systems at the moment.

The Windows operating system is not supported since gdbgui version 0.14. Because of the existence of other issues, you will need Python 3.10 with some specific versions of the dependencies. The last know working gdbgui and dependency versions can be installed with the following command:

```bash
pipx install "gdbgui==0.13.2.0" "pygdbmi<=0.9.0.2" "python-socketio<5" "jinja2<3.1" "itsdangerous<2.1"
```

On Linux or MacOS, you can use Python 3.11 or 3.12 and gdbgui version 0.15.2.0.

Please be aware that these recommendations may change over time and for an up-to-date list of issues refer to `the official issue tracker <https://github.com/cs01/gdbgui/issues>`_.

We recommend to use ``idf.py gdb`` instead of ``idf.py gdbgui``, or debug in Eclipse/Vscode if you encounter an issue with the installation.

``idf.py efuse*`` Commands Require Port
---------------------------------------

All commands with eFuse functionality now require a serial port to be specified. This was done to prevent accidental use of the wrong port, as these operations are irreversible.

For all ``idf.py efuse*`` commands, you now need to specify the serial port with the ``--port`` argument (or ``ESPPORT`` environment variable). If the port is not specified, the command will fail with an error message.
