NVS Partition Generator Utility
===============================

Introduction
------------

The utility :component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py` creates a binary file based on key-value pairs provided in a CSV file. The binary file is compatible with NVS architecture defined in :doc:`Non-Volatile Storage </api-reference/storage/nvs_flash>`.
This utility is ideally suited for generating a binary blob, containing data specific to ODM/OEM, which can be flashed externally at the time of device manufacturing. This allows manufacturers to generate many instances of the same application firmware with customized parameters for each device, such as a serial number.


Prerequisites
-------------
To use this utility in encryption mode, install the following packages:
    - cryptography package

All the required packages are included in `requirements.txt` in the root of the esp-idf directory.


CSV file format
---------------

Each line of a .csv file should contain 4 parameters, separated by a comma. The table below provides the description for each of these parameters.

+-----+-----------+----------------------------------------------------------------------+-----------------------------------------------------+
| No. | Parameter |                           Description                                |                        Notes                        |
+=====+===========+======================================================================+=====================================================+
| 1   | Key       | Key of the data. The data can be accessed later from                 |                                                     |
|     |           | an application using this key.                                       |                                                     |
+-----+-----------+----------------------------------------------------------------------+-----------------------------------------------------+
| 2   | Type      | Supported values are ``file``, ``data`` and ``namespace``.           |                                                     |
+-----+-----------+----------------------------------------------------------------------+-----------------------------------------------------+
| 3   | Encoding  | Supported values are: ``u8``, ``i8``, ``u16``, ``u32``,              | As of now, for the ``file`` type,                   |
|     |           | ``i32``, ``string``, ``hex2bin``, ``base64`` and ``binary``.         | only ``hex2bin``, ``base64``, ``string``,           |
|     |           | This specifies how actual data values are encoded in the             | and ``binary`` encoding is supported.               |
|     |           | resulting binary file. The difference between the ``string``         |                                                     |
|     |           | and ``binary`` encoding is that ``string`` data is terminated        |                                                     |
|     |           | with a NULL character, whereas ``binary`` data is not.               |                                                     |
+-----+-----------+----------------------------------------------------------------------+-----------------------------------------------------+
| 4   | Value     | Data value.                                                          | Encoding and Value cells for the ``namespace``      |
|     |           |                                                                      | field type should be empty. Encoding and Value      |
|     |           |                                                                      | of ``namespace`` is fixed and is not configurable.  |
|     |           |                                                                      | Any values in these cells are ignored.              |
+-----+-----------+----------------------------------------------------------------------+-----------------------------------------------------+

.. note:: The first line of the CSV file should always be the column header and it is not configurable.

Below is an example dump of such a CSV file::

    key,type,encoding,value     <-- column header
    namespace_name,namespace,,  <-- First entry should be of type "namespace"
    key1,data,u8,1
    key2,file,string,/path/to/file


.. note::

    Make sure there are **no spaces**:
        - before and after ','
        - at the end of each line in a CSV file


NVS Entry and Namespace association
-----------------------------------

When a namespace entry is encountered in a CSV file, each following entry will be treated as part of that namespace until the next namespace entry is found. At this point, all the following entries will be treated as part of the new namespace.

.. note:: First entry in a CSV file should always be a ``namespace`` entry.


Multipage Blob Support
----------------------

By default, binary blobs are allowed to span over multiple pages and are written in the format mentioned in Section :ref:`structure_of_entry`.
If you intend to use an older format, the utility provides an option to disable this feature.


Encryption Support
-------------------

The NVS Partition Generator utility also allows you to create an encrypted binary file. The utility uses the AES-XTS encryption. Please refer to :ref:`nvs_encryption` for more details.


Running the utility
-------------------

**Usage**::

    python nvs_partition_gen.py [-h] [--input INPUT] [--output OUTPUT]
                            [--size SIZE] [--version {v1,v2}]
                            [--keygen {true,false}] [--encrypt {true,false}]
                            [--keyfile KEYFILE] [--outdir OUTDIR]


+------------------------+---------------------------------------------------+-------------------+
|   Arguments            |                    Description                    |  Default Value    |
+========================+===================================================+===================+
| --input INPUT          | Path to a CSV file to parse.                      |                   |
+------------------------+---------------------------------------------------+-------------------+
| --output OUTPUT        | Path to the generated binary file.                |                   |
+------------------------+---------------------------------------------------+-------------------+
| --size SIZE            | Size of NVS Partition in bytes                    |                   |
|                        | (must be multiple of 4096).                       |                   |
+------------------------+---------------------------------------------------+-------------------+
| --version {v1,v2}      | Set version.                                      | v2                |
+------------------------+---------------------------------------------------+-------------------+
| --keygen {true,false}  | Generate keys for encryption.                     |                   |
+------------------------+---------------------------------------------------+-------------------+
| --encrypt {true,false} | Set encryption mode. Default: false.              | false             |
+------------------------+---------------------------------------------------+-------------------+
| --keyfile KEYFILE      | File containing the key for encryption            |                   |
|                        | (Applicable only if encryption mode is true).     |                   |
+------------------------+---------------------------------------------------+-------------------+
| --outdir OUTDIR        | The output directory to store the created files.  | current directory |
+------------------------+---------------------------------------------------+-------------------+

You can run this utility in two modes:

    - **Default mode**: You get an unencrypted binary file.
    - **Encryption mode**: You get an encrypted binary file.


**In default mode:**
--------------------

*Usage*::

    python nvs_partition_gen.py [-h] --input INPUT --output OUTPUT
                            --size SIZE [--version {v1,v2}]
                            [--keygen {true,false}] [--encrypt {true,false}]
                            [--keyfile KEYFILE] [--outdir OUTDIR]

You can run the utility using the command below::

    python nvs_partition_gen.py --input sample.csv --output sample.bin --size 0x3000


**In encryption mode:**
-----------------------

*Usage*::

    python nvs_partition_gen.py [-h] --input INPUT --output OUTPUT
                            --size SIZE --encrypt {true,false}
                            --keygen {true,false} --keyfile KEYFILE
                            [--version {v1,v2}] [--outdir OUTDIR]


You can run the utility using one of the commands below:

    - By enabling generation of encryption keys::

          python nvs_partition_gen.py --input sample.csv --output sample_encrypted.bin --size 0x3000 --encrypt true --keygen true

    - By taking encryption keys as an input file. A sample binary file containing encryption keys is provided with the utility::

          python nvs_partition_gen.py --input sample.csv --output sample_encrypted.bin --size 0x3000 --encrypt true --keyfile testdata/sample_encryption_keys.bin

    - By enabling generation of encryption keys and storing the keys in a binary file with a custom filename::

          python nvs_partition_gen.py --input sample.csv --output sample_encrypted.bin --size 0x3000 --encrypt true --keygen true --keyfile encryption_keys_generated.bin

.. note:: If `--keygen` is given with the `--keyfile` argument, generated keys will be stored in the `--keyfile` file. If `--keygen` argument is absent, `--keyfile` is taken as input file containing encryption keys.


*To generate* **only** *encryption keys with this utility*::

    python nvs_partition_gen.py --keygen true

This creates an `encryption_keys_<timestamp>.bin` file.

.. note:: This newly created file having encryption keys in `keys/` directory is compatible with NVS key-partition structure. Refer to :ref:`nvs_key_partition` for more details.


You can also provide the format version number (in any of the two modes):
    - Multipage Blob Support Enabled (v2)
    - Multipage Blob Support Disabled (v1)


**Multipage Blob Support Enabled (v2):**
----------------------------------------

You can run the utility in this format by setting the version parameter to v2, as shown below.
A sample CSV file is provided with the utility::

    python nvs_partition_gen.py --input sample_multipage_blob.csv --output partition_multipage_blob.bin --size 0x4000 --version v2


**Multipage Blob Support Disabled (v1):**
-----------------------------------------

You can run the utility in this format by setting the version parameter to v1, as shown below.
A sample CSV file is provided with the utility::

    python nvs_partition_gen.py --input sample_singlepage_blob.csv --output partition_single_page.bin --size 0x3000 --version v1


.. note::  *Minimum NVS Partition Size needed is 0x3000 bytes.*

.. note::  *When flashing the binary onto the device, make sure it is consistent with the application's sdkconfig.*

Caveats
-------
-  Utility does not check for duplicate keys and will write data pertaining to both keys. You need to make sure that the keys are distinct.
-  Once a new page is created, no data will be written in the space left on the previous page. Fields in the CSV file need to be ordered in such a way as to optimize memory.
-  64-bit datatype is not yet supported.

