| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# TEE: Attestation example

## Overview

- Attestation is the process of securely providing information about a device to external parties through a cryptographically secured token. It enables IoT devices, especially those without user interfaces or in remote locations, to introduce themselves securely to networks and IoT platforms by offering attestation information.
- In this example, the REE initiates a request to generate an entity attestation token in the TEE through the secure service call interface. The TEE creates the token, including information such as -
  - For all firmware images (Bootloader, active TEE and non-secure app)
      - Project and ESP-IDF version
      - Digest (SHA256)
  - Public key corresponding to the private key used for signing (in compressed format)
  - Signature (`r` and `s` components)
- The token is signed using the ECDSA key stored with the configured ID of the TEE's Secure Storage. Subsequently, the resulting token is handed back to the REE in the output buffer specified in the secure service call.

<details>
  <summary><b>Attestation: Sample Token</b></summary>

```json
{
  "header": {
    "magic": "44fef7cc",
    "encr_alg": "",
    "sign_alg": "ecdsa_secp256r1_sha256",
    "key_id": "tee_att_key0"
  },
  "eat": {
    "auth_challenge": "dcb9b53143ad6b081dad1a05c7ebda4e314d388762215799cf24ed52e9387678",
    "client_id": 262974944,
    "chip_id": 13,
    "device_ver": 1,
    "ueid": {
      "mac": "d885ac67c978",
      "optional_id": "94fa4d7e305682714d48e7bbd710c961"
    },
    "device_id": "e8cddb2a7f9a5a7c61735d6dda26e4bd153c6d772a9be6f26bd321dfe25e0ac8",
    "instance_id": "1adba85e0df997fd961f25a9e312430cef162b5c69466cd5b172f1e65ac7360c",
    "psa_cert_ref": "0716053550477-10100",
    "device_status": 255,
    "sw_claims": {
      "tee": {
        "type": 1,
        "ver": "1.2.0~v6.2.0",
        "idf_ver": "v6.2-dev-3162-g9aa4f93857d",
        "secure_ver": 0,
        "secure_ver_efuse": 0,
        "part_chip_rev": {
          "min": 0,
          "max": 99
        },
        "part_digest": {
          "type": 0,
          "calc_digest": "f732e7f285b7de7ac3167a867711eddbf17a2a05513d35e41cd1ebf2e0958b2e",
          "digest_validated": true,
          "sign_verified": true,
          "secure_padding": true,
          "calc_code_hash": "33e9d223e4f861e9f7a9f5209ee27cd4f69894bcfcd76bd96bb90c98795a24cd",
          "code_hash_ver": 1
        }
      },
      "app": {
        "type": 2,
        "ver": "v0.1.0",
        "idf_ver": "v6.2-dev-3162-g9aa4f93857d",
        "secure_ver": 0,
        "secure_ver_efuse": 0,
        "part_chip_rev": {
          "min": 0,
          "max": 99
        },
        "part_digest": {
          "type": 0,
          "calc_digest": "21e114fd30b9234c501525990dfab71d00348c531bb64224feff9deb32e66f9f",
          "digest_validated": true,
          "sign_verified": true,
          "secure_padding": true
        }
      },
      "bootloader": {
        "type": 0,
        "ver": "",
        "idf_ver": "",
        "secure_ver": -1,
        "secure_ver_efuse": -1,
        "part_chip_rev": {
          "min": 0,
          "max": 99
        },
        "part_digest": {
          "type": 0,
          "calc_digest": "516148649a7f670b894391ded9d64a0e8604c5cec9a1eeb0014d2549cdaa4725",
          "digest_validated": true,
          "sign_verified": true
        }
      }
    }
  },
  "public_key": {
    "compressed": "02a45c6c94c4be7722bd2513f4ccbc4daa369747e6e96e0f9f7a2eba055dee6d46"
  },
  "sign": {
    "r": "37bcc8ed9c15a4712c18fe20b257992e5d9ec273b6261675f247667b4575495b",
    "s": "28ce15da73880f7d5ee303948769b197077208f1f242aaee448e9ed23f9085fa"
  }
}
```
</details>

## How to use the example

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the TEE framework (see Supported Targets table above).

### Configure the project

Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

Open the project configuration menu (`idf.py menuconfig`).

- Configure the secure storage key ID for generating/fetching the ECDSA keypair for attestation token signing at `ESP-TEE (Trusted Execution Environment) → Secure Services → Attestation: Secure Storage key ID for EAT signing`.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Example Output

```log
I (438) example_tee_attest: TEE Attestation Service
I (1008) example_tee_attest: Attestation token - Length: 1813
I (1018) example_tee_attest: Attestation token - Data:
'{"header":{"magic":"44fef7cc","encr_alg":"","sign_alg":"ecdsa_secp256r1_sha256","key_id":"tee_att_key0"},"eat":{"auth_challenge":"dcb9b53143ad6b081dad1a05c7ebda4e314d388762215799cf24ed52e9387678","client_id":262974944,"chip_id":13,"device_ver":1,"ueid":{"mac":"d885ac67c978","optional_id":"94fa4d7e305682714d48e7bbd710c961"},"device_id":"e8cddb2a7f9a5a7c61735d6dda26e4bd153c6d772a9be6f26bd321dfe25e0ac8","instance_id":"1adba85e0df997fd961f25a9e312430cef162b5c69466cd5b172f1e65ac7360c","psa_cert_ref":"0716053550477-10100","device_status":255,"sw_claims":{"tee":{"type":1,"ver":"1.2.0~v6.2.0","idf_ver":"v6.2-dev-3162-g9aa4f93857d","secure_ver":0,"secure_ver_efuse":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"f732e7f285b7de7ac3167a867711eddbf17a2a05513d35e41cd1ebf2e0958b2e","digest_validated":true,"sign_verified":true,"secure_padding":true,"calc_code_hash":"33e9d223e4f861e9f7a9f5209ee27cd4f69894bcfcd76bd96bb90c98795a24cd","code_hash_ver":1}},"app":{"type":2,"ver":"v0.1.0","idf_ver":"v6.2-dev-3162-g9aa4f93857d","secure_ver":0,"secure_ver_efuse":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"21e114fd30b9234c501525990dfab71d00348c531bb64224feff9deb32e66f9f","digest_validated":true,"sign_verified":true,"secure_padding":true}},"bootloader":{"type":0,"ver":"","idf_ver":"","secure_ver":-1,"secure_ver_efuse":-1,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"516148649a7f670b894391ded9d64a0e8604c5cec9a1eeb0014d2549cdaa4725","digest_validated":true,"sign_verified":true}}}},"public_key":{"compressed":"02a45c6c94c4be7722bd2513f4ccbc4daa369747e6e96e0f9f7a2eba055dee6d46"},"sign":{"r":"37bcc8ed9c15a4712c18fe20b257992e5d9ec273b6261675f247667b4575495b","s":"28ce15da73880f7d5ee303948769b197077208f1f242aaee448e9ed23f9085fa"}}'
I (1148) main_task: Returned from app_main()
```

**Note:** The generated token's signature can be verified using the script given below.

<details>
  <summary><b>Attestation: Verifying the generated token</b></summary>

**Usage**: `python verify_att_token.py '<eat_json_string>'`

```python
import argparse
import hashlib
import json

from cryptography.hazmat.primitives.asymmetric import utils
from ecdsa.curves import NIST256p
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der

# Fetch the token from the console
parser = argparse.ArgumentParser(description='Verify the given Entity Attestation Token')
parser.add_argument('att_token', type=str, help='the EAT string to verify')

args = parser.parse_args()

# Parsing the token
tk_info = json.loads(args.att_token)

# Fetching the data to be verified
tk_hdr_val = json.dumps(tk_info['header'], separators=(',', ':')).encode('latin-1')
tk_eat_val = json.dumps(tk_info['eat'], separators=(',', ':')).encode('latin-1')
tk_pubkey_val = json.dumps(tk_info['public_key'], separators=(',', ':')).encode('latin-1')

# Pre-hashing the data
ctx = hashlib.new('sha256')
ctx.update(tk_hdr_val)
ctx.update(tk_eat_val)
ctx.update(tk_pubkey_val)
digest = ctx.digest()

# Fetching the public key
tk_pubkey_c = bytes.fromhex(tk_info['public_key']['compressed'])

# Fetching the appended signature
tk_sign_r = bytes.fromhex(tk_info['sign']['r'])
tk_sign_s = bytes.fromhex(tk_info['sign']['s'])

# Construct the signature using the R and S components
signature = utils.encode_dss_signature(int.from_bytes(tk_sign_r, 'big'), int.from_bytes(tk_sign_s, 'big'))

# Uncompress the public key and verify the signature
vk = VerifyingKey.from_string(tk_pubkey_c, NIST256p, hashfunc=hashlib.sha256)
assert vk.verify_digest(signature, digest, sigdecode=sigdecode_der)

print('Token signature verified!')
```
</details>
