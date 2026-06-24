#!/usr/bin/env python
#
# ESP32 x509 certificate bundle generation utility
#
# Converts PEM and DER certificates to a custom bundle format which stores just the
# subject name and public key to reduce space
#
# The bundle will have the format: number of certificates; crt 1 subject name length; crt 1 public key length;
# crt 1 subject name; crt 1 public key; crt 2...
#
# SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import csv
import os
import re
import struct
import sys
import warnings

DEFAULT_CERT_BUNDLE_MAX_CERTS = 200

# Ignore warning about non-positive serial numbers in certificates
# Some CA certificates from the certificate bundle contain zero as serial number
# Please see https://github.com/pyca/cryptography/issues/12948 for more details
warnings.filterwarnings(
    'ignore',
    message=(
        r"Parsed a serial number which wasn't positive \(i.e., it was negative or zero\), "
        'which is disallowed by RFC 5280. '
        'Loading this certificate will cause an exception in a future release of cryptography.'
    ),
)

try:
    from cryptography import x509
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives import serialization
except ImportError:
    print(
        'The cryptography package is not installed.'
        'Please refer to the Get Started section of the ESP-IDF Programming Guide for '
        'setting up the required packages.'
    )
    raise

ca_bundle_bin_file = 'x509_crt_bundle'

quiet = False


def status(msg):
    """Print status message to stderr"""
    if not quiet:
        critical(msg)


def critical(msg):
    """Print critical message to stderr"""
    sys.stderr.write('gen_crt_bundle.py: ')
    sys.stderr.write(msg)
    sys.stderr.write('\n')


class CertificateBundle:
    def __init__(self):
        self.certificates = []
        self.compressed_crts = []

    def add_from_path(self, crts_path):
        found = False
        for file_path in os.listdir(crts_path):
            found |= self.add_from_file(os.path.join(crts_path, file_path))

        if found is False:
            raise InputError('No valid x509 certificates found in %s' % crts_path)

    def add_from_file(self, file_path):
        try:
            if file_path.endswith('.pem'):
                status('Parsing certificates from %s' % file_path)
                with open(file_path, 'r', encoding='utf-8') as f:
                    crt_str = f.read()
                    self.add_from_pem(crt_str)
                    return True

            elif file_path.endswith('.der'):
                status('Parsing certificates from %s' % file_path)
                with open(file_path, 'rb') as f:
                    crt_str = f.read()
                    self.add_from_der(crt_str)
                    return True

        except ValueError:
            critical('Invalid certificate in %s' % file_path)
            raise InputError('Invalid certificate')

        return False

    def add_from_pem(self, crt_str):
        """A single PEM file may have multiple certificates"""

        crt = ''
        count = 0
        start = False

        for strg in crt_str.splitlines(True):
            if strg == '-----BEGIN CERTIFICATE-----\n' and start is False:
                crt = ''
                start = True
            elif strg == '-----END CERTIFICATE-----\n' and start is True:
                crt += strg + '\n'
                start = False
                self.certificates.append(x509.load_pem_x509_certificate(crt.encode(), default_backend()))
                count += 1
            if start is True:
                crt += strg

        if count == 0:
            status('No certificate found')
        else:
            status('Successfully added %d certificates' % count)

    def add_from_der(self, crt_str):
        self.certificates.append(x509.load_der_x509_certificate(crt_str, default_backend()))
        status('Successfully added 1 certificate')

    def create_bundle(self, max_certs=DEFAULT_CERT_BUNDLE_MAX_CERTS):
        if max_certs < len(self.certificates):
            critical(
                f'No. of certs in the certificate bundle = {len(self.certificates)} exceeds\n \
                    Max allowed certificates in the certificate bundle = {max_certs} \
                    Please update the menuconfig option with appropriate value'
            )
            raise ValueError

        # Sort certificates in order to do binary search when looking up certificates
        self.certificates = sorted(self.certificates, key=lambda cert: cert.subject.public_bytes(default_backend()))

        # List of offsets in bytes from the start of the bundle to each certificate inside
        offsets = []
        len_offsets = 4 * len(self.certificates)  # final size of the offsets list

        bundle = b''

        for crt in self.certificates:
            """ Read the public key as DER format """
            pub_key = crt.public_key()
            pub_key_der = pub_key.public_bytes(
                serialization.Encoding.DER, serialization.PublicFormat.SubjectPublicKeyInfo
            )

            """ Read the subject name as DER format """
            sub_name_der = crt.subject.public_bytes(default_backend())

            name_len = len(sub_name_der)
            key_len = len(pub_key_der)
            len_data = struct.pack('<HH', name_len, key_len)

            # Certificate starts at this position in the bundle
            offsets.append(len_offsets + len(bundle))

            bundle += len_data
            bundle += sub_name_der
            bundle += pub_key_der

        # Output all offsets before the first certificate
        bundle = struct.pack('<{0:d}L'.format(len(offsets)), *offsets) + bundle

        return bundle

    def add_with_filter(self, crts_path, filter_path):
        filter_set = set()
        with open(filter_path, 'r', encoding='utf-8') as f:
            csv_reader = csv.reader(f, delimiter=',')

            # Skip header
            next(csv_reader)
            for row in csv_reader:
                filter_set.add(row[1])

        status('Parsing certificates from %s' % crts_path)
        crt_str = []
        with open(crts_path, 'r', encoding='utf-8') as f:
            crt_str = f.read()

            # Split all certs into a list of (name, certificate string) tuples
            pem_crts = re.findall(r'(^.+?)\n(=+\n[\s\S]+?END CERTIFICATE-----\n)', crt_str, re.MULTILINE)

            filtered_crts = ''
            for name, crt in pem_crts:
                if name in filter_set:
                    filtered_crts += crt

        self.add_from_pem(filtered_crts)


class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)


def main():
    global quiet

    parser = argparse.ArgumentParser(description='ESP-IDF x509 certificate bundle utility')

    parser.add_argument('--quiet', '-q', help="Don't print non-critical status messages to stderr", action='store_true')
    parser.add_argument(
        '--input',
        '-i',
        nargs='+',
        required=True,
        help='Paths to the custom certificate folders or files to parse, parses all .pem or .der files',
    )
    parser.add_argument(
        '--filter',
        '-f',
        help='Path to CSV-file where the second columns contains the name of the certificates \
                        that should be included from cacrt_all.pem',
    )
    parser.add_argument(
        '--max-certs',
        '-m',
        help='Maximum number of certificates allowed in the certificate bundle',
        type=int,
        default=DEFAULT_CERT_BUNDLE_MAX_CERTS,
    )

    args = parser.parse_args()

    quiet = args.quiet

    bundle = CertificateBundle()

    for path in args.input:
        if os.path.isfile(path):
            if os.path.basename(path) == 'cacrt_all.pem' and args.filter:
                bundle.add_with_filter(path, args.filter)
            else:
                bundle.add_from_file(path)
        elif os.path.isdir(path):
            bundle.add_from_path(path)
        else:
            raise InputError('Invalid --input=%s, is neither file nor folder' % args.input)

    status('Successfully added %d certificates in total' % len(bundle.certificates))

    crt_bundle = bundle.create_bundle(args.max_certs)

    with open(ca_bundle_bin_file, 'wb') as f:
        f.write(crt_bundle)


if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e)
        sys.exit(2)
