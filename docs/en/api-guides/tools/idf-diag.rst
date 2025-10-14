********
IDF Diag
********

:link_to_translation:`zh_CN:[中文]`

IDF Diag is a tool intended to speed up and assist in troubleshooting problems associated with the ESP-IDF environment and projects. It integrates the esp-idf-diag_ Python package into :ref:`idf.py`. IDF Diag gathers information about installed IDF tools, the Python environment, project artifacts, logs and other relevant data, storing them in a report directory. The report directory can be easily reviewed and supplemented with files that were not automatically collected but are considered pertinent to the reported issue. Once the report directory is complete, it can be compressed into a zip file and shared, such as by attaching it to a GitHub issue.

Creating Report Directory
^^^^^^^^^^^^^^^^^^^^^^^^^

You can generate a report directory containing diagnostic information using the default configuration by:

.. code-block:: bash

    $ idf.py diag

The report directory is created in the current directory and named ``idf-diag-UUID``, where ``UUID`` is a randomly generated Universally Unique Identifier. For instance, it might look like ``idf-diag-5aaa949b-40dc-4d53-96f1-1280c801585a``. You can choose a different directory path for the output by using the ``--output`` option. If the specified output directory already exists, the ``idf.py diag`` command will not proceed unless the ``--force`` option is provided.

Diagnostic data is gathered using configuration files known as ``recipes``. These are YAML files that define which command outputs should be included in the report and which files need to be copied. You can view the available recipes by using the command ``idf.py --list``. By default, all ``builtin`` recipes are used, but this can be modified with the ``--recipe`` option or by choosing recipes with specific ``tags`` using the ``--tag`` option. For more information, see ``idf.py diag --help`` and the detailed description of recipes_.

By default, the diagnostic data in the report directory is redacted to remove potentially sensitive information, such as passwords and tokens in URLs. This redaction process uses a configuration file known as ``purge``, a YAML file that contains redaction rules made up of regular expressions and replacement strings. You can use your own custom purge file by using the ``--purge`` option. For further details, refer to the purge_ file description.

.. note::

    Please make sure to thoroughly check the report directory for any sensitive information before sharing, and remove files you do not want to share. Kindly include any additional files you find relevant that were not automatically added.

Creating Zip Archive
^^^^^^^^^^^^^^^^^^^^

After the report directory has been checked and is ready, it can be archived into a zip file by:

.. code-block:: bash

    $ idf.py diag --zip <REPORT_DIRECTORY>

The ``REPORT_DIRECTORY`` refers to the directory created by the ``idf.py diag`` command. By default, the zip archive is named after the ``REPORT_DIRECTORY`` with a ``.zip`` extension. For instance, it might be named ``idf-diag-5aaa949b-40dc-4d53-96f1-1280c801585a.zip``. You can use the ``--output`` option to specify a different filename.

The report archive can now be shared, for example, by attaching it to a GitHub issue. It might provide key insights into the project environment, helping with issue diagnosis.


Additional Information
^^^^^^^^^^^^^^^^^^^^^^

For more information please refer to the esp-idf-diag_ project and ``idf.py diag --help``.

.. _esp-idf-diag: https://github.com/espressif/esp-idf-diag
.. _recipes: https://github.com/espressif/esp-idf-diag/blob/master/esp_idf_diag/data/recipes/README.md
.. _purge: https://github.com/espressif/esp-idf-diag/tree/master/esp_idf_diag/data/purge
