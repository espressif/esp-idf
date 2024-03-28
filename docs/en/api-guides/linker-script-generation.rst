Linker Script Generation
========================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

There are several :ref:`memory regions <memory-layout>` where code and data can be placed. Code and read-only data are placed by default in flash, writable data in RAM, etc. However, it is sometimes necessary to change these default placements.

For example, it may be necessary to place:

.. list::

    * critical code in RAM for performance reasons.
    * executable code in IRAM so that it can be ran while cache is disabled.
    :SOC_RTC_MEM_SUPPORTED: * code in RTC memory for use in a wake stub.
    :SOC_ULP_SUPPORTED: * code in RTC memory for use by the ULP coprocessor.

With the linker script generation mechanism, it is possible to specify these placements at the component level within ESP-IDF. The component presents information on how it would like to place its symbols, objects or the entire archive. During build, the information presented by the components are collected, parsed and processed; and the placement rules generated is used to link the app.

Quick Start
------------

This section presents a guide for quickly placing code/data to RAM and RTC memory - placements ESP-IDF provides out-of-the-box.

For this guide, suppose we have the following::

    components
    └── my_component
        ├── CMakeLists.txt
        ├── Kconfig
        ├── src/
        │   ├── my_src1.c
        │   ├── my_src2.c
        │   └── my_src3.c
        └── my_linker_fragment_file.lf

- a component named ``my_component`` that is archived as library ``libmy_component.a`` during build
- three source files archived under the library, ``my_src1.c``, ``my_src2.c`` and ``my_src3.c`` which are compiled as ``my_src1.o``, ``my_src2.o`` and ``my_src3.o``, respectively
- under ``my_src1.o``, the function ``my_function1`` is defined; under ``my_src2.o``, the function ``my_function2`` is defined
- there is bool-type config ``PERFORMANCE_MODE`` (y/n) and int type config ``PERFORMANCE_LEVEL`` (with range 0-3) in ``my_component``'s Kconfig

Creating and Specifying a Linker Fragment File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before anything else, a linker fragment file needs to be created. A linker fragment file is simply a text file with a ``.lf`` extension upon which the desired placements will be written. After creating the file, it is then necessary to present it to the build system. The instructions for the build systems supported by ESP-IDF are as follows:

In the component's ``CMakeLists.txt`` file, specify argument ``LDFRAGMENTS`` in the ``idf_component_register`` call. The value of ``LDFRAGMENTS`` can either be an absolute path or a relative path from the component directory to the created linker fragment file.

.. code-block:: cmake

    # file paths relative to CMakeLists.txt
    idf_component_register(...
                           LDFRAGMENTS "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf"
                           ...
                           )


Specifying Placements
^^^^^^^^^^^^^^^^^^^^^

It is possible to specify placements at the following levels of granularity:

    - object file (``.obj`` or ``.o`` files)
    - symbol (function/variable)
    - archive (``.a`` files)

.. _ldgen-placing-object-files :

Placing Object Files
""""""""""""""""""""

Suppose the entirety of ``my_src1.o`` is performance-critical, so it is desirable to place it in RAM. On the other hand, the entirety of ``my_src2.o`` contains symbols needed coming out of deep sleep, so it needs to be put under RTC memory.

In the linker fragment file, we can write:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        my_src1 (noflash)     # places all my_src1 code/read-only data under IRAM/DRAM
        my_src2 (rtc)         # places all my_src2 code/ data and read-only data under RTC fast memory/RTC slow memory

What happens to ``my_src3.o``? Since it is not specified, default placements are used for ``my_src3.o``. More on default placements :ref:`here <ldgen-default-placements>`.

Placing Symbols
""""""""""""""""

Continuing our example, suppose that among functions defined under ``object1.o``, only ``my_function1`` is performance-critical; and under ``object2.o``, only ``my_function2`` needs to execute after the chip comes out of deep sleep. This could be accomplished by writing:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        my_src1:my_function1 (noflash)
        my_src2:my_function2 (rtc)

The default placements are used for the rest of the functions in ``my_src1.o`` and ``my_src2.o`` and the entire ``object3.o``. Something similar can be achieved for placing data by writing the variable name instead of the function name, like so::

       my_src1:my_variable (noflash)

.. warning::

    There are :ref:`limitations<ldgen-symbol-granularity-placements>` in placing code/data at symbol granularity. In order to ensure proper placements, an alternative would be to group relevant code and data into source files, and :ref:`use object-granularity placements<ldgen-placing-object-files>`.

Placing Entire Archive
"""""""""""""""""""""""

In this example, suppose that the entire component archive needs to be placed in RAM. This can be written as:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        * (noflash)

Similarly, this places the entire component in RTC memory:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        * (rtc)


.. _ldgen-conditional-placements:

Configuration-Dependent Placements
""""""""""""""""""""""""""""""""""

Suppose that the entire component library should only have special placement when a certain condition is true; for example, when ``CONFIG_PERFORMANCE_MODE == y``. This could be written as:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_MODE = y:
            * (noflash)
        else:
            * (default)

For a more complex config-dependent placement, suppose the following requirements: when ``CONFIG_PERFORMANCE_LEVEL == 1``, only ``object1.o`` is put in RAM; when ``CONFIG_PERFORMANCE_LEVEL == 2``, ``object1.o`` and ``object2.o``; and when ``CONFIG_PERFORMANCE_LEVEL == 3`` all object files under the archive are to be put into RAM. When these three are false however, put entire library in RTC memory. This scenario is a bit contrived, but, it can be written as:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_LEVEL = 1:
            my_src1 (noflash)
        elif PERFORMANCE_LEVEL = 2:
            my_src1 (noflash)
            my_src2 (noflash)
        elif PERFORMANCE_LEVEL = 3:
            my_src1 (noflash)
            my_src2 (noflash)
            my_src3 (noflash)
        else:
            * (rtc)

Nesting condition-checking is also possible. The following is equivalent to the snippet above:

.. code-block:: none

    [mapping:my_component]
    archive: libmy_component.a
    entries:
        if PERFORMANCE_LEVEL <= 3 && PERFORMANCE_LEVEL > 0:
            if PERFORMANCE_LEVEL >= 1:
                object1 (noflash)
                if PERFORMANCE_LEVEL >= 2:
                    object2 (noflash)
                    if PERFORMANCE_LEVEL >= 3:
                        object2 (noflash)
        else:
            * (rtc)

.. _ldgen-default-placements:

The 'default' Placements
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Up until this point, the term  'default placements' has been mentioned as fallback placements when the placement rules ``rtc`` and ``noflash`` are not specified. It is important to note that the tokens ``noflash`` or ``rtc`` are not merely keywords, but are actually entities called fragments, specifically :ref:`schemes <ldgen-scheme-fragment>`.

In the same manner as ``rtc`` and ``noflash`` are schemes, there exists a ``default`` scheme which defines what the default placement rules should be. As the name suggests, it is where code and data are usually placed, i.e., code/constants is placed in flash, variables placed in RAM, etc.  More on the default scheme :ref:`here <ldgen-default-scheme>`.

.. note::

    For an example of an ESP-IDF component using the linker script generation mechanism, see :component_file:`freertos/CMakeLists.txt`. ``freertos`` uses this to place its object files to the instruction RAM for performance reasons.

This marks the end of the quick start guide. The following text discusses the internals of the mechanism in a little bit more detail. The following sections should be helpful in creating custom placements or modifying default behavior.

Linker Script Generation Internals
----------------------------------

Linking is the last step in the process of turning C/C++ source files into an executable. It is performed by the toolchain's linker, and accepts linker scripts which specify code/data placements, among other things. With the linker script generation mechanism, this process is no different, except that the linker script passed to the linker is dynamically generated from: (1) the collected :ref:`linker fragment files <ldgen-linker-fragment-files>` and (2) :ref:`linker script template <ldgen-linker-script-template>`.

.. note::

    The tool that implements the linker script generation mechanism lives under :idf:`tools/ldgen`.

.. _ldgen-linker-fragment-files :

Linker Fragment Files
^^^^^^^^^^^^^^^^^^^^^

As mentioned in the quick start guide, fragment files are simple text files with the ``.lf`` extension containing the desired placements. This is a simplified description of what fragment files contain, however. What fragment files actually contain are 'fragments'. Fragments are entities which contain pieces of information which, when put together, form placement rules that tell where to place sections of object files in the output binary. There are three types of fragments: :ref:`sections <ldgen-sections-fragment>`, :ref:`scheme <ldgen-scheme-fragment>` and :ref:`mapping <ldgen-mapping-fragment>`.

Grammar
"""""""

The three fragment types share a common grammar:

.. code-block:: none

    [type:name]
    key: value
    key:
        value
        value
        value
        ...

- type: Corresponds to the fragment type, can either be ``sections``, ``scheme`` or ``mapping``.
- name: The name of the fragment, should be unique for the specified fragment type.
- key, value: Contents of the fragment; each fragment type may support different keys and different grammars for the key values.

    - For :ref:`sections <ldgen-sections-fragment>` and :ref:`scheme <ldgen-scheme-fragment>`, the only supported key is ``entries``
    - For :ref:`mappings <ldgen-mapping-fragment>`, both ``archive`` and ``entries`` are supported.

.. note::

    In cases where multiple fragments of the same type and name are encountered, an exception is thrown.

.. note::

    The only valid characters for fragment names and keys are alphanumeric characters and underscore.

.. _ldgen-condition-checking :

**Condition Checking**

Condition checking enable the linker script generation to be configuration-aware. Depending on whether expressions involving configuration values are true or not, a particular set of values for a key can be used. The evaluation uses ``eval_string`` from kconfiglib package and adheres to its required syntax and limitations. Supported operators are as follows:

    - comparison
        - LessThan ``<``
        - LessThanOrEqualTo ``<=``
        - MoreThan ``>``
        - MoreThanOrEqualTo ``>=``
        - Equal ``=``
        - NotEqual ``!=``
    - logical
        - Or ``||``
        - And ``&&``
        - Negation ``!``
    - grouping
        - Parenthesis ``()``

Condition checking behaves as you would expect an ``if...elseif/elif...else`` block in other languages. Condition-checking is possible for both key values and entire fragments. The two sample fragments below are equivalent:

.. code-block:: none

    # Value for keys is dependent on config
    [type:name]
    key_1:
        if CONDITION = y:
            value_1
        else:
            value_2
    key_2:
        if CONDITION = y:
            value_a
        else:
            value_b

.. code-block:: none

    # Entire fragment definition is dependent on config
    if CONDITION = y:
        [type:name]
        key_1:
            value_1
        key_2:
            value_a
    else:
        [type:name]
        key_1:
            value_2
        key_2:
            value_b

**Comments**

Comment in linker fragment files begin with ``#``. Like in other languages, comment are used to provide helpful descriptions and documentation and are ignored during processing.

Types
"""""

.. _ldgen-sections-fragment :

**Sections**

Sections fragments defines a list of object file sections that the GCC compiler emits. It may be a default section (e.g., ``.text``, ``.data``) or it may be user defined section through the ``__attribute__`` keyword.

The use of an optional '+' indicates the inclusion of the section in the list, as well as sections that start with it. This is the preferred method over listing both explicitly.

.. code-block:: none

    [sections:name]
    entries:
        .section+
        .section
        ...

Example:

.. code-block:: none

    # Non-preferred
    [sections:text]
    entries:
        .text
        .text.*
        .literal
        .literal.*

    # Preferred, equivalent to the one above
    [sections:text]
    entries:
        .text+              # means .text and .text.*
        .literal+           # means .literal and .literal.*

.. _ldgen-scheme-fragment :

**Scheme**

Scheme fragments define what ``target`` a sections fragment is assigned to.

.. code-block:: none

    [scheme:name]
    entries:
        sections -> target
        sections -> target
        ...

Example:

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text          # the entries under the sections fragment named text will go to iram0_text
        rodata -> dram0_data        # the entries under the sections fragment named rodata will go to dram0_data

.. _ldgen-default-scheme:

The ``default`` scheme

There exists a special scheme with the name ``default``. This scheme is special because catch-all placement rules are generated from its entries. This means that, if one of its entries is ``text -> flash_text``, the placement rule will be generated for the target ``flash_text``.

.. code-block:: none

    *(.literal .literal.* .text .text.*)

These catch-all rules then effectively serve as fallback rules for those whose mappings were not specified.


The ``default scheme`` is defined in :component_file:`esp_system/app.lf`. The ``noflash`` and ``rtc`` scheme fragments which are
built-in schemes referenced in the quick start guide are also defined in this file.


.. _ldgen-mapping-fragment :

**Mapping**

Mapping fragments define what scheme fragment to use for mappable entities, i.e., object files, function names, variable names, archives.

.. code-block:: none

    [mapping:name]
    archive: archive                # output archive file name, as built (i.e., libxxx.a)
    entries:
        object:symbol (scheme)      # symbol granularity
        object (scheme)             # object granularity
        * (scheme)                  # archive granularity

There are three levels of placement granularity:

    - symbol: The object file name and symbol name are specified. The symbol name can be a function name or a variable name.
    - object: Only the object file name is specified.
    - archive: ``*`` is specified, which is a short-hand for all the object files under the archive.

To know what an entry means, let us expand a sample object-granularity placement:

.. code-block:: none

    object (scheme)

Then expanding the scheme fragment from its entries definitions, we have:

.. code-block:: none

    object (sections -> target,
            sections -> target,
            ...)

Expanding the sections fragment with its entries definition:

.. code-block:: none

    object (.section,      # given this object file
            .section,      # put its sections listed here at this
            ... -> target, # target

            .section,
            .section,      # same should be done for these sections
            ... -> target,

            ...)           # and so on

Example:

.. code-block:: none

    [mapping:map]
    archive: libfreertos.a
    entries:
        * (noflash)

Aside from the entity and scheme, flags can also be specified in an entry. The following flags are supported (note: <> = argument name, [] = optional):

1. ALIGN(<alignment>[, pre, post])

    Align the placement by the amount specified in ``alignment``. Generates

.. code-block::none

    . = ALIGN(<alignment>)

    before and/or after (depending whether ``pre``, ``post`` or both are specified) the input section description generated from the mapping fragment entry. If neither 'pre' or 'post' is specified, the alignment command is generated before the input section description. Order sensitive.

2. SORT([<sort_by_first>, <sort_by_second>])

    Emits ``SORT_BY_NAME``, ``SORT_BY_ALIGNMENT``, ``SORT_BY_INIT_PRIORITY`` or ``SORT`` in the input section description.

    Possible values for ``sort_by_first`` and ``sort_by_second`` are: ``name``, ``alignment``, ``init_priority``.

    If both ``sort_by_first`` and ``sort_by_second`` are not specified, the input sections are sorted by name. If both are specified, then the nested sorting follows the same rules discussed in https://sourceware.org/binutils/docs/ld/Input-Section-Wildcards.html.

3. KEEP()

    Prevent the linker from discarding the placement by surrounding the input section description with KEEP command. See https://sourceware.org/binutils/docs/ld/Input-Section-Keep.html for more details.

4.SURROUND(<name>)

    Generate symbols before and after the placement. The generated symbols follow the naming ``_<name>_start`` and ``_<name>_end``. For example, if ``name`` == sym1,

.. code-block::none

    _sym1_start = ABSOLUTE(.)
    ...
    _sym2_end = ABSOLUTE(.)

    These symbols can then be referenced from C/C++ code. Order sensitive.

When adding flags, the specific ``section -> target`` in the scheme needs to be specified. For multiple ``section -> target``, use a comma as a separator. For example,

.. code-block:: none

    # Notes:
    # A. semicolon after entity-scheme
    # B. comma before section2 -> target2
    # C. section1 -> target1 and section2 -> target2 should be defined in entries of scheme1
    entity1 (scheme1);
        section1 -> target1 KEEP() ALIGN(4, pre, post),
        section2 -> target2 SURROUND(sym) ALIGN(4, post) SORT()

Putting it all together, the following mapping fragment, for example,

.. code-block:: none

    [mapping:name]
    archive: lib1.a
    entries:
        obj1 (noflash);
            rodata -> dram0_data KEEP() SORT() ALIGN(8) SURROUND(my_sym)

generates an output on the linker script:

.. code-block:: none

    . = ALIGN(8)
    _my_sym_start = ABSOLUTE(.)
    KEEP(lib1.a:obj1.*( SORT(.rodata) SORT(.rodata.*) ))
    _my_sym_end = ABSOLUTE(.)

Note that ALIGN and SURROUND, as mentioned in the flag descriptions, are order sensitive. Therefore, if for the same mapping fragment these two are switched, the following is generated instead:

.. code-block:: none

    _my_sym_start = ABSOLUTE(.)
    . = ALIGN(8)
    KEEP(lib1.a:obj1.*( SORT(.rodata) SORT(.rodata.*) ))
    _my_sym_end = ABSOLUTE(.)

.. _ldgen-symbol-granularity-placements :

On Symbol-Granularity Placements
""""""""""""""""""""""""""""""""

Symbol granularity placements is possible due to compiler flags ``-ffunction-sections`` and ``-ffdata-sections``. ESP-IDF compiles with these flags by default.
If the user opts to remove these flags, then the symbol-granularity placements will not work. Furthermore, even with the presence of these flags, there are still other limitations to keep in mind due to the dependence on the compiler's emitted output sections.

For example, with ``-ffunction-sections``, separate sections are emitted for each function; with section names predictably constructed i.e., ``.text.{func_name}`` and ``.literal.{func_name}``. This is not the case for string literals within the function, as they go to pooled or generated section names.

With ``-fdata-sections``, for global scope data the compiler predictably emits either ``.data.{var_name}``, ``.rodata.{var_name}`` or ``.bss.{var_name}``; and so ``Type I`` mapping entry works for these.
However, this is not the case for static data declared in function scope, as the generated section name is a result of mangling the variable name with some other information.

.. _ldgen-linker-script-template :

Linker Script Template
^^^^^^^^^^^^^^^^^^^^^^

The linker script template is the skeleton in which the generated placement rules are put into. It is an otherwise ordinary linker script, with a specific marker syntax that indicates where the generated placement rules are placed.

To reference the placement rules collected under a ``target`` token, the following syntax is used:

.. only:: SOC_MEM_NON_CONTIGUOUS_SRAM

    .. code-block:: none

        arrays[target]      /* refers to objects under the SURROUND keyword */
        mapping[target]     /* refers to all other data */

.. only:: not SOC_MEM_NON_CONTIGUOUS_SRAM

    .. code-block:: none

        mapping[target]

Example:

The example below is an excerpt from a possible linker script template. It defines an output section ``.iram0.text``, and inside is a marker referencing the target ``iram0_text``.

.. only:: SOC_MEM_NON_CONTIGUOUS_SRAM

    .. code-block:: none

        .iram0.text :
        {
            /* Code marked as running out of IRAM */
            _iram_text_start = ABSOLUTE(.);

            /* Markers referencing iram0_text */
            arrays[iram0_text]
            mapping[iram0_text]

            _iram_text_end = ABSOLUTE(.);
        } > iram0_0_seg

.. only:: not SOC_MEM_NON_CONTIGUOUS_SRAM

    .. code-block:: none

        .iram0.text :
        {
            /* Code marked as running out of IRAM */
            _iram_text_start = ABSOLUTE(.);

            /* Marker referencing iram0_text */
            mapping[iram0_text]

            _iram_text_end = ABSOLUTE(.);
        } > iram0_0_seg

Suppose the generator collected the fragment definitions below:

.. code-block:: none

    [sections:text]
        .text+
        .literal+

    [sections:iram]
        .iram1+

    [scheme:default]
    entries:
        text -> flash_text
        iram -> iram0_text

    [scheme:noflash]
    entries:
        text -> iram0_text

    [mapping:freertos]
    archive: libfreertos.a
    entries:
        * (noflash)

Then the corresponding excerpt from the generated linker script will be as follows:

.. code-block:: c

    .iram0.text :
    {
        /* Code marked as running out of IRAM */
        _iram_text_start = ABSOLUTE(.);

        /* Placement rules generated from the processed fragments, placed where the marker was in the template */
        *(.iram1 .iram1.*)
        *libfreertos.a:(.literal .text .literal.* .text.*)

        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

``*libfreertos.a:(.literal .text .literal.* .text.*)``

    Rule generated from the entry ``* (noflash)`` of the ``freertos`` mapping fragment. All ``text`` sections of all object files under the archive ``libfreertos.a`` will be collected under the target ``iram0_text`` (as per the ``noflash`` scheme) and placed wherever in the template ``iram0_text`` is referenced by a marker.

``*(.iram1 .iram1.*)``

    Rule generated from the default scheme entry ``iram -> iram0_text``. Since the default scheme specifies an ``iram -> iram0_text`` entry, it too is placed wherever ``iram0_text`` is referenced by a marker. Since it is a rule generated from the default scheme, it comes first among all other rules collected under the same target name.

.. only:: not esp32c5

    The linker script template currently used is :component_file:`esp_system/ld/{IDF_TARGET_PATH_NAME}/sections.ld.in`; the generated output script ``sections.ld`` is put under its build directory.

.. only:: esp32c5

    The linker script template currently used is :component_file:`esp_system/ld/esp32c5/beta3/sections.ld.in`; the generated output script ``sections.ld`` is put under its build directory.
