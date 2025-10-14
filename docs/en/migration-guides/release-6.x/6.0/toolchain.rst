Toolchain
==========

:link_to_translation:`zh_CN:[中文]`

GCC Version
-----------

The previous GCC version 14.2.0 has been upgraded to 15.1.0 across all chip targets. Upgrading to ESP-IDF v6.0 requires porting code to GCC 15.1.0. Refer to the official guide `Porting to GCC 15 <https://gcc.gnu.org/gcc-15/porting_to.html>`_.

Warnings
--------

The upgrade to GCC 15.1.0 has resulted in the addition of new warnings, or enhancements to existing warnings. The full details of all GCC warnings can be found in `GCC Warning Options <https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Warning-Options.html>`_. Users are advised to double-check their code, then fix the warnings if possible. Unfortunately, depending on the warning and the complexity of the user's code, some warnings will be false positives that require non-trivial fixes. In such cases, users can choose to suppress the warning in multiple ways. This section outlines some common warnings that users are likely to encounter and ways to fix them.

To suprress all new warnings, enable :ref:`CONFIG_COMPILER_DISABLE_GCC15_WARNINGS` config option.

``-Wno-unterminated-string-initialization``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Warn about character arrays initialized as unterminated character sequences with a string literal, unless the declaration being initialized has the nonstring attribute.

.. code-block:: c

    #include "esp_attr.h"

    char arr[3] = "foo"; /* Warning.  */
    NONSTRING_ATTR char arr2[3] = "bar"; /* No warning.  */

``-Wno-header-guard``
^^^^^^^^^^^^^^^^^^^^^^^

Warn if a header file has a typo in its include guard. When #ifndef and #define use different names.

.. code-block:: c

    #ifndef WHEADER_GUARD_2
    #define WHEADERGUARD2 /* Warning. Must be changed to WHEADER_GUARD_2.  */
    /* ... */
    #endif

``-Wno-self-move (C++ only)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Warns when a value is moved to itself with `std::move`. Such a `std::move` typically has no effect.

.. code-block:: cpp

    struct T {
      /* ... */
    };
    void fn()
    {
      T t;
      /* ... */
      t = std::move (t); /* Warning. The line can be removed.  */
    }


``-Wno-template-body (C++ only)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Disable diagnosing errors when parsing a template, and instead issue an error only upon instantiation of the template.

.. code-block:: cpp

    template<class T>
    void f() {
      const int n = 42;
      ++n; /* read-only variable 'n' */
    }

``-Wno-dangling-reference (C++ only)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Warn when a reference is bound to a temporary whose lifetime has ended.

.. code-block:: cpp

    int n = 1;
    const int& r = std::max(n - 1, n + 1); /* r is dangling.  */

``-Wno-defaulted-function-deleted (C++ only)``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Warn when an explicitly defaulted function is deleted by the compiler. That can occur when the function’s declared type does not match the type of the function that would have been implicitly declared.

.. code-block:: cpp

    template<typename>
    struct C {
       C();
       C(const C&&) = default; /* Implicitly deleted.  */
    };

``sys/dirent.h`` No Longer Includes Function Prototypes
-------------------------------------------------------

Issue
^^^^^^

Compilation errors may occur in code that previously worked with the old toolchain. For example:

.. code-block:: c

    #include <sys/dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");
    /* .... */
    /**
     * Compile error:
     * test.c: In function 'test_opendir':
     * test.c:100:16: error: implicit declaration of function 'opendir' [-Werror=implicit-function-declaration]
     *   100 |     DIR* dir = opendir(path);
     *       |                ^~~~~~~
     */

Solution
^^^^^^^^^

To resolve this issue, the correct header must be included. Refactor the code like this:

.. code-block:: c

    #include <dirent.h>
    /* .... */
    DIR* dir = opendir("test_dir");

Picolibc
--------

When building with :ref:`CONFIG_LIBC_PICOLIBC<CONFIG_LIBC_PICOLIBC>` enabled, the following adaptation is required.

``sys/signal.h header removed``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The header ``<sys/signal.h>`` is no longer available in Picolibc. To ensure compatibility and improve portability across libc implementations, replace it with the standard C header ``<signal.h>``.

.. code-block:: c

    #include <sys/signal.h> /* fatal error: sys/signal.h: No such file or directory */
    #include <signal.h>     /* Ok: standard and portable */

.. only:: CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY

    RISC-V Chips and Misaligned Memory Access in LibC Functions
    -----------------------------------------------------------

    Espressif RISC-V chips can perform misaligned memory accesses with only a small performance penalty compared to aligned accesses.

    Previously, LibC functions that operate on memory (such as copy or comparison functions) were implemented using byte-by-byte operations when a non-word-aligned pointer was passed. Now, these functions use word (4-byte) load/store operations whenever possible, resulting in a significant performance increase. These optimized implementations are enabled by default via :ref:`CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS`, which reduces the application's memory budget (IRAM) by approximately 800–1000 bytes.

    The table below shows benchmark results on the ESP32-C3 chip using 4096-byte buffers:

    .. list-table:: Benchmark Results
       :header-rows: 1
       :widths: 20 20 20 20

       * - Function
         - Old (CPU cycles)
         - Optimized (CPU cycles)
         - Improvement (%)
       * - memcpy
         - 32873
         - 4200
         - 87.2
       * - memcmp
         - 57436
         - 14722
         - 74.4
       * - memmove
         - 49336
         - 9237
         - 81.3
       * - strcpy
         - 28678
         - 16659
         - 41.9
       * - strcmp
         - 36867
         - 11146
         - 69.8

    .. note::

        The results above apply to misaligned memory operations. Performance for aligned memory operations remains unchanged.

    Functions with Improved Performance
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - ``memcpy``
    - ``memcmp``
    - ``memmove``
    - ``strcpy``
    - ``strncpy``
    - ``strcmp``
    - ``strncmp``
