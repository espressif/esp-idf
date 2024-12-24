| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# TEE: Attestation example

## Overview

- Attestation is the process of securely providing information about a device to external parties through a cryptographically secured token. It enables IoT devices, especially those without user interfaces or in remote locations, to introduce themselves securely to networks and IoT platforms by offering attestation information.
- In this example, the REE initiates a request to generate an entity attestation token in the TEE through the secure service call interface. The TEE creates the token, including information such as -
  - For all firmware images (Bootloader, active TEE and non-secure app)
      - Project and ESP-IDF version
      - Digest (SHA256)
  - Public key corresponding to the private key used for signing (in compressed format)
  - Signature (`r` and `s` components)
- The token is signed using the ECDSA key stored in the designated slot ID of the TEE's Secure Storage. Subsequently, the resulting token is handed back to the REE in the output buffer specified in the secure service call.

<details>
  <summary><b>Attestation: Sample Token</b></summary>

```json
{
  "header": {
    "magic": "44fef7cc",
    "encr_alg": "",
    "sign_alg": "ecdsa_secp256r1_sha256",
    "key_id": 0
  },
  "eat": {
    "nonce": -1582119980,
    "client_id": 262974944,
    "device_ver": 1,
    "device_id": "e8cddb2a7f9a5a7c61735d6dda26e4bd153c6d772a9be6f26bd321dfe25e0ac8",
    "instance_id": "1adba85e0df997fd961f25a9e312430cef162b5c69466cd5b172f1e65ac7360c",
    "psa_cert_ref": "0716053550477-10100",
    "device_status": 255,
    "sw_claims": {
      "tee": {
        "type": 1,
        "ver": "v0.3.0",
        "idf_ver": "v5.1-679-gcba25b2512",
        "secure_ver": 0,
        "part_chip_rev": {
          "min": 0,
          "max": 99
        },
        "part_digest": {
          "type": 0,
          "calc_digest": "f732e7f285b7de7ac3167a867711eddbf17a2a05513d35e41cd1ebf2e0958b2e",
          "digest_validated": true,
          "sign_verified": true,
          "secure_padding": true
        }
      },
      "app": {
        "type": 2,
        "ver": "v0.1.0",
        "idf_ver": "v5.1-679-gcba25b2512",
        "secure_ver": 0,
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

- Configure the secure storage slot ID for generating/fetching the ECDSA keypair for attestation token signing at `(Top) → Security features → TEE: Secure Storage slot ID for EAT signing`.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Usage

- Use console commands to dump the attestation info: `tee_att_info`
- The generated token's signature can be verified using the script given below.

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

### Example Output

```log
I (416) main_task: Calling app_main()
I (416) example_tee_attest: TEE Attestation Service

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (476) main_task: Returned from app_main()
esp32c6> tee_att_info
I (6206) cmd_tee_attest: Attestation token - Length: 1525
I (6206) cmd_tee_attest: Attestation token - Data:
'{"header":{"magic":"44fef7cc","encr_alg":"","sign_alg":"ecdsa_secp256r1_sha256","key_id":0},"eat":{"nonce":-1582119980,"client_id":262974944,"device_ver":1,"device_id":"4ecc458ef4290329552b4dcdccb99d55e5ea7624f24c87b27b71515e1666f39c","instance_id":"77eb3dfec7633302fe4bcf04ffe3be5e83c0513057aa070d387f1e8350271329","psa_cert_ref":"0716053550477-10100","device_status":165,"sw_claims":{"tee":{"type":1,"ver":"1.0.0","idf_ver":"v5.5-dev-727-g624f640f61d-dirty","secure_ver":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"6e6548a5d64cd3d6e2e6dc166384f32f73558fbd9c0c0985c6095d643f053eb5","digest_validated":true,"sign_verified":false,"secure_padding":false}},"app":{"type":2,"ver":"v0.1.0","idf_ver":"v5.5-dev-727-g624f640f61d-dirty","secure_ver":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"7f10992d4bb32c497184fd2da0e3a593b235d82bde24de868c8eb4636d4b7bdc","digest_validated":true,"sign_verified":false,"secure_padding":false}},"bootloader":{"type":0,"ver":"01000000","idf_ver":"v5.5-dev-727-g624f640f61d-dirty","secure_ver":0,"part_chip_rev":{"min":0,"max":99},"part_digest":{"type":0,"calc_digest":"2cdf1bac1792df04ad10d67287ef3ab7024e183dc32899a190668cbb7d21a5a8","digest_validated":true,"sign_verified":false}}}},"public_key":{"compressed":"030df5c5fd9a4096a58ba16dfc4f1d53781bab555fc307d71367f0afc663005174"},"sign":{"r":"c3a0fc8ce3cd1dec2a0e38c4a63c03bd1e044febd5847178fe304b06d48b3eaf","s":"c8e34bc5d854e728cffdfd701ea09deabc9a9a22c4b06f312a61a1448a56b8b1"}}'
```