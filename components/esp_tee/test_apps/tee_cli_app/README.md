| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# TEE CLI Application: Secure Services Demonstration

## Example Usage

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the TEE framework (see Supported Targets table above).

### Configure the project

- Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

- Open the project configuration menu (`idf.py menuconfig`).

- Configure the secure storage key ID for generating/fetching the ECDSA keypair for attestation token signing at `ESP-TEE (Trusted Execution Environment) → Secure Services → Attestation: Secure Storage key ID for EAT signing`.

Configure the Secure Storage mode for determining how the NVS XTS-AES encryption keys are derived at `ESP-TEE (Trusted Execution Environment) → Secure Services → Secure Storage: Mode`

  - **Development** Mode: Encryption keys are embedded in the ESP-TEE firmware (identical across all instances).
  - **Release** Mode: Encryption keys are derived using a key stored in eFuse, specified by `CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID`.
    - Set the eFuse key ID at `ESP-TEE (Trusted Execution Environment) → Secure Services → Secure Storage: eFuse HMAC key ID for storage encryption keys`.
    - Before running the application, users must program the required key into the configured eFuse block - refer to the instructions below.

**For targets without HMAC peripheral (ESP32-C61):**

```shell
# Generate a random 32-byte key
openssl rand -out hmac_key_file.bin 32
# Program the USER purpose key (256-bit) in eFuse
# Here, BLOCK_KEYx is a free eFuse key-block between BLOCK_KEY0 and BLOCK_KEY5
espefuse -p PORT burn-key --no-read-protect BLOCK_KEYx hmac_key_file.bin USER
```

> [!IMPORTANT]
> When programming the key into eFuse for targets without HMAC peripheral, ensure that it is **NOT** marked as read-protected (use the `--no-read-protect` flag). If the key is read-protected, the TEE will be unable to access it. However, this does not weaken security: the APM peripheral already blocks software access to the key, and any illegal read or write attempt from the REE triggers a fault.

**For targets with HMAC peripheral:**

```shell
# Generate a random 32-byte HMAC key
openssl rand -out hmac_key_file.bin 32
# Program the HMAC key (256-bit) in eFuse
# Here, BLOCK_KEYx is a free eFuse key-block between BLOCK_KEY0 and BLOCK_KEY5
espefuse -p PORT burn-key BLOCK_KEYx hmac_key_file.bin HMAC_UP
```

### Build and Flash

- Build the project and flash it to the board, then run the monitor tool to view the serial output:

```shell
# Replace PORT with the name of the serial port to use
idf.py -p <PORT> flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

Refer the _Getting Started_ guide for full steps to configure and use ESP-IDF to build projects.

### Example Output

Enter the `help` command get a full list of all available commands provided by the example.

```log
I (627) app_start: Starting scheduler on CPU0
I (632) main_task: Started on CPU0
I (632) main_task: Calling app_main()
I (632) example: ESP-TEE: Secure services demonstration

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (702) main_task: Returned from app_main()
esp32c6> help
wifi_connect  [--timeout=<t>] <ssid> [<pass>]
  Join WiFi AP as a station
  --timeout=<t>  Connection timeout, ms
        <ssid>  SSID of AP
        <pass>  PSK of AP

tee_ota  <url>
  Initiate TEE app OTA
         <url>  URL for fetching the update

user_ota  <url>
  Initiate User app OTA
         <url>  URL for fetching the update

tee_att_info
  Dump the TEE-generated entity attestation token

get_msg_sha256  "<msg>"
  Get the SHA256 digest for the given message
       "<msg>"  Message for SHA256 digest calculation

tee_sec_stg_gen_key  <key_id> <key_type>
  Generate and store a new key of the specified type with the given ID
      <key_id>  TEE Secure storage key ID
    <key_type>  Key type (0: AES256, 1: ECDSA_SECP256R1)

tee_sec_stg_sign  <key_id> <msg_sha256>
  Sign a message using the ECDSA keypair stored with the given key ID and
  verify the signature
      <key_id>  TEE Secure storage key ID
  <msg_sha256>  SHA256 digest of the message to be signed and verified

tee_sec_stg_encrypt  <key_id> <plaintext>
  Encrypt data using AES-GCM key with the given ID from secure storage
      <key_id>  TEE Secure storage key ID
   <plaintext>  Plaintext to be encrypted

tee_sec_stg_decrypt  <key_id> <ciphertext> <tag>
  Decrypt data using AES-GCM key with the given ID from secure storage
      <key_id>  TEE Secure storage key ID
  <ciphertext>  Ciphertext to be decrypted
         <tag>  AES-GCM authentication tag

help  [<string>] [-v <0|1>]
  Print the summary of all registered commands if no arguments are given,
  otherwise print summary of given command.
      <string>  Name of command
  -v, --verbose=<0|1>  If specified, list console commands with given verbose level
```

## Secure Services

### Attestation

- The `tee_att_info` command provided by the attestation service generates and dumps an Entity Attestation Token (EAT) signed by the TEE.
- The token is signed using the ECDSA key (`secp256r1` curve) stored in the TEE Secure Storage with the configured key ID.

<details>
  <summary><b>Sample output:</b> <i>tee_att_info</i></summary>

```log
esp32c6> tee_att_info
I (8180) tee_attest: Attestation token - Length: 1587
I (8180) tee_attest: Attestation token - Data:
'{"header":{"magic":"44fef7cc","encr_alg":"","sign_alg":"ecdsa_secp256r1_sha256","key_id":"tee_att_key0"},"eat":{"auth_challenge":"dcb9b53143ad6b081dad1a05c7ebda4e314d388762215799cf24ed52e9387678","client_id":262974944,"device_ver":0,"device_id":"cd9c173cb3675c7adfae243f0cd9841e4bce003237cb5321927a85a86cb4b32e","instance_id":"9616ef0ecf02cdc89a3749f8fc16b3103d5100bd42d9312fcd04593baa7bac64","psa_cert_ref":"0716053550477-10100","device_status":165,"sw_claims":{"tee":{"type":1,"ver":"v0.3.0","idf_ver":"v5.1.4-241-g7ff01fd46f-dirty","secure_ver":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"94536998e1dcb2a036477cb2feb01ed4fff67ba6208f30482346c62bca64b280","digest_validated":true,"sign_verified":true}},"app":{"type":2,"ver":"v0.1.0","idf_ver":"v5.1.4-241-g7ff01fd46f-dirty","secure_ver":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"3d4c038fcec76852b4d07acb9e94afaf5fca69fc2eb212a32032d09ce5b4f2b3","digest_validated":true,"sign_verified":true,"secure_padding":true}},"bootloader":{"type":0,"ver":"","idf_ver":"","secure_ver":-1,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"1bef421beb1a4642c6fcefb3e37fd4afad60cb4074e538f42605b012c482b946","digest_validated":true,"sign_verified":true}}}},"public_key":{"compressed":"02039c4bfab0762af1aff2fe5596b037f629cf839da8c4a9c0018afedfccf519a6"},"sign":{"r":"915e749f5a780bc21a2b21821cfeb54286dc742e9f12f2387e3de9b8b1a70bc9","s":"1e583236f2630b0fe8e291645ffa35d429f14035182e19868508d4dac0e1a441"}}'
```

</details>

### Secure Storage

- The TEE secure storage service provides the following commands:
    - `tee_sec_stg_gen_key`: Generate and store a new key (ECDSA or AES) in the TEE secure storage with the specified ID
    - `tee_sec_stg_sign`: Sign a message using an ECDSA `secp256r1` key pair with the specified ID and verify the signature
    - `tee_sec_stg_encrypt`: Encrypt data with AES256-GCM using the key with the specified ID and outputs the ciphertext and tag
    - `tee_sec_stg_decrypt`: Decrypt ciphertext using key with the specified ID and tag for integrity verification
- The `get_msg_sha256` command computes the SHA256 hash of a given message, which can be used as input for the `tee_sec_stg_sign` command.

<details>
  <summary><b>Sample output:</b> <i>tee_sec_stg_gen_key + get_msg_sha256 + tee_sec_stg_sign</i></summary>

```log
esp32c6> tee_sec_stg_gen_key ecdsa_p256_k0 1
I (2964) tee_sec_stg: Generated ECDSA_SECP256R1 key with ID ecdsa_p256_k0
esp32c6> get_msg_sha256 "hello world"
I (3984) tee_sec_stg: Message digest (SHA256) -
b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
esp32c6> tee_sec_stg_sign ecdsa_p256_k0 b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
I (5384) tee_sec_stg: Generated signature -
944684f6ddcf4c268ac6b65e34ccb8d95bd2849567a87867101bc1f09208f0885d935d7b3ba9d46014f28e4c7c988d68c775431fcb2cb2d4ca5c6862db771088
I (6404) tee_sec_stg: Public key (Uncompressed) -
04a515bf1c43766cc34980dd6934b9ff54fd3d5d70fe7a694b1fea7a0bbc74434d008c7c3117ce0a5216ffdb2b807f2668cce9c973d524c038ab47b4344064dbbf
I (6444) tee_sec_stg: Signature verified successfully!
```

</details>

<details>
  <summary><b>Sample output:</b> <i>tee_sec_stg_gen_key + tee_sec_stg_encrypt + tee_sec_stg_decrypt</i></summary>

```log
esp32c6> tee_sec_stg_gen_key aes256_k0 0
I (2784) tee_sec_stg: Generated AES256 key with ID key0
esp32c6> tee_sec_stg_encrypt aes256_k0 b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
I (3084) tee_sec_stg: Ciphertext -
58054310a96d48c2dccdf2e34005aa63b40817723d3ec3d597ab362efea084c1
I (3594) tee_sec_stg: Tag -
caeedb43e08dc3b4e35a58b2412908cc
esp32c6> tee_sec_stg_decrypt aes256_k0 58054310a96d48c2dccdf2e34005aa63b40817723d3ec3d597ab362efea084c1 caeedb43e08dc3b4e35a58b2412908cc
I (4314) tee_sec_stg: Decrypted plaintext -
b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
```

</details>

### Over-the-Air updates

- The TEE/REE OTA service demonstrates the following workflow:
    1. Connect to a WiFi network (`wifi_connect` command)
    2. Initiate the TEE/REE OTA update, fetching the _new_ application image from the given URL (`tee_ota` and `user_ota` commands)

<details>
  <summary><b>Sample output:</b> <i>wifi_connect</i></summary>

```log
esp32c6> wifi_connect myssid mypassword
I (498) connect: Connecting to 'myssid'
I (498) pp: pp rom version: 5b8dcfa
I (508) net80211: net80211 rom version: 5b8dcfa
I (518) wifi_init: rx ba win: 6
I (518) wifi_init: tcpip mbox: 32
I (518) wifi_init: udp mbox: 6
I (518) wifi_init: tcp mbox: 6
I (528) wifi_init: tcp tx win: 5760
I (528) wifi_init: tcp rx win: 5760
I (538) wifi_init: tcp mss: 1440
I (538) wifi_init: WiFi IRAM OP enabled
I (538) wifi_init: WiFi RX IRAM OP enabled
I (548) phy_init: phy_version 290,81efd96,May  8 2024,10:42:13
W (598) wifi:(bf)761:0x600a7cac:0x01b4b4b0
W (598) wifi:(agc)0x600a7128:0xd2173800, min.avgNF:0xce->0xd2(dB), RCalCount:0x173, min.RRssi:0x800(-128.00)
W (608) wifi:(TB)WDEV_PWR_TB_MCS0:19
W (608) wifi:(TB)WDEV_PWR_TB_MCS1:19
W (608) wifi:(TB)WDEV_PWR_TB_MCS2:19
W (608) wifi:(TB)WDEV_PWR_TB_MCS3:19
W (618) wifi:(TB)WDEV_PWR_TB_MCS4:19
W (618) wifi:(TB)WDEV_PWR_TB_MCS5:19
W (618) wifi:(TB)WDEV_PWR_TB_MCS6:18
W (628) wifi:(TB)WDEV_PWR_TB_MCS7:18
W (628) wifi:(TB)WDEV_PWR_TB_MCS8:17
W (628) wifi:(TB)WDEV_PWR_TB_MCS9:15
W (648) wifi:(TB)WDEV_PWR_TB_MCS10:15
W (648) wifi:(TB)WDEV_PWR_TB_MCS11:15
W (1328) wifi:<ba-add>idx:0, ifx:0, tid:0, TAHI:0x1008fe0, TALO:0xc7e45510, (ssn:0, win:64, cur_ssn:0), CONF:0xc0000005
I (6358) esp_netif_handlers: sta ip: 192.168.1.30, mask: 255.255.255.0, gw: 192.168.1.1
I (6358) event_handler: got ip:192.168.1.30
I (6368) connect: Connected
```

</details>

<details>
  <summary><b>Sample output:</b> <i>tee_ota</i></summary>

```log
esp32c6> tee_ota https://192.168.1.1:4443/esp_tee/esp_tee.bin
I (5884) ota_with_tee: Starting TEE OTA...
esp32c6> I (1066394) esp-x509-crt-bundle: Certificate validated
I (7424) ota_with_tee: esp_tee_ota_begin succeeded
I (7904) ota_with_tee: Connection closed
I (7904) ota_with_tee: esp_tee_ota_write succeeded
I (7904) ota_with_tee: Total binary data written: 118784
I (8064) ota_with_tee: esp_tee_ota_end succeeded
I (8064) ota_with_tee: Prepare to restart system!
```

</details>

<details>
  <summary><b>Sample output:</b> <i>user_ota</i></summary>

```log
esp32c6> user_ota https://192.168.1.1:4443/tee_cli.bin
I (2388) ota_with_tee: Starting User OTA task...
I (2388) ota_with_tee: Attempting to download update from https://192.168.1.1:4443/tee_cli.bin
I (2438) esp-x509-crt-bundle: Certificate validated
esp32c6> I (62888) esp_https_ota: Starting OTA...
I (2888) esp_https_ota: Writing to partition subtype 17 at offset 0x1f0000
I (37338) esp_image: segment 0: paddr=001f0020 vaddr=420e0020 size=2ecc0h (191680) map
I (37368) esp_image: segment 1: paddr=0021ece8 vaddr=40811000 size=01330h (  4912)
I (37378) esp_image: segment 2: paddr=00220020 vaddr=42020020 size=b88b0h (755888) map
I (37518) esp_image: segment 3: paddr=002d88d8 vaddr=40812330 size=14488h ( 83080)
I (37538) esp_image: segment 4: paddr=002ecd68 vaddr=408267c0 size=032c0h ( 12992)
I (37538) esp_image: segment 5: paddr=002f0030 vaddr=00000000 size=0ffa0h ( 65440)
I (37568) esp_image: Verifying image signature...
I (37568) secure_boot_v2: Take trusted digest key(s) from eFuse block(s)
I (37568) secure_boot_v2: #0 app key digest == #0 trusted key digest
I (37578) secure_boot_v2: Verifying with RSA-PSS...
I (37638) secure_boot_v2_rsa: Signature verified successfully!
I (37648) esp_image: segment 0: paddr=001f0020 vaddr=420e0020 size=2ecc0h (191680) map
I (37678) esp_image: segment 1: paddr=0021ece8 vaddr=40811000 size=01330h (  4912)
I (37678) esp_image: segment 2: paddr=00220020 vaddr=42020020 size=b88b0h (755888) map
I (37828) esp_image: segment 3: paddr=002d88d8 vaddr=40812330 size=14488h ( 83080)
I (37848) esp_image: segment 4: paddr=002ecd68 vaddr=408267c0 size=032c0h ( 12992)
I (37848) esp_image: segment 5: paddr=002f0030 vaddr=00000000 size=0ffa0h ( 65440)
I (37868) esp_image: Verifying image signature...
I (37878) secure_boot_v2: Take trusted digest key(s) from eFuse block(s)
I (37878) secure_boot_v2: #0 app key digest == #0 trusted key digest
I (37888) secure_boot_v2: Verifying with RSA-PSS...
I (37948) secure_boot_v2_rsa: Signature verified successfully!
I (37998) ota_with_tee: OTA Succeed, Rebooting...
```

</details>

#### Local HTTPS server for hosting OTA images

- The script `https_server.py` is a helper script for the OTA service. Executing it with the `python https_server.py` command starts a local HTTPS server with the test certificates from the `test_certs` directory and serves files from the `build` directory that holds the generated binaries for the TEE and REE. Following is the script help -

```
python https_server.py --help
usage: https_server.py [-h] [--certfile CERTFILE] [--keyfile KEYFILE] [--port PORT] [--path PATH]

Start a local HTTPS server.

options:
  -h, --help           show this help message and exit
  --certfile CERTFILE  Path to the SSL certificate file (default: test_certs/server_cert.pem)
  --keyfile KEYFILE    Path to the SSL key file (default: test_certs/server_key.pem)
  --port PORT          Port number to bind the server to (default: 4443)
  --path PATH          Path to the directory to serve files from (default: build directory)

E.g., python https_server.py --certfile test_certs/server_cert.pem --keyfile test_certs/server_key.pem --port 8070 --path build
```
