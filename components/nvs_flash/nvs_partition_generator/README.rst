NVS Partition Generator Utility
===============================

Introduction
------------

:component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py` utility is designed to help create a binary file, compatible with NVS architecture defined in :doc:`Non-Volatile Storage </api-reference/storage/nvs_flash>`, based on user provided key-value pairs in a CSV file.
Utility is ideally suited for generating a binary blob, containing data specific to ODM/OEM, which can be flashed externally at the time of device manufacturing. This helps manufacturers set unique value for various parameters for each device, e.g. serial number, while using same application firmware for all devices.

Prerequisites
-------------
To use this utility in encryption mode, the following packages need to be installed:
    - cryptography package

This dependency is already captured by including these packages in `requirement.txt` in top level IDF directory.

CSV file format
---------------

Each row of the .csv file should have 4 parameters, separated by comma. Below is the description of each of these parameters:

Key
	Key of the data. Data can later be accessed from an application via this key.

Type
	Supported values are ``file``, ``data`` and ``namespace``.

Encoding
    Supported values are: ``u8``, ``i8``, ``u16``, ``u32``, ``i32``, ``string``, ``hex2bin``, ``base64`` and ``binary``. This specifies how actual data values are encoded in the resultant binary file. Difference between ``string`` and ``binary`` encoding is that ``string`` data is terminated with a NULL character, whereas ``binary`` data is not.

    .. note:: For ``file`` type, only ``hex2bin``, ``base64``, ``string`` and ``binary`` is supported as of now.

Value
	Data value.

.. note:: Encoding and Value cells for ``namespace`` field type should be empty. Encoding and Value of ``namespace`` is fixed and isn't configurable. Any value in these cells are ignored.

.. note:: First row of the CSV file should always be column header and isn't configurable.

Below is an example dump of such CSV file::

    key,type,encoding,value     <-- column header
    namespace_name,namespace,,  <-- First entry should be of type "namespace"
    key1,data,u8,1
    key2,file,string,/path/to/file

.. note:: Make sure there are no spaces before and after ',' in CSV file.

NVS Entry and Namespace association
-----------------------------------

When a new namespace entry is encountered in the CSV file, each follow-up entries will be part of that namespace, until next namespace entry is found, in which case all the follow-up entries will be part of the new namespace.

.. note:: First entry in a CSV file should always be ``namespace`` entry.

Multipage Blob Support
----------------------

By default, binary blobs are allowed to span over multiple pages and written in the format mentioned in section :ref:`structure_of_entry`. 
If older format is intended to be used, the utility provides an option to disable this feature.

Encryption Support
-------------------
This utility allows you to create an enrypted binary file also. Encryption used is AES-XTS encryption. Refer to :ref:`nvs_encryption` for more details.

Running the utility
-------------------

*Usage*::

    python nvs_partition_gen.py [--version {v1,v2}] input output

You can run this utility in two modes:
    -   Normal mode - Binary generated in this mode is an unencrypted binary file.
    -   Encryption mode - Binary generated in this mode is an encrypted binary file.

*In normal mode:*

    A sample CSV file is provided with the utility. You can run the utility using below command::

        python nvs_partition_generator.py sample.csv sample.bin

*In encryption mode:*

    You can run the utility using below commands:

            -   By taking encryption keys as an input file. A sample encryption keys file is provided with the utility::

                   python nvs_partition_gen.py sample.csv sample_encrypted.bin --encrypt True --keyfile testdata/keys.txt

            -   By enabling generation of encryption keys::

                   python nvs_partition_gen.py sample.csv sample_encrypted.bin --encrypt True --keygen True


.. note:: In encryption mode, this utility creates a binary file named `encryption_keys.bin` containing the encryption keys used. This binary file is compatible with NVS key-partition structure. Refer to :ref:`nvs_key_partition` for more details.


You can also provide the format version number while running this utility:
    - Multipage Blob Support Enabled (v2)
    - Multipage Blob Support Disabled (v1)


*Multipage Blob Support Enabled (v2):*

You can run the utility in this format by setting the version parameter to v2, as shown below.
A sample CSV file is provided with the utility::

    python nvs_partition_gen.py sample_multipage_blob.csv partition_multipage_blob.bin --version v2


*Multipage Blob Support Disabled (v1):*

You can run the utility in this format by setting the version parameter to v1, as shown below.
A sample CSV file is provided with the utility::

    python nvs_partition_gen.py sample_singlepage_blob.csv partition_single_page.bin --version v1

+------------------------+----------------------------------------------------------------------------------------------+
|   Arguments            |                                     Description                                              |
+========================+==============================================================================================+
| input                  | Path to CSV file to parse. Will use stdin if omitted                                         |
+------------------------+----------------------------------------------------------------------------------------------+
| output                 | Path to output converted binary file. Will use stdout if omitted                             |
+------------------------+----------------------------------------------------------------------------------------------+
| size                   | Size of NVS Partition in hex (must be multiple of 4096). Eg. 0x1000                          |
+------------------------+----------------------------------------------------------------------------------------------+
| --version {v1,v2}      |  Set version. Default: v2                                                                    |
+-------------------------------+---------------------------------------------------------------------------------------+
| --keygen {True,False}  |  Generate keys for encryption. Default: False                                                |
|                        |  (Applicable only if encryption mode is true)                                                |
+------------------------+----------------------------------------------------------------------------------------------+
| --encrypt {True,False} |  Set encryption mode. Default: False                                                         |
+------------------------+----------------------------------------------------------------------------------------------+
| --keyfile KEYFILE      | File having key for encryption (Applicable only if encryption mode is true)                  |
+------------------------+----------------------------------------------------------------------------------------------+

Caveats
-------
-  Utility doesn't check for duplicate keys and will write data pertaining to both keys. User needs to make sure keys are distinct.
-  Once a new page is created, no data will be written in the space left in previous page. Fields in the CSV file need to be ordered in such a way so as to optimize memory.
-  64-bit datatype is not yet supported.
