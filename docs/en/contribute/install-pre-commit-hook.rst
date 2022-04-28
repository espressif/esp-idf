Install pre-commit Hook for ESP-IDF Project
===========================================

Required Dependency
-------------------

Python 3.7.* or above. This is our recommended python version for IDF developers.

If you still have python versions not compatible, update your python versions before installing the pre-commit hook.

Install pre-commit
------------------

Run ``pip install pre-commit``

Install pre-commit hook
-----------------------

1. Go to the IDF Project Directory

2. Run ``pre-commit install --allow-missing-config``. Install hook by this approach will let you commit successfully even in branches without the ``.pre-commit-config.yaml``

3. pre-commit hook will run automatically when you're running ``git commit`` command

Uninstall pre-commit
--------------------

Run ``pre-commit uninstall``

What's More?
------------

For detailed usage, please refer to the documentation of pre-commit_.

.. _pre-commit: https://pre-commit.com/

Common Problems For Windows Users
---------------------------------

``/usr/bin/env: python: Permission denied.``

   If you're in Git Bash, please check the python executable location by run ``which python``.

   If the executable is under ``~/AppData/Local/Microsoft/WindowsApps/``, then it's a link to Windows AppStore, not a real one.

   Please install python manually and update this in your ``PATH`` environment variable.


Your %USERPROFILE% contains non-ASCII characters

   ``pre-commit`` may fail when initializing an environment for a particular hook when the path of ``pre-commit``'s cache contains non-ASCII characters. The solution is to set ``PRE_COMMIT_HOME`` to a path containing only standard characters before running pre-commit.

   - CMD: ``set PRE_COMMIT_HOME=C:\somepath\pre-commit``
   - PowerShell: ``$Env:PRE_COMMIT_HOME = "C:\somepath\pre-commit"``
   - git bash: ``export PRE_COMMIT_HOME="/c/somepath/pre-commit"``
