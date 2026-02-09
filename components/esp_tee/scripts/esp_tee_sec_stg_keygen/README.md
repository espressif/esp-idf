# TEE Secure Storage: Image Generation


## A. Generate Secure Key Blobs

### `esp_tee_sec_stg_keygen` tool

```
$ python esp_tee_sec_stg_keygen.py --help
usage: esp_tee_sec_stg_keygen.py [-h] -k {aes256,ecdsa_p256,ecdsa_p384} -o OUTPUT [-i INPUT] [--write-once]

Generate or import a cryptographic key structure for secure storage

options:
  -h, --help            show this help message and exit
  -k, --key-type {aes256,ecdsa_p256,ecdsa_p384}
                        key type to be processed
  -o, --output OUTPUT   output binary file name
  -i, --input INPUT     input key file (.pem for ecdsa, .bin for aes)
  --write-once          make key persistent - cannot be modified or deleted once written
```

### ECDSA Keys

```bash
python esp_tee_sec_stg_keygen.py -k ecdsa_p256 -o ecdsa_p256_k0.bin
python esp_tee_sec_stg_keygen.py -k ecdsa_p384 -o ecdsa_p384_k0.bin
```

#### With custom PEM:

```bash
openssl ecparam -name prime256v1 -genkey -noout -out ecdsa_p256.pem
python esp_tee_sec_stg_keygen.py -k ecdsa_p256 -o ecdsa_p256_k1.bin -i ecdsa_p256.pem --write-once
```

### AES-256 Key

```bash
python esp_tee_sec_stg_keygen.py -k aes256 -o aes256_gcm_k0.bin --write-once
```

#### With custom key and IV

```bash
# Generate 32 bytes AES key
openssl rand 32 > aes_key.bin

# Generate 12 bytes IV (optional)
openssl rand 12 >> aes_key.bin

# Generate AES key blob using custom key + IV
python esp_tee_sec_stg_keygen.py -k aes256 -o aes256_gcm_k1.bin -i aes_key.bin
```

## B. NVS Partition Image Generation

### Create the TEE Secure Storage CSV

Prepare a CSV file that defines the data to be used as input for the NVS Partition Generator utility.

```csv
key,type,encoding,value
tee_sec_stg_ns,namespace,,
aes256_key0,file,binary,aes256_gcm_k0.bin
p256_key0,file,binary,ecdsa_p256_k0.bin
p384_key0,file,binary,ecdsa_p384_k0.bin
attest_key0,file,binary,ecdsa_p256_k1.bin
```
> [!IMPORTANT]
> As per the current implementation, all data objects in the TEE Secure Storage are to be stored in the `tee_sec_stg_ns` namespace.

### Generate NVS Encryption Keys

```bash
python nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_inputkey hmac_key.bin --keyfile nvs_keys.bin
```

> [!IMPORTANT]
> `hmac_key.bin` must match the HMAC key programmed into the eFuse block specified by `CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID`.

### Generate encrypted NVS partition

```bash
python nvs_partition_gen.py encrypt tee_sec_stg_val.csv tee_sec_stg_nvs.bin 0x8000 --inputkey nvs_keys.bin
```

> [!NOTE]
> Replace `0x8000` with the actual size of the TEE Secure Storage partition as configured in your project's partition table.

## (Optional) Decrypt for Verification

```bash
python nvs_partition_gen.py decrypt tee_sec_stg_nvs.bin nvs_keys.bin tee_sec_stg_nvs_decr.bin
```
