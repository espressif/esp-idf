Documenting Code
================

:link_to_translation:`zh_CN:[中文]`

The purpose of this description is to provide quick summary on documentation style used in `espressif/esp-idf`_ repository and how to add new documentation.


Introduction
------------

When documenting code for this repository, please follow `Doxygen style <http://doxygen.nl/manual/docblocks.html#specialblock>`_. You are doing it by inserting special commands, for instance ``@param``, into standard comments blocks, for example: ::

    /**
     * @param ratio this is oxygen to air ratio
     */

Doxygen is phrasing the code, extracting the commands together with subsequent text, and building documentation out of it.

Typical comment block, that contains documentation of a function, looks like below.

.. image:: ../../_static/doc-code-documentation-inline.png
    :align: center
    :alt: Sample inline code documentation

Doxygen supports couple of formatting styles. It also gives you great flexibility on level of details to include in documentation. To get familiar with available features, please check data rich and very well organized `Doxygen Manual <http://doxygen.nl/manual/index.html>`_.


Why we need it?
---------------

The ultimate goal is to ensure that all the code is consistently documented, so we can use tools like `Sphinx`_ and `Breathe`_ to aid preparation and automatic updates of API documentation when the code changes.

With these tools the above piece of code renders like below:

.. image:: ../../_static/doc-code-documentation-rendered.png
    :align: center
    :alt: Sample inline code after rendering


Go for it!
----------

When writing code for this repository, please follow guidelines below.

1. Document all building blocks of code: functions, structs, typedefs, enums, macros, etc. Provide enough information on purpose, functionality and limitations of documented items, as you would like to see them documented when reading the code by others.

2. Documentation of function should describe what this function does. If it accepts input parameters and returns some value, all of them should be explained.

3. Do not add a data type before parameter or any other characters besides spaces. All spaces and line breaks are compressed into a single space. If you like to break a line, then break it twice.

    .. image:: ../../_static/doc-code-function.png
        :align: center
        :alt: Sample function documented inline and after rendering

4. If function has void input or does not return any value, then skip ``@param`` or ``@return``

    .. image:: ../../_static/doc-code-void-function.png
        :align: center
        :alt: Sample void function documented inline and after rendering

5. When documenting a ``define`` as well as members of a ``struct`` or ``enum``, place specific comment like below after each member.

    .. image:: ../../_static/doc-code-member.png
        :align: center
        :alt: Sample of member documentation inline and after rendering

6. To provide well formatted lists, break the line after command (like ``@return`` in example below). ::

    *
    * @return
    *    - ESP_OK if erase operation was successful
    *    - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
    *    - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
    *    - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
    *    - other error codes from the underlying storage driver
    *

7. Overview of functionality of documented header file, or group of files that make a library, should be placed in the same directory in a separate ``README.rst`` file. If directory contains header files for different APIs, then the file name should be ``apiname-readme.rst``.


Go one extra mile
-----------------

There is couple of tips, how you can make your documentation even better and more useful to the reader.

1. Add code snippets to illustrate implementation. To do so, enclose snippet using ``@code{c}`` and ``@endcode`` commands. ::

    *
    * @code{c}
    * // Example of using nvs_get_i32:
    * int32_t max_buffer_size = 4096; // default value
    * esp_err_t err = nvs_get_i32(my_handle, "max_buffer_size", &max_buffer_size);
    * assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
    * // if ESP_ERR_NVS_NOT_FOUND was returned, max_buffer_size will still
    * // have its default value.
    * @endcode
    *

   The code snippet should be enclosed in a comment block of the function that it illustrates.

2. To highlight some important information use command ``@attention`` or ``@note``. ::

    *
    * @attention
    *     1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
    *     2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
    *

   Above example also shows how to use a numbered list.

3. To provide common description to a group of similar functions, enclose them using ``/**@{*/`` and ``/**@}*/`` markup commands::

    /**@{*/
    /**
     * @brief common description of similar functions
     *
     */
    void  first_similar_function (void);
    void second_similar_function (void);
    /**@}*/

   For practical example see :component_file:`nvs_flash/include/nvs.h`.

4. You may want to go even further and skip some code like e.g. repetitive defines or enumerations. In such case enclose the code within ``/** @cond */`` and ``/** @endcond */`` commands. Example of such implementation is provided in :component_file:`driver/include/driver/gpio.h`.

5. Use markdown to make your documentation even more readable. You will add headers, links, tables and more. ::

    *
    * [{IDF_TARGET_NAME} Technical Reference Manual]({IDF_TARGET_TRM_EN_URL})
    *

.. note::

    Code snippets, notes, links, etc. will not make it to the documentation, if not enclosed in a comment block associated with one of documented objects.

6. Prepare one or more complete code examples together with description. Place description in a separate file ``README.md`` in specific folder of :idf:`examples` directory.

.. _link-custom-roles:

Linking Examples
----------------

When linking to examples on GitHub do not use absolute / hardcoded URLs. Instead, use docutils custom roles that will generate links for you. These auto-generated links point to the tree or blob for the git commit ID (or tag) of the repository. This is needed to ensure that links do not get broken when files in master branch are moved around or deleted. The roles will transparently handle files that are located in submodules and will link to the submodule's repository with the correct commit ID.

The following roles are provided:

- ``:idf:`path``` - points to directory inside ESP-IDF
- ``:idf_file:`path``` - points to file inside ESP-IDF
- ``:idf_raw:`path``` - points to raw view of the file inside ESP-IDF
- ``:component:`path``` - points to directory inside ESP-IDF components dir
- ``:component_file:`path``` - points to file inside ESP-IDF components dir
- ``:component_raw:`path``` - points to raw view of the file inside ESP-IDF components dir
- ``:example:`path``` - points to directory inside ESP-IDF examples dir
- ``:example_file:`path``` - points to file inside ESP-IDF examples dir
- ``:example_raw:`path``` - points to raw view of the file inside ESP-IDF examples dir

Example implementation::

    * :example:`get-started/hello_world`
    * :example:`Hello World! <get-started/hello_world>`

How it renders:

* :example:`get-started/hello_world`
* :example:`Hello World! <get-started/hello_world>`

A check is added to the CI build script, which searches RST files for presence of hard-coded links (identified by tree/master, blob/master, or raw/master part of the URL). This check can be run manually: ``cd docs`` and then ``make gh-linkcheck``.


.. _link-language-versions:

Linking Language Versions
-------------------------

Switching between documentation in different languages may be done using ``:link_to_translation:`` custom role. The role placed on a page of documentation provides a link to the same page in a language specified as a parameter. Examples below show how to enter links to Chinese and English versions of documentation::

    :link_to_translation:`zh_CN:中文版`
    :link_to_translation:`en:English`

The language is specified using standard abbreviations like ``en`` or ``zh_CN``. The text after last semicolon is not standardized and may be entered depending on the context where the link is placed, e.g.::

    :link_to_translation:`en:see description in English`


.. _add-illustrations:

Add Illustrations
-----------------

Consider adding diagrams and pictures to illustrate described concepts.

Sometimes it is better to add an illustration than writing a lengthy paragraph to describe a complex idea, a data structure or an algorithm. This repository is using `blockdiag <http://blockdiag.com/en/index.html>`_ suite of tools to generate diagram images from simple text files.

The following types of diagrams are supported:

* `Block diagram <http://blockdiag.com/en/blockdiag/index.html>`_
* `Sequence diagram <http://blockdiag.com/en/seqdiag/index.html>`_
* `Activity diagram <http://blockdiag.com/en/actdiag/index.html>`_
* `Logical network diagram <http://blockdiag.com/en/nwdiag/index.html>`_

With this suite of tools it is possible to generate beautiful diagram images from simple text format (similar to graphviz’s DOT format). The diagram elements are laid out automatically. The diagram code is then converted into ".png" graphics and integrated "behind the scenes" into **Sphinx** documents.

For the diagram preparation you can use an on-line `interactive shell`_ that instantly shows the rendered image.

Below are couple of diagram examples:

* Simple **block diagram** / ``blockdiag`` - `Wi-Fi Buffer Configuration <http://interactive.blockdiag.com/?compression=deflate&src=eJylUk1rwkAQvfsrBntpIUKiRQqSgK0VSj0EtCi0EjbJxCyuuyG7QW3pf-9m06hJeyg0t33zmHkfCZmItjElGwiLJME8IEwjRFHBA3WAj04H9HcFGyZCwoAoldOwUCgNzkWMwZ7GKgUXnKE9gjOcIt2kSuN39sigMiP8jDqX6GmF_Y3GmJCCqUCmJEM9yEXBY4xDcWjOE8GVpO9oztdaGQmRSRAJlMZysjOCKsVj358Fi_H8GV4Nze2Os4zRyvEbB0XktrseQWVktn_ym-wS-UFb0ilt0pa0N6Vn3i_KUEY5zcqrbXWTx_nDaZHjwYvEHGKiSNeC2q_r3FpQZekObAtMTi4XCi2IBBO5e0Rd5L7ppLG574GvO__PUuO7sXTgweTIyY5GcD1XOtToBhYruDf_VvuUad3tD-0_Xq1TLPPSI84xKvNrF9vzLnrTj1M7rYhrXv24cCPVkZUaOK47n1-lOvbk>`_
* Slightly more complicated **block diagram** - `Wi-Fi programming model <http://interactive.blockdiag.com/?compression=deflate&src=eJyFk09P40AMxe98CqscIVILq72UIFX8kSoQWy0RHABFTuImFtOZaGYKuyC-O840bagaRI7Pfs7Pz0mmTP5cMJbwynNOa2tKi4sF6zJdmIIUvO_tgTz7UCqToQL03nK29OSCrqUpfeXCVxDD6Gg47tSKuKy8yL9b1dWov1E3E4atWtAcl8qnrsKapGDNUhdUZObfdr2UQp3mRhkrXdpoGq-BGwhQmJFaoSZns_Q2mZxdwUNQ44Eojxqcx_x5cAhzo73jN4pHv55WL7m4u0nSZHLbOeiFtBePR9dvmcxm19sWrGvFOXo2utd4CGH5eHQ8bGfcTy-n6fnfO9jMuOfoksV9bvmFbO-Lr27-JPAQ4oqbGJ62c8iN1pQ3EA4O-lOJTncXDvvupCGdu3vmqFQmSQqm3CIYBx0EWou6pADjQJbw3Bj-h3I4onxpsHrCQLnmoD0yVKgLJXuP1x3GsowPmUpfbay3yH5T7khPoi7NnpU-1nisPdkFyY_gV4x9XB3Y0pHdpfoJ60toURQOtqbYuvpJ1B6zDXYym0qmTVpNnh-fpWcbRA>`_
* **Sequence diagram** / ``seqdiag`` - `Scan for a Specific AP in All Channels <http://interactive.blockdiag.com/seqdiag/?compression=deflate&src=eJyVkU1PwzAMhu_7FdburUgQXMomTaPcKIdOIIRQlDVuG1EloUknPsR_J2s2rRsT2nKJ9drvY8ex-C4kr8AWXLFSt8waLBg38D0Cf3jh5Io7qRVMQGmFSS-jqJA1qCpXe51cXwTZGg-pUVa1W8tXQRVY8q5xzNbcoNdb3SmBYqk_9vOlVs7Kr3UJoQmMwgDGMMftWwK4QuU28ZOM7uQm3q_zYTQd5OGl4UtsJmMSE5jCXKtSVl2LUPgpXPvpb4Hj1-RUCPWQ3O_K-wKpX84WMLAcB9B-igCouVLYADnDTA_N9GRzHMdnNMoOG2Vb8-4b4CY6Zr4MT3zOF-k9Sx_TbMHy-Sxjtw9Z-mfRHjEA7hD0X8TPLxU91AQ>`_
* **Packet diagram** / ``packetdiag`` - `NVS Page Structure <http://interactive.blockdiag.com/packetdiag/?compression=deflate&src=eJxFkMFOwzAQRO_9ij2mh63idRKaSj1V_ACIE6DIxG4StTgh3oCg6r_j2JTs8c3szNqDqk-GdacasJ-uGlRjKsfjVPM0GriswE_dn786zS3sQRJAYLbXprpRkS-sNV3TcrAGqM1RTWeujr1l1_2Y2U6rIKUod_DIis2LTbJ1YBneeWY-Nj5ts-AtkudPdnJGQ0JppLRFKXZweDhIWrySsPDB95bHb3BzPLx1_K4GSCSt_-4vMizzmykNSuBlgWKuioJYBOHLROnbEBGe_ZfEh-7pNcolIdF_raA8rl5_AaqqWyE>`_

Try them out by modifying the source code and see the diagram instantly rendering below.

.. note::

    There may be slight differences in rendering of font used by the `interactive shell`_ compared to the font used in the esp-idf documentation.


Add Notes
---------

Working on a document, you might need to:

- Place some suggestions on what should be added or modified in future.
- Leave a reminder for yourself or somebody else to follow up.

In this case, add a todo note to your reST file using the directive ``.. todo::``. For example:

.. code-block:: none

   .. todo::

      Add a package diagram.

If you add ``.. todolist::`` to a reST file, the directive will be replaced by a list of all todo notes from the whole documentation.

By default, the directives ``.. todo::`` and ``.. todolist::`` are ignored by documentation builders. If you want the notes and the list of notes to be visible in your locally built documentation, do the following:

1. Open your local ``conf_common.py`` file.
2. Find the parameter ``todo_include_todos``.
3. Change its value from ``False`` to ``True``.

Before pushing your changes to origin, please set the value of ``todo_include_todos`` back to ``False``.

For more details about the extension, see `sphinx.ext.todo <https://www.sphinx-doc.org/en/master/usage/extensions/todo.html#directive-todolist>`_ documentation.

Writing generic documentation for multiple chips
------------------------------------------------

The documentation for all of Espressif's chips is built from the same files. To faciliate the writing of documents that can be re-used for multiple different chips (called below "targets"), we provide you with the following functionality:

Exclusion of content based on chip-target
"""""""""""""""""""""""""""""""""""""""""
Occasionally there will be content that is only relevant for one of targets. When this is the case, you can exclude that content by using the ''.. only:: TAG'' directive, where you replace 'TAG' with one of the following names:

Chip name:

* esp32
* esp32s2

Define identifiers from 'sdkconfig.h', generated by the default menuconfig settings for the target, e.g:

* CONFIG_FREERTOS_UNICORE

Define identifiers from the soc '\*_caps' headers, e.g:

* SOC_BT_SUPPORTED
* SOC_CAN_SUPPORTED

Example:

.. code-block:: none

    .. only:: esp32

        ESP32 specific content.

This directive also supports the boolean operators 'and', 'or' and 'not'. Example:

.. code-block:: none

    .. only:: SOC_BT_SUPPORTED and CONFIG_FREERTOS_UNICORE

        BT specific content only relevant for single-core targets.

This functionality is provided by the `Sphinx selective exclude <https://github.com/pfalcon/sphinx_selective_exclude>`_ extension.

A weakness in this extension is that it does not correctly handle the case were you exclude a section, and that is directly followed by a labeled new section. In these cases everything will render correctly, but the label will not correctly link to the section that follows. A temporary work-around for the cases were this can't be avoided is the following:

.. code-block:: none

    .. only:: esp32

        .. _section_1_label:

        Section 1
        ^^^^^^^^^

        Section one content

        .. _section_2_label:

    .. only:: esp32s2

        _section_2_label:

    Section 2
    ^^^^^^^^^
    Section 2 content

The :TAG: role is used for excluding content from a table of content tree. For example:

.. code-block:: none

    .. toctree::
        :maxdepth: 1

        :esp32: configure-wrover
        configure-other-jtag

When building the documents, Sphinx will use the above mentioned directive and role to include or exclude content based on the target tag it was called with.

.. note::

    If excluding an entire document from the toctree based on targets, it's necessary to also update the ``exclude_patterns`` list in :idf_file:`docs/conf_common.py` to exclude the file for other targets, or a Sphinx warning "WARNING: document isn't included in any toctree" will be generated..

    The recommended way of doing it is adding the document to one of the list that gets included in ``conditional_include_dict`` in :idf_file:`docs/conf_common.py`, e.g. a document which should only be shown for BT capable targets should be added to ``BT_DOCS``. :idf_file:`docs/idf_extensions/exclude_docs.py` will then take care of adding it to ``exclude_patterns`` if the corresponding tag is not set.

If you need to exclude content inside a list or bullet points then this should be done by using the '':TAG:'' role inside the ''.. list:: '' directive.

.. code-block:: none

    .. list::

        :esp32: - ESP32 specific content
        :SOC_BT_SUPPORTED: - BT specific content
        - Common bullet point
        - Also common bullet point


Substitution macros
"""""""""""""""""""
When you need to refer to the chip's name, toolchain name, path or other common names that depend on the target type you can consider using the substitution macros supplied by :idf_file:`docs/idf_extensions/format_idf_target.py`.

For example, the following reStructuredText content:

    This is a {\IDF_TARGET_NAME}, with /{\IDF_TARGET_PATH_NAME}/soc.c, compiled with `xtensa-{\IDF_TARGET_TOOLCHAIN_NAME}-elf-gcc` with `CONFIG_{\IDF_TARGET_CFG_PREFIX}_MULTI_DOC`

Would render in the documentation as:

    This is a {IDF_TARGET_NAME}, with /{IDF_TARGET_PATH_NAME}/soc.c, compiled with `xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf-gcc` with `CONFIG_{IDF_TARGET_CFG_PREFIX}_MULTI_DOC`.

This extension also supports markup for defining local (within a single source file) substitutions. Place a definition like the following into a single line of the RST file::

    {\IDF_TARGET_SUFFIX:default="DEFAULT_VALUE", esp32="ESP32_VALUE", esp32s2="ESP32S2_VALUE"}

This will define a target-dependent substitution of the tag {\IDF_TARGET_SUFFIX} in the current RST file. For example::

    {\IDF_TARGET_TX_PIN:default="IO3", esp32="IO4", esp32s2="IO5"}

Will define a substitution for the tag {\IDF_TARGET_TX_PIN}, which would be replaced by the text IO5 if sphinx was called with the tag esp32s2.

.. note::

   These single-file definitions can be placed anywhere in the .rst file (on their own line), but the name of the directive must start with ``IDF_TARGET_``.


Put it all together
-------------------

Once documentation is ready, follow instruction in :doc:`../api-reference/template` and create a single file, that will merge all individual pieces of prepared documentation. Finally add a link to this file to respective ``.. toctree::`` in ``index.rst`` file located in ``/docs`` folder or subfolders.


OK, but I am new to Sphinx!
---------------------------

1. No worries. All the software you need is well documented. It is also open source and free. Start by checking `Sphinx`_ documentation. If you are not clear how to write using rst markup language, see `reStructuredText Primer <https://www.sphinx-doc.org/en/stable/rest.html>`_. You can also use markdown (.md) files, and find out more about the specific markdown syntax that we use on `Recommonmark parser's documentation page <https://recommonmark.readthedocs.io/en/latest/>`_.

2. Check the source files of this documentation to understand what is behind of what you see now on the screen. Sources are maintained on GitHub in `espressif/esp-idf`_ repository in :idf:`docs` folder. You can go directly to the source file of this page by scrolling up and clicking the link in the top right corner. When on GitHub, see what's really inside, open source files by clicking ``Raw`` button.

3. You will likely want to see how documentation builds and looks like before posting it on the GitHub. There are two options to do so:

    * Install `Sphinx`_, `Breathe`_, `Blockdiag <http://blockdiag.com/en/index.html>`_ and `Doxygen <http://doxygen.nl/>`_ to build it locally, see chapter below.

    * Set up an account on `Read the Docs <https://readthedocs.org/>`_ and build documentation in the cloud. Read the Docs provides document building and hosting for free and their service works really quick and great.

4. To preview documentation before building, use `Sublime Text <https://www.sublimetext.com/>`_ editor together with `OmniMarkupPreviewer <https://github.com/timonwong/OmniMarkupPreviewer>`_ plugin.


.. _setup-for-building-documentation:

Setup for building documentation locally
----------------------------------------

Install Dependencies
""""""""""""""""""""

You can setup environment to build documentation locally on your PC by installing:

1. Doxygen - http://doxygen.nl/
2. Sphinx - https://github.com/sphinx-doc/sphinx/#readme-for-sphinx
3. Breathe - https://github.com/michaeljones/breathe#breathe
4. Document theme "sphinx_idf_theme" - https://github.com/espressif/sphinx_idf_theme
5. Custom 404 page "sphinx-notfound-page" - https://github.com/readthedocs/sphinx-notfound-page
6. Blockdiag - http://blockdiag.com/en/index.html
7. Recommonmark - https://github.com/rtfd/recommonmark


The package "sphinx_idf_theme" is added to have the same "look and feel" of `ESP-IDF Programming Guide <https://docs.espressif.com/projects/esp-idf/en/latest/index.html>`_.

Do not worry about being confronted with several packages to install. Besides Doxygen, all remaining packages are written in pure Python. Therefore installation of all of them is combined into one simple step.

.. important:: Docs building now supports Python 3 only. Python 2 installations will not work.

Doxygen
@@@@@@@

Installation of Doxygen is OS dependent:

**Linux**

::

	sudo apt-get install doxygen

**Windows** - install in MSYS2 console

::

	pacman -S doxygen

**MacOS**

::

	brew install doxygen

.. note::

    If you are installing on Windows MSYS2 system (Linux and MacOS users should skip this note, Windows users who don't use MSYS2 will need to find other alternatives), **before** going further, execute two extra steps below. These steps are required to install dependencies of "blockdiag" discussed under :ref:`add-illustrations`.

    1.  Update all the system packages:

        ::

            $ pacman -Syu

        This process will likely require restarting of the MSYS2 MINGW32 console and repeating above commands, until update is complete.

    2.  Install *pillow*, that is one of dependences of the *blockdiag*:

        ::

            $ pacman -S mingw32/mingw-w64-i686-python-pillow

        Check the log on the screen that ``mingw-w64-i686-python-pillow-4.3.0-1`` or newer is installed. Previous versions of *pillow* will not work.

    A downside of Windows installation is that fonts of the `blockdiag pictures <add-illustrations>` do not render correctly, you will see some random characters instead. Until this issue is fixed, you can use the `interactive shell`_ to see how the complete picture looks like.


Remaining applications
@@@@@@@@@@@@@@@@@@@@@@


All remaining applications are `Python <https://www.python.org/>`_ packages and you can install them in one step as follows:

::

	cd ~/esp/esp-idf/docs
	pip install --user -r requirements.txt

.. note::

	Installation steps assume that ESP-IDF is placed in ``~/esp/esp-idf`` directory, that is default location of ESP-IDF used in documentation.

Building Documentation
""""""""""""""""""""""

::

    cd ~/esp/esp-idf/docs

Now you should be ready to build documentation by invoking::

    ./build_docs.py build

This will build docs for all supported ESP-IDF languages & targets. This can take some time, although jobs will run in parallel up to the number of CPU cores you have (can modify this with the ``--sphinx-parallel-builds`` option, see ``./build_docs.py --help`` for details).

To build for a single language and target combination only::

    ./build_docs.py -l en -t esp32 build

Choices for language (``-l``) are ``en`` and ``zh_CN``. Choices for target (``-t``) are any supported ESP-IDF build system target (for example ``esp32`` and ``esp32s2``).

Build documentation will be placed in ``_build/<language>/<target>/html`` folder. To see it, open the ``index.html`` inside this directory in a web browser.

Building a subset of the documentation
""""""""""""""""""""""""""""""""""""""
Since building the full documentation can be quite slow, it might be useful to just build just the subset of the documentation you are interested in.

This can be achieved by listing the document you want to build::

    ./build_docs.py -l en -t esp32 -i api-reference/peripherals/can.rst build

Building multiple documents is also possible::

    ./build_docs.py -l en -t esp32 -i api-reference/peripherals/can.rst api-reference/peripherals/adc.rst build

As well as wildcards::

    ./build_docs.py -l en -t esp32 -i api-reference/peripherals/* build

Note that this is a feature intended to simply testing and debugging during writing of documentation. The HTML output won't be perfect, i.e. it will not build a proper index that lists all the documents, and any references to documents that are not built will result in warnings.

Building PDF
""""""""""""

It is also possible to build latex files and a PDF of the documentation using ``build_docs.py``. To do this the following Latex packages are required to be installed:

 * latexmk
 * texlive-latex-recommended
 * texlive-fonts-recommended
 * texlive-xetex

The following fonts are also required to be installed:

 * Freefont Serif, Sans and Mono OpenType fonts, available as the package ``fonts-freefont-otf`` on Ubuntu
 * Lmodern, available as the package ``fonts-lmodern`` on Ubuntu
 * Fandol, can be downloaded from `here <https://ctan.org/tex-archive/fonts/fandol>`_

Now you can build the PDF for a target by invoking::

    ./build_docs.py -bs latex -l en -t esp32 build

Or alternatively build both html and PDF::

    ./build_docs.py -bs html latex -l en -t esp32 build

Latex files and the PDF will be placed in ``_build/<language>/<target>/latex`` folder.

Wrap up
-------

We love good code that is doing cool things.
We love it even better, if it is well documented, so we can quickly make it run and also do the cool things.

Go ahead, contribute your code and documentation!


Related Documents
-----------------

* :doc:`../api-reference/template`
* :doc:`add-ons-reference`


.. _espressif/esp-idf: https://github.com/espressif/esp-idf/

.. _interactive shell: http://interactive.blockdiag.com/?compression=deflate&src=eJxlUMFOwzAMvecrrO3aITYQQirlAIIzEseJQ5q4TUSIq8TVGIh_J2m7jbKc7Ge_5_dSO1Lv2soWvoVYgieNoMh7VGzJR9FJtugZ7lYQ0UcKEbYNOY36rRQHZHUPT68vV5tceGLbWCUzPfeaFFMoBZzecVc56vWwJFnWMmJ59CCZg617xpOFbTSyw0pmvT_HJ7hxtFNGBr6wvuu5SCkchcrZ1vAeXZomznh5YgTqfcpR02cBO6vZVDeXBRjMjKEcFRbLh8f18-Z2UUBDnqP9wmp9ncRmSSfND2ldGo2h_zse407g0Mxc1q7HzJ3-4jzYYTJjtQH3iSV-fgFzx50J

.. _Sphinx:  https://www.sphinx-doc.org/
.. _Breathe: https://breathe.readthedocs.io
