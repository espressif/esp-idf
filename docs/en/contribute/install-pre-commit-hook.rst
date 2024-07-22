Install Pre-commit Hook for ESP-IDF Project
===========================================

:link_to_translation:`zh_CN:[中文]`

Install ``pre-commit``
----------------------

Run ``pip install pre-commit``.

Install ``pre-commit`` Hook
---------------------------

1. Go to the ESP-IDF project directory.

2. Run ``pre-commit install --allow-missing-config -t pre-commit -t commit-msg``. Install hook by this approach will let you commit successfully even in branches without the ``.pre-commit-config.yaml``

3. pre-commit hook will run automatically when you are running ``git commit`` command

Uninstall ``pre-commit`` Hook
-----------------------------

Run ``pre-commit uninstall``.

Related Documents
-------------------

For detailed usage, please refer to the documentation of pre-commit_.

.. _pre-commit: https://pre-commit.com/

Common Problems For Windows Users
---------------------------------

``/usr/bin/env: python: Permission denied.``

    If you are in Git Bash, please check the python executable location by run ``which python``.

    If the executable is under ``~/AppData/Local/Microsoft/WindowsApps/``, then it is a link to Windows AppStore, not a real one.

    Please install Python manually and update this in your ``PATH`` environment variable.


Your ``USERPROFILE`` contains non-ASCII characters

    ``pre-commit`` may fail when initializing an environment for a particular hook when the path of ``pre-commit``'s cache contains non-ASCII characters. The solution is to set ``PRE_COMMIT_HOME`` to a path containing only standard characters before running pre-commit.

    - CMD: ``set PRE_COMMIT_HOME=C:\somepath\pre-commit``
    - PowerShell: ``$Env:PRE_COMMIT_HOME = "C:\somepath\pre-commit"``
    - git bash: ``export PRE_COMMIT_HOME="/c/somepath/pre-commit"``
