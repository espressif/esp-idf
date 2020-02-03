# Preprovisioning Firmware

**Note:** The binaries/code uses the flash encryption & secure boot schemes, thus ensure that you are either using ESP32 modules or FPGA for testing.

This is an internal , we only give out the compiled binaries of this firmware to the customers. 

**Internal Documentation:** https://gitlab.espressif.cn:6688/idf/esp-idf/wikis/Secure-element

### Command specification
The _init1\_mfg_ firmware initialises a console, Enter ‘help’ to know the available commands.
The app has the following commands:

1. **generate-keys** -  
Generates a Key-Pair with given parameters   
**Usage:** `generate-keys <algorithm> <key-size>`  
**Example:** `generate-keys RSA 2048`  
(Currently only RSA-2048 is supported)

2. **generate-csr** -  
Generates a CSR with given subject string.  
**Usage:** `generate-csr <subject-string>`.  
The subject string is optional, you can omit it completely if you don’t want to specify the CSR parameters.  
We offer customisations to be added in the subject string. Please mention the specifics module pre-provisoning form provided by Espressif sales team.
(Common customizations: Adding MAC addresses/ FQDN's in common name, Validity, Country, Organization, etc.)
**Example:** `generate-csr "CN=Cert,O=mbed TLS,C=UK”`

3. **program-dev-cert** -
Sets the UART command handler to accept the device certificate(Signed by the CA certificate).  
The UART command handler accepts the input until ‘\0’ character is received.  
**Usage:** `program-dev-cert`

4. **program-ca-cert** -
Sets the UART command handler to accept the CA certificate.  
The UART command handler accepts the input until ‘\0’ character is received.  
**Usage:** `program-ca-cert`

5. **finalise** -  
The ESP boot-loader is overwritten by the plaintext customer boot-loader & plaintext customer bootloader secure boot digest.   
The private-key, device-cert, ca-cert are kept in-memory until this step. Now we write them to the flash.
`private-key(0xd000), device-cert(0xe000), ca-cert (0xf000)`  
**Usage:** `finalise`

6. **status** -   
Display the current state of the process.  
**Usage:** `status`

### Assessment with the provided binaries
The compiled binaries for Development and Preproduction have been provided, Please refer to the README.md in the corresponding folders for further details.

### Building the example from scratch
***Note:*** The script `generate_bin.sh` does this process & stores the binaries in `preproduction_bins/`.  
You can use the safe mode in `generate_bin.sh` to generate the development binaries(reprogrammable) in `development_bins/`.   
Usage: `sh generate_bin.sh safe_mode`

Enable `Mfg_Configuration->MFG_DEBUGGING=y` in make menuconfig to enable debugging logs.

1. Clone esp-idf. Checkout the branch _release/v3.2_.   
`git clone --recursive https://gitlab.espressif.cn:6688/idf/esp-idf.git`
`git checkout -b release/v3.2 origin/release/v3.2`
`git submodule update --init --recursive`
2. Set the _IDF\_PATH_ and _ESPPORT_.
3. Go to the init1_mfg folder.  
`cd $IDF_PATH/examples/init1_mfg`
4. Run make defconfig. Compile the app using make.
You can additionally enable Debugging by setting Example Configuration-> Mfg Debugging from menuconfig to true.  
`make defconfig`   
`make -j8`  
5. Flash the artifacts using the command.  
(ESP-Bootloader - 0x1000, Init1-FW - 0x20000, Partition Table - 0xc000)   
`python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port $ESPPORT --baud 2000000 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 $IDF_PATH/examples/init1_mfg/build/bootloader/bootloader.bin 0x20000 $IDF_PATH/examples/init1_mfg/build/init1_mfg.bin 0xd000 $IDF_PATH/examples/init1_mfg/build/partitions.bin`
6. Go to the sample_cust_app/ directory. Run make defconfig.
`cd sample_cust_app/`
`make defconfig`
`make bootloader`
6. Flash the cust\_bootloader at 0x11000.  
` python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/cu.usbserial-00006114B --baud 2000000 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x11000 $IDF_PATH/examples/mfg/build/bootloader/bootloader.bin `
7. Run `make monitor`, play around with the app. 
8. Stop make monitor & run secure_cert_mfg.py.  
Generate CA Private key(without password) and Certificates.
`openssl genrsa -out rootCA.key 2048`
`openssl req -x509 -new -key rootCA.key -days 1024 -out rootCA.pem` 

Generate CA Private key(with password) and Certificates.
`openssl genrsa -des -out rootCA.key 2048`
`openssl req -x509 -new -key rootCA.key -days 1024 -out rootCA.pem` 
Ensure that **Basic Constraints** Certificate Authority param in the rootCA.pem is set to true using a certificate viewer.
   
- If the private key is secured by a password
	`python tools/secure_cert_mfg.py -pkey rootCA.key -crt rootCA.pem -pwd "enter/password"`  
	
- If the private key doesn't have a password
`python tools/secure_cert_mfg.py -pkey rootCA.key -crt rootCA.pem"`
Press the enable button, to begin the execution.

 _We ship the module in this state. Following is the customer sequence_
	
1. Go to the sample\_cust\_app/ directory. 
`cd sample_cust_app/`
2. Compile the app. The app & partitions table will be signed during build by the `cust_secure_boot_key.pem` generated in step5.
`make`
3. Run `make flash`
4. Wait for a minute until the flash encryption takes place(Don't reboot, don't use any other command). 
Then run `make monitor`
The sample app fetches the decrypted private key, device cert & ca cert.

_To test the functionality without permanantly burning the modules:_ 

- Remove the encrypted flag from the partition table of init1\_mfg/partitions.csv.
- Remove sdkconfig, sdkconfig.old & build/ from  init1\_mfg.
- Add CONFIG\_MFG\_TEST\_MODE=y in sdkconfig.defaults, run make defconfig. Compile the app.
- Remove sdkconfig, sdkconfig.old & build/ from sample\_cust\_app.
- Remove Flash encryption & secure boot from sdkconfig.defaults, run make defconfig. Compile the app. 

## Working in a nutshell
![Workflow]

### Our workflow
- Flash the `esp_bootloader.bin` at `0x1000`, `esp_partitions.bin` at `0xc000`, `init1_mfg.bin` at `0x20000` & `cust_bootloader.bin` at `0x11000`.
- The python tool executes the commands on init1_mfg.bin.
- The commands generate the RSA private key(0xd000), store it in encrypted flash, prints the CSR on UART.
- This CSR is signed by the CA private key (-pkey) and the CA certificate(-crt) input to tools/secure\_cert\_mfg.py and kept in-memory in the ESP32. 
- The secure boot digest of the customer bootloader is generated at 0x10000.
- The flash encryption key is generated and stored in the EFUSE.
- The in-memory keys/certs are stored encrypted in the flash at the `finalise` command from the tool.
- `Private-key(0xd000), device-cert(0xe000), ca-cert (0xf000).`
-  Then the esp bootloader is overwritten by the plaintext cust bootloader & its digest.
-  We ship the module to the customer.

## Customer workflow
- The cust_bootloader.bin has flash encryption & secure boot enabled. The `cust_secure_boot_key.pem` has been compiled into the customer bootloader, thus the app & the partition table needs to be signed by this key.
`python $IDF_PATH/components/esptool_py/esptool/espsecure.py sign_data partitions.bin -k cust_secure_boot_key.pem -o signed_partitions.bin`    `python $IDF_PATH/components/esptool_py/esptool/espsecure.py sign_data app.bin -k cust_secure_boot_key.pem -o signed_app.bin`
- Flash the signed\_partitions.bin at 0xc000 and the signed_app after 0x10000(As per in the partitions table).
- After waiting for a minute. Run make monitor.
- The customer can access the cert, private key through the api's in secure_cert_operations.h in the sample customer app.

[Workflow]: ../Workflow.jpg