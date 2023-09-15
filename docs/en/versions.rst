ESP-IDF Versions
================

:link_to_translation:`zh_CN:[中文]`

The ESP-IDF GitHub repository is updated regularly, especially the master branch where new development takes place.

For production use, there are also stable releases available.


Releases
--------

The documentation for the current stable release version can always be found at this URL:

https://docs.espressif.com/projects/esp-idf/en/stable/

Documentation for the latest version (master branch) can always be found at this URL:

https://docs.espressif.com/projects/esp-idf/en/latest/

The full history of releases can be found on the GitHub repository `Releases page`_. There you can find release notes, links to each version of the documentation, and instructions for obtaining each version.

.. only:: html

    Another place to find documentation for all current releases is the documentation page, where you can go to the upper-left corner and click the version dropdown (between the target dropdown and the search bar). You can also use this dropdown to switch between versions of the documentation.

    .. image:: /../_static/choose_version.png

    Documentation for older versions are also still available:

    .. raw:: html
        :file: ../_static/version_table.html



Which Version Should I Start With?
----------------------------------

- For production purposes, use the `current stable version`_. Stable versions have been manually tested, and are updated with "bugfix releases" which fix bugs without changing other functionality (see `Versioning Scheme`_ for more details). Every stable release version can be found on the `Releases page`_. Also refer to `Compatibility Between ESP-IDF Releases and Revisions of Espressif SoCs <https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY.md>`_ to make sure the ESP-IDF version you selected is compatible with the chip revision you are going to produce with.

- For prototyping, experimentation or for developing new ESP-IDF features, use the `latest version (master branch in Git) <https://docs.espressif.com/projects/esp-idf/en/latest/>`_. The latest version in the master branch has all the latest features and has passed automated testing, but has not been completely manually tested ("bleeding edge").

- If a required feature is not yet available in a stable release, but you do not want to use the master branch, it is possible to check out a pre-release version or a release branch. It is recommended to start from a stable version and then follow the instructions for :ref:`updating-pre-release` or :ref:`updating-release-branch`.

- If you plan to use another project which is based on ESP-IDF, please check the documentation of that project to determine the version(s) of ESP-IDF it is compatible with.

See :ref:`updating` if you already have a local copy of ESP-IDF and wish to update it.

.. _versioning-scheme:

Versioning Scheme
-----------------

ESP-IDF uses `Semantic Versioning <https://semver.org/>`_. This means that:

- Major Releases, like ``v3.0``, add new functionality and may change functionality. This includes removing deprecated functionality.

  If updating to a new major release (for example, from ``v2.1`` to ``v3.0``), some of your project's code may need updating and functionality may need to be re-tested. The release notes on the `Releases page`_ include lists of Breaking Changes to refer to.

- Minor Releases like ``v3.1`` add new functionality and fix bugs but will not change or remove documented functionality, or make incompatible changes to public APIs.

  If updating to a new minor release (for example, from ``v3.0`` to ``v3.1``), your project's code does not require updating, but you should re-test your project. Pay particular attention to the items mentioned in the release notes on the `Releases page`_.

- Bugfix Releases like ``v3.0.1`` only fix bugs and do not add new functionality.

  If updating to a new bugfix release (for example, from ``v3.0`` to ``v3.0.1``), you do not need to change any code in your project, and you only need to re-test the functionality directly related to bugs listed in the release notes on the `Releases page`_.

Support Periods
---------------

Each ESP-IDF major and minor release version has an associated support period. After this period, the release is End of Life and no longer supported.

The `ESP-IDF Support Period Policy`_ explains this in detail, and describes how the support periods for each release are determined.

Each release on the `Releases page`_ includes information about the support period for that particular release.

As a general guideline:

- If starting a new project, use the latest stable release.
- If you have a GitHub account, click the "Watch" button in the top-right of the `Releases page`_ and choose "Releases only". GitHub will notify you whenever a new release is available. Whenever a bug fix release is available for the version you are using, plan to update to it.
- If possible, periodically update the project to a new major or minor ESP-IDF version (for example, once a year.) The update process should be straightforward for Minor updates, but may require some planning and checking of the release notes for Major updates.
- Always plan to update to a newer release before the release you are using becomes End of Life.

Each ESP-IDF major and minor release (V4.1, V4.2, etc) is supported for 30 months after the initial stable release date.

Supported means that the ESP-IDF team will continue to apply bug fixes, security fixes, etc to the release branch on GitHub, and periodically make new bugfix releases as needed.

Support period is divided into "Service" and "Maintenance" period:

.. list-table::
   :header-rows: 1
   :widths: 25 25 55

   * - Period
     - Duration
     - Recommended for new projects?
   * - Service
     - 12 months
     - Yes
   * - Maintenance
     - 18 months
     - No

During the Service period, bugfixes releases are more frequent. In some cases, support for new features may be added during the Service period (this is reserved for features which are needed to meet particular regulatory requirements or standards for new products, and which carry a very low risk of introducing regressions.)

During the Maintenance period, the version is still supported but only bugfixes for high severity issues or security issues will be applied.

Using an "In Service" version is recommended when starting a new project.

Users are encouraged to upgrade all projects to a newer ESP-IDF release before the support period finishes and the release becomes End of Life (EOL). It is our policy to not continue fixing bugs in End of Life releases.

Pre-release versions (betas, previews, ``-rc`` and ``-dev`` versions, etc) are not covered by any support period. Sometimes a particular feature is marked as "Preview" in a release, which means it is also not covered by the support period.

The ESP-IDF Programming Guide has information about the `different versions of ESP-IDF <https://docs.espressif.com/projects/esp-idf/en/latest/versions.html>`_ (major, minor, bugfix, etc).


.. image:: https://dl.espressif.com/dl/esp-idf/support-periods.svg


Checking the Current Version
----------------------------

The local ESP-IDF version can be checked by using idf.py::

  idf.py --version

The ESP-IDF version is also compiled into the firmware and can be accessed (as a string) via the macro ``IDF_VER``. The default ESP-IDF bootloader will print the version on boot (the version information is not always updated if the code in the GitHub repo is updated, it only changes if there is a clean build or if that particular source file is recompiled).

If writing code that needs to support multiple ESP-IDF versions, the version can be checked at compile time using :ref:`compile-time macros <idf-version-h>`.

Examples of ESP-IDF versions:

============================ =====================================================================================
Version String               Meaning
============================ =====================================================================================
``v3.2-dev-306-gbeb3611ca``  | Master branch pre-release.
                             | - ``v3.2-dev`` - in development for version 3.2.
                             | - ``306`` - number of commits after v3.2 development started.
                             | - ``beb3611ca`` - commit identifier.
``v3.0.2``                   | Stable release, tagged ``v3.0.2``.
``v3.1-beta1-75-g346d6b0ea`` | Beta version in development (on a :ref:`release branch <updating-release-branch>`).
                             | - ``v3.1-beta1`` - pre-release tag.
                             | - ``75`` - number of commits after the pre-release beta tag was assigned.
                             | - ``346d6b0ea`` - commit identifier.
``v3.0.1-dirty``             | Stable release, tagged ``v3.0.1``.
                             | - ``dirty`` means that there are modifications in the local ESP-IDF directory.
============================ =====================================================================================


Git Workflow
------------

The development (Git) workflow of the Espressif ESP-IDF team is as follows:

- New work is always added on the master branch (latest version) first. The ESP-IDF version on ``master`` is always tagged with ``-dev`` (for "in development"), for example ``v3.1-dev``.
- Changes are first added to an internal Git repository for code review and testing but are pushed to GitHub after automated testing passes.
- When a new version (developed on ``master``) becomes feature complete and "beta" quality, a new branch is made for the release, for example ``release/v3.1``. A pre-release tag is also created, for example ``v3.1-beta1``. You can see a full `list of branches`_ and a `list of tags`_ on GitHub. Beta pre-releases have release notes which may include a significant number of Known Issues.
- As testing of the beta version progresses, bug fixes will be added to both the ``master`` branch and the release branch. New features for the next release may start being added to ``master`` at the same time.
- Once testing is nearly complete a new release candidate is tagged on the release branch, for example ``v3.1-rc1``. This is still a pre-release version.
- If no more significant bugs are found or reported, then the final Major or Minor Version is tagged, for example ``v3.1``. This version appears on the `Releases page`_.
- As bugs are reported in released versions, the fixes will continue to be committed to the same release branch.
- Regular bugfix releases are made from the same release branch. After manual testing is complete, a bugfix release is tagged (i.e., ``v3.1.1``) and appears on the `Releases page`_.


.. _updating:

Updating ESP-IDF
----------------

Updating ESP-IDF depends on which version(s) you wish to follow:

- :ref:`updating-stable-releases` is recommended for production use.
- :ref:`updating-master` is recommended for the latest features, development use, and testing.
- :ref:`updating-release-branch` is a compromise between the first two.

.. note::

  These guides assume that you already have a local copy of ESP-IDF cloned. To get one, check Step 2 in the :doc:`Getting Started </get-started/index>` guide for any ESP-IDF version.


.. _`updating-stable-releases`:

Updating to Stable Release
^^^^^^^^^^^^^^^^^^^^^^^^^^

To update to a new ESP-IDF release (recommended for production use), this is the process to follow:

- Check the `Releases page`_ regularly for new releases.
- When a bugfix release for the version you are using is released (for example, if using ``v3.0.1`` and ``v3.0.2`` is released), check out the new bugfix version into the existing ESP-IDF directory.
- In Linux or macOS system, please run the following commands to update the local branch to vX.Y.Z:

.. code-block:: bash

    cd $IDF_PATH
    git fetch
    git checkout vX.Y.Z
    git submodule update --init --recursive

- In the Windows system, please replace ``cd $IDF_PATH`` with ``cd %IDF_PATH%``.
- When major or minor updates are released, check the Release Notes on the releases page and decide if you want to update or to stay with your current release. Updating is via the same Git commands shown above.

.. note::

  If you installed the stable release via zip file instead of using git, it might not be possible to update versions using the commands. In this case, update by downloading a new zip file and replacing the entire ``IDF_PATH`` directory with its contents.


.. _`updating-pre-release`:

Updating to a Pre-Release Version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is also possible to ``git checkout`` a tag corresponding to a pre-release version or release candidate, the process is the same as :ref:`updating-stable-releases`.

Pre-release tags are not always found on the `Releases page`_. Consult the `list of tags`_ on GitHub for a full list. Caveats for using a pre-release are similar to :ref:`updating-release-branch`.

.. _`updating-master`:

Updating to Master Branch
^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::

  Using Master branch means living "on the bleeding edge" with the latest ESP-IDF code.

To use the latest version on the ESP-IDF master branch, this is the process to follow:

- In Linux or macOS system, please run the following commands to check out to the master branch locally:

.. code-block:: bash

    cd $IDF_PATH
    git checkout master
    git pull
    git submodule update --init --recursive

- In the Windows system, please replace ``cd $IDF_PATH`` with ``cd %IDF_PATH%``.
- Periodically, re-run ``git pull`` to pull the latest version of master. Note that you may need to change your project or report bugs after updating your master branch.
- To switch from master to a release branch or stable version, run ``git checkout`` as shown in the other sections.

.. important::

    It is strongly recommended to regularly run ``git pull`` and then ``git submodule update --init --recursive`` so a local copy of master does not get too old. Arbitrary old master branch revisions are effectively unsupportable "snapshots" that may have undocumented bugs. For a semi-stable version, try :ref:`updating-release-branch` instead.

.. _`updating-release-branch`:

Updating to a Release Branch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In terms of stability, using a release branch is part-way between using the master branch and only using stable releases. A release branch is always beta quality or better, and receives bug fixes before they appear in each stable release.

You can find a `list of branches`_ on GitHub.

For example, in Linux or macOS system, you can execute the following commands to follow the branch for ESP-IDF v3.1, including any bugfixes for future releases like ``v3.1.1``, etc:

.. code-block:: bash

  cd $IDF_PATH
  git fetch
  git checkout release/v3.1
  git pull
  git submodule update --init --recursive

In the Windows system, please replace ``cd $IDF_PATH`` with ``cd %IDF_PATH%``.

Each time you ``git pull`` this branch, ESP-IDF will be updated with fixes for this release.

.. note::

    There is no dedicated documentation for release branches. It is recommended to use the documentation for the closest version to the branch which is currently checked out.

.. _`Releases page`: https://github.com/espressif/esp-idf/releases
.. _`list of branches`: https://github.com/espressif/esp-idf/branches
.. _`list of tags`: https://github.com/espressif/esp-idf/tags
.. _`current stable version`: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _`ESP-IDF Support Period Policy`:  https://github.com/espressif/esp-idf/blob/master/SUPPORT_POLICY.md
