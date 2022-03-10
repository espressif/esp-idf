********************************
Getting Started with VS Code IDE
********************************

:link_to_translation:`zh_CN:[中文]`

We have official support for Visual Studio Code and we aim to provide complete end-to-end support for all actions related to ESP-IDF: namely build, flash, monitor, debug, tracing, core-dump, System Trace Viewer, etc.

Quick Install Guide
===================

The recommended way to install the ESP-IDF Visual Studio Code Extension is by downloading it from `VS Code Marketplace <https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension>`_ or following `Quick Installation Guide <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_.

Review the `tutorials <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/toc.md>`_ for the ESP-IDF Visual Studio Code Extension to learn how to use all of the features.

Supported Features
==================

* **Setup**, will help you to quickly install ESP-IDF and its relevant toolchain with just a few clicks.
* **Build**, with one-click build and multi-target build, you can easily build and deploy your applications.
* **Flash**, with both UART and JTAG flash out-of-the-box.
* **Monitoring**, comes with a built-in terminal, you can trigger IDF Monitor Commands from within VS Code as you are used to in traditional terminals.
* **Debugging**, with out-of-the-box hardware `debugging <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/debugging.md>`_.
* **GUI Menu Config**, provides a simplified UI for configuring your chip.
* **App & Heap Tracing**, provides support for collecting traces from your application, and a simplified UI for analyzing them.
* **System View Tracing Viewer**, aims to read and display the *.svdat* files into the trace UI (we also support multiple core tracing views).
* **IDF Size Analysis Overview** presents a UI for binary size analysis.
* **Rainmaker Cloud**, inbuilt Rainmaker Cloud support where you can edit/read the state of your connected IoT devices easily. For more information see the `ESP Rainmaker <https://rainmaker.espressif.com/>`_ page.
* **Code Coverage**, inbuilt code coverage support with color highlights showing which lines have been covered. The HTML report renders directly inside the IDE.

Bugs & Feature Requests
=======================

If you face an issue with certain feature of VS Code or VS Code in general we recommend you ask your question in the `forum <http://esp32.com/viewforum.php?f=40>`_, or open a `GitHub Issue <https://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`_ for our dev teams to review.

We also welcome new feature requests. Most of the features we have today are a result of people asking for them to be implemented. To improve certain aspects of the extension, `raise your feature request on GitHub <http://github.com/espressif/vscode-esp-idf-extension/issues/new/choose>`_.
