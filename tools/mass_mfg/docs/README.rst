Manufacturing Utility
=====================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

This utility is designed to create instances of factory NVS partition images on a per-device basis for mass manufacturing purposes. The NVS partition images are created from CSV files containing user-provided configurations and values.

Please note that this utility only creates manufacturing binary images which then need to be flashed onto your devices using:

- esptool.py
- Flash Download tool (available on Windows only)
- Direct flash programming


Prerequisites
-------------

**This utility is dependent on esp-idf's NVS partition utility.**

* Operating System requirements:
	-	Linux / MacOS / Windows (standard distributions)

* The following packages are needed to use this utility:
	-	Python version: 2.7 (minimum) is required. You can find it here: <https://www.python.org/downloads/>

.. note::

    Before using this utility, please make sure that:
        - The path to Python is added to the PATH environment variable.
        - You have installed the packages from `requirement.txt`, the file in the root of the esp-idf directory.


Workflow
--------

.. blockdiag:: 

    blockdiag {
    A [label = "CSV Configuration file"];
    B [label = "Master Value CSV file"];
    C [label = "Binary files", stacked];

    A -- B -> C
    }


CSV Configuration File
----------------------

This file contains the configuration of the device to be flashed.
  
The data in the configuration file has the following format (the `REPEAT` tag is optional):: 

       name1,namespace,	   <-- First entry should be of type "namespace"
       key1,type1,encoding1
       key2,type2,encoding2,REPEAT
       name2,namespace,	   
       key3,type3,encoding3
       key4,type4,encoding4

.. note:: The first line in this file should always be the ``namespace`` entry.

Each line should have three parameters: ``key,type,encoding``, separated by a comma.
If the ``REPEAT`` tag is present, the value corresponding to this key in the master value CSV file will be the same for all devices.

*Please refer to README of the NVS Partition Generator utility for detailed description of each parameter.*

Below is a sample example of such a configuration file::


	app,namespace,
	firmware_key,data,hex2bin
	serial_no,data,string,REPEAT
	device_no,data,i32


.. note::

    Make sure there are **no spaces**:
        - before and after ','
        - at the end of each line in a CSV file


Master Value CSV File
---------------------

This file contains details of the devices to be flashed. Each line in this file corresponds to a device instance.
  
The data in the master value CSV file has the following format::

	key1,key2,key3,.....
	value1,value2,value3,.... 

.. note:: The first line in the file should always contain the ``key`` names. All the keys from the configuration file should be present here in the **same order**. This file can have additional columns (keys). The additional keys will be treated as metadata and would not be part of the final binary files.

Each line should contain the ``value`` of the corresponding keys, separated by a comma. If the key has the ``REPEAT`` tag, its corresponding value **must** be entered in the second line only. Keep the entry empty for this value in the following lines.

The description of this parameter is as follows:

``value``
	Data value

Data value is the value of data corresponding to the key.

Below is a sample example of a master value CSV file::

	id,firmware_key,serial_no,device_no 
	1,1a2b3c4d5e6faabb,A1,101 
	2,1a2b3c4d5e6fccdd,,102 
	3,1a2b3c4d5e6feeff,,103 

.. note:: *If the 'REPEAT' tag is present, a new master value CSV file will be created in the same folder as the input Master CSV File with the values inserted at each line for the key with the 'REPEAT' tag*.

This utility creates intermediate CSV files which are used as input for the NVS partition utility to generate the binary files.

The format of this intermediate CSV file is as follows::

	key,type,encoding,value
	key,namespace, ,
	key1,type1,encoding1,value1
	key2,type2,encoding2,value2

An instance of an intermediate CSV file will be created for each device on an individual basis.


Running the utility
-------------------

**Usage**::

        python mfg_gen.py [-h] {generate,generate-key} ...

        Optional Arguments:
        +-----+------------+----------------------------------------------------------------------+
        | No. | Parameter  |                           Description                                |
        +=====+============+======================================================================+
        | 1   | -h, --help |        show this help message and exit                               |
        +-----+------------+----------------------------------------------------------------------+

        Commands:
  	        Run mfg_gen.py {command} -h for additional help
        +-----+--------------+--------------------------------------------------------------------+
        | No. | Parameter    |                           Description                              |
        +=====+==============+====================================================================+
        | 1   | generate     |      Generate NVS partition                                        |
        +-----+--------------+--------------------------------------------------------------------+
        | 2   | generate-key |      Generate keys for encryption                                  |
        +-----+--------------+--------------------------------------------------------------------+

**To generate factory images for each device (Default):**
    **Usage**::

        python mfg_gen.py generate [-h] [--fileid FILEID] [--version {1,2}] [--keygen]
                                        [--keyfile KEYFILE] [--inputkey INPUTKEY]
                                        [--outdir OUTDIR]
                                        conf values prefix size
        
        Positional Arguments:
        +--------------+----------------------------------------------------------------------+
        | Parameter    |                           Description                                |
        +==============+======================================================================+
        | conf         |        Path to configuration csv file to parse                       | 
        +--------------+----------------------------------------------------------------------+
        | values       |        Path to values csv file to parse                              |
        +--------------+----------------------------------------------------------------------+
        | prefix       |        Unique name for each output filename prefix                   |                          
        +-----+--------------+----------------------------------------------------------------+
        | size         |        Size of NVS partition in bytes                                |
        |              |        (must be multiple of 4096)                                    |
        +--------------+----------------------------------------------------------------------+

        Optional Arguments:
        +---------------------+--------------------------------------------------------------------+
        | Parameter           |                           Description                              |
        +=====================+====================================================================+
        | -h, --help          |     show this help message and exit                                |
        +---------------------+--------------------------------------------------------------------+
        | --fileid FILEID     |     Unique file identifier(any key in values file)                 |
        |                     |     for each filename suffix (Default: numeric value(1,2,3...)     |
        +---------------------+--------------------------------------------------------------------+
        | --version {1,2}     |     Set multipage blob version.                                    |
        |                     |     Version 1 - Multipage blob support disabled.                   |
        |                     |     Version 2 - Multipage blob support enabled.                    |
        |                     |     Default: Version 2                                             |
        +---------------------+--------------------------------------------------------------------+
        | --keygen            |     Generates key for encrypting NVS partition                     |
        +---------------------+--------------------------------------------------------------------+
        | --inputkey INPUTKEY |     File having key for encrypting NVS partition                   |
        +---------------------+--------------------------------------------------------------------+
        | --outdir OUTDIR     |     Output directory to store files created                        |
        |                     |     (Default: current directory)                                   |
        +---------------------+--------------------------------------------------------------------+

You can run the utility to generate factory images for each device using the command below. A sample CSV file is provided with the utility::

    python mfg_gen.py generate samples/sample_config.csmples/sample_values_singlepage_blob.csv Sample 0x3000

The master value CSV file should have the path in the ``file`` type relative to the directory from which you are running the utility.

**To generate encrypted factory images for each device:**

You can run the utility to encrypt factory images for each device using the command below. A sample CSV file is provided with the utility:

- Encrypt by allowing the utility to generate encryption keys::

    python mfg_gen.py generate samples/sample_config.csv samples/sample_values_singlepage_blob.csv Sample 0x3000 --keygen

.. note:: Encryption key of the following format ``<outdir>/keys/keys-<prefix>-<fileid>.bin`` is created.
.. note:: This newly created file having encryption keys in ``keys/`` directory is compatible with NVS key-partition structure. Refer to :ref:`nvs_key_partition` for more details.

- Encrypt by providing the encryption keys as input binary file::

    python mfg_gen.py generate samples/sample_config.csv samples/sample_values_singlepage_blob.csv Sample 0x3000 --inputkey keys/sample_keys.bin

**To generate only encryption keys:**
  **Usage**::

        python mfg_gen.py generate-key [-h] [--keyfile KEYFILE] [--outdir OUTDIR]

        Optional Arguments:
        +--------------------+----------------------------------------------------------------------+
        | Parameter          |                           Description                                |
        +====================+======================================================================+
        | -h, --help         |      show this help message and exit                                 |
        +--------------------+----------------------------------------------------------------------+
        | --keyfile KEYFILE  |      Path to output encryption keys file                             |
        +--------------------+----------------------------------------------------------------------+
        | --outdir OUTDIR    |      Output directory to store files created.                        |
        |                    |      (Default: current directory)                                    |
        +--------------------+----------------------------------------------------------------------+
    
You can run the utility to generate only encryption keys using the command below::

    python mfg_gen.py generate-key

.. note:: Encryption key of the following format ``<outdir>/keys/keys-<timestamp>.bin`` is created. Timestamp format is: ``%m-%d_%H-%M``.
.. note:: To provide custom target filename use the --keyfile argument.

Generated encryption key binary file can further be used to encrypt factory images created on the per device basis.

The default numeric value: 1,2,3... of the ``fileid`` argument corresponds to each line bearing device instance values in the master value CSV file.

While running the manufacturing utility, the following folders will be created in the specified ``outdir`` directory:

- ``bin/`` for storing the generated binary files
- ``csv/`` for storing the generated intermediate CSV files
- ``keys/`` for storing encryption keys (when generating encrypted factory images)
