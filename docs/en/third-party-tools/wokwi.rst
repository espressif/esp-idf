.. _wokwi:

Wokwi
#####

:link_to_translation:`zh_CN:[中文]`

What Is Wokwi?
~~~~~~~~~~~~~~

`Wokwi <https://wokwi.com/esp32>`__ is an online electronics simulator. You can use it to simulate most Espressif chips and supported parts and sensors.

Wokwi provides a browser-based interface and IDE integrations that offer a simple and intuitive way to start coding your next IoT project within seconds. It supports ESP-IDF projects and provides Wi-Fi simulation, virtual logic analyzers, advanced debugging with GDB and screenshot capture for automated testing.

Key Features
~~~~~~~~~~~~

Wokwi provides many features for embedded development:

- **Wi-Fi Simulation**: Test IoT projects without physical hardware.
- **Virtual Logic Analyzers**: Debug digital signals and timing.
- **Advanced GDB Debugging**: Set breakpoints and inspect variables.
- ``idf.py`` **Integration**: Use the familiar ``idf.py`` interface to control Wokwi (see :ref:`Set Up a Project with idf-wokwi <idf-wokwi-setup>`).
- **VS Code Integration**: Develop and simulate directly from VS Code.
- **CLion Plugin**: Professional development workflow with `CLion <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__.
- **Screenshot Capture**: Automated visual testing for CI/CD.
- **Custom Chips API**: Build your own virtual chips in addition to the main MCU.

.. note::

   Simulation results may differ from real hardware. Always test your project on actual hardware before deployment.

Installation
~~~~~~~~~~~~

Wokwi can be used in the following ways:

**Browser-based (Online)**
   Visit `wokwi.com <https://wokwi.com/esp32>`__ to start simulating immediately in your browser. No installation required.

**Via** ``idf.py`` **using the** ``idf-wokwi`` **package**
   Install the `idf-wokwi <https://pypi.org/project/idf-wokwi/>`__ package to integrate Wokwi controls directly into ``idf.py``.

**VS Code Extension**
   Install the `Wokwi for VS Code extension <https://docs.wokwi.com/vscode/getting-started>`__ to integrate simulation directly into your development environment.

**CLion Plugin**
   Install the `Wokwi Simulator plugin <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__ for CLion to use Wokwi with professional IDE features and embedded development tools.

Configuration
~~~~~~~~~~~~~

**Set Up a Project with wokwi-cli**

For local development and CI/CD integration, you can use ``wokwi-cli`` to configure your ESP-IDF project for Wokwi simulation.

To install ``wokwi-cli``, please refer to the `official Wokwi CLI installation guide <https://docs.wokwi.com/wokwi-ci/cli-installation>`__.

Once installed, run the following command in your ESP-IDF project directory:

.. code-block:: bash

   wokwi-cli init

The ``wokwi-cli init`` command will prompt you with a few questions and automatically create the necessary configuration files in your project directory.

Wokwi projects are configured using two main files:

- ``wokwi.toml``: A configuration file that specifies firmware paths, ELF files for debugging, and simulator settings.
- ``diagram.json``: A circuit diagram file that describes the board, connected components, and their wiring.

For detailed information about configuration files, see the `Wokwi project configuration guide <https://docs.wokwi.com/vscode/project-config>`__.

You are encouraged to read the official `ESP32 Simulation guide <https://docs.wokwi.com/guides/esp32>`__ to understand which boards, languages, and features are supported by Wokwi.

**Set Up a Project with** ``idf-wokwi``

.. _idf-wokwi-setup:

Similar to ``wokwi-cli``, you can use ``idf-wokwi`` for local development and CI/CD integration. The advantage of using ``idf-wokwi`` is not only that the workflow remains within ``idf.py``, but also that Wokwi will be able to automatically fetch information from ESP-IDF. This avoids the need for ``wokwi.toml`` and ``diagram.json`` to exist — Wokwi will implicitly generate these files.

**Choose Between** ``wokwi-cli`` **and** ``idf-wokwi``

.. list-table:: Comparison of Wokwi integration options
   :header-rows: 1
   :widths: 25 38 37

   * - Feature
     - ``wokwi-cli``
     - ``idf-wokwi``
   * - **Config files**
     - Required (``wokwi.toml`` and ``diagram.json``)
     - Auto-generated from ESP-IDF
   * - **Build integration**
     - Manual (build first, then simulate)
     - Automatic via ``idf.py``
   * - **ESP-IDF version**
     - Any version
     - 6.0 or greater only
   * - **Use case**
     - Non-ESP-IDF projects, custom workflows
     - ESP-IDF 6.0+ projects, native workflow

Prerequisites
~~~~~~~~~~~~~

Before using ``idf-wokwi``, ensure that you have:

- ESP-IDF 6.0 or greater (``idf.py`` module extensions require this)
- Wokwi API token (You can create an API token on the `Wokwi CI Dashboard <https://wokwi.com/dashboard/ci>`__)

Getting Started
~~~~~~~~~~~~~~~

Install and configure ``idf-wokwi``:

.. code-block:: bash

   # Install the package
   pip install idf-wokwi

   # Set your API token
   export WOKWI_CLI_TOKEN=your_token_here

   # Run simulation
   idf.py wokwi

.. important::

   Module extensions to ``idf.py`` are only supported on ESP-IDF versions 6.0 or greater.

.. tip::

   ``idf.py wokwi`` automatically builds your project before simulation. Skip build with ``idf.py wokwi --no-build`` if firmware already exists.

Available CLI Options
~~~~~~~~~~~~~~~~~~~~~

.. list-table:: ``idf.py wokwi`` command options
   :header-rows: 1
   :widths: 30 70

   * - Option
     - Description
   * - ``--diagram-file``
     - Path to ``diagram.json`` (defaults to project root)
   * - ``--timeout``
     - Simulation timeout in milliseconds (exit code 42 on timeout)
   * - ``--expect-text``
     - Exit successfully when this text appears in serial output
   * - ``--fail-text``
     - Exit with error when this text appears in serial output
   * - ``--expect-regex``
     - Exit successfully when this regex matches a serial output line
   * - ``--fail-regex``
     - Exit with error when this regex matches a serial output line

Example Output
~~~~~~~~~~~~~~

Running ``idf.py wokwi`` produces output similar to the following:

.. code-block:: none

   $ idf.py wokwi
   Running Wokwi simulation...
   Firmware: build/your_project.bin
   ELF: build/your_project.elf
   Simulator ready at: https://wokwi.com/...
   Press Ctrl+C to stop...

   I (123) main: Hello, World!
   I (145) main: System initialized

Troubleshooting
~~~~~~~~~~~~~~~

- ``Module not found`` or ``No module named 'idf_wokwi'``

   Verify that ``idf-wokwi`` is installed in the same Python environment used by ESP-IDF:

   .. code-block:: bash

      pip show idf-wokwi
      xtensa-esp32-elf-gdb --version  # Verify ESP-IDF environment

- ``idf.py: error: no such option: wokwi``

   Your ESP-IDF version is earlier than 6.0 and does not support module extensions. Upgrade ESP-IDF or use ``wokwi-cli`` instead.

- ``Invalid token`` or ``401 Unauthorized``

   Verify that ``WOKWI_CLI_TOKEN`` is set correctly:

   .. code-block:: bash

      echo $WOKWI_CLI_TOKEN  # Should show your token, not empty

   Get a new token at `Wokwi CI Dashboard <https://wokwi.com/dashboard/ci>`__ if needed.

CI/CD Integration
~~~~~~~~~~~~~~~~~~

For automated testing in CI/CD pipelines, ``idf-wokwi`` integrates seamlessly with GitHub Actions:

.. code-block:: yaml

   - name: Simulate with Wokwi
     run: |
       export WOKWI_CLI_TOKEN=${{ secrets.WOKWI_CLI_TOKEN }}
       idf.py wokwi --timeout 30000 --expect-text "Tests passed"

.. note::

   Store your ``WOKWI_CLI_TOKEN`` as a secret in your CI/CD platform (e.g., GitHub Secrets). Never commit tokens to the repository.

For automated testing frameworks, also see the :ref:`wokwi-pytest-embedded` section below.

Additional documentation is available on the official `Wokwi ESP-IDF simulation extension usage <https://docs.wokwi.com/wokwi-ci/idf-wokwi-usage>`__ page.

IDE Integration
~~~~~~~~~~~~~~~

**VS Code**

To use Wokwi with VS Code:

1. Install the `Wokwi for VS Code extension <https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode>`__.
2. Create ``wokwi.toml`` and ``diagram.json`` files in your project.
3. Press ``F1`` and select ``Wokwi: Start Simulator`` to begin simulation.

**CLion**

The `Wokwi Simulator plugin for CLion <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__ provides:

- Integration with CLion's embedded development tools.
- Professional debugging workflow.
- Support for ESP-IDF projects.
- Seamless simulator access from the IDE.

**Espressif IDE**

Version 2.9.0 and later of `Espressif IDE <https://developer.espressif.com/blog/how-to-use-wokwi-simulator-with-espressif-ide/>`__ includes Wokwi integration:

- Build applications in the IDE.
- Flash directly to the Wokwi simulator.
- View serial monitor output in the IDE console while communicating with the simulator.

.. _wokwi-pytest-embedded:

Testing with pytest-embedded
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Wokwi integrates with ESP-IDF's testing framework through `pytest-embedded-wokwi <https://pypi.org/project/pytest-embedded-wokwi/>`__. This enables:

- Automated unit and integration testing.
- CI/CD pipeline integration with GitHub Actions.
- Screenshot validation for visual testing.
- Regression testing without physical hardware.

**Basic Usage**

Run Wokwi simulations from the command line with ``wokwi-cli``:

.. code-block:: bash

   # Run simulation and expect specific text in output
   wokwi-cli --timeout 5000 --expect-text "Hello World"

   # Take a screenshot after 4.5 seconds
   wokwi-cli --screenshot-part esp --screenshot-time 4500 --screenshot-file screenshot.png

   # Save serial output to a file
   wokwi-cli --serial-log-file output.log --timeout 10000

For a complete project template with CI/CD configured, see the `ESP-IDF Project Template with CI and Wokwi <https://github.com/wokwi/wokwi-esp-test-template>`__.

**Related Resources**

- `pytest-embedded-wokwi Documentation <https://pypi.org/project/pytest-embedded-wokwi/>`__
- `ESP-IDF Tests with Pytest Guide <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/contribute/esp-idf-tests-with-pytest.html>`__
- `pytest-embedded 2.x Documentation <https://docs.espressif.com/projects/pytest-embedded>`__

Advanced Features
~~~~~~~~~~~~~~~~~

**Debugging**

Wokwi provides built-in GDB server support for advanced debugging:

1. Add to ``wokwi.toml``:

   .. code-block:: toml

      [wokwi]
      version = 1
      firmware = 'build/your_app.bin'
      elf = 'build/your_app.elf'
      gdbServerPort = 3333

2. Create a VS Code launch configuration (``.vscode/launch.json``):

   .. code-block:: json

      {
        "version": "0.2.0",
        "configurations": [
          {
            "name": "Wokwi GDB",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/your_app.elf",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "miDebuggerPath": "xtensa-esp32-elf-gdb",
            "miDebuggerServerAddress": "localhost:3333"
          }
        ]
      }

3. Start simulator with ``F1`` → ``Wokwi: Start Simulator and Wait for Debugger``.
4. Press ``F5`` in VS Code to attach the debugger.

For more details, see the `Wokwi debugging guide <https://docs.wokwi.com/guides/debugger>`__.

**Wi-Fi Networking**

Wokwi supports Wi-Fi simulation for IoT projects. See the `ESP32 Wi-Fi networking guide <https://docs.wokwi.com/guides/esp32-wifi>`__ for information on:

- Connecting to Wi-Fi networks.
- MQTT, HTTP, and HTTPS protocols.
- WebSocket communication.
- Network testing without physical hardware.

**CI/CD Integration**

Automate testing in GitHub Actions using Wokwi:

.. code-block:: yaml

   - name: Simulate & take screenshot
     run: |
       wokwi-cli \
         --screenshot-part "esp" \
         --screenshot-time 5000 \
         --screenshot-file "screenshot-${{ matrix.board }}.png" \
         "boards/${{ matrix.board }}"

   - name: Upload screenshot
     uses: actions/upload-artifact@v4
     with:
       name: screenshot-${{ matrix.board }}
       path: screenshot-${{ matrix.board }}.png

This enables automated visual regression testing and ensures consistent behavior across commits.

Resources
~~~~~~~~~

**Videos**

- `DevCon24 - Flash Less, Do More: The Magic of Virtual Hardware <https://youtu.be/8PTYnNFK1pw>`__: Learn about the power of simulation for embedded development

**Documentation**

- `Wokwi Documentation <https://docs.wokwi.com>`__: Comprehensive resource for all Wokwi features
- `ESP32 Simulation Guide <https://docs.wokwi.com/guides/esp32>`__: Supported boards, languages, and features
- `ESP32 Wi-Fi Networking <https://docs.wokwi.com/guides/esp32-wifi>`__: Wi-Fi simulation for IoT projects
- `VS Code Integration <https://docs.wokwi.com/vscode/getting-started>`__: Setup and configuration guide
- `CLion Plugin <https://plugins.jetbrains.com/plugin/23826-wokwi-simulator>`__: CLion integration details
- `Wokwi Articles on Developer Portal <https://developer.espressif.com/tags/wokwi/>`__: Collection of tutorials and use cases

**Getting Help**

- `Wokwi Community <https://discord.gg/wokwi>`__: Join the Wokwi Discord server to get help from the community

