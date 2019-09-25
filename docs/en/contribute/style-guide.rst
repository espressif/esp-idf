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

Indentation
^^^^^^^^^^^

Use 4 spaces for each indentation level. Don't use tabs for indentation. Configure the editor to emit 4 spaces each time you press tab key.

Vertical Space
^^^^^^^^^^^^^^

Place one empty line between functions. Don't begin or end a function with an empty line.
::

    void function1()
    {
        do_one_thing();
        do_another_thing();
                                    // INCORRECT, don't place empty line here
    }
                                    // place empty line here
    void function2()
    {
                                    // INCORRECT, don't use an empty line here
        int var = 0;
        while (var < SOME_CONSTANT) {
            do_stuff(&var);
        }
    }

The maximum line length is 120 characters as long as it doesn't seriously affect the readability.

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

    gpio_matrix_in(PIN_CAM_D6,   I2S0I_DATA_IN14_IDX, false);
    gpio_matrix_in(PIN_CAM_D7,   I2S0I_DATA_IN15_IDX, false);
    gpio_matrix_in(PIN_CAM_HREF, I2S0I_H_ENABLE_IDX,  false);
    gpio_matrix_in(PIN_CAM_PCLK, I2S0I_DATA_IN15_IDX, false);

Note however that if someone goes to add new line with a longer identifier as first argument (e.g.  ``PIN_CAM_VSYNC``), it will not fit. So other lines would have to be realigned, adding meaningless changes to the commit. 

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

- Don't use single comments to disable some functionality::

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

- Same goes for ``#if 0 ... #endif`` blocks. Remove code block completely if it is not used. Otherwise, add comment explaining why the block is disabled. Don't use ``#if 0 ... #endif`` or comments to store code snippets which you may need in the future.

- Don't add trivial comments about authorship and change date. You can always look up who modified any given line using git. E.g. this comment adds clutter to the code without adding any useful information::

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

Windows users can configure git to check out CRLF (Windows style) endings locally and commit LF endings by setting the ``core.autocrlf`` setting. `Github has a document about setting this option <github-line-endings>`. However because MSYS2 uses Unix-style line endings, it is often easier to configure your text editor to use LF (Unix style) endings when editing ESP-IDF source files.

If you accidentally have some commits in your branch that add LF endings, you can convert them to Unix by running this command in an MSYS2 or Unix terminal (change directory to the IDF working directory and check the correct branch is currently checked out, beforehand)::

  git rebase --exec 'git diff-tree --no-commit-id --name-only -r HEAD | xargs dos2unix && git commit -a --amend --no-edit --allow-empty' master

(Note that this line rebases on master, change the branch name at the end to rebase on another branch.)

For updating a single commit, it's possible to run ``dos2unix FILENAME`` and then run ``git commit --amend``

Formatting Your Code
^^^^^^^^^^^^^^^^^^^^

You can use ``astyle`` program to format your code according to the above recommendations.

If you are writing a file from scratch, or doing a complete rewrite, feel free to re-format the entire file. If you are changing a small portion of file, don't re-format the code you didn't change. This will help others when they review your changes.

To re-format a file, run::

    tools/format.sh components/my_component/file.c


C++ Code Formatting
-------------------

The same rules as for C apply. Where they are not enough, apply the following rules.

File Naming
^^^^^^^^^^^^
C++ Header files have the extension ``.h``. C++ source files have the extension ``.cpp``, which is important for the compiler to distiguish them from normal C source files.

Naming
^^^^^^

* **Class and struct** names shall be written in ``CamelCase`` with a capital letter as beginning. Member variables and methods shall be in ``snake_case``.
* **Namespaces** shall be in lower ``snake_case``.
* **Templates** are specified in the line above the function declaration.

Member Order in Classes
^^^^^^^^^^^^^^^^^^^^^^^

First put the public members, then the protected, then private ones. Omit public, protected or private sections without any members.

Spacing
^^^^^^^

* Don't indent inside namespaces.
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
- Don't put anything in the optional parentheses after ``endforeach()``, ``endif()``, etc.
- Use lowercase (``with_underscores``) for command, function, and macro names.
- For locally scoped variables, use lowercase (``with_underscores``).
- For globally scoped variables, use uppercase (``WITH_UNDERSCORES``).
- Otherwise follow the defaults of the cmake-lint_ project.

Configuring the Code Style for a Project Using EditorConfig
-----------------------------------------------------------

EditorConfig helps developers define and maintain consistent coding styles between different editors and IDEs. The EditorConfig project consists of a file format for defining coding styles and a collection of text editor plugins that enable editors to read the file format and adhere to defined styles. EditorConfig files are easily readable and they work nicely with version control systems.

For more information, see `EditorConfig <http://editorconfig.org>`_ Website.


Documenting Code
----------------

Please see the guide here: :doc:`documenting-code`.

.. _style-guide-naming:

Naming
------

- Any variable or function which is only used in a single source file should be declared ``static``.

- Public names (non-static variables and functions) should be namespaced with a per-component or per-unit prefix, to avoid naming collisions. ie ``esp_vfs_register()`` or ``esp_console_run()``. Starting the prefix with ``esp_`` for Espressif-specific names is optional, but should be consistent with any other names in the same component.

- Static variables should be prefixed with ``s_`` for easy identification. For example, ``static bool s_invert``.

- Avoid unnecessary abbreviations (ie shortening ``data`` to ``dat``), unless the resulting name would otherwise be very long.

Structure
---------

To be written.


Language Features
-----------------

To be written.

.. _cmake-lint: https://github.com/richq/cmake-lint
