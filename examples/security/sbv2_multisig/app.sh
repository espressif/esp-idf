#!/bin/bash

set -e

if [ -z "$IDF_PATH" ]; then
    echo "Please set IDF_PATH environment variable to the path of the ESP-IDF repository."
    exit 1
fi

usage() {
    echo "Usage: $0 sign"
    echo "       $0 qemu"
    exit 1
}

SIGN=0
QEMU=0
BOOTLOADER_NAME=bootloader.bin
BOOTLOADER_FILE=build/bootloader/$BOOTLOADER_NAME
SIGNED_BOOTLOADER_NAME=bootloader_signed.bin
SIGNED_BOOTLOADER_FILE=build/bootloader/$SIGNED_BOOTLOADER_NAME
BIN_NAME=sbv2_multisig.bin
BIN_FILE=build/$BIN_NAME
SIGNED_BIN_NAME=sbv2_multisig_signed.bin
SIGNED_BIN_FILE=build/$SIGNED_BIN_NAME

while true; do
    case "$1" in
        sign) SIGN=1; shift ;;
        qemu) QEMU=1; shift ;;
        -h | --help) usage ;;
        "") break ;;
        *) echo "Unknown option: $1"; usage ;;
    esac
done

if [ "$SIGN" -eq 1 ] && [ "$QEMU" -eq 1 ]; then
    echo "Cannot specify both 'sign' and 'qemu' options."
    usage
elif [ "$SIGN" -eq 0 ] && [ "$QEMU" -eq 0 ]; then
    echo "Must specify either 'sign' or 'qemu' option."
    usage
fi

sign_binary() {
    local INPUT_FILE="$1"
    local OUTPUT_FILE="$2"

    if [ ! -f "$INPUT_FILE" ]; then
        echo "File $INPUT_FILE does not exist."
        exit 1
    fi

    KEY1=secure_boot_signing_key1.pem
    KEY2=secure_boot_signing_key2.pem
    KEY3=secure_boot_signing_key3.pem
    if [ ! -f "$KEY1" ] || [ ! -f "$KEY2" ] || [ ! -f "$KEY3" ]; then
        echo "One or more signing keys ($KEY1, $KEY2, $KEY3) are missing."
        exit 1
    fi

    local HASH_FILE="${INPUT_FILE%.bin}.hash"
    local SIG_FILE1="${INPUT_FILE%.bin}.sig1"
    local SIG_FILE2="${INPUT_FILE%.bin}.sig2"
    local SIG_FILE3="${INPUT_FILE%.bin}.sig3"

    local HASH_OPTS="-sha256 -binary"
    local SIGN_OPTS="-pkeyopt digest:sha256 -pkeyopt rsa_padding_mode:pss -pkeyopt rsa_pss_saltlen:32 -pkeyopt rsa_mgf1_md:sha256"

    # Generate hash of the binary file
    openssl dgst $HASH_OPTS -out "$HASH_FILE" "$INPUT_FILE"
    echo "Generated hash of $INPUT_FILE and saved to $HASH_FILE"

    # Generate signatures with each of the three keys
    openssl pkeyutl -sign -inkey "$KEY1" -in "$HASH_FILE" -out "$SIG_FILE1" $SIGN_OPTS
    openssl pkeyutl -sign -inkey "$KEY2" -in "$HASH_FILE" -out "$SIG_FILE2" $SIGN_OPTS
    openssl pkeyutl -sign -inkey "$KEY3" -in "$HASH_FILE" -out "$SIG_FILE3" $SIGN_OPTS
    echo "Generated signatures for $INPUT_FILE using keys $KEY1, $KEY2, and $KEY3. Signatures saved to $SIG_FILE1, $SIG_FILE2, and $SIG_FILE3"

    # Apply the signatures to the binary file
    espsecure sign-data --version 2 --pub-key "$KEY1" "$KEY2" "$KEY3" --signature "$SIG_FILE1" "$SIG_FILE2" "$SIG_FILE3" --output "$OUTPUT_FILE" "$INPUT_FILE"

    echo "Successfully signed $INPUT_FILE. Signed file: $OUTPUT_FILE"
    echo "Signature information:"
    espsecure signature-info-v2 "$OUTPUT_FILE"
}

if [ "$SIGN" -eq 1 ]; then
    if [ ! -f "$BIN_FILE" ]; then
        echo "File $BIN_FILE does not exist."
        exit 1
    fi
    if [ ! -f "$BOOTLOADER_FILE" ]; then
        echo "File $BOOTLOADER_FILE does not exist."
        exit 1
    fi

    sign_binary "$BIN_FILE" "$SIGNED_BIN_FILE"
    sign_binary "$BOOTLOADER_FILE" "$SIGNED_BOOTLOADER_FILE"

elif [ "$QEMU" -eq 1 ]; then
    FLASH_ARGS_FILE=flash_args
    FLASH_ARGS_SIGNED_FILE="${FLASH_ARGS_FILE}_signed"

    if [ ! -f "$SIGNED_BIN_FILE" ]; then
        echo "File $SIGNED_BIN_FILE does not exist."
        exit 1
    fi
    if [ ! -f "$SIGNED_BOOTLOADER_FILE" ]; then
        echo "File $SIGNED_BOOTLOADER_FILE does not exist."
        exit 1
    fi

    # replace binaries in the flash_args file with signed binaries
    # and merge the binaries into a single binary for qemu using esptool.py
    if [ ! -f "build/$FLASH_ARGS_FILE" ]; then
        echo "File $FLASH_ARGS_FILE does not exist."
        exit 1
    fi
    MERGED_BIN_FILE="qemu_flash_image.bin"
    cd build
    sed "s|$BIN_NAME|$SIGNED_BIN_NAME|g" "$FLASH_ARGS_FILE" > "$FLASH_ARGS_SIGNED_FILE"
    sed -i "s|$BOOTLOADER_NAME|$SIGNED_BOOTLOADER_NAME|g" "$FLASH_ARGS_SIGNED_FILE"
    echo "Updated flash arguments file with signed binaries: $FLASH_ARGS_SIGNED_FILE"
    esptool --chip esp32s3 merge-bin -o "$MERGED_BIN_FILE" --pad-to-size 4MB @"$FLASH_ARGS_SIGNED_FILE"
    cd ..
    MERGED_BIN_FILE="build/$MERGED_BIN_FILE"
    echo "Merged binary for QEMU: $MERGED_BIN_FILE"

    # create an efuse binary file for qemu
    EFUSE_BIN="qemu_efuse.bin"
    if [ -f "$EFUSE_BIN" ]; then
        echo "EFUSE binary $EFUSE_BIN already exists. Skipping creation."
    else
        echo "Creating EFUSE binary $EFUSE_BIN"
        dd if=/dev/zero bs=1KiB count=1 of="$EFUSE_BIN"
    fi

    # run qemu
    echo "============================="
    echo "Starting QEMU with signed binary..."
    echo "============================="
    qemu-system-xtensa -nographic -machine esp32s3 \
        -drive file="$MERGED_BIN_FILE",if=mtd,format=raw \
        -drive file="$EFUSE_BIN",if=none,format=raw,id=efuse \
        -global driver=nvram.esp32s3.efuse,property=drive,value=efuse \
        -global driver=timer.esp32s3.timg,property=wdt_disable,value=true &
    QEMU_PID=$!

    # wait for qemu to boot and print the secure boot status
    sleep 2

    # kill the qemu process
    kill $QEMU_PID

fi
