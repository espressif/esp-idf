# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# Check that the Secure Boot signing key matches the selected app signing
# scheme. The signing step (espsecure sign-data) derives the signature type
# from the key file itself, so a mismatching key would produce an image that
# only fails signature verification at boot.
#
# Invoked at configure time from components/bootloader/project_include.cmake.
# Exits non-zero with an explanatory message on a mismatch.
import argparse

from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.serialization import load_pem_private_key

ECDSA_KEY_SIZE_TO_CURVE = {
    '256': 'secp256r1',
    '384': 'secp384r1',
}


def key_description(key: object) -> str:
    if isinstance(key, rsa.RSAPrivateKey):
        return f'an RSA {key.key_size}-bit key'
    if isinstance(key, ec.EllipticCurvePrivateKey):
        return f'an ECDSA key (curve {key.curve.name})'
    return f'a {type(key).__name__} key'


def main() -> None:
    parser = argparse.ArgumentParser(
        description='Check that the secure boot signing key matches the selected app signing scheme'
    )
    parser.add_argument(
        '--scheme',
        choices=['rsa', 'ecdsa_v1', 'ecdsa_v2'],
        required=True,
        help='App signing scheme selected in sdkconfig',
    )
    parser.add_argument(
        '--ecdsa-key-size',
        choices=sorted(ECDSA_KEY_SIZE_TO_CURVE),
        default=None,
        help='ECDSA key size selected in sdkconfig (ECDSA V2 scheme only)',
    )
    parser.add_argument('key_file', help='Path to the secure boot signing key')
    args = parser.parse_args()

    try:
        with open(args.key_file, 'rb') as f:
            key = load_pem_private_key(f.read(), password=None)
    except Exception:
        raise SystemExit(
            f'{args.key_file} could not be parsed as a PEM private key. '
            'Secure boot signing keys must be unencrypted PEM private keys.'
        )

    if args.scheme == 'rsa':
        if not isinstance(key, rsa.RSAPrivateKey):
            raise SystemExit(
                f'{args.key_file} is {key_description(key)}, but the selected app signing scheme '
                'requires an RSA key. Update the App Signing Scheme choice or point '
                'CONFIG_SECURE_BOOT_SIGNING_KEY to a matching key.'
            )
        return

    if args.scheme in ('ecdsa_v1', 'ecdsa_v2'):
        if not isinstance(key, ec.EllipticCurvePrivateKey):
            raise SystemExit(
                f'{args.key_file} is {key_description(key)}, but the selected app signing scheme '
                'requires an ECDSA key. Update the App Signing Scheme choice or point '
                'CONFIG_SECURE_BOOT_SIGNING_KEY to a matching key.'
            )

        if args.scheme == 'ecdsa_v1':
            expected_curve = ECDSA_KEY_SIZE_TO_CURVE['256']
        else:
            expected_curve = ECDSA_KEY_SIZE_TO_CURVE.get(args.ecdsa_key_size or '', '')

        if expected_curve and key.curve.name != expected_curve:
            raise SystemExit(
                f'{args.key_file} is {key_description(key)}, but the selected app signing scheme '
                f'requires curve {expected_curve}. Signature verification would fail at boot. '
                'Update the ECDSA key size choice or point CONFIG_SECURE_BOOT_SIGNING_KEY to '
                'a matching key.'
            )


if __name__ == '__main__':
    main()
