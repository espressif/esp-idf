BitScrambler Driver
========================

Introduction
------------

The BitScrambler is a peripheral that allows doing various transformation of data in a DMA stream based on an user-supplied program. ESP-IDF comes with an assembler for BitScrambler programs as well as build system and driver support for this peripheral. The BitScrambler peripheral in the {IDF_TARGET_NAME} has an independent TX and RX channel; both can be associated with the same or different peripherals.

Functional Overview
-------------------

.. list::

    -  `BitScrambler assembly <#bitscrambler-assembly>`__ - covers how a BitScrambler assembly program is structured
    -  `Build system integration <#bitscrambler-build>`__ - covers how BitScrambler programs are integrated in the ESP-IDF build system
    -  `Resource allocation and program loading <#bitscrambler-load>`__ Covers how to allocate BitScrambler instances and how to load a program into them
    -  `Loopback mode <#bitscrambler-loopback>`__ Covers how to use the BitScrambler in loopback mode

.. _bitscrambler-assembly:

BitScrambler Assembly
^^^^^^^^^^^^^^^^^^^^^

The operations a BitScrambler performs on the data in the DMA stream are defined in a BitScrambler program. As a BitScrambler program is a binary blob that is hard to write by hand, ESP-IDF includes an assembler. This assembler converts easier-to-write text files into BitScrambler binary programs.

BitScrambler assembly files consist of comments, labels, instruction bundles, meta-instructions. The assembler ignores comments. Labels define a location in the program; instructions can e.g. jump to the location indicated by a label. Instruction bundles are a collection of sub-instructions that get assembled into one 257-bit binary BitScrambler instruction. Meta-instructions define global BitScrambler configuration, like the amount of trailing bytes, the prefetch mode, or the contents of the LUT ram.

BitScrambler assembly files are case-insensitive as well as not sensitive to indentation. This documentation will use upper and lower caps for readability, but the assembler itself doesn't care. Fields that can contain an integer normally are given in a base-10 number, but can also contain a hex value if prepended with ``0x`` or a binary value if prepended with ``0b``.

Comments
~~~~~~~~

A comment starts with a '#' and lasts until the end of the line. A comment can be placed anywhere where a space can be placed; this means that comments can be inside instruction bundles
in between sub-instructions.


Labels
~~~~~~

Any string of (non-whitespace) characters followed by a colon is a label. The label is a symbolic reference to the next instruction bundle in the assembly file. Note that a label cannot be part of an instruction bundle; it needs to be located before the start of the instruction bundle.

Example:

.. code:: asm

    loop_back:
        set 0..3 4..7,
        set 4..7 0..3,
        read 8,
        write 8,
        jmp loop_back

The 'jmp' opcode in the instruction bundle will jump back to the start of itself, meaning that this instruction bundle will be executed over and over again in a tight loop.

Instruction bundle
~~~~~~~~~~~~~~~~~~

An instruction bundle consists of sub-instructions, separated by a comma. The entirety of the instruction bundle is assembled into one 257-bit instruction that the BitScrambler will execute in a single clock cycle; in other words, all sub-instructions in an instruction word will execute in parallel, at the same time. This also means the order of sub-instructions in an instruction in the assembly source does not matter. The instruction bundle ends after the last sub-instruction that is not followed by a comma.

The specific details of the BitScrambler can be found in the  *{IDF_TARGET_NAME} Technical Reference Manual* > *BitScrambler (BITSCRM)* [`PDF <{IDF_TARGET_TRM_EN_URL}#bitscrm>`__]. As a summary: the BitScrambler consists of a 32-bit output register. Each of its bits takes the value of any bit in any of the sources. The sources are:

 - a 64-bit input register that is being fed from the incoming DMA stream,
 - two 16-bit counters,
 - 30 registers that contain the output of various comparisons,
 - a fixed high and low bit,
 - the output of a look-up table (LUT) RAM,
 - the value of the output register in the previous cycle.

Sub-instructions
""""""""""""""""

``set [output] [source_bits]`` - Routes one or more source bits to output bits. Note that it's possible to route multiple bits using the ``..`` operator: for instance ``set 0..3 O4..O6`` will have the same effect as ``set 0 O4, set 1 O5, set 2 O6, set 3 O7``. The first argument is the output bit or output bit range; output bits are numbered from 0 to 31. The second argument is one or a range of `source bits`_. Note that any output bits that do not have a corresponding ``set`` sub-instruction in an instruction bundle will be set to a low logic level.

``write [n]`` - After routing all output bits, take the least significant ``n`` output register bits and push them into the output DMA pipeline. ``n`` can be one of 0, 8, 16 or 32. If an instruction bundle does not have a ``write`` sub-instruction, it will be equivalent to a ``write 0``.

``read [n]`` - After routing all output bits and writing to the output register, take ``n`` bits from the input DMA pipeline and push them into the 64-bit input register. ``n`` can be one of 0, 8, 16 or 32. These bits will be shifted into the input FIFO starting from the MSB. As an example, a ``read 16`` will shift bits 63-16 of the input register down to bits 47-0 and the new 16 bits read from the input DMA pipeline will occupy bits 63-48 in the input register. If an instruction bundle does not have a ``read`` sub-instruction, it will be equivalent to a ``read 0``.

An opcode - The opcodes are fully documented in the Technical Reference manual; here's a summary.

.. only:: esp32p4

    - ``LOOP(A|B) end_val ctr_add tgt`` - If the selected counter (A or B) ls smaller than end_val, add ``ctr_add`` to the selected counter (A or B) and jump to the label ``tgt``. If not, continue execution.
    - ``ADD(A|B)[H|L] val`` - Add ``val`` to the selected counter. If 'H' or 'L' is appended, only the high or low 8-bit, respectively, of the counter is written back.
    - ``IF[N] source_bit tgt`` - If the source bit `source_bit` is one (for IF) or zero (for IFN), jump to the label ``tgt``.
    - ``LDCTD(A|B)[H|L] val`` - Load ``val`` into the indicated counter. If H or L is appended, only the high or low 8-bit, respectively, will be updated.
    - ``LDCTI(A|B)[H|L]`` - Load the indicated counter (A or B) with bits 16-31 sent to the output register. If H or L is appended, only the high or low 8-bit, respectively, will be updated.
    - ``JMP tgt`` - Unconditional jump to label ``tgt``. This is equal to ``IF h tgt``.
    - ``NOP`` - No operation. This is equal to ``ADDA 0``.

.. only:: esp32c5

    - ``LOOP(A|B) end_val ctr_add tgt`` - If the selected counter (A or B) ls smaller than end_val, add ``ctr_add`` to the selected counter (A or B) and jump to the label ``tgt``. If not, continue execution.
    - ``ADD(A|B)[H|L] val`` - Add ``val`` to the selected counter. If 'H' or 'L' is appended, only the high or low 8-bit, respectively, of the counter is written back.
    - ``IF[N] source_bit tgt`` - If the source bit `source_bit` is one (for IF) or zero (for IFN), jump to the label ``tgt``.
    - ``LDCTD(A|B)[H|L] val`` - Load ``val`` into the indicated counter. If H or L is appended, only the high or low 8-bit, respectively, will be updated.
    - ``LDCTI(A|B)[H|L]`` - Load the indicated counter (A or B) with bits 16-31 sent to the output register. If H or L is appended, only the high or low 8-bit, respectively, will be updated.
    - ``ADDCTI(A|B)[H|L]`` - Add bits 16-31 sent to the output register to the indicated counter (A or B) . If H or L is appended, only the high or low 8-bit, respectively, will be evaluated and updated.
    - ``JMP tgt`` - Unconditional jump to label ``tgt``. This is equal to ``IF h tgt``.
    - ``NOP`` - No operation. This is equal to ``ADDA 0``.

Note that an instruction bundle can only contain one opcode, one ``read``, and one ``write``. It can contain multiple ``set`` instructions, although multiple ``set`` instruction cannot assign a value to the same output bits.

Source bits
"""""""""""

The ``set`` and ``if``/``ifn`` instructions have a ``source bit`` field. The following values can be put there:

- ``0``-``63`` -  The bit selected is sourced from the selected bit in the input register.
- ``O0``-``O31`` - The bit selected is sourced from the value the output register was assigned in the previous cycle.
- ``A0``-``A15`` - The bit selected is sourced from the selected bit in the A counter register.
- ``B0``-``B15`` - The bit selected is sourced from the selected bit in the B counter register.
- ``L0``-``L31`` - The bit selected is sourced from the output from the LUT ram. As described in the Technical Reference Manual, the LUT RAM output is the LUT item at the position indicated by the most significant N bits of the bits routed to the output register in the previous cycle, with N being 9, 10 or 11 for a LUT width of 32, 16 or 8-bits respectively.
- A condition comparing (a portion of) counter B with bits that were routed to the output register in the previous cycle. These conditions consist of three parts: depending on if you want to compare the entirety of the B register or only the upper or lower 8 bits, the first part is 'B', 'BH' or BL' respectively. The second part is the comparison operator: '<=', '>' and '=' are supported here. The third is the offset into the output register that will be compared to the selected part of the B register: this can be O0 or O16 for 16-bit comparisons and O0, O8, O16 or O24 for 8-bit comparison.
- ``H`` or ``L``. These sources are fixed-high or fixed-low.

Note that not all sources can be used together in the same instruction. For instance, it is not possible to use a bit from one of the two counters as well as a bit from the upper 32 bits of the input FIFO in the same instruction bundle. The assembler will generate an error if an instruction bundle tries to do this anyway.


Example
"""""""

An example BitScrambler program might look like this:

.. code:: asm

    loop_back:
        set 0..3 4..7,
        set 4..7 0..3,
        read 8,
        write 8,
        jmp loop_back


This program only has one instruction (as only the line with the ``jmp`` does not end in a comma). It
takes the lower 4 bits of the data read from memory and sends it to the upper 4 bits of the first byte
of the output register. It also takes the next 4 bits of the input register and sends it to the lower
4 bits of the output register. It then writes 8 bits (one byte) to the output, while reading 8 bits
from the input. Finally, the program continues by jumping back to the start of the instruction. Note
that this all is executed in one BitScrambler cycle, and as the sub-instructions all are part of
the same instruction, they could be specified in any order within the instruction. The end result
of this small BitScrambler program is that it takes in data, e.g. ``01 23 45 67`` and swaps the high
and low nibble of every bytes, resulting in an output of ``10 32 54 76``.


Meta-instructions
~~~~~~~~~~~~~~~~~

Meta-instructions set global BitScrambler configuration. Meta-instructions are allowed anywhere within the assembly file (except within an instruction bundle) and due to their nature will also have effect on the preceding assembly code. At the moment, two meta-instructions are defined. ``cfg`` sets a global BitScrambler setting, while ``lut`` defines lookuptable RAM content.


Global configuration meta-instructions
""""""""""""""""""""""""""""""""""""""

- ``cfg prefetch true|false`` - If prefetch is set to ``true``, on BitScrambler start it will read 64 bits from the input DMA stream into the input register. If set to ``false``, the input register will be initialized to zero. This setting defaults to ``true`` if not specified.
- ``cfg eof_on upstream|downstream`` - After the input stream ends, the BitScrambler will still process a certain amount of 'trailing' dummy bytes so it can flush any data contained in its registers. This setting indicates from where the data will be counted: ``upstream`` makes the bitscrambler count the bytes being read, ``downstream`` makes it count the bytes being written. This defaults to ``upstream`` if not specified.
- ``cfg trailing_bytes N`` - This indicates how many dummy bytes will be read or written (depending on the ``eof_on`` setting) before the BitScrambler indicates an end-of-stream on its output. This defaults to ``0`` if not specified.
- ``cfg lut_width_bits 8|16|32`` - This selects the bus width of the LUT output RAM, in bits. The LUT can be 2048x8bit, 1024*16bit or 512*32bits in size. This defaults to ``32`` if not specified.


LUT content meta-instructions
"""""""""""""""""""""""""""""

``lut`` instructions are used to specify the contents of the LUT RAM. This meta-instruction is followed by one or more numerical values, separated by spaces or commas. LUT RAM locations
are defined in the order they're encountered in the assembly program; the first value is always stored at location 0, the second value encountered is always stored at location 1, etc. The amount of arguments to a LUT meta-instruction is arbitrary as LUT meta-instructions can always be broken up or merged. For instance, ``lut 1,2,3,4`` is the same as ``lut 1,2`` on one line and ``lut 3,4`` on the next line.
Note that LUT values must be within range with respect to the value given to the ``cfg lut_width_bits`` configuration meta-statement.

.. _bitscrambler-build:

Build system integration
^^^^^^^^^^^^^^^^^^^^^^^^

The BitScrambler has full ESP-IDF build system support. A component (including the main component) can have BitScrambler assembly source files in its source directories. These files generally have the suffix ``.bsasm``. To assemble and link such a file into the main application, the CMakeLists.txt file for the component can call ``target_bitscrambler_add_src("assembly_file.bsasm")``. For instance, for an assembly file called ``my_program.bsasm``, a CMakeLists.txt file may look like this:

.. code:: cmake

    idf_component_register(SRCS "main.c" "some-file.c"
                    INCLUDE_DIRS "./include")

    target_bitscrambler_add_src("my_program.bsasm")

To use the assembled BitScrambler program, you would refer to it as such:

.. code:: c

    // Create a variable 'my_bitscrambler_program' that resolves to
    // the binary bitscrambler program.
    // 2nd arg is same as name of assembly file without ".bsasm"
    BITSCRAMBLER_PROGRAM(my_bitscrambler_program, "my_program");

    [...]

    bitscrambler_handle_t bs;
    [...create bitscrambler instance]
    bitscrambler_load_program(bs, my_bitscrambler_program);


.. _bitscrambler-load:


Loopback mode
^^^^^^^^^^^^^
The BitScrambler supports a loopback mode which is useful for data transformations that do not involve a peripheral. The loopback mode occupies both the TX and RX channels of the BitScrambler, although only the TX BitScrambler actually executes code. Note that even if loopback mode does not involve a peripheral, one still needs to be selected; the peripheral does not need to be initialized or used, but if it is, its DMA features will be unavailable.

Resource allocation and program loading
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In loopback mode, a BitScrambler object is created using :cpp:func:`bitscrambler_loopback_create`. If there is a BitScrambler peripheral matching the requested characteristics, this function will return a handle to it. You can then use :cpp:func:`bitscrambler_load_program` to load a program into it, then call :cpp:func:`bitscrambler_loopback_run` to transform a memory buffer using the loaded program. You can call :cpp:func:`bitscrambler_loopback_run` any number of times; it's also permissible to use :cpp:func:`bitscrambler_load_program` to change programs between calls. Finally, to free the hardware resources and clean up memory, call :cpp:func:`bitscrambler_free`.

Application Example
-------------------

* :example:`peripherals/bitscrambler` demonstrates how to use the BitScrambler loopback mode to transform a buffer of data into a different format.

API Reference
-------------

.. include-build-file:: inc/bitscrambler.inc
.. include-build-file:: inc/bitscrambler_loopback.inc
.. include-build-file:: inc/bitscrambler_peri_select.inc
