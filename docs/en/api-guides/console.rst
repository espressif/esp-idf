Console
=======

ESP-IDF provides ``console`` component, which includes building blocks needed to develop an interactive console over serial port. This component includes following facilities:

- Line editing, provided by `linenoise`_ library. This includes handling of backspace and arrow keys, scrolling through command history, command auto-completion, and argument hints.
- Splitting of command line into arguments.
- Argument parsing, provided by `argtable3`_ library. This library includes APIs useful for parsing GNU style command line arguments.
- Functions for registration and dispatching of commands.

These facilities can be used together or independently. For example, it is possible to use line editing and command registration features, but use ``getopt`` or custom code for argument parsing, instead of `argtable3`_. Likewise, it is possible to use simpler means of command input (such as ``fgets``) together with the rest of the means for command splitting and argument parsing.

Line editing
------------

Line editing feature lets users compose commands by typing them, erasing symbols using 'backspace' key, navigating within the command using left/right keys, navigating to previously typed commands using up/down keys, and performing autocompletion using 'tab' key. 

.. note:: This feature relies on ANSI escape sequence support in the terminal application. As such, serial monitors which display raw UART data can not be used together with the line editing library. If you see ``[6n`` or similar escape sequence when running get_started/console example instead of a command prompt (``[esp32]>``), it means that the serial monitor does not support escape sequences. Programs which are known to work are  GNU screen, minicom, and idf_monitor.py (which can be invoked using ``make monitor`` from project directory).

Here is an overview of functions provided by `linenoise`_ library.

Configuration
^^^^^^^^^^^^^

Linenoise library does not need explicit initialization. However, some configuration defaults may need to be changed before invoking the main line editing function.

``linenoiseClearScreen``
  Clear terminal screen using an escape sequence and position the cursor at the top left corner.

``linenoiseSetMultiLine``
  Switch between single line and multi line editing modes. In single line mode, if the length of the command exceeds the width of the terminal, the command text is scrolled within the line to show the end of the text. In this case the beginning of the text is hidden. Single line needs less data to be sent to refresh screen on each key press, so exhibits less glitching compared to the multi line mode. On the flip side, editing commands and copying command text from terminal in single line mode is harder. Default is single line mode.


Main loop
^^^^^^^^^

``linenoise``
  In most cases, console applications have some form of read/eval loop. ``linenoise`` is the single function which handles user's key presses and returns completed line once 'enter' key is pressed. As such, it handles the 'read' part of the loop.

``linenoiseFree``
  This function must be called to release the command line buffer obtained from ``linenoise`` function.

Hints and completions
^^^^^^^^^^^^^^^^^^^^^

``linenoiseSetCompletionCallback``
  When user presses 'tab' key, linenoise library invokes completion callback. The callback should inspect the contents of the command typed so far and provide a list of possible completions using calls to ``linenoiseAddCompletion`` function. ``linenoiseSetCompletionCallback`` function should be called to register this completion callback, if completion feature is desired.

  ``console`` component provides a ready made function to provide completions for registered commands, ``esp_console_get_completion`` (see below).

``linenoiseAddCompletion``
  Function to be called by completion callback to inform the library about possible completions of the currently typed command.

``linenoiseSetHintsCallback``
  Whenever user input changes, linenoise invokes hints callback. This callback can inspect the command line typed so far, and provide a string with hints (which can include list of command arguments, for example). The library then displays the hint text on the same line where editing happens, possibly with a different color.

``linenoiseSetFreeHintsCallback``
  If hint string returned by hints callback is dynamically allocated or needs to be otherwise recycled, the function which performs such cleanup should be registered via ``linenoiseSetFreeHintsCallback``.


History
^^^^^^^

``linenoiseHistorySetMaxLen``
  This function sets the number of most recently typed commands to be kept in memory. Users can navigate the history using up/down arrows.

``linenoiseHistoryAdd``
  Linenoise does not automatically add commands to history. Instead, applications need to call this function to add command strings to the history.

``linenoiseHistorySave``
  Function saves command history from RAM to a text file, for example on an SD card or on a filesystem in flash memory.

``linenoiseHistoryLoad``
  Counterpart to ``linenoiseHistorySave``, loads history from a file.

``linenoiseHistoryFree``
  Releases memory used to store command history. Call this function when done working with linenoise library.

Splitting of command line into arguments
----------------------------------------

``console`` component provides ``esp_console_split_argv`` function to split command line string into arguments. The function returns the number of arguments found (``argc``) and fills an array of pointers which can be passed as ``argv`` argument to any function which accepts arguments in ``argc, argv`` format.

The command line is split into arguments according to the following rules:

- Arguments are separated by spaces
- If spaces within arguments are required, they can be escaped using ``\`` (backslash) character.
- Other escape sequences which are recognized are ``\\`` (which produces literal backslash) and ``\"``, which produces a double quote.
- Arguments can be quoted using double quotes. Quotes may appear only in the beginning and at the end of the argument. Quotes within the argument must be escaped as mentioned above. Quotes surrounding the argument are stripped by ``esp_console_split_argv`` function.

Examples:

- ``abc def 1 20 .3`` ⟶ [ ``abc``, ``def``, ``1``, ``20``, ``.3`` ]
- ``abc "123 456" def`` ⟶ [ ``abc``, ``123 456``, ``def`` ]
- ```a\ b\\c\"`` ⟶ [ ``a b\c"`` ]


Argument parsing
----------------

For argument parsing, ``console`` component includes `argtable3`_ library. Please see `tutorial`_ for an introduction to `argtable3`_. Github repository also includes `examples`_.

.. _argtable3: http://www.argtable.org/
.. _linenoise: https://github.com/antirez/linenoise
.. _tutorial: http://www.argtable.org/tutorial/
.. _examples: https://github.com/argtable/argtable3/tree/master/examples


Command registration and dispatching
------------------------------------

``console`` component includes utility functions which handle registration of commands, matching commands typed by the user to registered ones, and calling these commands with the arguments given on the command line.

Application first initializes command registration module using a call to ``esp_console_init``, and calls ``esp_console_cmd_register`` function to register command handlers.

For each command, application provides the following information (in the form of ``esp_console_cmd_t`` structure):

- Command name (string without spaces)
- Help text explaining what the command does
- Optional hint text listing the arguments of the command. If application uses Argtable3 for argument parsing, hint text can be generated automatically by providing a pointer to argtable argument definitions structure instead.
- The command handler function.

A few other functions are provided by the command registration module:

``esp_console_run``
  This function takes the command line string, splits it into argc/argv argument list using ``esp_console_split_argv``, looks up the command in the list of registered components, and if it is found, executes its handler.

``esp_console_split_argv``
  Adds ``help`` command to the list of registered commands. This command prints the list of all the registered commands, along with their arguments and help texts.

``esp_console_get_completion``
  Callback function to be used with ``linenoiseSetCompletionCallback`` from linenoise library. Provides completions to linenoise based on the list of registered commands.

``esp_console_get_hint``
  Callback function to be used with ``linenoiseSetHintsCallback`` from linenoise library. Provides argument hints for registered commands to linenoise.

Example
-------

Example application illustrating usage of the ``console`` component is available in ``examples/system/console`` directory. This example shows how to initialize UART and VFS functions, set up linenoise library, read and handle commands from UART, and store command history in Flash. See README.md in the example directory for more details.



