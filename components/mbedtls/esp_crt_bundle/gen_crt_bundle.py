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
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import csv
import os
import re
import struct
import warnings

import rich_click as click
from esp_pylib.cli_options import OptionEatAll
from esp_pylib.errors import FatalError
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape

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
    log.die(
        'The cryptography package is not installed. '
        'Please refer to the Get Started section of the ESP-IDF Programming Guide for '
        'setting up the required packages.'
    )

ca_bundle_bin_file = 'x509_crt_bundle'

# Only files with these extensions are parsed, the extension has to match the encoding of the file
PEM_FILE_EXTENSION = '.pem'
DER_FILE_EXTENSION = '.der'

quiet = False


def status(msg):
    """Print a non-critical status message unless --quiet was given"""
    if not quiet:
        log.print(msg)


class CertificateBundle:
    def __init__(self):
        self.certificates = []
        self.compressed_crts = []

    def add_from_path(self, crts_path):
        found = False
        for file_path in os.listdir(crts_path):
            found |= self.add_from_file(os.path.join(crts_path, file_path), strict=False)

        if found is False:
            raise InputError(f'No valid x509 certificates found in {crts_path}')

    def add_from_file(self, file_path, strict=True):
        """Parse a certificate file

        Only files with a .pem or .der extension are parsed. If strict is True (the file was
        given directly on the command line), any other extension is an error, otherwise
        (while scanning a directory) the file is skipped with a warning.
        """
        try:
            if file_path.endswith(PEM_FILE_EXTENSION):
                status(f'Parsing certificates from {escape(file_path)}')
                with open(file_path, encoding='utf-8') as f:
                    crt_str = f.read()
                    self.add_from_pem(crt_str)
                    return True

            elif file_path.endswith(DER_FILE_EXTENSION):
                status(f'Parsing certificates from {escape(file_path)}')
                with open(file_path, 'rb') as f:
                    crt_str = f.read()
                    self.add_from_der(crt_str)
                    return True

            elif os.path.isfile(file_path):
                msg = (
                    f'Unsupported file extension in {file_path}, certificates are only parsed from files with a '
                    f'{PEM_FILE_EXTENSION} (PEM encoded) or {DER_FILE_EXTENSION} (DER encoded) extension. '
                    'Please rename the file to match its encoding.'
                )
                if strict:
                    raise InputError(msg)
                log.warn(f'Skipping file. {escape(msg)}')

        except ValueError:
            raise InputError(f'Invalid certificate in {file_path}')

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
            status(f'Successfully added {count} certificates')

    def add_from_der(self, crt_str):
        self.certificates.append(x509.load_der_x509_certificate(crt_str, default_backend()))
        status('Successfully added 1 certificate')

    def create_bundle(self, max_certs=DEFAULT_CERT_BUNDLE_MAX_CERTS):
        if max_certs < len(self.certificates):
            raise InputError(
                f'The certificate bundle has {len(self.certificates)} certificates, '
                f'exceeding the maximum of {max_certs}. '
                'Please update the menuconfig option with an appropriate value.'
            )

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
        bundle = struct.pack(f'<{len(offsets):d}L', *offsets) + bundle

        return bundle

    def add_with_filter(self, crts_path, filter_path):
        filter_set = set()
        with open(filter_path, encoding='utf-8') as f:
            csv_reader = csv.reader(f, delimiter=',')

            # Skip header
            next(csv_reader)
            for row in csv_reader:
                filter_set.add(row[1])

        status(f'Parsing certificates from {escape(crts_path)}')
        crt_str = []
        with open(crts_path, encoding='utf-8') as f:
            crt_str = f.read()

            # Split all certs into a list of (name, certificate string) tuples
            pem_crts = re.findall(r'(^.+?)\n(=+\n[\s\S]+?END CERTIFICATE-----\n)', crt_str, re.MULTILINE)

            filtered_crts = ''
            for name, crt in pem_crts:
                if name in filter_set:
                    filtered_crts += crt

        self.add_from_pem(filtered_crts)


class InputError(FatalError):
    """Raised when the given certificate input cannot be turned into a bundle"""

    pass


@click.command(context_settings={'help_option_names': ['-h', '--help']})
@click.option(
    '--quiet',
    '-q',
    'quiet_flag',
    is_flag=True,
    help="Don't print non-critical status messages",
)
@click.option(
    '--input',
    '-i',
    'input_paths',
    multiple=True,
    required=True,
    cls=OptionEatAll,
    type=str,
    help='Paths to the custom certificate folders or files to parse, parses all .pem or .der files',
)
@click.option(
    '--filter',
    '-f',
    'filter_path',
    help='Path to CSV-file where the second column contains the name of the certificates '
    'that should be included from cacrt_all.pem',
)
@click.option(
    '--max-certs',
    '-m',
    type=int,
    default=DEFAULT_CERT_BUNDLE_MAX_CERTS,
    show_default=True,
    help='Maximum number of certificates allowed in the certificate bundle',
)
def cli(quiet_flag, input_paths, filter_path, max_certs):
    """ESP-IDF x509 certificate bundle utility"""
    global quiet

    # The build system captures this output through a pipe, where Rich would wrap at 80
    # columns and break the certificate paths in the messages across lines.
    log.set_console_options(soft_wrap=True)

    quiet = quiet_flag

    bundle = CertificateBundle()

    for path in input_paths:
        if os.path.isfile(path):
            if os.path.basename(path) == 'cacrt_all.pem' and filter_path:
                bundle.add_with_filter(path, filter_path)
            else:
                bundle.add_from_file(path)
        elif os.path.isdir(path):
            bundle.add_from_path(path)
        else:
            raise InputError(f'Invalid --input={path}, is neither file nor folder')

    status(f'Successfully added {len(bundle.certificates)} certificates in total')

    crt_bundle = bundle.create_bundle(max_certs)

    with open(ca_bundle_bin_file, 'wb') as f:
        f.write(crt_bundle)


def main():
    install_exception_reporting()
    try:
        cli()
    except InputError as e:
        log.die(escape(str(e)), exit_code=2)


if __name__ == '__main__':
    main()
