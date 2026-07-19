| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# Secure Boot V2 Multisig Application

### Run example on QEMU

The following command can be used to run example on qemu

```sh
idf.py build
./create_keys.sh
./app.sh sign 1 # sign with just one signature
./app.sh qemu   # app will be stuck in a boot loop due to failing secure boot verification
./app.sh sign 2 # sign with two signatures
./app.sh qemu   # app will now run
```

Delete the file `qemu_efuse.bin` to effectively reset the state of the efuses
