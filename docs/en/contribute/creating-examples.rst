Creating Examples
=================

Each ESP-IDF example is a complete project that someone else can copy and adapt the code to solve their own problem. Examples should demonstrate ESP-IDF functionality, while keeping this purpose in mind.

Structure
---------

- The ``main`` directory should contain a source file named ``(something)_example_main.c`` with the main functionality.
- If the example has additional functionality, split it logically into separate C or C++ source files under ``main`` and place a corresponding header file in the same directory.
- If the example has a lot of additional functionality, consider adding a ``components`` directory to the example project and make some example-specific components with library functionality. Only do this if the components are specific to the example, if they're generic or common functionality then they should be added to ESP-IDF itself.
- The example should have a ``README.md`` file. Use the :idf_file:`template example README <docs/TEMPLATE_EXAMPLE_README.md>` and adapt it for your particular example.
- Examples should have an ``example_test.py`` file for running an automated example test. If submitting a GitHub Pull Request which includes an example, it's OK not to include this file initially. The details can be discussed as part of the PR.

General Guidelines
------------------

Example code should follow the :doc:`style-guide`.

Checklist
---------

Checklist before submitting a new example:

* Example project name (in ``Makefile`` and ``README.md``) uses the word "example". Use "example" instead of "demo", "test" or similar words.
* Example does one distinct thing. If the example does more than one thing at a time, split it into two or more examples.
* Example has a ``README.md`` file which is similar to the :idf_file:`template example README <docs/TEMPLATE_EXAMPLE_README.md>`.
* Functions and variables in the example are named according to :ref:`naming section of the style guide <style-guide-naming>`. (For non-static names which are only specific to the example's source files, you can use ``example`` or something similar as a prefix.)
* All code in the example is well structured and commented.
* Any unnecessary code (old debugging logs, commented-out code, etc.) is removed from the example.
* Options in the example (like network names, addresses, etc) are not hard-coded. Use configuration items if possible, or otherwise declare macros or constants)
* Configuration items are provided in a ``KConfig.projbuild`` file with a menu named "Example Configuration". See existing example projects to see how this is done.
* All original example code has a license header saying it is "in the public domain / CC0", and a warranty disclaimer clause. Alternatively, the example is licensed under Apache License 2.0. See existing examples for headers to adapt from.
* Any adapted or third party example code has the original license header on it. This code must be licensed compatible with Apache License 2.0.
