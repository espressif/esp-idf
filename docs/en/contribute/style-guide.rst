Espressif IoT Development Framework Style Guide
===============================================


About This Guide
----------------

Purpose of this style guide is to encourage use of common coding practices within the ESP-IDF.

Style guide is a set of rules which are aimed to help create readable, maintainable, and robust code. By writing code which looks the same way across the code base we help others read and comprehend the code. By using same conventions for spaces and newlines we reduce chances that future changes will produce huge unreadable diffs. By following common patterns for module structure and by using language features consistently we help others understand code behavior.

We try to keep rules simple enough, which means that they can not cover all potential cases. In some cases one has to bend these simple rules to achieve readability, maintainability, or robustness.

When doing modifications to third-party code used in ESP-IDF, follow the way that particular project is written. That will help propose useful changes for merging into upstream project.

C Code Formatting
-----------------

.. highlight:: c

.. _style-guide-naming:

Naming
^^^^^^

* Any variable or function which is only used in a single source file should be declared ``static``.
* Public names (non-static variables and functions) should be namespaced with a per-component or per-unit prefix, to avoid naming collisions. ie ``esp_vfs_register()`` or ``esp_console_run()``. Starting the prefix with ``esp_`` for Espressif-specific names is optional, but should be consistent with any other names in the same component.
* Static variables should be prefixed with ``s_`` for easy identification. For example, ``static bool s_invert``.
* Avoid unnecessary abbreviations (ie shortening ``data`` to ``dat``), unless the resulting name would otherwise be very long.


Indentation
^^^^^^^^^^^

Use 4 spaces for each indentation level. Do not use tabs for indentation. Configure the editor to emit 4 spaces each time you press tab key.

Vertical Space
^^^^^^^^^^^^^^

Place one empty line between functions. Do not begin or end a function with an empty line.
::

    void function1()
    {
        do_one_thing();
        do_another_thing();
                                    // INCORRECT, do not place empty line here
    }
                                    // place empty line here
    void function2()
    {
                                    // INCORRECT, do not use an empty line here
        int var = 0;
        while (var < SOME_CONSTANT) {
            do_stuff(&var);
        }
    }

The maximum line length is 120 characters as long as it does not seriously affect the readability.

Horizontal Space
^^^^^^^^^^^^^^^^

Always add single space after conditional and loop keywords::

    if (condition) {    // correct
        // ...
    }

    switch (n) {        // correct
        case 0:
            // ...
    }

    for(int i = 0; i < CONST; ++i) {    // INCORRECT
        // ...
    }

Add single space around binary operators. No space is necessary for unary operators. It is okay to drop space around multiply and divide operators::

    const int y = y0 + (x - x0) * (y1 - y0) / (x1 - x0);    // correct

    const int y = y0 + (x - x0)*(y1 - y0)/(x1 - x0);        // also okay

    int y_cur = -y;                                         // correct
    ++y_cur;

    const int y = y0+(x-x0)*(y1-y0)/(x1-x0);                // INCORRECT


No space is necessary around ``.`` and ``->`` operators.


Sometimes adding horizontal space within a line can help make code more readable. For example, you can add space to align function arguments::

    esp_rom_gpio_connect_in_signal(PIN_CAM_D6,   I2S0I_DATA_IN14_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_D7,   I2S0I_DATA_IN15_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_HREF, I2S0I_H_ENABLE_IDX,  false);
    esp_rom_gpio_connect_in_signal(PIN_CAM_PCLK, I2S0I_DATA_IN15_IDX, false);

Note however that if someone goes to add new line with a longer identifier as first argument (e.g.,  ``PIN_CAM_VSYNC``), it will not fit. So other lines would have to be realigned, adding meaningless changes to the commit.

Therefore, use horizontal alignment sparingly, especially if you expect new lines to be added to the list later.

Never use TAB characters for horizontal alignment.

Never add trailing whitespace at the end of the line.


Braces
^^^^^^

- Function definition should have a brace on a separate line::

    // This is correct:
    void function(int arg)
    {

    }

    // NOT like this:
    void function(int arg) {

    }

- Within a function, place opening brace on the same line with conditional and loop statements::

    if (condition) {
        do_one();
    } else if (other_condition) {
        do_two();
    }


Comments
^^^^^^^^

Use ``//`` for single line comments. For multi-line comments it is okay to use either ``//`` on each line or a ``/* */`` block.

Although not directly related to formatting, here are a few notes about using comments effectively.

- Do not use single comments to disable some functionality::

    void init_something()
    {
        setup_dma();
        // load_resources();                // WHY is this thing commented, asks the reader?
        start_timer();
    }

- If some code is no longer required, remove it completely. If you need it you can always look it up in git history of this file. If you disable some call because of temporary reasons, with an intention to restore it in the future, add explanation on the adjacent line::

    void init_something()
    {
        setup_dma();
        // TODO: we should load resources here, but loader is not fully integrated yet.
        // load_resources();
        start_timer();
    }

- Same goes for ``#if 0 ... #endif`` blocks. Remove code block completely if it is not used. Otherwise, add comment explaining why the block is disabled. Do not use ``#if 0 ... #endif`` or comments to store code snippets which you may need in the future.

- Do not add trivial comments about authorship and change date. You can always look up who modified any given line using git. E.g., this comment adds clutter to the code without adding any useful information::

    void init_something()
    {
        setup_dma();
        // XXX add 2016-09-01
        init_dma_list();
        fill_dma_item(0);
        // end XXX add
        start_timer();
    }


Line Endings
^^^^^^^^^^^^

Commits should only contain files with LF (Unix style) endings.

Windows users can configure git to check out CRLF (Windows style) endings locally and commit LF endings by setting the ``core.autocrlf`` setting. `Github has a document about setting this option <github-line-endings>`.

If you accidentally have some commits in your branch that add LF endings, you can convert them to Unix by running this command in an MSYS2 or Unix terminal (change directory to the IDF working directory and check the correct branch is currently checked out, beforehand):

.. code-block:: bash

  git rebase --exec 'git diff-tree --no-commit-id --name-only -r HEAD | xargs dos2unix && git commit -a --amend --no-edit --allow-empty' master

(Note that this line rebases on master, change the branch name at the end to rebase on another branch.)

For updating a single commit, it is possible to run ``dos2unix FILENAME`` and then run ``git commit --amend``

Formatting Your Code
^^^^^^^^^^^^^^^^^^^^

ESP-IDF uses Astyle to format source code. The configuration is stored in :project_file:`tools/ci/astyle-rules.yml` file.

Initially, all components are excluded from formatting checks. You can enable formatting checks for the component by removing it from ``components_not_formatted_temporary`` list. Then run:

.. code-block:: bash

    pre-commit run --files <path_to_files> astyle_py

Alternatively, you can run ``astyle_py`` manually. You can install it with ``pip install astyle_py==VERSION``. Make sure you have the same version installed as the one specified in :project_file:`.pre-commit-config.yaml` file. With ``astyle_py`` installed, run:

.. code-block:: bash

    astyle_py --rules=$IDF_PATH/tools/ci/astyle-rules.yml <path-to-file>


Type Definitions
^^^^^^^^^^^^^^^^

Should be snake_case, ending with _t suffix::

    typedef int signed_32_bit_t;

Enum
^^^^

Enums should be defined through the `typedef` and be namespaced::

    typedef enum
    {
        MODULE_FOO_ONE,
        MODULE_FOO_TWO,
        MODULE_FOO_THREE
    } module_foo_t;


.. _assertions:

Assertions
^^^^^^^^^^

The standard C ``assert()`` function, defined in ``assert.h`` should be used to check conditions that should be true in source code. In the default configuration, an assert condition that returns ``false`` or 0 will call ``abort()`` and trigger a :doc:`Fatal Error </api-guides/fatal-errors>`.

``assert()`` should only be used to detect unrecoverable errors due to a serious internal logic bug or corruption, where it is not possible for the program to continue. For recoverable errors, including errors that are possible due to invalid external input, an :doc:`error value should be returned </api-guides/error-handling>`.

.. note::

   When asserting a value of type ``esp_err_t`` is equal to ``ESP_OK``, use the :ref:`esp-error-check-macro` instead of an ``assert()``.

It is possible to configure ESP-IDF projects with assertions disabled (see :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL`). Therefore, functions called in an ``assert()`` statement should not have side-effects.

It is also necessary to use particular techniques to avoid "variable set but not used" warnings when assertions are disabled, due to code patterns such as::

  int res = do_something();
  assert(res == 0);

Once the ``assert`` is optimized out, the ``res`` value is unused and the compiler will warn about this. However the function ``do_something()`` must still be called, even if assertions are disabled.

When the variable is declared and initialized in a single statement, a good strategy is to cast it to ``void`` on a new line. The compiler will not produce a warning, and the variable can still be optimized out of the final binary::

  int res = do_something();
  assert(res == 0);
  (void)res;

If the variable is declared separately, for example if it is used for multiple assertions, then it can be declared with the GCC attribute ``__attribute__((unused))``. The compiler will not produce any unused variable warnings, but the variable can still be optimized out::

  int res __attribute__((unused));

  res = do_something();
  assert(res == 0);

  res = do_something_else();
  assert(res != 0);


Header File Guards
------------------

All public facing header files should have preprocessor guards. A pragma is preferred::

    #pragma once

over the following pattern::

    #ifndef FILE_NAME_H
    #define FILE_NAME_H
    ...
    #endif // FILE_NAME_H

In addition to guard macros, all C header files should have ``extern "C"`` guards to allow the header to be used from C++ code. Note that the following order should be used: ``pragma once``, then any ``#include`` statements, then ``extern "C"`` guards::

    #pragma once

    #include <stdint.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    /* declarations go here */

    #ifdef __cplusplus
    }
    #endif


Include Statements
------------------

When writing ``#include`` statements, try to maintain the following order:

* C standard library headers.
* Other POSIX standard headers and common extensions to them (such as ``sys/queue.h``.)
* Common IDF headers (``esp_log.h``, ``esp_system.h``, ``esp_timer.h``, ``esp_sleep.h``, etc.)
* Headers of other components, such as FreeRTOS.
* Public headers of the current component.
* Private headers.

Use angle brackets for C standard library headers and other POSIX headers (``#include <stdio.h>``).

Use double quotes for all other headers (``#include "esp_log.h"``).


C++ Code Formatting
-------------------

The same rules as for C apply. Where they are not enough, apply the following rules.

File Naming
^^^^^^^^^^^^
C++ Header files have the extension ``.hpp``. C++ source files have the extension ``.cpp``. The latter is important for the compiler to distinguish them from normal C source files.

Naming
^^^^^^

* **Class and struct** names shall be written in ``CamelCase`` with a capital letter as beginning. Member variables and methods shall be in ``snake_case``. An exception from ``CamelCase`` is if the readability is severely decreased, e.g., in ``GPIOOutput``, then an underscore ``_`` is allowed to make it more readable: ``GPIO_Output``.
* **Namespaces** shall be in lower ``snake_case``.
* **Templates** are specified in the line above the function declaration.
* Interfaces in terms of Object-Oriented Programming shall be named without the suffix ``...Interface``. Later, this makes it easier to extract interfaces from normal classes and vice versa without making a breaking change.

Member Order in Classes
^^^^^^^^^^^^^^^^^^^^^^^
In order of precedence:

* First put the public members, then the protected, then private ones. Omit public, protected or private sections without any members.
* First put constructors/destructors, then member functions, then member variables.

For example:

::

    class ForExample {
    public:
        // first constructors, then default constructor, then destructor
        ForExample(double example_factor_arg);
        ForExample();
        ~ForExample();

        // then remaining pubic methods
        set_example_factor(double example_factor_arg);

        // then public member variables
        uint32_t public_data_member;

    private:
        // first private methods
        void internal_method();

        // then private member variables
        double example_factor;
    };

Spacing
^^^^^^^

* Do not indent inside namespaces.
* Put ``public``, ``protected`` and ``private`` labels at the same indentation level as the corresponding ``class`` label.

Simple Example
^^^^^^^^^^^^^^^
::

    // file spaceship.h
    #ifndef SPACESHIP_H_
    #define SPACESHIP_H_
    #include <cstdlib>

    namespace spaceships {

    class SpaceShip {
    public:
        SpaceShip(size_t crew);
        size_t get_crew_size() const;

    private:
        const size_t crew;
    };

    class SpaceShuttle : public SpaceShip {
    public:
        SpaceShuttle();
    };

    class Sojuz : public SpaceShip {
    public:
        Sojuz();
    };

    template <typename T>
    class CargoShip {
    public:
        CargoShip(const T &cargo);

    private:
        T cargo;
    };

    } // namespace spaceships

    #endif // SPACESHIP_H_

    // file spaceship.cpp
    #include "spaceship.h"

    namespace spaceships {

    // Putting the curly braces in the same line for constructors is OK if it only initializes
    // values in the initializer list
    SpaceShip::SpaceShip(size_t crew) : crew(crew) { }

    size_t SpaceShip::get_crew_size() const
    {
        return crew;
    }

    SpaceShuttle::SpaceShuttle() : SpaceShip(7)
    {
        // doing further initialization
    }

    Sojuz::Sojuz() : SpaceShip(3)
    {
        // doing further initialization
    }

    template <typename T>
    CargoShip<T>::CargoShip(const T &cargo) : cargo(cargo) { }

    } // namespace spaceships


CMake Code Style
----------------

- Indent with four spaces.
- Maximum line length 120 characters. When splitting lines, try to
  focus on readability where possible (for example, by pairing up
  keyword/argument pairs on individual lines).
- Do not put anything in the optional parentheses after ``endforeach()``, ``endif()``, etc.
- Use lowercase (``with_underscores``) for command, function, and macro names.
- For locally scoped variables, use lowercase (``with_underscores``).
- For globally scoped variables, use uppercase (``WITH_UNDERSCORES``).
- Otherwise follow the defaults of the cmake-lint_ project.

Configuring the Code Style for a Project Using EditorConfig
-----------------------------------------------------------

EditorConfig helps developers define and maintain consistent coding styles between different editors and IDEs. The EditorConfig project consists of a file format for defining coding styles and a collection of text editor plugins that enable editors to read the file format and adhere to defined styles. EditorConfig files are easily readable and they work nicely with version control systems.

For more information, see `EditorConfig <https://editorconfig.org>`_ Website.

Third Party Component Code Styles
---------------------------------

ESP-IDF integrates a number of third party components where these components may have differing code styles.

FreeRTOS
^^^^^^^^

The code style adopted by FreeRTOS is described in the `FreeRTOS style guide <https://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html#StyleGuide>`_. Formatting of FreeRTOS source code is automated using `Uncrustify <https://github.com/uncrustify/uncrustify>`_, thus a copy of the FreeRTOS code style's Uncrustify configuration (``uncrustify.cfg``) is stored within ESP-IDF FreeRTOS component.

If a FreeRTOS source file is modified, the updated file can be formatted again by following the steps below:

1. Ensure that Uncrustify (v0.69.0) is installed on your system
2. Run the following command on the update FreeRTOS source file (where ``source.c`` is the path to the source file that requires formatting).

.. code-block:: bash

    uncrustify -c $IDF_PATH/components/freertos/FreeRTOS-Kernel/uncrustify.cfg --replace source.c --no-backup

Documenting Code
----------------

Please see the guide here: :doc:`documenting-code`.

Structure
---------

To be written.


Language Features
-----------------

To be written.

.. _cmake-lint: https://github.com/richq/cmake-lint
