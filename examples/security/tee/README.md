# ESP-TEE (Trusted Execution Environment) Examples

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

A Trusted Execution Environment (TEE) serves as a isolated processing environment where applications can run and data can be stored __securely__. This environment ensures the confidentiality and integrity of sensitive information, safeguarding it from unauthorized access, even in the presence of potential threats in the rest of the system.

## Examples

### `tee_basic`

- In this example, the REE raises a request for AES operation in TEE through the secure service call interface. The TEE performs encrypts/decrypts the given buffer with the `aes-256-cbc` mode using the key protected by TEE.
- The result of the AES operation is returned in the output buffer provided in the secure service call by the REE. It also demonstrates how to create and add a custom service call to the ESP-TEE framework.

### `tee_secure_storage`

- This example demonstrates the ESP-TEE framework's Secure Storage with two workflows:
  - Signing and verification
    - Create and securely store an ECDSA `secp256r1` keypair in protected memory (secure storage partition)
    - Sign a message in TEE using the stored ECDSA keypair from a given slot ID
    - Retrieve the corresponding ECDSA public key for verification
    - Verify the signature in REE
  - Encryption and decryption
    - Generate and securely store an AES-256 key in the secure storage partition
    - Encrypt a message in the TEE using the AES key stored in the given slot ID with the `aes-256-gcm` algorithm and generate an authentication tag
    - Decrypt the ciphertext using the same AES key and validate the authentication tag
    - Verify that the decrypted message matches the original

### `tee_secure_ota`

This example illustrates a secure Over-The-Air (OTA) update for the TEE app with rollback support. The REE fetches the new TEE image from the HTTP/S server and securely writes it to the TEE passive partition using the secure service call interface. It also updates the TEE OTA data partition for switching the active TEE partition for subsequent boots.

### `tee_attestation`

- In this example, the REE initiates a request to generate an entity attestation token in the TEE through the secure service call interface. The TEE creates the token, including information like image and ESP-IDF versions, along with SHA256 digests of the bootloader, active TEE, and app images.
- The token is signed using the ECDSA key stored in the designated slot ID of the TEE's Secure Storage. Subsequently, the resulting token is handed back to the REE within the output buffer specified in the secure service call.
