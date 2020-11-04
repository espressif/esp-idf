Install pre-commit Hook for ESP-IDF Project
===========================================

Required Dependency
-------------------

Python 3.6.1 or above. This is our recommendation python version for IDF developers.

If you still have versions not compatible, please do not install pre-commit hook and update your python versions.

Install pre-commit
------------------

Run ``pip install pre-commit``

Install pre-commit hook
-----------------------

1. Go to the IDF Project Directory

2. Run ``pre-commit install --allow-missing-config``. Install hook by this approach will let you commit successfully even in branches without the ``.pre-commit-config.yaml``

3. pre-commit hook will run automatically when you're running ``git commit`` command

What's More?
------------

For detailed usage, Please refer to the documentation of pre-commit_.

.. _pre-commit: http://www.pre-commit.com/

Common Problems For Windows Users
---------------------------------

1. ``/usr/bin/env: python: Permission denied.``

   If you're in Git Bash or MSYS terminal, please check the python executable location by run ``which python``.

   If the executable is under ``~/AppData/Local/Microsoft/WindowsApps/``, then it's a link to Windows AppStore, not a real one.

   Please install python manually and update this in your ``PATH`` environment variable.
