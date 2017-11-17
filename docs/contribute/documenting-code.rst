Documenting Code
================

The purpose of this description is to provide quick summary on documentation style used in `espressif/esp-idf`_ repository and how to add new documentation. 


Introduction
------------

When documenting code for this repository, please follow `Doxygen style <https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#specialblock>`_. You are doing it by inserting special commands, for instance ``@param``, into standard comments blocks, for example: ::

    /**
     * @param ratio this is oxygen to air ratio
     */

Doxygen is phrasing the code, extracting the commands together with subsequent text, and building documentation out of it.

Typical comment block, that contains documentation of a function, looks like below.

.. image:: ../_static/doc-code-documentation-inline.png
    :align: center
    :alt: Sample inline code documentation
 
Doxygen supports couple of formatting styles. It also gives you great flexibility on level of details to include in documentation. To get familiar with available features, please check data reach and very well organized `Doxygen Manual <https://www.stack.nl/~dimitri/doxygen/manual/index.html>`_.


Why we need it?
---------------

The ultimate goal is to ensure that all the code is consistently documented, so we can use tools like `Sphinx <http://www.sphinx-doc.org/>`_ and `Breathe <https://breathe.readthedocs.io/>`_ to aid preparation and automatic updates of API documentation when the code changes. 

With these tools the above piece of code renders like below:

.. image:: ../_static/doc-code-documentation-rendered.png
    :align: center
    :alt: Sample inline code after rendering


Go for it!
----------

When writing code for this repository, please follow guidelines below.

1. Document all building blocks of code: functions, structs, typedefs, enums, macros, etc. Provide enough information on purpose, functionality and limitations of documented items, as you would like to see them documented when reading the code by others.

2. Documentation of function should describe what this function does. If it accepts input parameters and returns some value, all of them should be explained.

3. Do not add a data type before parameter or any other characters besides spaces. All spaces and line breaks are compressed into a single space. If you like to break a line, then break it twice.

    .. image:: ../_static/doc-code-function.png
        :align: center
        :alt: Sample function documented inline and after rendering

4. If function has void input or does not return any value, then skip ``@param`` or ``@return``

    .. image:: ../_static/doc-code-void-function.png
        :align: center
        :alt: Sample void function documented inline and after rendering
 
5. When documenting a ``define`` as well as members of a ``struct`` or ``enum``, place specific comment like below after each member.

    .. image:: ../_static/doc-code-member.png
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
    * [ESP32 Technical Reference](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
    *

.. note::

    Code snippets, notes, links, etc. will not make it to the documentation, if not enclosed in a comment block associated with one of documented objects.

6. Prepare one or more complete code examples together with description. Place description in a separate file ``README.md`` in specific folder of :idf:`examples` directory. 

.. _link-custom-roles:

Linking Examples
----------------

When linking to examples on GitHub do not use absolute / hadcoded URLs. Instead, use docutils custom roles that will generate links for you. These auto-generated links point to the tree or blob for the git commit ID (or tag) of the repository. This is needed to ensure that links do not get broken when files in master branch are moved around or deleted.

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

A check is added to the CI build script, which searches RST files for presence of hard-coded links (identified by tree/master, blob/master, or raw/master part of the URL). This check can be run manually: ``cd docs`` and then ``make gh-linkcheck``.


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

For the diagram preparation you can use an on-line `interactive shell <http://interactive.blockdiag.com/?compression=deflate&src=eJxlUMFOwzAMvecrrO3aITYQQirlAIIzEseJQ5q4TUSIq8TVGIh_J2m7jbKc7Ge_5_dSO1Lv2soWvoVYgieNoMh7VGzJR9FJtugZ7lYQ0UcKEbYNOY36rRQHZHUPT68vV5tceGLbWCUzPfeaFFMoBZzecVc56vWwJFnWMmJ59CCZg617xpOFbTSyw0pmvT_HJ7hxtFNGBr6wvuu5SCkchcrZ1vAeXZomznh5YgTqfcpR02cBO6vZVDeXBRjMjKEcFRbLh8f18-Z2UUBDnqP9wmp9ncRmSSfND2ldGo2h_zse407g0Mxc1q7HzJ3-4jzYYTJjtQH3iSV-fgFzx50J>`_ that instantly shows the rendered image. 

Below are couple of diagram examples:

* Simple **block diagram** / ``blockdiag`` - `Wi-Fi Buffer Configuration <http://interactive.blockdiag.com/?compression=deflate&src=eJylUk1rwkAQvfsrBntpIUKiRQqSgK0VSj0EtCi0EjbJxCyuuyG7QW3pf-9m06hJeyg0t33zmHkfCZmItjElGwiLJME8IEwjRFHBA3WAj04H9HcFGyZCwoAoldOwUCgNzkWMwZ7GKgUXnKE9gjOcIt2kSuN39sigMiP8jDqX6GmF_Y3GmJCCqUCmJEM9yEXBY4xDcWjOE8GVpO9oztdaGQmRSRAJlMZysjOCKsVj358Fi_H8GV4Nze2Os4zRyvEbB0XktrseQWVktn_ym-wS-UFb0ilt0pa0N6Vn3i_KUEY5zcqrbXWTx_nDaZHjwYvEHGKiSNeC2q_r3FpQZekObAtMTi4XCi2IBBO5e0Rd5L7ppLG574GvO__PUuO7sXTgweTIyY5GcD1XOtToBhYruDf_VvuUad3tD-0_Xq1TLPPSI84xKvNrF9vzLnrTj1M7rYhrXv24cCPVkZUaOK47n1-lOvbk>`_
* Slightly more complicated **block diagram** - `Wi-Fi programming model <http://interactive.blockdiag.com/?compression=deflate&src=eJyFk09P40AMxe98CqscIVILq72UIFX8kSoQWy0RHABFTuImFtOZaGYKuyC-O840bagaRI7Pfs7Pz0mmTP5cMJbwynNOa2tKi4sF6zJdmIIUvO_tgTz7UCqToQL03nK29OSCrqUpfeXCVxDD6Gg47tSKuKy8yL9b1dWov1E3E4atWtAcl8qnrsKapGDNUhdUZObfdr2UQp3mRhkrXdpoGq-BGwhQmJFaoSZns_Q2mZxdwUNQ44Eojxqcx_x5cAhzo73jN4pHv55WL7m4u0nSZHLbOeiFtBePR9dvmcxm19sWrGvFOXo2utd4CGH5eHQ8bGfcTy-n6fnfO9jMuOfoksV9bvmFbO-Lr27-JPAQ4oqbGJ62c8iN1pQ3EA4O-lOJTncXDvvupCGdu3vmqFQmSQqm3CIYBx0EWou6pADjQJbw3Bj-h3I4onxpsHrCQLnmoD0yVKgLJXuP1x3GsowPmUpfbay3yH5T7khPoi7NnpU-1nisPdkFyY_gV4x9XB3Y0pHdpfoJ60toURQOtqbYuvpJ1B6zDXYym0qmTVpNnh-fpWcbRA>`_
* **Sequence diagram** / ``seqdiag`` - `Scan for a Specific AP in All Channels <http://interactive.blockdiag.com/seqdiag/?compression=deflate&src=eJyVkU1PwzAMhu_7FdburUgQXMomTaPcKIdOIIRQlDVuG1EloUknPsR_J2s2rRsT2nKJ9drvY8ex-C4kr8AWXLFSt8waLBg38D0Cf3jh5Io7qRVMQGmFSS-jqJA1qCpXe51cXwTZGg-pUVa1W8tXQRVY8q5xzNbcoNdb3SmBYqk_9vOlVs7Kr3UJoQmMwgDGMMftWwK4QuU28ZOM7uQm3q_zYTQd5OGl4UtsJmMSE5jCXKtSVl2LUPgpXPvpb4Hj1-RUCPWQ3O_K-wKpX84WMLAcB9B-igCouVLYADnDTA_N9GRzHMdnNMoOG2Vb8-4b4CY6Zr4MT3zOF-k9Sx_TbMHy-Sxjtw9Z-mfRHjEA7hD0X8TPLxU91AQ>`_
* **Packet diagram** / ``packetdiag`` - `NVS Page Structure <http://interactive.blockdiag.com/packetdiag/?compression=deflate&src=eJxFkMFOwzAQRO_9ij2mh63idRKaSj1V_ACIE6DIxG4StTgh3oCg6r_j2JTs8c3szNqDqk-GdacasJ-uGlRjKsfjVPM0GriswE_dn786zS3sQRJAYLbXprpRkS-sNV3TcrAGqM1RTWeujr1l1_2Y2U6rIKUod_DIis2LTbJ1YBneeWY-Nj5ts-AtkudPdnJGQ0JppLRFKXZweDhIWrySsPDB95bHb3BzPLx1_K4GSCSt_-4vMizzmykNSuBlgWKuioJYBOHLROnbEBGe_ZfEh-7pNcolIdF_raA8rl5_AaqqWyE>`_

Try them out by modifying the source code and see the diagram instantly rendering below.

.. note::

    There may be slight differences in rendering of font used by the interactive shell compared to the font used in the esp-idf documentation.


Put it all together
-------------------

Once documentation is ready, follow instruction in :doc:`../api-reference/template` and create a single file, that will merge all individual pieces of prepared documentation. Finally add a link to this file to respective ``.. toctree::`` in ``index.rst`` file located in ``/docs`` folder or subfolders.


OK, but I am new to Sphinx!
---------------------------

1. No worries. All the software you need is well documented. It is also open source and free. Start by checking `Sphinx <http://www.sphinx-doc.org/>`_ documentation. If you are not clear how to write using rst markup language, see `reStructuredText Primer <http://www.sphinx-doc.org/en/stable/rest.html>`_.

2. Check the source files of this documentation to understand what is behind of what you see now on the screen. Sources are maintained on GitHub in `espressif/esp-idf`_ repository in :idf:`docs` folder. You can go directly to the source file of this page by scrolling up and clicking the link in the top right corner. When on GitHub, see what's really inside, open source files by clicking ``Raw`` button.

3. You will likely want to see how documentation builds and looks like before posting it on the GitHub. There are two options to do so:

    * Install `Sphinx <http://www.sphinx-doc.org/>`_, `Breathe <https://breathe.readthedocs.io/>`_, `Blockdiag <http://blockdiag.com/en/index.html>`_ and `Doxygen <https://www.stack.nl/~dimitri/doxygen/>`_ to build it locally, see chapter below.
   
    * Set up an account on `Read the Docs <https://readthedocs.org/>`_ and build documentation in the cloud. Read the Docs provides document building and hosting for free and their service works really quick and great.

4. To preview documentation before building use `Sublime Text <https://www.sublimetext.com/>`_ editor together with `OmniMarkupPreviewer <https://github.com/timonwong/OmniMarkupPreviewer>`_ plugin. 


Setup for building documentation locally
----------------------------------------

You can setup environment to build documentation locally on your PC by installing:

1. Doxygen - https://www.stack.nl/~dimitri/doxygen/
2. Sphinx - https://github.com/sphinx-doc/sphinx/#readme-for-sphinx
3. Docment theme "sphinx_rtd_theme" - https://github.com/rtfd/sphinx_rtd_theme
4. Breathe - https://github.com/michaeljones/breathe#breathe
5. Blockdiag - http://blockdiag.com/en/index.html

The package "sphinx_rtd_theme" is added to have the same "look and feel" of `ESP32 Programming Guide <https://esp-idf.readthedocs.io/en/latest/index.html>`_ documentation like on the "Read the Docs" hosting site.

Do not worry about being confronted with several packages to install. Besides Doxygen, all remaining packages are written in Python. Therefore installation of all of them is combined into one simple step.

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

All remaining applications are `Python <https://www.python.org/>`_ packages and you can install them in one step as follows:

::

	cd ~/esp/esp-idf/docs
	pip install -r requirements.txt

.. note::

	Installation steps assume that ESP-IDF is placed in ``~/esp/esp-idf`` directory, that is default location of ESP-IDF used in documentation.

Now you should be ready to build documentation by invoking::

	make html

This may take couple of minutes. After completion, documentation will be placed in ``~/esp/esp-idf/docs/_buld/html`` folder. To see it, open ``index.html`` in a web browser.  


Wrap up
-------

We love good code that is doing cool things. 
We love it even better, if it is well documented, so we can quickly make it run and also do the cool things.

Go ahead, contribute your code and documentation!


Related Documents
-----------------

* :doc:`../api-reference/template`


.. _espressif/esp-idf: https://github.com/espressif/esp-idf/
