API Documentation Template 
==========================

.. note::

    *INSTRUCTIONS*

    1. Use this file (:idf_file:`docs/en/api-reference/template.rst`) as a template to document API.
    2. Change the file name to the name of the header file that represents documented API.
    3. Include respective files with descriptions from the API folder using ``..include::``

        * README.rst
        * example.rst
        * ...

    4. Optionally provide description right in this file.
    5. Once done, remove all instructions like this one and any superfluous headers.

Overview
--------

.. note::

    *INSTRUCTIONS*

    1. Provide overview where and how this API may be used. 
    2. Where applicable include code snippets to illustrate functionality of particular functions.
    3. To distinguish between sections, use the following `heading levels <http://www.sphinx-doc.org/en/stable/rest.html#sections>`_:

        * ``#`` with overline, for parts
        * ``*`` with overline, for chapters
        * ``=``, for sections
        * ``-``, for subsections
        * ``^``, for subsubsections
        * ``"``, for paragraphs

Application Example
-------------------

.. note::

    *INSTRUCTIONS*

    1. Prepare one or more practical examples to demonstrate functionality of this API.
    2. Each example should follow pattern of projects located in ``esp-idf/examples/`` folder.
    3. Place example in this folder complete with ``README.md`` file.
    4. Provide overview of demonstrated functionality in ``README.md``.
    5. With good overview reader should be able to understand what example does without opening the source code.
    6. Depending on complexity of example, break down description of code into parts and provide overview of functionality of each part.
    7. Include flow diagram and screenshots of application output if applicable.
    8. Finally add in this section synopsis of each example together with link to respective folder in ``esp-idf/examples/``.
  
API Reference
-------------

.. highlight:: none

.. note::

    *INSTRUCTIONS*

    1. This repository provides for automatic update of API reference documentation using :doc:`code markup retrieved by Doxygen from header files <../contribute/documenting-code>`.
    
    1. Update is done on each documentation build by invoking Sphinx extension :idf_file:`docs/idf_extensions/run_doxygen.py` for all header files listed in the ``INPUT`` statement of :idf_file:`docs/Doxyfile`. 

    1. Each line of the ``INPUT`` statement  (other than a comment that begins with ``##``) contains a path to header file ``*.h`` that will be used to generate corresponding ``*.inc`` files::

        ##
        ## Wi-Fi - API Reference
        ##
        ../components/esp32/include/esp_wifi.h \
        ../components/esp32/include/esp_smartconfig.h \

    1. When the headers are expanded, any macros defined by default in ``sdkconfig.h`` as well as any macros defined in SOC-specific ``include/soc/*_caps.h`` headers will be expanded. This allows the headers to include/exclude material based on the ``IDF_TARGET`` value.

    1. The ``*.inc`` files contain formatted reference of API members generated automatically on each documentation build. All ``*.inc`` files are placed in Sphinx ``_build`` directory. To see directives generated for e.g. ``esp_wifi.h``, run ``python gen-dxd.py esp32/include/esp_wifi.h``.

    1. To show contents of ``*.inc`` file in documentation, include it as follows::

       .. include-build-file:: inc/esp_wifi.inc

       For example see :idf_file:`docs/en/api-reference/network/esp_wifi.rst`
    
    1. Optionally, rather that using ``*.inc`` files, you may want to describe API in you own way. See :idf_file:`docs/en/api-guides/ulp.rst` for example.

       Below is the list of common ``.. doxygen...::`` directives:

        * Functions - ``.. doxygenfunction:: name_of_function``
        * Unions -``.. doxygenunion:: name_of_union``
        * Structures -``.. doxygenstruct:: name_of_structure`` together with ``:members:``
        * Macros - ``.. doxygendefine:: name_of_define``
        * Type Definitions - ``.. doxygentypedef:: name_of_type``
        * Enumerations - ``.. doxygenenum:: name_of_enumeration``

       See `Breathe documentation <https://breathe.readthedocs.io/en/latest/directives.html>`_ for additional information.

       To provide a link to header file, use the :ref:`link custom role <link-custom-roles>` as follows::

       * :component_file:`path_to/header_file.h`

    1. In any case, to generate API reference, the file :idf_file:`docs/Doxyfile` should be updated with paths to ``*.h`` headers that are being documented.
    
    1. When changes are committed and documentation is build, check how this section has been rendered. :doc:`Correct annotations <../contribute/documenting-code>` in respective header files, if required.
