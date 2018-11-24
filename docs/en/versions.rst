ESP-IDF Versions
================

The ESP-IDF GitHub repository is updated regularly, especially on the "master branch" where new development happens. There are also stable releases which are recommended for production use.

Releases
--------

Documentation for the current stable version can always be found at this URL:

https://docs.espressif.com/projects/esp-idf/en/stable/

Documentation for the latest version ("master branch") can always be found at this URL:

https://docs.espressif.com/projects/esp-idf/en/latest/

The full history of releases can be found on the GitHub repository `Releases page`_. There you can find release notes, links to each version of the documentation, and instructions for obtaining each version.

Documentation for all releases can also be found in the HTML documentation by clicking the "versions" pop up in the bottom-left corner of the page. You can use this popup to switch between versions of the documentation.

.. image:: /../_static/choose_version.png

Which Version Should I Start With?
----------------------------------

- For production purposes, use the `current stable version`_. Stable versions have been manually tested, and are updated with "bugfix releases" which fix bugs without changing other functionality (see `Versioning Scheme`_ for more details).

- For prototyping, experimentation or for developing new ESP-IDF features, use the `latest version (master branch in Git) <https://docs.espressif.com/projects/esp-idf/en/latest/>`_. The latest version in the master branch has all the latest features and has passed automated testing, but has not been completely manually tested ("bleeding edge").

- If a required feature is not yet available in a stable release, but you don't want to use the master branch, it is possible to check out a pre-release version or a release branch. It is recommended to start from a stable version and then follow the instructions for :ref:`updating-pre-release` or :ref:`updating-release-branch`.

See :ref:`updating` if you already have a local copy of ESP-IDF and wish to update it.

Versioning Scheme
-----------------

ESP-IDF uses `Semantic Versioning <http://semver.org/>`_. This means:

- Major Releases like ``v3.0`` add new functionality and may change functionality. This includes removing deprecated functionality.

  When updating to a new major release (for example, from ``v2.1`` to ``v3.0``), some of your project's code may need updating and functionality will need to be re-tested. The release notes on the `Releases page`_ include lists of Breaking Changes to refer to.
- Minor Releases like ``v3.1`` add new functionality and fix bugs but will not change or remove documented functionality, or make incompatible changes to public APIs.

  If updating to a new minor release (for example, from ``v3.0`` to ``v3.1``) then none of your project's code should need updating, but you should re-test your project. Pay particular attention to items mentioned in the release notes on the `Releases page`_.
- Bugfix Releases like ``v3.0.1`` only fix bugs and do not add new functionality.

  If updating to a new bugfix release (for example, from ``v3.0`` to ``v3.0.1``), you should not need to change any code in your project and should only need to re-test functionality relating directly to bugs listed in the release notes on the `Releases page`_.

Checking The Current Version
----------------------------

The local ESP-IDF version can be checked using git::

  cd $IDF_PATH
  git describe --tags --dirty

The version is also compiled into the firmware and can be accessed (as a string) via the macro ``IDF_VER``. The default ESP-IDF bootloader will print the version on boot (these versions in code will not always update, it only changes if that particular source file is recompiled).

Examples of ESP-IDF versions:

============================ ==================================================
Version String               Meaning
============================ ==================================================
``v3.2-dev-306-gbeb3611ca``  Master branch pre-release, in development for
                             version 3.2. 306 commits after v3.2 development
                             started. Commit identifier ``beb3611ca``.
``v3.0.2``                   Stable release, tagged ``v3.0.2``.
``v3.1-beta1-75-g346d6b0ea`` Beta version in development (on a
                             :ref:`release branch <updating-release-branch>`).
                             75 commits after ``v3.1-beta1`` pre-release tag.
                             Commit identifier ``346d6b0ea``.
``v3.0.1-dirty``             Stable release, tagged ``v3.0.1``.
                             There are modifications in the local ESP-IDF
                             directory ("``dirty``").
============================ ==================================================



Git Workflow
------------

The development (Git) workflow of the Espressif ESP-IDF team is:

- New work is always added on the master branch (latest version) first. The ESP-IDF version on ``master`` is always tagged with ``-dev`` (for "in development"), for example ``v3.1-dev``.
- Changes are first added to an internal Git repository for code review and testing, but are pushed to GitHub after automated testing passes.
- When a new version (developed on ``master``) becomes feature complete and "beta" quality, a new branch is made for the release, for example ``release/v3.1``. A pre-release tag is also created, for example ``v3.1-beta1``. You can see a full `list of branches`_ and a `list of tags`_ on GitHub. Beta pre-releases have release notes which may include a significant number of Known Issues.
- As testing of the beta version progresses, bug fixes will be added to both the ``master`` branch and the release branch. New features (for the next release) may start being added to ``master`` at the same time.
- Once testing is nearly complete a new release candidate is tagged on the release branch, for example ``v3.1-rc1``. This is still a pre-release version.
- If no more significant bugs are found or reported then the final Major or Minor Version is tagged, for example ``v3.1``. This version appears on the `Releases page`_.
- As bugs are reported in released versions, the fixes will continue to be committed to the same release branch.
- Regular bugfix releases are made from the same release branch. After manual testing is complete, a bugfix release is tagged (i.e. ``v3.1.1``) and appears on the `Releases page`_.

.. _updating:

Updating ESP-IDF
----------------

Updating ESP-IDF depends on which version(s) you wish to follow:

- :ref:`updating-stable-releases` is recommended for production use.
- :ref:`updating-master` is recommended for latest features, development use, and testing.
- :ref:`updating-release-branch` is a compromise between these two.

.. note:: These guides assume you already have a local copy of ESP-IDF. To get one, follow the :doc:`Getting Started </get-started/index>` guide for any ESP-IDF version.

.. _`updating-stable-releases`:

Updating to Stable Release
^^^^^^^^^^^^^^^^^^^^^^^^^^

To update to new ESP-IDF releases (recommended for production use), this is the process to follow:

- Check the `Releases page`_ regularly for new releases.
- When a bugfix release for a version you are using is released (for example if using ``v3.0.1`` and ``v3.0.2`` is available), check out the new bugfix version into the existing ESP-IDF directory::

    cd $IDF_PATH
    git fetch
    git checkout vX.Y.Z
    git submodule update --init --recursive
- When major or minor updates are released, check the Release Notes  on the releases page and decide if you would like to update or to stay with your existing release. Updating is via the same Git commands shown above.

.. note:: If you installed the stable release via zip file rather than using git, it may not be possible to change versions this way. In this case, update by downloading a new zip file and replacing the entire ``IDF_PATH`` directory with its contents.


.. _`updating-pre-release`:

Updating to a Pre-Release Version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is also possible to ``git checkout`` a tag corresponding to a pre-release version or release candidate, the process is the same as :ref:`updating-stable-releases`.

Pre-release tags are not always found on the `Releases page`_. Consult the `list of tags`_ on GitHub for a full list. Caveats for using a pre-release are similar to :ref:`updating-release-branch`.

.. _`updating-master`:

Updating to Master Branch
^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: Using Master branch means living "on the bleeding edge" with the latest ESP-IDF code.

To use the latest version on the ESP-IDF master branch, this is the process to follow:

- Check out the master branch locally::

    cd $IDF_PATH
    git checkout master
    git pull
    git submodule update --init --recursive
- Periodically, re-run ``git pull`` to pull the latest version of master. Note that you may need to change your project or report bugs after updating master branch.
- To switch from ``master`` to a release branch or stable version, run ``git checkout`` as shown in the other sections.

.. important:: It is strongly recommended to regularly run ``git pull`` and then ``git submodule update --init --recursive`` so a local copy of ``master`` does not get too old. Arbitrary old master branch revisions are effectively unsupportable "snapshots" that may have undocumented bugs. For a semi-stable version, try :ref:`updating-release-branch` instead.

.. _`updating-release-branch`:

Updating to a Release Branch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In stability terms, using a release branch is part-way between using ``master`` branch and only using stable releases. A release branch is always beta quality or better, and receives bug fixes before they appear in each stable release.

You can find a `list of branches`_ on GitHub.

For example, to follow the branch for ESP-IDF v3.1, including any bugfixes for future releases like ``v3.1.1``, etc::

  cd $IDF_PATH
  git fetch
  git checkout release/v3.1
  git pull
  git submodule update --init --recursive

Each time you ``git pull`` this branch, ESP-IDF will be updated with fixes for this release.

.. note:: The is no dedicated documentation for release branches. It is recommended to use the documentation for the closest version to the branch which is currently checked out.

.. _`Releases page`: http://github.com/espressif/esp-idf/releases
.. _`list of branches`: https://github.com/espressif/esp-idf/branches
.. _`list of tags`: https://github.com/espressif/esp-idf/tags
.. _`current stable version`: https://docs.espressif.com/projects/esp-idf/en/stable/
