#This is the step for ESP32-S2/S3/C3

#!/bin/bash
set -e
if [ -z "$ESPPORT" ]; then
    echo "ESPPORT must be set"
    exit 1
fi
dd if=/dev/zero of=key.bin bs=1 count=32
# Change the first byte as espsecure uses modules that won't
# allow symmetric keys
echo -ne \\xFF | dd conv=notrunc bs=1 count=1 of=key.bin
espefuse.py --do-not-confirm -p $ESPPORT burn_efuse SPI_BOOT_CRYPT_CNT 0x1
espefuse.py --do-not-confirm -p $ESPPORT burn_key BLOCK_KEY2 key.bin XTS_AES_128_KEY
