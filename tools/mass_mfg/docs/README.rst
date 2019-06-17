Manufacturing Utility
=====================

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

The mfg\_gen.py utility uses the generated CSV Configuration file and the master value CSV file to generate factory images for each device. 

*A sample CSV Configuration file and a master value CSV file are both provided with this utility.* 

**Usage**::
    
    ./mfg_gen.py [-h] [--conf CONFIG_FILE] [--values VALUES_FILE]
                    [--prefix PREFIX] [--fileid FILEID] [--outdir OUTDIR]
                    [--size PART_SIZE] [--version {v1,v2}]
                    [--keygen {true,false}] [--encrypt {true,false}]
                    [--keyfile KEYFILE]

The description of the arguments is given in the table below.

+------------------------+------------------------------------------------------------+-------------------+
|   Arguments            |                        Description                         |  Default Value    |
+========================+============================================================+===================+
| --conf CONFIG_FILE     | Path to existing CSV configuration file                    |                   |
+------------------------+------------------------------------------------------------+-------------------+
| --values VALUES_FILE   | Path to existing master value CSV file                     |                   |
+------------------------+------------------------------------------------------------+-------------------+
| --prefix PREFIX        | Unique filename prefix                                     |                   |
+------------------------+------------------------------------------------------------+-------------------+
| --fileid FILEID        | Unique file identifier (any key in the file with values)   | numeric value     |
|                        | as a filename suffix                                       | (1,2,3...)        |
+------------------------+------------------------------------------------------------+-------------------+
| --outdir OUTDIR        | Output directory to store created files                    | current directory |
+------------------------+------------------------------------------------------------+-------------------+
| --size PART_SIZE       | Size of NVS Partition in bytes (must be multiple of 4096)  |                   |
+------------------------+------------------------------------------------------------+-------------------+
| --version {v1,v2}      | Set version                                                | v2                |
+------------------------+------------------------------------------------------------+-------------------+
| --keygen {true,false}  | Generate keys for encryption                               | false             |
+------------------------+------------------------------------------------------------+-------------------+
| --encrypt {true,false} | Set encryption mode                                        | false             |
+------------------------+------------------------------------------------------------+-------------------+
| --keyfile KEYFILE      | File storing key for encryption (Applicable only if        |                   |
|                        | Encryption mode is true).                                  |                   |
+------------------------+------------------------------------------------------------+-------------------+

*To run this utility with the provided sample files, use the commands below*::

    ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000

    ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_multipage_blob.csv --prefix Fan --size 0x4000

When you use this utility to generate factory images on a per device basis, keep in mind that the arguments --conf, --values, --prefix, and --size are mandatory.

    ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000 --outdir tmp

.. note:: If the --outdir directory does not exist, it will be created.

The master value CSV file should have the path in the ``file`` type relative to the directory from which you are running the utility.

    ./mfg_gen.py --conf samples/sample_config.csv --values samples/sample_values_singlepage_blob.csv --prefix Fan --size 0x3000 --encrypt true --keygen true

.. note:: The generated ``keys/`` directory is named as the file with encryption keys of the form ``prefix-fileid-keys.bin``.

*If you* **only** *want to generate a binary file with encryption keys, you can run the command below.*::

    ./mfg_gen.py --keygen true

.. note:: When you use this utility to generate encryption keys only, the --keygen argument is mandatory.

In the following example, the 'keys/' directory will be created at the current path. This binary file can further be used to encrypt factory images created on the per device basis*.::

    ./mfg_gen.py --keygen true --keyfile encr_keys.bin

.. note:: When running the utility to generate encryption keys only, if --keyfile is given, it will generate encryption keys with the filename given in the --keyfile argument.

The default numeric value: 1,2,3... of the ``fileid`` argument corresponds to each line bearing device instance values in the master value CSV file.

While running the manufacturing utility, the following folders will be created in the specified ``outdir`` directory:

- ``bin/`` for storing the generated binary files
- ``csv/`` for storing the generated intermediate CSV files
