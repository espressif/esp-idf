.. _app_trace-gcov-source-code-coverage:

Gcov (Source Code Coverage)
===========================

:link_to_translation:`zh_CN:[中文]`

Gcov is a source code coverage analysis tool. In ESP-IDF, coverage data generated on the target is dumped to the host over apptrace (JTAG or UART), where it is turned into standard ``.gcda`` files and processed with the usual host-side tools.

Report generation also needs the ``.gcno`` notes files that the compiler generates at build time for each source compiled with ``--coverage``. The host-side tools combine the runtime ``.gcda`` counts with the ``.gcno`` files and the original sources to produce the coverage report.

Gcov uses the tracing infrastructure for host data transfer, but it does not yet fully follow the :doc:`esp_trace <index>` encoder/transport model. In particular, it is tied to the apptrace transport (JTAG or UART) and does not support selecting a custom transport.

Coverage support is provided by the managed component `espressif/esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_. Add it to your project's ``idf_component.yml``:

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

Coverage data can be dumped either at a hard-coded point in your application (over apptrace via JTAG or UART) or on demand from the host via the OpenOCD ``esp gcov`` command (JTAG only). For the full setup, configuration options, and command usage, see README of the component linked above.

Application Example
-------------------

- :example:`system/gcov` demonstrates how to add code coverage to a project and collect coverage data over JTAG.
