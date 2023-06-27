# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import argparse
import datetime
import os

from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, rsa
from cryptography.hazmat.primitives.serialization import Encoding, NoEncryption, PrivateFormat
from cryptography.x509.oid import ExtendedKeyUsageOID

CERT_KEYS = ['2048', '4096', '3072', 'p384']
CERT_EXPIRY_DAYS = 3650


class GenCerts:
    def __init__(self, cert_type: str, message_digest: str, file_path: str) -> None:
        self.cert_type = cert_type
        self.message_digest = message_digest
        self.file_path = file_path
        self.digest = hashes.SHA256()
        self.init_param()

    def init_param(self) -> None:

        if self.message_digest == 'sha256':
            self.digest = hashes.SHA256()
        else:
            self.digest = hashes.SHA384()

    def generate_ca_certificate(self) -> None:

        if self.cert_type == 'p384':
            private_key = ec.generate_private_key(
                ec.SECP384R1(),
                default_backend()
            )
        else:
            private_key = rsa.generate_private_key(
                public_exponent=65537,
                key_size=int(self.cert_type)
            )

        public_key = private_key.public_key()
        subject = issuer = x509.Name([
            x509.NameAttribute(x509.NameOID.COMMON_NAME, 'example root CA')
        ])

        ca_crt_builder = (
            x509.CertificateBuilder()
            .subject_name(subject)
            .issuer_name(issuer).public_key(public_key)
            .serial_number(x509.random_serial_number())
            .not_valid_before(datetime.datetime.utcnow())
            .not_valid_after(datetime.datetime.utcnow() + datetime.timedelta(days=CERT_EXPIRY_DAYS))
            .add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)
            .add_extension(x509.SubjectKeyIdentifier.from_public_key(public_key), critical=False)
        )

        self.certificate = ca_crt_builder.sign(private_key, self.digest)

        self.ca_pem = os.path.join(self.file_path, 'ca.pem')
        self.ca_key = os.path.join(self.file_path, 'ca.key')
        with open(self.ca_pem, 'wb') as f:
            f.write(self.certificate.public_bytes(Encoding.PEM))
        with open(self.ca_key, 'wb') as f:
            f.write(private_key.private_bytes(
                encoding=Encoding.PEM,
                format=PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=NoEncryption()
            ))

    def generate_user_certificate(self, role: str) -> None:

        if role == 'server':
            key_usage = [ExtendedKeyUsageOID.SERVER_AUTH]
        elif role == 'client':
            key_usage = [ExtendedKeyUsageOID.CLIENT_AUTH]

        with open(self.ca_key, 'rb') as f:
            ca_key = serialization.load_pem_private_key(f.read(), password=None)
        with open(self.ca_pem, 'rb') as f:
            ca_cert = x509.load_pem_x509_certificate(f.read())

        if self.cert_type == 'p384':
            user_private_key = ec.generate_private_key(
                ec.SECP384R1(),
                default_backend()
            )
        else:
            user_private_key = rsa.generate_private_key(
                public_exponent=65537,
                key_size=int(self.cert_type)
            )

        user_key = os.path.join(self.file_path, f'{role}.key')
        with open(user_key, 'wb') as f:
            f.write(user_private_key.private_bytes(
                encoding=Encoding.PEM,
                format=PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=NoEncryption()
            ))

        subject = x509.Name([
            x509.NameAttribute(x509.NameOID.COMMON_NAME, role)
        ])

        user_csr = x509.CertificateSigningRequestBuilder().subject_name(
            subject
        ).sign(user_private_key, self.digest)

        user_crt_builder = (
            x509.CertificateBuilder()
            .subject_name(user_csr.subject)
            .issuer_name(ca_cert.subject)
            .public_key(user_csr.public_key())
            .serial_number(x509.random_serial_number())
            .not_valid_before(datetime.datetime.utcnow())
            .not_valid_after(datetime.datetime.utcnow() + datetime.timedelta(days=CERT_EXPIRY_DAYS))
            .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=False)
            .add_extension(x509.ExtendedKeyUsage(key_usage), critical=False)
            .add_extension(x509.SubjectKeyIdentifier.from_public_key(user_csr.public_key()), critical=False)
        )
        user_crt = user_crt_builder.sign(ca_key, self.digest)

        user_pem = os.path.join(self.file_path, f'{role}.pem')
        user_cert = os.path.join(self.file_path, f'{role}.crt')
        with open(user_pem, 'wb') as f:
            f.write(user_crt.public_bytes(Encoding.PEM))
        with open(user_cert, 'wb') as f:
            f.write(user_crt.public_bytes(Encoding.PEM))


def main() -> None:

    parser = argparse.ArgumentParser()
    parser.add_argument('cert_type', choices=['2048', '3072', '4096', 'p384'], help='select cert type')
    parser.add_argument('message_digest', choices=['sha256', 'sha384'], help='select message digest, suiteB should be sha384')
    file_path = os.path.dirname(__file__)
    parser.add_argument('--output', '-o', default=file_path, help='path to store certificates')
    args = parser.parse_args()
    gencerts = GenCerts(args.cert_type, args.message_digest, args.output)
    gencerts.generate_ca_certificate()
    gencerts.generate_user_certificate('server')
    gencerts.generate_user_certificate('client')


if __name__ == '__main__':
    main()
