#!/bin/bash

set -e

if [ -z "$IDF_PATH" ]; then
    echo "Please set IDF_PATH environment variable to the path of the ESP-IDF repository."
    exit 1
fi

KEY1=secure_boot_signing_key1.pem
KEY2=secure_boot_signing_key2.pem
KEY3=secure_boot_signing_key3.pem
DIGEST1=secure_boot_signing_key1.digest
DIGEST2=secure_boot_signing_key2.digest
DIGEST3=secure_boot_signing_key3.digest
if [ ! -f "$KEY1" ]; then
    echo "Creating $KEY1..."
    idf.py secure-generate-signing-key -v2 --scheme rsa3072 "$KEY1"
else
    echo "Signing key $KEY1 already exists. Skipping creation."
fi
if [ ! -f "$KEY2" ]; then
    echo "Creating $KEY2..."
    idf.py secure-generate-signing-key -v2 --scheme rsa3072 "$KEY2"
else
    echo "Signing key $KEY2 already exists. Skipping creation."
fi
if [ ! -f "$KEY3" ]; then
    echo "Creating $KEY3..."
    idf.py secure-generate-signing-key -v2 --scheme rsa3072 "$KEY3"
else
    echo "Signing key $KEY3 already exists. Skipping creation."
fi
if [ ! -f "$DIGEST1" ]; then
    echo "Creating digest for $KEY1..."
    idf.py secure-generate-key-digest --keyfile "$KEY1" --output "$DIGEST1"
else
    echo "Digest $DIGEST1 already exists. Skipping creation."
fi
if [ ! -f "$DIGEST2" ]; then
    echo "Creating digest for $KEY2..."
    idf.py secure-generate-key-digest --keyfile "$KEY2" --output "$DIGEST2"
else
    echo "Digest $DIGEST2 already exists. Skipping creation."
fi
if [ ! -f "$DIGEST3" ]; then
    echo "Creating digest for $KEY3..."
    idf.py secure-generate-key-digest --keyfile "$KEY3" --output "$DIGEST3"
else
    echo "Digest $DIGEST3 already exists. Skipping creation."
fi
