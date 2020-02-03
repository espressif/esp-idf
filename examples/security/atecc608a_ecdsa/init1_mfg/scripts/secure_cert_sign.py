from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from datetime import datetime
from datetime import timedelta
import uuid

# Loads the RSA Private Key
def load_privatekey(key_file_path, password=None):
    key_file = open(key_file_path, "rb")
    key = key_file.read()
    key_file.close()
    return serialization.load_pem_private_key(key, password = password , backend = default_backend())

# Loads the ca certificate
def load_ca_certificate(cert_file_path):
    cert_file = open(cert_file_path, "rb")
    ca_cert = cert_file.read()
    cert_file.close()
    return x509.load_pem_x509_certificate(ca_cert, default_backend())

def sign_csr(cert_sign_req, ca_cert, ca_privkey):

    csr = x509.load_pem_x509_csr(cert_sign_req, default_backend())
    # if isinstance(csr.public_key(), rsa.RSAPublicKey) == True:
        # print "Python Utility: Subject:"
        # display_oids(csr.subject)

    device_cert = x509.CertificateBuilder().subject_name(
        csr.subject
    ).issuer_name(
        ca_cert.subject
    ).public_key(
        csr.public_key()
    ).serial_number(
        uuid.uuid4().int
    ).not_valid_before(
        datetime.utcnow()
    ).not_valid_after(
        datetime.utcnow() + timedelta(days=365 * 10)
    ).add_extension(
        extension=x509.KeyUsage(
            digital_signature=True, key_encipherment=True, content_commitment=True,
            data_encipherment=False, key_agreement=False, encipher_only=False, decipher_only=False, key_cert_sign=False, crl_sign=False
        ),
        critical=True
    ).add_extension(
        extension=x509.BasicConstraints(ca=False, path_length=None),
        critical=True
    ).add_extension(
        extension=x509.AuthorityKeyIdentifier.from_issuer_public_key(ca_privkey.public_key()),
        critical=False
    ).sign(
        private_key=ca_privkey,
        algorithm=hashes.SHA256(),
        backend=default_backend()
    )
    return device_cert.public_bytes(serialization.Encoding.PEM)

# Print the object Identifiers in the certificate(Issuer and Subject fields)
def display_oids(oid_list):
    if oid_list.get_attributes_for_oid(NameOID.COMMON_NAME) != []:
        print("COMMON_NAME:" + oid_list.get_attributes_for_oid(NameOID.COMMON_NAME)[0].value)
    if oid_list.get_attributes_for_oid(NameOID.STATE_OR_PROVINCE_NAME) != []:
        print("STATE_OR_PROVINCE_NAME:" + oid_list.get_attributes_for_oid(NameOID.STATE_OR_PROVINCE_NAME)[0].value)
    if oid_list.get_attributes_for_oid(NameOID.LOCALITY_NAME) != []:
        print("LOCALITY_NAME:" + oid_list.get_attributes_for_oid(NameOID.LOCALITY_NAME)[0].value)
    if oid_list.get_attributes_for_oid(NameOID.ORGANIZATION_NAME) != []:
        print("ORGANIZATION_NAME:" + oid_list.get_attributes_for_oid(NameOID.ORGANIZATION_NAME)[0].value)
    if oid_list.get_attributes_for_oid(NameOID.COUNTRY_NAME) != []:
        print("COUNTRY_NAME:" + oid_list.get_attributes_for_oid(NameOID.COUNTRY_NAME)[0].value)
