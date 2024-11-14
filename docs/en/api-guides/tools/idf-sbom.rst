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

To generate NTIA compliant SBOM file for an ESP-IDF project, you can use the `esp-idf-sbom`_ tool. This tool generates SBOM file in the Software Package Data Exchange (`SPDX`_) format version 2.2 and allows to scan it for known vulnerabilities against the National Vulnerability Database (`NVD`_) based on the Common Platform Enumeration (`CPE`_).

This document outlines the basic usage and features of `esp-idf-sbom`_. For more detailed information, please refer to the documentation within the `esp-idf-sbom`_ project.


Installation
^^^^^^^^^^^^
Currently, `esp-idf-sbom`_ is not integrated into the ESP-IDF and needs to be installed separately from PyPI by using

.. code-block:: bash

    $ pip install esp-idf-sbom

Once installed, the `esp-idf-sbom`_ tool can be invoked as a Python module using the command ``python -m esp_idf_sbom``, or as a console script with the ``esp-idf-sbom`` command.


Generating SBOM for ESP-IDF project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
To generate the SBOM file for your project, start by building it using ``idf.py build``. The `esp-idf-sbom`_ tool requires the ``project_description.json`` file as input, which is created during the project's configuration. This file contains essential metadata, including details about components and references to build artifacts necessary for the `esp-idf-sbom`_ tool to produce the SBOM file. Typically, you can find it in the project's ``build`` directory.

To create a default SPDX SBOM file for your project, use

.. code-block:: bash

    $ esp-idf-sbom create -o sbom.spdx build/project_description.json

In the `SPDX` format, the project's application and its components are represented as ``SPDX Packages``, linked by ``SPDX relationships`` to illustrate the supply chain. By default, the generated SBOM contains an SPDX Package for each component involved in the build process. This implies that the SBOM might list components that were built but not ultimately included in the project's final binary image, as none of their symbols were used. Although these components appear in the SBOM, they do not have a relationship with the project's application ``SPDX Package``. For additional information on the SPDX format, please see the `SPDX specification`_.

To generate a minimal SBOM file that includes only the components linked to the project's application, use

.. code-block:: bash

    $ esp-idf-sbom create --rem-unused --rem-config -o sbom.spdx build/project_description.json

The SPDX SBOM produced might also include other information, such as licenses. The level of information in the generated SBOM can be modified using command-line options. For further details, please refer to `esp-idf-sbom`_.


Checking SBOM for vulnerabilities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `esp-idf-sbom`_ tools comes with a built-in vulnerability checker, but you have the option to use other tools supporting the SPDX SBOM format. For example, `cve-bin-tool`_ is a free and open-source tool that allows to scan SPDX SBOM for vulnerabilities.

To check the SBOM for vulnerabilities with `esp-idf-sbom`_ tool, use

.. code-block:: bash

    $ esp-idf-sbom check sbom.spdx

By default, this will create a report table that summarizes and details any potential vulnerabilities found during the scan. Below is an example of shortened report for a hypothetical project that intentionally uses an outdated version of the ``expat`` component, demonstrating what a vulnerability detection report might look like.

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


For detailed information about the component layout within the SPDX SBOM, the manifest files used to describe components, and all available options, please visit the `esp-idf-sbom`_ project GitHub page.


.. _esp-idf-sbom: https://github.com/espressif/esp-idf-sbom
.. _SBOM: https://en.wikipedia.org/wiki/Software_supply_chain
.. _NVD: https://nvd.nist.gov
.. _SPDX: https://spdx.dev
.. _SPDX specification: https://spdx.github.io/spdx-spec/v2.2.2/
.. _NTIA: https://www.ntia.gov
.. _CPE: https://nvd.nist.gov/products/cpe
.. _cve-bin-tool: https://github.com/intel/cve-bin-tool
.. _The Minimum Elements for a Software Bill of Materials (SBOM): https://www.ntia.doc.gov/files/ntia/publications/sbom_minimum_elements_report.pdf
