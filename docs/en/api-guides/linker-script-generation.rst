Linker Script Generation
========================

Overview
--------

There are several :ref:`memory regions<memory-layout>` where code and data can be placed. Usually, code and read-only data are placed in flash regions,
writable data in RAM, etc. A common action is changing where code/data are mapped by default, say placing critical code/rodata in RAM for performance
reasons or placing code/data/rodata in RTC memory for use in a wake stub or the ULP coprocessor.

IDF provides the ability for defining these placements at the component level using the linker script generation mechanism. The component presents
how it would like to map the input sections of its object files (or even functions/data) through :ref:`linker fragment files<ldgen-fragment-files>`. During app build, 
the linker fragment files are collected, parsed and processed; and the :ref:`linker script template<ldgen-script-templates>` is augmented with
information generated from the fragment files to produce the final linker script. This linker script is then used for the linking
the final app binary.

Quick Start
------------

This section presents a guide for quickly placing code/data to RAM and RTC memory; as well as demonstrating how to make these placements 
dependent on project configuration values. In a true quick start fashion, this section glosses over terms and concepts that will be discussed 
at a later part of the document. However, whenever it does so, it provides a link to the relevant section on the first mention.

.. _ldgen-add-fragment-file :

Preparation
^^^^^^^^^^^

Make
""""

Create a linker fragment file inside the component directory, which is just a text file with a .lf extension. In order for the build system to collect your fragment file,
add an entry to it from the component, set the variable ``COMPONENT_ADD_LDFRAGMENTS`` to your linker file/s before the ``register_component`` call.

.. code-block:: make

    # file paths relative to component Makefile 
    COMPONENT_ADD_LDFRAGMENTS += "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf"

CMake
"""""

For CMake set the variable ``COMPONENT_ADD_LDFRAGMENTS`` to your linker file/s before the ``register_component`` call.

.. code-block:: cmake

    # file paths relative to CMakeLists.txt  
    set(COMPONENT_ADD_LDFRAGMENTS "path/to/linker_fragment_file.lf" "path/to/another_linker_fragment_file.lf")

    register_component()


Specifying placements
^^^^^^^^^^^^^^^^^^^^^

This mechanism allows specifying placement of the following entities:

    - one or multiple object files within the component
    - one or multiple function/variable using their names
    - the entire component library

For the following text, suppose we have the following:

    - a component named ``component`` that is archived as library ``libcomponent.a`` during build
    - three object files archived under the library, ``object1.o``, ``object2.o`` and ``object3.o``
    - under ``object1.o``, the function ``function1`` is defined; under ``object2.o``, the function ``function2`` is defined
    - there exists configuration ``PERFORMANCE_MODE`` and ``PERFORMANCE_LEVEL`` in one of the IDF KConfig files, with the set value indicated by entries ``CONFIG_PERFORMANCE_MODE`` and ``CONFIG_PERFORMANCE_LEVEL`` in the project sdkconfig

In the created linker fragment file, we write:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:

This creates an empty :ref:`mapping fragment<ldgen-mapping-fragment>`, which doesn't do anything yet. During linking the :ref:`default placements<ldgen-default-placements>` 
will still be used for ``libcomponent.a``, unless the ``entries`` key is populated.

.. _ldgen-placing-object-files :

Placing object files
""""""""""""""""""""

Suppose the entirety of ``object1.o``  is performance-critical, so it is desirable to place it in RAM. On the other hand, all of ``object2.o``
contains things to be executed coming out of deep sleep, so it needs to be put under RTC memory. We can write:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        object1 (noflash)     # places all code / read-only data under IRAM/ DRAM
        object2 (rtc)         # places all code/ data and read-only data under RTC fast memory/ RTC slow memory

What happens to ``object3.o``? Since it is not specified, default placements are used for ``object3.o``.

Placing functions/data using their names
""""""""""""""""""""""""""""""""""""""""

Continuing our example, suppose that among functions defined under ``object1.o``, only ``function1`` is performance-critical; and under ``object2.o``,
only ``function2`` needs to execute after the chip comes out of deep sleep. This could be accomplished by writing:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        object1:function1 (noflash) 
        object2:function2 (rtc) 

The default placements are used for the rest of the functions in ``object1.o`` and ``object2.o`` and the entire ``object3.o``. Something similar
can be achieved for placing data by writing the variable name instead of the function name after ``:``.

.. warning::

    There are :ref:`limitations<ldgen-type3-limitations>` in placing code/data using their symbol names. In order to ensure proper placements, an alternative would be to group
    relevant code and data into source files, and :ref:`use object file placement<ldgen-placing-object-files>`.

Placing entire component
""""""""""""""""""""""""

In this example, suppose that the entire component needs to be placed in RAM. This can be written as:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        * (noflash)

Similarly, this places the entire component in RTC memory:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        * (rtc)

Configuration-dependent placements
""""""""""""""""""""""""""""""""""

Suppose that the entire component library should only be placed when ``CONFIG_PERFORMANCE_MODE == y`` in the sdkconfig. This could be written as:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        : PERFORMANCE_MODE = y
        * (noflash)

In pseudocode, this translates to:

.. code-block:: none

    if PERFORMANCE_MODE = y
        place entire libcomponent.a in RAM
    else
        use default placements

It is also possible to have multiple conditions to test. Suppose the following requirements: when ``CONFIG_PERFORMANCE_LEVEL == 1``, only ``object1.o`` is put in RAM;
when ``CONFIG_PERFORMANCE_LEVEL == 2``, ``object1.o`` and ``object2.o``; and when ``CONFIG_PERFORMANCE_LEVEL == 3`` all object files under the archive
are to be put into RAM. When these three are false however, put entire library in RTC memory. This scenario is a bit contrived, but, 
it can be written as:

.. code-block:: none

    [mapping]
    archive: libcomponent.a
    entries:
        : PERFORMANCE_LEVEL = 3
        * (noflash)
        : PERFORMANCE_LEVEL = 2
        object1 (noflash)
        object2 (noflash)
        : PERFORMANCE_LEVEL = 1
        object1 (noflash)
        : default
        * (rtc)

Which reads:

.. code-block:: none

    if CONFIG_PERFORMANCE_LEVEL == 3
        place entire libcomponent.a in RAM
    else if CONFIG_PERFORMANCE_LEVEL == 2
        only place object1.o and object2.o in RAM
    else if CONFIG_PERFORMANCE_LEVEL == 1
        only place object1.o in RAM
    else
        place entire libcomponent.a in RTC memory 

The conditions test :ref:`support other operations<ldgen-condition-entries>`.

.. _ldgen-default-placements:

The 'default' placements
^^^^^^^^^^^^^^^^^^^^^^^^

Up until this point, the term  'default placements' has been mentioned as fallback placements for when the 
placement rules ``rtc`` and ``noflash`` are not specified. The tokens ``noflash`` or ``rtc`` are not merely keywords known by the mechanism, but are actually 
objects called :ref:`scheme fragments<ldgen-scheme-fragment>` that are specified by the user. Due to the commonness of these placement use cases,
they are pre-defined in IDF.

Similarly, there exists a ``default`` scheme fragment which defines what the default placement rules should be, which is discussed :ref:`here<ldgen-default-scheme>`.

.. note::
    For an example of an IDF component using this feature, see :component_file:`freertos/CMakeLists.txt`. The ``freertos`` component uses this 
    mechanism to place all code, literal and rodata of all of its object files to the instruction RAM memory region for performance reasons.

This marks the end of the quick start guide. The following text discusses this mechanism in a little bit more detail, such its components, essential concepts,
the syntax, how it is integrated with the build system, etc. The following sections should be helpful in creating custom mappings or modifying default 
behavior.

Components
----------

.. _ldgen-fragment-files :

Linker Fragment Files
^^^^^^^^^^^^^^^^^^^^^

The fragment files contain objects called 'fragments'. These fragments contain pieces of information which, when put together, form
placement rules that tell where to place sections of object files in the output binary.

Another way of putting it is that processing linker fragment files aims to create the section placement rules inside GNU LD ``SECTIONS`` command. 
Where to collect and put these section placement rules is represented internally as a ``target`` token.

The three types of fragments are discussed below.

.. note::

    Fragments have a name property (except mapping fragments) and are known globally. 
    Fragment naming follows C variable naming rules, i.e. case sensitive, must begin with a letter or underscore, alphanumeric/underscore after
    initial characters are allowed, no spaces/special characters. Each type of fragment has its own namespace. In cases where multiple fragments
    of the same type and name are encountered, an exception is thrown.

.. _ldgen-sections-fragment :

I. Sections
"""""""""""

Sections fragments defines a list of object file sections that the GCC compiler emits. It may be a default section (e.g. ``.text``, ``.data``) or 
it may be user defined section through the ``__attribute__`` keyword. 

The use of an optional '+' indicates the inclusion of the section in the list, as well as sections that start with it. This is the preferred method over listing both explicitly. 

**Syntax**

.. code-block:: none

    [sections:name]
    entries:
        .section+
        .section
        ...

**Example**

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

II. Scheme
""""""""""

Scheme fragments define what ``target`` a sections fragment is assigned to. 

**Syntax**

.. code-block:: none

    [scheme:name]
    entries:
        sections -> target
        sections -> target
        ...

**Example**

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text          # the entries under the sections fragment named text will go to iram0_text
        rodata -> dram0_data        # the entries under the sections fragment named rodata will go to dram0_data

.. _ldgen-default-scheme:

**The** ``default`` **scheme**

There exists a special scheme with the name ``default``. This scheme is special because catch-all placement rules are generated from
its entries. This means that, if one of its entries is ``text -> flash_text``, the placement rule 

.. code-block:: none

    *(.literal .literal.* .text .text.*)

will be generated for the target ``flash_text``. 

These catch-all rules then effectively serve as fallback rules for those whose mappings were not specified. 

.. note::

    The ``default scheme`` is defined in :component:`esp32/ld/esp32_fragments.lf`. The ``noflash`` and ``rtc`` scheme fragments which are 
    built-in schemes referenced in the quick start guide are also defined in this file.

.. _ldgen-mapping-fragment :

III. Mapping
""""""""""""

Mapping fragments define what scheme fragment to use for mappable entities, i.e. object files, function names, variable names. There are two types of entries
for this fragment: mapping entries and condition entries.

.. note::

    Mapping fragments have no explicit name property. Internally, the name is constructed from the value of the archive entry. 

**Syntax**

.. code-block:: none

    [mapping]
    archive: archive                # output archive file name, as built (i.e. libxxx.a)
    entries:
        : condition                 # condition entry, non-default
        object:symbol (scheme)      # mapping entry, Type I
        object (scheme)             # mapping entry, Type II
        * (scheme)                  # mapping entry, Type III

        # optional separation/comments, for readability

        : default                   # condition entry, default
        * (scheme)                  # mapping entry, Type III

.. _ldgen-mapping-entries :

**Mapping Entries**

There are three types of mapping entries:

    ``Type I``
        The object file name and symbol name are specified. The symbol name can be a function name or a variable name.

    ``Type II``
        Only the object file name is specified.

    ``Type III``
        ``*`` is specified, which is a short-hand for all the object files under the archive.

To know what a mapping entry means, let us expand a ``Type II`` entry. Originally: 

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

.. _ldgen-type3-limitations :

**On** ``Type I`` **Mapping Entries**

``Type I`` mapping entry is possible due to compiler flags ``-ffunction-sections`` and ``-ffdata-sections``. If the user opts to remove these flags, then
the ``Type I`` mapping will not work. Furthermore, even if the user does not opt to compile without these flags, there are still limitations 
as the implementation is dependent on the emitted output sections.

For example, with ``-ffunction-sections``, separate sections are emitted for each function; with section names predictably constructed i.e. ``.text.{func_name}``
and ``.literal.{func_name}``. This is not the case for string literals within the function, as they go to pooled or generated section names.

With ``-fdata-sections``, for global scope data the compiler predictably emits either ``.data.{var_name}``, ``.rodata.{var_name}`` or ``.bss.{var_name}``; and so ``Type I`` mapping entry works for these. 
However, this is not the case for static data declared in function scope, as the generated section name is a result of mangling the variable name with some other information.

.. _ldgen-condition-entries :

**Condition Entries**

Condition entries enable the linker script generation to be configuration-aware. Depending on whether expressions involving configuration values
are true or not, a particular set of mapping entries can be used. The evaluation uses ``eval_string`` from ``:idf_file:`tools/kconfig_new/kconfiglib.py``` and adheres to its required syntax and limitations.

All mapping entries defined after a condition entry until the next one or the end of the mapping fragment belongs to that condition entry. During processing 
conditions are tested sequentially, and the mapping entries under the first condition that evaluates to ``TRUE`` are used.

A default condition can be defined (though every mapping contains an implicit, empty one), whose mapping entries get used in the event no conditions evaluates to ``TRUE``.

**Example**

.. code-block:: none

    [scheme:noflash]
    entries:
        text -> iram0_text
        rodata -> dram0_data

    [mapping:lwip]
    archive: liblwip.a
    entries:
        : LWIP_IRAM_OPTIMIZATION = y         # if CONFIG_LWIP_IRAM_OPTIMIZATION is set to 'y' in sdkconfig
        ip4:ip4_route_src_hook (noflash)     # map ip4.o:ip4_route_src_hook, ip4.o:ip4_route_src and
        ip4:ip4_route_src (noflash)          # ip4.o:ip4_route using the noflash scheme, which puts 
        ip4:ip4_route (noflash)              # them in RAM
        
        : default                            # else no special mapping rules apply

.. _ldgen-script-templates :

Linker Script Template
^^^^^^^^^^^^^^^^^^^^^^

The linker script template is the skeleton in which the generated placement rules are put into. It is an otherwise ordinary linker script, with a specific marker syntax
that indicates where the generated placement rules are placed.

**Syntax**

To reference the placement rules collected under a ``target`` token, the following syntax is used:

.. code-block:: none

    mapping[target]

**Example**

The example below is an excerpt from a possible linker script template. It defines an output section ``.iram0.text``, and inside is a marker referencing
the target ``iram0_text``.

.. code-block:: none

    .iram0.text :
    {
        /* Code marked as runnning out of IRAM */
        _iram_text_start = ABSOLUTE(.);

        /* Marker referencing iram0_text */
        mapping[iram0_text]

        INCLUDE esp32.spiram.rom-functions-iram.ld
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
        /* Code marked as runnning out of IRAM */
        _iram_text_start = ABSOLUTE(.);

        /* Placement rules generated from the processed fragments, placed where the marker was in the template */
        *(.iram1 .iram1.*)
        *libfreertos.a:(.literal .text .literal.* .text.*)

        INCLUDE esp32.spiram.rom-functions-iram.ld
        _iram_text_end = ABSOLUTE(.);
    } > iram0_0_seg

``*libfreertos.a:(.literal .text .literal.* .text.*)``

    Rule generated from the entry ``* (noflash)`` of the ``freertos`` mapping fragment. All ``text`` sections of all
    object files under the archive ``libfreertos.a`` will be collected under the target ``iram0_text`` (as per the ``noflash`` scheme)
    and placed wherever in the template ``iram0_text`` is referenced by a marker.

``*(.iram1 .iram1.*)``

    Rule generated from the default scheme entry 	``iram -> iram0_text``. Since the default scheme specifies an ``iram -> iram0_text`` entry,
    it too is placed wherever ``iram0_text`` is referenced by a marker. Since it is a rule generated from the default scheme, it comes first
    among all other rules collected under the same target name.


Integration with Build System
-----------------------------

The linker script generation occurs during application build, before the final output binary is linked. The tool that implements the mechanism 
lives under ``$(IDF_PATH)/tools/ldgen``.

Linker Script Template
^^^^^^^^^^^^^^^^^^^^^^
Currently, the linker script template used is :component:`esp32/ld/esp32.common.ld.in`, and is used only for the app build. The generated output script is 
put under the build directory of the same component. Modifying this linker script template triggers a re-link of the app binary. 

Linker Fragment File
^^^^^^^^^^^^^^^^^^^^
Any component can add a fragment file to the build. In order to add a fragment file to process, use the command ``ldgen_add_fragment_file`` as mentioned :ref:`here<ldgen-add-fragment-file>`.
Modifying any fragment file presented to the build system triggers a re-link of the app binary.
