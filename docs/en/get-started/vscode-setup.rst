********************************
Getting Started with VS Code IDE
********************************

:link_to_translation:`zh_CN:[中文]`

We have official support for VS Code and we aim to provide complete end to end support for all actions related to ESP-IDF namely build, flash, monitor, debug, tracing, core-dump, System Trace Viewer, etc.

Quick Install Guide
===================

Recommended way to install ESP-IDF Visual Studio Code Extension is by downloading it from `VS Code Marketplace <https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension>`_ or following `Quick Installation Guide <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_.

Review the `tutorials <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/toc.md>` for ESP-IDF Visual Studio Code Extension to learn how to use all features.

Supported Features
==================

* **Setup**, will help you to quickly install ESP-IDF and its relevant toolchain with just few clicks.
* **Build**, with one click build and multi target build, you can easily build and deploy your applications.
* **Flash**, with both UART and JTAG flash out of the box.
* **Monitoring** comes with built-in terminal where you can trigger IDF Monitor Commands from within VS Code as you are used to in traditional terminals.
* `Debugging <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/debugging.md>`, with out of box hardware debugging and also support for postmortem debugging like core-dump, you can analyze the bugs with convenience.
* **GUI Menu Config**, provides with simplified UI for configuring your chip.
* **App & Heap Tracing**, provides support for collecting traces from your application and simplified UI for analyzing them.
* **System View Tracing Viewer**, aims to read and display the *.svdat* files into trace UI, we also support multiple core tracing views.
* **IDF Size Analysis Overview** presents an UI for binary size analysis.
* **`Rainmaker Cloud <https://rainmaker.espressif.com/>`_**, we have inbuilt Rainmaker Cloud support where you can edit/read state of your connected IoT devices easily.
* **Code Coverage**, we have inbuilt code coverage support which shall highlight in color which line have been covered. We also render the existing HTML report directly inside the IDE.


Bugs & Feature Requests
=======================

If you face an issue with certain feature of VS Code or VS Code in general we recommend to ask your question in the `forum <http://esp32.com/viewforum.php?f=40>`_, or open a `github issue <https://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`_ for our dev teams to review.

We also welcome new feature request, most of the features we have today is result of people asking it to implement, or improve certain aspect of the extension, `raise your feature request on github <http://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`_.
