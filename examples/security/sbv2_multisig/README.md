| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# Secure Boot V2 Multisig Application

### Run example on QEMU

The following command can be used to run example on qemu

```sh
idf.py build
./create_keys.sh
./app.sh sign
./app.sh qemu
```
