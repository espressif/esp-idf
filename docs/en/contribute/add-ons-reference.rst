Documentation Add-ons and Extensions Reference
==============================================

This documentation is created using `Sphinx <http://www.sphinx-doc.org/>`_ application that renders text source files in `reStructuredText <https://en.wikipedia.org/wiki/ReStructuredText>`_ (``.rst``) format located in :idf:`docs` directory. For some more details on that process, please refer to section :doc:`documenting-code`.

Besides Sphinx there are several other applications that help to provide nicely formatted and easy to navigate documentation. These applications are listed in section :ref:`setup-for-building-documentation` with the installed version numbers provided in file :idf_file:`docs/requirements.txt`.

On top of that we have created a couple of custom add-ons and extensions to help integrate documentation with underlining `ESP-IDF`_ repository and further improve navigation as well as maintenance of documentation.

The purpose of this section is to provide a quick reference to the add-ons and the extensions.


Documentation Folder Structure
------------------------------

* The ESP-IDF repository contains a dedicated documentation folder :idf:`docs` in the root.
* The ``docs`` folder contains localized documentation in :idf:`docs/en` (English) and :idf:`docs/zh_CN` (simplified Chinese) subfolders.
* Graphics files and fonts common to localized documentation are contained in :idf:`docs/_static` subfolder
* Remaining files in the root of ``docs`` as well as ``docs/en`` and ``docs/zh_CN`` provide configuration and scripts used to automate documentation processing including the add-ons and extensions.
* Several folders and files are generated dynamically during documentations build and placed primarily in ``docs/[lang]/_build`` folders. These folders are temporary and not visible in `ESP-IDF`_ repository,


Add-ons and Extensions Reference
--------------------------------

:idf_file:`docs/conf_common.py`
    This file contains configuration common to each localized documentation (e.g. English, Chinese). The contents of this file is imported to standard Sphinx configuration file ``conf.py`` located in respective language folders (e.g. ``docs/en``, ``docs/zh_CN``) during build for each language.

:idf_file:`docs/check_doc_warnings.sh`
    If there are any warnings reported during documentation build, then the build is failed. The warnings should be resolved before merging any documentation updates. This script is doing check for warnings in respective log file to fail the build. See also description of ``sphinx-known-warnings.txt`` below.

:idf_file:`docs/check_lang_folder_sync.sh`
    To reduce potential discrepancies when maintaining concurrent language version, the structure and filenames of language folders ``docs/en`` and ``docs/zh_CN`` folders should be kept identical. The script ``check_lang_folder_sync.sh`` is run on each documentation build to verify if this condition is met.

    .. note::

        If a new content is provided in e.g. English, and there is no any translation yet, then the corresponding file in ``zh_CN`` folder should contain an ``.. include::`` directive pointing to the source file in English. This will automatically include the English version visible to Chinese readers. For example if a file ``docs/zh_CN/contribute/documenting-code.rst`` does not have a Chinese translation, then it should contain  ``.. include:: ../../en/contribute/documenting-code.rst`` instead.

:idf_file:`docs/docs_common.mk`
    It contains the common code which is included into the language-specific ``Makefiles``. Note that this file contains couple of customizations comparing to what is provided within standard Sphinx installation, e.g. ``gh-linkcheck`` command has been added.

:idf_file:`docs/gen-dxd.py`
    A Python script that generates API reference files based on Doxygen ``xml`` output. The files have an ``inc`` extension and are located in ``docs/[lang]/_build/inc`` directory created dynamically when documentation is build. Please refer to :doc:`documenting-code` and :doc:`../api-reference/template`, section **API Reference** for additional details on this process.

:idf_file:`docs/gen-toolchain-links.py`
    There couple of places in documentation that provide links to download the toolchain. To provide one source of this information and reduce effort to manually update several files, this script generates toolchain download links and toolchain unpacking code snippets based on information found in :idf_file:`tools/toolchain_versions.mk`.

:idf_file:`docs/gen-version-specific-includes.py`
    Another Python script to automatically generate reStructuredText Text ``.inc`` snippets with version-based content for this ESP-IDF version.

:idf_file:`docs/html_redirects.py`
    During documentation lifetime some source files are moved between folders or renamed. This Python script is adding a mechanism to redirect documentation pages that have changed URL by generating in the Sphinx output static HTML redirect pages. The script is used together with a redirection list ``html_redirect_pages`` defined in file :idf_file:`docs/conf_common.py`.

:idf_file:`docs/link-roles.py`
    This is an implementation of a custom `Sphinx Roles <https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html>`_ to help linking from documentation to specific files and folders in `ESP-IDF`_. For description of implemented roles please see :ref:`link-custom-roles` and :ref:`link-language-versions`.

:idf_file:`docs/local_util.py`
    A collection of utility functions useful primarily when building documentation locally (see :ref:`setup-for-building-documentation`) to reduce the time to generate documentation on a second and subsequent builds. The utility functions check what Doxygen ``xml`` input files have been changed and copy these files to destination folders, so only the changed files are used during build process.

:idf_file:`docs/sphinx-known-warnings.txt`
    There are couple of spurious Sphinx warnings that cannot be resolved without doing update to the Sphinx source code itself. For such specific cases respective warnings are documented in ``sphinx-known-warnings.txt`` file, that is checked during documentation build, to ignore the spurious warnings.

:idf_file:`tools/gen_esp_err_to_name.py`
    This script is traversing the `ESP-IDF`_ directory structure looking for error codes and messages in source code header files to generate an ``.inc`` file to include in documentation under :doc:`../api-reference/error-codes`.

:idf_file:`tools/kconfig_new/confgen.py`
    Options to configure ESP-IDF's :idf:`components` are contained in ``Kconfig`` files located inside directories of individual components, e.g. :idf_file:`components/bt/Kconfig`. This script is traversing the ``component`` directories to collect configuration options and generate an ``.inc`` file to include in documentation under :ref:`configuration-options-reference`.



Related Documents
-----------------

* :doc:`documenting-code`


.. _ESP-IDF: https://github.com/espressif/esp-idf/
