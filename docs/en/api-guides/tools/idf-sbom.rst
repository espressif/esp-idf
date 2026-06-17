********
IDF SBOM
********

A Software Bill of Materials (`SBOM`_) is a comprehensive inventory that identifies all the components and libraries within a software application, along with their relationships. This is essential for the software supply chain, as it enables the tracking and management of components obtained from various vendors or open-source projects.

The component in an SBOM is described with information such as its name and version, among other details that may include, for example, licenses. This helps in identifying potential vulnerabilities, ensuring compliance with licensing requirements, and managing risks associated with third-party components. The minimal data fields each component must contain, according to National Telecommunications and Information Administration (`NTIA`_), is described by `The Minimum Elements for a Software Bill of Materials (SBOM)`_.

.. list-table::
    :header-rows: 1
    :widths: 30 70
    :align: center

    * - Data Field
      - Description
    * - Supplier Name
      - The name of an entity that creates, defines, and identifies components.
    * - Component Name
      - Designation assigned to a unit of software defined by the original supplier.
    * - Version of the Component
      - Identifier used by the supplier to specify a change in software from a previously identified version.
    * - Other Unique Identifiers
      - Other identifiers that are used to identify a component, or serve as a look-up key for relevant databases.
    * - Dependency Relationship
      - Characterizing the relationship that an upstream component X is included in software Y.
    * - Author of SBOM Data
      - The name of the entity that creates the SBOM data for this component.
    * - Timestamp
      - Record of the date and time of the SBOM data assembly.

To generate an NTIA‑compliant SBOM file for an ESP‑IDF project, use the ``idf.py sbom-create`` command. This command generates an SBOM file in Software Package Data Exchange (`SPDX`_) format, version 2.2. The generated SBOM can then be scanned for known vulnerabilities in the National Vulnerability Database (`NVD`_) using Common Platform Enumeration (`CPE`_) identifiers with the ``idf.py sbom-check`` command.

The ``idf.py sbom-create`` and ``idf.py sbom-check`` commands provide basic integration of the `esp-idf-sbom`_ tool into ``idf.py``. For more detailed information, see the documentation in the `esp-idf-sbom`_ project.

Generating SPDX SBOM File for ESP-IDF Project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The generated SPDX SBOM file contains information derived from the project’s build artifacts. To ensure these artifacts are up-to-date, the ``idf.py sbom-create`` command depends on the ``idf.py build`` command. If the project has not yet been built, or if source files have changed, a build is automatically triggered before the SBOM is generated. This guarantees that the resulting SBOM always accurately reflects the current state of the project.

To generate a default minimal SPDX SBOM file for your project, run the following command in your project directory.

.. code-block:: bash

    $ idf.py sbom-create

By default, the SPDX SBOM file is created in the build directory and named after the project, with the ``.spdx`` extension. For example, for the ``hello_world`` example project, the file will be located at ``build/hello_world.spdx`` by default. The output location of the generated SPDX SBOM file can be changed using the ``--spdx-file`` option. For more information and additional options, see ``idf.py sbom-create --help``.

The ``idf.py sbom-create`` command generates the default SPDX SBOM file for a project. If more control is required over how the SPDX SBOM file is generated and what information it contains, refer to the `esp-idf-sbom`_ tool documentation. That documentation also provides detailed information about the component layout within SPDX SBOM files and the manifest files used to describe components.

Checking SPDX SBOM File for Vulnerabilities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``idf.py sbom-check`` command scans a project's SPDX SBOM file for known vulnerabilities. While this command is the primary method within ESP-IDF, you can also use any other third-party utility that supports the SPDX format, such as `cve-bin-tool`_.

To check an SPDX SBOM file for vulnerabilities, use the following syntax:

.. code-block:: bash

    $ idf.py sbom-check --spdx-file <path_to_file>

For example, to scan the SPDX SBOM file generated for the ``hello_world`` example, run:

.. code-block:: bash

    $ idf.py sbom-check --spdx-file build/hello_world.spdx


By default, the ``idf.py sbom-check`` command uses a local mirror of the `NVD`_ database. You can choose between two modes of operation:

* **Local Mirror (Default):** This requires approximately 900 MB of disk space. While the initial population of the database may take some time, this method is convenient for frequent, fast, and offline scans.
* **NVD REST API:** Use the ``--nvd-api`` option to query the NVD database directly online. This is better suited for occasional or ad hoc scans where you want to avoid the disk space overhead of a local mirror.

The command by default generates a report table summarizing any potential vulnerabilities found. Below is an example of a shortened report for a hypothetical project that intentionally uses an outdated version of the ``expat`` component to demonstrate the detection capabilities. For more information and additional options, see ``idf.py sbom-check --help``.

.. code-block:: bash

                                                            Report summary

    ┌───────────────────────────────────┬────────────────────────────────────────────────────────────────────────────────────────┐
    │ Date:                             │ 2024-11-13T14:20:23Z                                                                   │
    │ Project name:                     │ project-expat_test                                                                     │
    │ Project version:                  │ 6c23b9cdb268                                                                           │
    │ Vulnerability database:           │ NATIONAL VULNERABILITY DATABASE REST API (https://nvd.nist.gov)                        │
    │ Generated by tool:                │ esp-idf-sbom (0.19.1)                                                                  │
    │ Generated with command:           │ esp-idf-sbom check sbom.spdx                                                           │
    │ Number of scanned packages:       │ 32                                                                                     │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ CRITICAL CVEs found:              │ CVE-2024-45491, CVE-2024-45492                                                         │
    │ Packages affect by CRITICAL CVEs: │ libexpat                                                                               │
    │ Number of CRITICAL CVEs:          │ 2                                                                                      │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ HIGH CVEs found:                  │ CVE-2024-45490                                                                         │
    │ Packages affect by HIGH CVEs:     │ libexpat                                                                               │
    │ Number of HIGH CVEs:              │ 1                                                                                      │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ MEDIUM CVEs found:                │                                                                                        │
    │ Packages affect by MEDIUM CVEs:   │                                                                                        │
    │ Number of MEDIUM CVEs:            │ 0                                                                                      │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ LOW CVEs found:                   │                                                                                        │
    │ Packages affect by LOW CVEs:      │                                                                                        │
    │ Number of LOW CVEs:               │ 0                                                                                      │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ UNKNOWN CVEs found:               │                                                                                        │
    │ Packages affect by UNKNOWN CVEs:  │                                                                                        │
    │ Number of UNKNOWN CVEs:           │ 0                                                                                      │
    ├───────────────────────────────────┼────────────────────────────────────────────────────────────────────────────────────────┤
    │ All CVEs found:                   │ CVE-2024-45491, CVE-2024-45492, CVE-2024-45490                                         │
    │ All packages affect by CVEs:      │ libexpat                                                                               │
    │ Total number of CVEs:             │ 3                                                                                      │
    └───────────────────────────────────┴────────────────────────────────────────────────────────────────────────────────────────┘


                                                  Packages with Identified Vulnerabilities

    ┏━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
    ┃ Package  ┃ Version ┃     CVE ID     ┃ Base Score ┃ Base Severity ┃                          Information                           ┃
    ┡━━━━━━━━━━╇━━━━━━━━━╇━━━━━━━━━━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━━━━━━━━━╇━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┩
    │          │         │                │            │               │  Status  Analyzed                                              │
    │          │         │                │            │               │  CVSS    3.1                                                   │
    │          │         │                │            │               │  Vec.    CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H          │
    │          │         │                │            │               │  CPE     cpe:2.3:a:libexpat_project:libexpat:2.6.0:*:*:*:*:*:  │
    │ libexpat │  2.6.0  │ CVE-2024-45491 │    9.8     │   CRITICAL    │          *:*                                                   │
    │          │         │                │            │               │  Link    https://nvd.nist.gov/vuln/detail/CVE-2024-45491       │
    │          │         │                │            │               │  Desc.   An issue was discovered in libexpat before 2.6.3.     │
    │          │         │                │            │               │          dtdCopy in xmlparse.c can have an integer overflow    │
    │          │         │                │            │               │          for nDefaultAtts on 32-bit platforms (where UINT_MAX  │
    │          │         │                │            │               │          equals SIZE_MAX).                                     │
    ├──────────┼─────────┼────────────────┼────────────┼───────────────┼────────────────────────────────────────────────────────────────┤
    │          │         │                │            │               │  Status  Analyzed                                              │
    │          │         │                │            │               │  CVSS    3.1                                                   │
    │          │         │                │            │               │  Vec.    CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H          │
    │          │         │                │            │               │  CPE     cpe:2.3:a:libexpat_project:libexpat:2.6.0:*:*:*:*:*:  │
    │ libexpat │  2.6.0  │ CVE-2024-45492 │    9.8     │   CRITICAL    │          *:*                                                   │
    │          │         │                │            │               │  Link    https://nvd.nist.gov/vuln/detail/CVE-2024-45492       │
    │          │         │                │            │               │  Desc.   An issue was discovered in libexpat before 2.6.3.     │
    │          │         │                │            │               │          nextScaffoldPart in xmlparse.c can have an integer    │
    │          │         │                │            │               │          overflow for m_groupSize on 32-bit platforms (where   │
    │          │         │                │            │               │          UINT_MAX equals SIZE_MAX).                            │
    ├──────────┼─────────┼────────────────┼────────────┼───────────────┼────────────────────────────────────────────────────────────────┤
    │          │         │                │            │               │  Status  Analyzed                                              │
    │          │         │                │            │               │  CVSS    3.1                                                   │
    │          │         │                │            │               │  Vec.    CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:N/I:N/A:H          │
    │          │         │                │            │               │  CPE     cpe:2.3:a:libexpat_project:libexpat:2.6.0:*:*:*:*:*:  │
    │ libexpat │  2.6.0  │ CVE-2024-45490 │    7.5     │     HIGH      │          *:*                                                   │
    │          │         │                │            │               │  Link    https://nvd.nist.gov/vuln/detail/CVE-2024-45490       │
    │          │         │                │            │               │  Desc.   An issue was discovered in libexpat before 2.6.3.     │
    │          │         │                │            │               │          xmlparse.c does not reject a negative length for      │
    │          │         │                │            │               │          XML_ParseBuffer.                                      │
    └──────────┴─────────┴────────────────┴────────────┴───────────────┴────────────────────────────────────────────────────────────────┘
                            Newly identified vulnerabilities. Further analysis may be required for confirmation.


    [... additional lines removed ...]

Checking Components for Vulnerabilities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In addition to build artifacts, the ``idf.py sbom-create`` command uses SBOM manifest files located in component directories as a primary data source. You can use ``idf.py sbom-check`` to scan these component manifest files directly for vulnerabilities without needing to generate a complete project SBOM file.

If ``idf.py sbom-check`` is run without any options, it recursively searches the current directory for all SBOM manifest files and scans them.

For example, to check for vulnerabilities in your currently checked-out version of ESP-IDF, run the following from the ESP-IDF root directory:

.. code-block:: bash

    idf.py sbom-check

To scan a specific directory instead of the current one, use the ``--path`` option:

.. code-block:: bash

    idf.py sbom-check --path <path_to_directory>

.. _esp-idf-sbom: https://github.com/espressif/esp-idf-sbom
.. _SBOM: https://en.wikipedia.org/wiki/Software_supply_chain
.. _NVD: https://nvd.nist.gov
.. _SPDX: https://spdx.dev
.. _SPDX specification: https://spdx.github.io/spdx-spec/v2.2.2/
.. _NTIA: https://www.ntia.gov
.. _CPE: https://nvd.nist.gov/products/cpe
.. _cve-bin-tool: https://github.com/intel/cve-bin-tool
.. _The Minimum Elements for a Software Bill of Materials (SBOM): https://www.ntia.doc.gov/files/ntia/publications/sbom_minimum_elements_report.pdf
