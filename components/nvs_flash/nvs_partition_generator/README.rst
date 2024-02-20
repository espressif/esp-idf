NVS Partition Generator Utility
===============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The utility :component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py` creates a binary file, compatible with the NVS architecture defined in :doc:`nvs_flash`, based on the key-value pairs provided in a CSV file.

This utility is ideally suited for generating a binary blob, containing data specific to ODM/OEM, which can be flashed externally at the time of device manufacturing. This allows manufacturers to generate many instances of the same application firmware with customized parameters for each device, such as a serial number.

Prerequisites
-------------

To use this utility in encryption mode, install the following packages:
    - ``cryptography``

All the required packages are included in `requirements.txt` in the root of the ESP-IDF directory.

CSV File Format
~~~~~~~~~~~~~~~

Each line of a CSV file should contain 4 parameters, separated by a comma. The table below describes each of these parameters.

.. list-table::
    :widths: 10 20 35 35
    :header-rows: 1

    * - No.
      - Parameter
      - Description
      - Notes
    * - 1
      - Key
      - Key of the data. The data can be accessed later from an application using this key.
      -
    * - 2
      - Type
      - Supported values are ``file``, ``data``, and ``namespace``.
      -
    * - 3
      - Encoding
      - Supported values are: ``u8``, ``i8``, ``u16``, ``i16``, ``u32``, ``i32``, ``u64``, ``i64``, ``string``, ``hex2bin``, ``base64``, and ``binary``. This specifies how actual data values are encoded in the resulting binary file. The difference between the ``string`` and ``binary`` encoding is that ``string`` data is terminated with a NULL character, whereas ``binary`` data is not.
      - As of now, for the ``file`` type, only ``hex2bin``, ``base64``, ``string``, and ``binary`` encoding is supported.
    * - 4
      - Value
      - Data value
      - ``Encoding`` and ``Value`` cells for the ``namespace`` field type should be empty. ``Encoding`` and ``Value`` of ``namespace`` are fixed and are not configurable. Any values in these cells are ignored.

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


NVS Entry and Namespace Association
-----------------------------------

When a namespace entry is encountered in a CSV file, each following entry will be treated as part of that namespace until the next namespace entry is found. At this point, all the following entries will be treated as part of the new namespace.

.. note:: First entry in a CSV file should always be a ``namespace`` entry.


Multipage Blob Support
----------------------

By default, binary blobs are allowed to span over multiple pages and are written in the format mentioned in Section :ref:`structure_of_entry`. If you intend to use the older format, the utility provides an option to disable this feature.


Encryption-Decryption Support
-----------------------------

The NVS Partition Generator utility also allows you to create an encrypted binary file and decrypt an encrypted one. The utility uses the XTS-AES encryption. Please refer to :ref:`nvs_encryption` for more details.


Running the Utility
-------------------

**Usage**::

    python nvs_partition_gen.py [-h] {generate,generate-key,encrypt,decrypt} ...

**Optional Arguments**:

.. list-table::
    :widths: 20 40 40
    :header-rows: 1

    * - No.
      - Parameter
      - Description
    * - 1
      - ``-h`` / ``--help``
      - Show the help message and exit

**Commands**:

  	Run ``nvs_partition_gen.py {command} -h`` for additional help

.. list-table::
    :widths: 20 40 40
    :header-rows: 1

    * - No.
      - Parameter
      - Description
    * - 1
      - ``generate`` 
      - Generate NVS partition
    * - 2
      - ``generate-key``
      - Generate keys for encryption
    * - 3
      - ``encrypt``  
      - Generate NVS encrypted partition
    * - 4
      - ``decrypt`` 
      - Decrypt NVS encrypted partition

Generate NVS Partition (Default)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Usage**::

    python nvs_partition_gen.py generate [-h] [--version {1,2}] [--outdir OUTDIR] input output size

**Positional Arguments**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - Parameter 
      - Description 
    * - ``input``
      - Path to CSV file to parse
    * - ``output``
      - Path to output NVS binary file
    * - ``size`` 
      - Size of NVS partition in bytes (must be multiple of 4096)

**Optional Arguments**:

+------------------------+----------------------------------------------------------------------+
| Parameter              | Description                                                          |
+========================+======================================================================+
| ``-h`` / ``--help``    | Show the help message and exit                                       |
+------------------------+----------------------------------------------------------------------+
| ``--version {1,2}``    | Set multipage blob version (Default: Version 2)                      |
|                        |                                                                      |
|                        | Version 1 - Multipage blob support disabled                          |
|                        |                                                                      |
|                        | Version 2 - Multipage blob support enabled                           |
+------------------------+----------------------------------------------------------------------+
| ``--outdir OUTDIR``    | Output directory to store file created (Default: current directory)  |
+------------------------+----------------------------------------------------------------------+

You can run the utility to generate NVS partition using the command below. A sample CSV file is provided with the utility::

    python nvs_partition_gen.py generate sample_singlepage_blob.csv sample.bin 0x3000


Generate Encryption Keys Partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. only:: SOC_HMAC_SUPPORTED

    **Usage**::

        python nvs_partition_gen.py generate-key [-h] [--key_protect_hmac] [--kp_hmac_keygen]
                                                    [--kp_hmac_keyfile KP_HMAC_KEYFILE] [--kp_hmac_inputkey KP_HMAC_INPUTKEY]
                                                    [--keyfile KEYFILE] [--outdir OUTDIR]

.. only:: not SOC_HMAC_SUPPORTED

    **Usage**::

        python nvs_partition_gen.py generate-key [-h] [--keyfile KEYFILE] [--outdir OUTDIR]

**Optional Arguments**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - Parameter 
      - Description 
    * - ``-h`` / ``--help``
      - Show the help message and exit
    * - ``--keyfile KEYFILE``
      - Path to output encryption keys file
    * - ``--outdir OUTDIR``
      - Output directory to store files created. (Default: current directory)

.. only:: SOC_HMAC_SUPPORTED

    **Optional Arguments (HMAC scheme-specific)**:

    .. list-table::
        :widths: 30 70
        :header-rows: 1

        * - Parameter 
          - Description 
        * - ``--key_protect_hmac`` 
          - If set, the NVS encryption key protection scheme based on HMAC peripheral is used; else the default scheme based on flash encryption is used
        * - ``--kp_hmac_keygen``
          - Generate the HMAC key for HMAC-based encryption scheme
        * - ``--kp_hmac_keyfile KP_HMAC_KEYFILE`` 
          - Path to output the HMAC key file
        * - ``--kp_hmac_inputkey KP_HMAC_INPUTKEY``
          - File having the HMAC key for generating the NVS encryption keys

You can run the utility to generate only the encryption key partition using the command below::

    python nvs_partition_gen.py generate-key

.. only:: SOC_HMAC_SUPPORTED

    For generating encryption key for the HMAC-based scheme, the following commands can be used:

    - Generate the HMAC key and the NVS encryption keys::

        python nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_keygen

    .. note:: Encryption key of the format ``<outdir>/keys/keys-<timestamp>.bin`` and HMAC key of the format ``<outdir>/keys/hmac-keys-<timestamp>.bin`` are created.

    - Generate the NVS encryption keys, given the HMAC key::

        python nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_inputkey testdata/sample_hmac_key.bin

    .. note:: You can provide the custom filename for the HMAC key as well as the encryption key as a parameter.

Generate Encrypted NVS Partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. only:: SOC_HMAC_SUPPORTED

    **Usage**::

            python nvs_partition_gen.py encrypt [-h] [--version {1,2}] [--keygen]
                                                [--keyfile KEYFILE] [--inputkey INPUTKEY] [--outdir OUTDIR]
                                                [--key_protect_hmac] [--kp_hmac_keygen]
                                                [--kp_hmac_keyfile KP_HMAC_KEYFILE] [--kp_hmac_inputkey KP_HMAC_INPUTKEY]
                                                input output size

.. only:: not SOC_HMAC_SUPPORTED

    **Usage**::

            python nvs_partition_gen.py encrypt [-h] [--version {1,2}] [--keygen]
                                                [--keyfile KEYFILE] [--inputkey INPUTKEY] [--outdir OUTDIR]
                                                input output size


**Positional Arguments**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - Parameter 
      - Description 
    * - ``input``
      - Path to CSV file to parse
    * - ``output``
      - Path to output NVS binary file
    * - ``size`` 
      - Size of NVS partition in bytes (must be multiple of 4096)

**Optional Arguments**:

+---------------------------------------------+-------------------------------------------------------------------------------+
| Parameter                                   | Description                                                                   |
+=============================================+===============================================================================+
| ``-h`` / ``--help``                         | Show the help message and exit                                                |
+---------------------------------------------+-------------------------------------------------------------------------------+
| ``--version {1,2}``                         | Set multipage blob version (Default: Version 2)                               |
|                                             |                                                                               |
|                                             | Version 1 - Multipage blob support disabled                                   |
|                                             |                                                                               |
|                                             | Version 2 - Multipage blob support enabled                                    |
+---------------------------------------------+-------------------------------------------------------------------------------+
| ``--keygen``                                | Generates key for encrypting NVS partition                                    |
+---------------------------------------------+-------------------------------------------------------------------------------+
| ``--keyfile KEYFILE``                       | Path to output encryption keys file                                           |
+---------------------------------------------+-------------------------------------------------------------------------------+
| ``--inputkey INPUTKEY``                     | File having key for encrypting NVS partition                                  |
+---------------------------------------------+-------------------------------------------------------------------------------+
| ``--outdir OUTDIR``                         | Output directory to store file created (Default: current directory)           |
+---------------------------------------------+-------------------------------------------------------------------------------+

.. only:: SOC_HMAC_SUPPORTED

    **Optional Arguments (HMAC scheme-specific)**:

    .. list-table::
        :widths: 30 70
        :header-rows: 1

        * - Parameter 
          - Description 
        * - ``--key_protect_hmac`` 
          - If set, the NVS encryption key protection scheme based on HMAC peripheral is used; else the default scheme based on flash encryption is used 
        * - ``--kp_hmac_keygen``
          - Generate the HMAC key for HMAC-based encryption scheme 
        * - ``--kp_hmac_keyfile KP_HMAC_KEYFILE`` 
          - Path to output HMAC key file
        * - ``--kp_hmac_inputkey KP_HMAC_INPUTKEY``
          - File having the HMAC key for generating the NVS encryption keys

You can run the utility to encrypt NVS partition using the command below. A sample CSV file is provided with the utility:

- Encrypt by allowing the utility to generate encryption keys::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen

  .. note:: Encryption key of the format ``<outdir>/keys/keys-<timestamp>.bin`` is created.

.. only:: SOC_HMAC_SUPPORTED

  - To generate an encrypted partition using the HMAC-based scheme, the above command can be used along with some additional parameters.

    - Encrypt by allowing the utility to generate encryption keys and the HMAC-key::

        python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --key_protect_hmac --kp_hmac_keygen

    .. note:: Encryption key of the format ``<outdir>/keys/keys-<timestamp>.bin`` and HMAC key of the format ``<outdir>/keys/hmac-keys-<timestamp>.bin`` are created.

    - Encrypt by allowing the utility to generate encryption keys with user-provided HMAC-key::

        python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --key_protect_hmac --kp_hmac_inputkey testdata/sample_hmac_key.bin

  .. note:: You can provide the custom filename for the HMAC key as well as the encryption key as a parameter.

- Encrypt by allowing the utility to generate encryption keys and store it in provided custom filename::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --keyfile sample_keys.bin

.. note::

    - Encryption key of the format ``<outdir>/keys/sample_keys.bin`` is created.

    - This newly created file having encryption keys in ``keys/`` directory is compatible with NVS key-partition structure. Refer to :ref:`nvs_encr_key_partition` for more details.

- Encrypt by providing the encryption keys as input binary file::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --inputkey sample_keys.bin

Decrypt Encrypted NVS Partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Usage**::

    python nvs_partition_gen.py decrypt [-h] [--outdir OUTDIR] input key output

**Positional Arguments**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - Parameter
      - Description
    * - ``input``
      - Path to encrypted NVS partition file to parse
    * - ``key``
      - Path to file having keys for decryption
    * - ``output``
      - Path to output decrypted binary file 

**Optional Arguments**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - Parameter 
      - Description 
    * - ``-h`` / ``--help``
      - Show the help message and exit
    * - ``--outdir OUTDIR``
      - Output directory to store files created. (Default: current directory)

You can run the utility to decrypt encrypted NVS partition using the command below::

    python nvs_partition_gen.py decrypt sample_encr.bin sample_keys.bin sample_decr.bin

You can also provide the format version number:
    - Multipage blob support disabled (Version 1)
    - Multipage blob support enabled (Version 2)


Multipage Blob Support Disabled (Version 1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can run the utility in this format by setting the version parameter to 1, as shown below. A sample CSV file for the same is provided with the utility::

   python nvs_partition_gen.py generate sample_singlepage_blob.csv sample.bin 0x3000 --version 1


Multipage Blob Support Enabled (Version 2)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can run the utility in this format by setting the version parameter to 2, as shown below. A sample CSV file for the same is provided with the utility::

   python nvs_partition_gen.py generate sample_multipage_blob.csv sample.bin 0x4000 --version 2

.. note::

    - Minimum NVS Partition Size needed is 0x3000 bytes.

    - When flashing the binary onto the device, make sure it is consistent with the application's sdkconfig.


Caveats
-------

-  Utility does not check for duplicate keys and will write data pertaining to both keys. You need to make sure that the keys are distinct.
-  Once a new page is created, no data will be written in the space left on the previous page. Fields in the CSV file need to be ordered in such a way as to optimize memory.
-  64-bit datatype is not yet supported.
