import binascii
import string
import datetime
import argparse
from pyasn1_modules import pem, rfc2459, rfc3279, rfc2314
from pyasn1.codec.der import decoder, encoder
from pyasn1.type import univ

import os
import sys
from sys import exit
import time
import serial
import threading
import subprocess
import filecmp

from scripts import secure_cert_sign
from scripts import secure_cert_flash

esptool_command = 'python ' + os.getenv('IDF_PATH') + '/components/esptool_py/esptool/esptool.py --port ' + os.getenv('ESPPORT') + ' '

class cmd_interpreter:
    """
    This class is for is the command line interaction with the Init1 firmware for manufacturing.
    It executes the specified commands and returns its result.
    It is a stateless, does not maintain the current state of the firmware. 
    """
    def __init__(self):
        # Create definitions for the certificates
        # Serial Port settings
        self.port = serial.Serial()
        self.port.timeout = 2
        self.port.rts = False
        self.port.dtr = False
        self.port.baudrate = 115200
        #To-DO - FIx the following sequence to automatically put the ESP32 in required mode.
        #self.port.setDTR(False)  # IO0=HIGH
        #self.port.setRTS(True)   # EN=LOW, chip in reset
        #time.sleep(1.3) # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.1
        #self.port.setDTR(True)   # IO0=LOW
        #self.port.reset_input_buffer()
        #self.port.reset_output_buffer()
        self.port.port = os.getenv('ESPPORT')
        self.port.open()

    def wait_for_init(self):
        print("Wait for init")
        while True:
            line = self.port.readline()
            if 'Initialising Command line: >>'.encode() in line:
                print("- CLI Initialised")
                return True
            #elif not line:
             #   return False
            print(line.decode())

    def exec_cmd(self, command, args = None):
        ret = ""
        status = None
        self.port.write(command.encode() + '\r'.encode())

        if args:
            time.sleep(0.1)
            self.port.write(args)
            self.port.write('\0'.encode())

        while True:
            line = self.port.readline()
            sys.stdout.flush()
            sys.stdout.write(line.decode())
            if 'Status: Success'.encode() in line:
                status = True
            elif 'Status: Failure'.encode() in line:
                status = False
            if status == True or status == False:
                while True:
                    line = self.port.readline()
                    if ">>".encode() in line:
                        if status == True:
                            print(line.decode())
                        break
                    else:
                        ret += line.decode()
                return [{"Status": status}, {"Return": ret}]

def _exec_shell_cmd(command):
    result = subprocess.Popen((command).split(), stdout=subprocess.PIPE)
    out, err = result.communicate()
    return out

def get_mac():
    global esptool_command
    operation = 'read_mac'
    out = _exec_shell_cmd(esptool_command+operation)
    for line in out.split('\n'.encode()):
        if 'MAC:' in line:
            return line.split(' '.encode())[1]
    return False

def main():
    parser = argparse.ArgumentParser( description="Pre-provision the devices through signing its CSR and programming into the device.& Generate atcacert_def_t structure from sample certificate.")
        
    # parser.add_argument("-flash",
    #                     help='To flash the binaries onto the module',
    #                     dest='bin_path',
    #                     metavar='[relative/path/to/bins]')
    
    parser.add_argument(
        '--signer-cert-private-key',
        dest='signer_privkey',
        nargs = '?',
        default='sample_certs/sample_signer_key.pem',
        metavar='path/to/signer-priv-key',# required=True,
        help='The path of the private key of ESP CA( used to sign the device cert) for the specific customer')

    parser.add_argument(
        "-pwd", '--password', 
        dest='password',
        nargs = '?',
        metavar='[password]',
        help='The password associated with the private key')
    parser.add_argument(
        '--signer-cert',
        dest='signer_cert',
        nargs='?',
        default='sample_certs/sample_signer_cert.pem',
        metavar='path/to/signer_cert.pem', #required=True,
        help='Generate signer certificate definition from sample certificate.')

    parser.add_argument(
        '--sample-device-cert',
        dest='device_cert',
        nargs='?',
        default='sample_certs/sample_device_cert.pem',
        metavar='path/to/device-cert',# required=True,
        help='Generate device certificate definition from sample certificate.')
    
    parser.add_argument(
        '--sample-device-csr',
        dest='device_csr',
        nargs='?',
        default='sample_certs/sample_device_cert_csr.csr',
        metavar='path/to/device-csr', # required=True,
        help='Generate device CSR definition from sample CSR.')
    args = parser.parse_args() 

    if args.signer_cert is "sample_certs/sample_signer_cert.pem":
        print("No cert provided, hence using certs provided as sample")
    cert_der = pem.readPemFromFile(open(args.signer_cert))
    print((gen_cert_def_c_signer(cert_der)), file=open("components/cryptoauthlib/port/cert_1_def_signer.c", "w+"))

    if args.device_cert is "sample_certs/sample_device_cert.pem":
        print("No cert provided, hence using certs provided as sample")
    cert_der = pem.readPemFromFile(open(args.device_cert))
    print((gen_cert_def_c_device(cert_der)), file=open("components/cryptoauthlib/port/cert_1_def_device.c", "w+"))

    if args.device_csr is "sample_certs/sample_device_cert_csr.csr":
        print("No cert provided, hence using certs provided as sample")
    csr_der = pem.readPemFromFile(open(args.device_csr),
        startMarker='-----BEGIN CERTIFICATE REQUEST-----',
        endMarker='-----END CERTIFICATE REQUEST-----')
    print((gen_cert_def_c_device_csr(csr_der)), file=open("components/cryptoauthlib/port/cert_3_def_device_csr.c", "w+"))

    #mac = get_mac()
    #if mac == False:
     #   exit(0)
    #print "MAC:" + mac.replace(":","").upper()
    mac = '12345'
    
    init_mfg = cmd_interpreter()
    err = "failed to execute"

    retval = init_mfg.wait_for_init()
    if retval != True:
        print ("CMD prompt timed out.")
        exit(0)

    retval = init_mfg.exec_cmd("generate-keys RSA 2048")
    # for now the command is RSA 2048 , which will use ATECC if ESPWROOM32SE flag is set
    # TO DO - add the appopriate attributes to the command
    if retval[0]['Status'] != True:
        print(("keygen " + err))
        print((retval[1]['Return']))
        exit(0)

    mac = mac.rstrip()
    # retval = init_mfg.exec_cmd("generate-csr \"C=IN,ST=MH,O=Espressif\"") # Without MAC address in the CommonName
    retval = init_mfg.exec_cmd("generate-csr \"CN="+mac.replace(":","").upper()+",C=IN,ST=MH,O=Espressif\"")
    if retval[0]['Status'] != True:
        print(("csrgen " + err))
        print(retval[1]['Return'])
        exit(0)
    print(retval[1]['Return'])

    # Returning after successfully generating the CSR for now

    return
    #To-Do run the following code
    try:
        private_key = secure_cert_sign.load_privatekey(args.path_privkey, args.password)
        ca_cert = secure_cert_sign.load_ca_certificate(args.path_cacert)

    # Sign the CSR using the generated keys
        device_cert = secure_cert_sign.sign_csr(retval[1]['Return'], ca_cert, private_key)
    except ValueError as error:
        print("Unsupported Key,Cert or CSR format specified.")
        exit(0)

    retval = init_mfg.exec_cmd("program-dev-cert", device_cert)
    if retval[0]['Status'] != True:
        print("program-certs " + err)
        print(retval[1]['Return'])
        exit(0)

    ca_cert_file = open(args.path_cacert, 'r')
    ca_cert_data = ca_cert_file.read()
    ca_cert_file.close()

    retval = init_mfg.exec_cmd("program-ca-cert", ca_cert_data)
    if retval[0]['Status'] != True:
        print("program-certs " + err)
        print(retval[1]['Return'])
        exit(0)

    retval = init_mfg.exec_cmd("finalise")
    if retval[0]['Status'] != True:
        print("finalise " + err)
        print(retval[1]['Return'])
        exit(0)

def gen_cert_def_c_signer(cert_der):
    cert = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]

    params = {}

    # CA public key needs to be handled dynamically in the kit, so we can use fake data for now
    params['ca_public_key'] = bin_to_c_hex(b'\x00'*64)

    info = cert_sn_offset_length(cert)
    params['cert_sn_cert_loc_offset'] = info['offset']
    params['cert_sn_cert_loc_count']  = info['length']
    if info['length'] < 1 or info['length'] > 20:
        raise ValueError('Invalid certificate SN length (no more than 20 bytes).')

    not_before = set_time_params(params, cert, 'notBefore')
    not_after  = set_time_params(params, cert, 'notAfter')
    
    expire_years = not_after.year - not_before.year
    if expire_years < 1 or expire_years > 31:
        expire_years = 0
    params['expire_years'] = expire_years

    info = cert_signer_id_offset_length(cert, 'subject')
    params['signer_id_cert_loc_offset'] = info['offset']
    params['signer_id_cert_loc_count']  = info['length']

    info = cert_public_key_offset_length(cert)
    params['public_key_cert_loc_offset'] = info['offset']
    params['public_key_cert_loc_count']  = info['length']

    info = cert_subj_key_id_offset_length(cert)
    if info is not None:
        params['subj_key_id_cert_loc_offset'] = info['offset']
        params['subj_key_id_cert_loc_count']  = info['length']
    else:
        params['subj_key_id_cert_loc_offset'] = 0
        params['subj_key_id_cert_loc_count']  = 0

    info = cert_auth_key_id_offset_length(cert)
    if info is not None:
        params['auth_key_id_cert_loc_offset'] = info['offset']
        params['auth_key_id_cert_loc_count']  = info['length']
    else:
        params['auth_key_id_cert_loc_offset'] = 0
        params['auth_key_id_cert_loc_count']  = 0

    info = cert_tbs_offset_length(cert)
    params['tbs_cert_loc_offset'] = info['offset']
    params['tbs_cert_loc_count']  = info['length']

    info = cert_sig_offset_length(cert)
    params['signature_cert_loc_offset'] = info['offset']
    params['signature_cert_loc_count']  = info['length']

    params['cert_template'] = bin_to_c_hex(cert_der)

    return string.Template(cert_def_1_signer_c).substitute(params)

def gen_cert_def_c_device(cert_der):
    cert = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    params = {}

    info = cert_sn_offset_length(cert)
    params['cert_sn_cert_loc_offset'] = info['offset']
    params['cert_sn_cert_loc_count']  = info['length']
    if info['length'] < 1 or info['length'] > 20:
        raise ValueError('Invalid certificate SN length (no more than 20 bytes).')

    info = cert_signer_id_offset_length(cert, 'issuer')
    params['signer_id_cert_loc_offset'] = info['offset']
    params['signer_id_cert_loc_count']  = info['length']

    not_before = set_time_params(params, cert, 'notBefore')
    not_after  = set_time_params(params, cert, 'notAfter')
    
    expire_years = not_after.year - not_before.year
    if expire_years < 1 or expire_years > 31:
        expire_years = 0
        # Don't bother re-setting notAfter
        params['expire_date_cert_loc_offset'] = 0
        params['expire_date_cert_loc_count']  = 0
    params['expire_years'] = expire_years

    info = cert_public_key_offset_length(cert)
    params['public_key_cert_loc_offset'] = info['offset']
    params['public_key_cert_loc_count']  = info['length']

    info = cert_subj_key_id_offset_length(cert)
    if info is not None:
        params['subj_key_id_cert_loc_offset'] = info['offset']
        params['subj_key_id_cert_loc_count']  = info['length']
    else:
        params['subj_key_id_cert_loc_offset'] = 0
        params['subj_key_id_cert_loc_count']  = 0

    info = cert_auth_key_id_offset_length(cert)
    if info is not None:
        params['auth_key_id_cert_loc_offset'] = info['offset']
        params['auth_key_id_cert_loc_count']  = info['length']
    else:
        params['auth_key_id_cert_loc_offset'] = 0
        params['auth_key_id_cert_loc_count']  = 0

    info = cert_tbs_offset_length(cert)
    params['tbs_cert_loc_offset'] = info['offset']
    params['tbs_cert_loc_count']  = info['length']

    info = cert_sig_offset_length(cert)
    params['signature_cert_loc_offset'] = info['offset']
    params['signature_cert_loc_count']  = info['length']

    params['cert_template'] = bin_to_c_hex(cert_der)

    return string.Template(cert_def_2_device_c).substitute(params)

def gen_cert_def_c_device_csr(csr_der):
    # Use the device certificate to create a CSR template
    csr = decoder.decode(csr_der, asn1Spec=rfc2314.CertificationRequest())[0]

    params = {}

    info = csr_public_key_offset_length(csr)
    params['public_key_cert_loc_offset'] = info['offset']
    params['public_key_cert_loc_count']  = info['length']

    info = cert_tbs_offset_length(csr) # cert TBS works for CSR too
    params['tbs_cert_loc_offset'] = info['offset']
    params['tbs_cert_loc_count']  = info['length']

    info = cert_sig_offset_length(csr) # cert sig works for CSR too
    params['signature_cert_loc_offset'] = info['offset']
    params['signature_cert_loc_count']  = info['length']

    params['cert_template'] = bin_to_c_hex(csr_der)

    return string.Template(cert_def_3_device_csr_c).substitute(params)

def set_time_params(params, cert, name):
    if name == 'notBefore':
        param_prefix = 'issue_date'
    elif name == 'notAfter':
        param_prefix = 'expire_date'

    info = cert_time_offset_length(cert, name)
    params[param_prefix + '_cert_loc_offset'] = info['offset']
    params[param_prefix + '_cert_loc_count']  = info['length']
    
    if info['length'] == 13:
        params[param_prefix + '_format'] = 'DATEFMT_RFC5280_UTC'
        time_str = str(cert['tbsCertificate']['validity'][name][0])
        dt = datetime.datetime.strptime(time_str, '%y%m%d%H%M%SZ')
    elif info['length'] == 15:
        params[param_prefix + '_format'] = 'DATEFMT_RFC5280_GEN'
        time_str = str(cert['tbsCertificate']['validity'][name][1])
        dt = datetime.datetime.strptime(time_str, '%Y%m%d%H%M%SZ')
    else:
        raise ValueError(name + ' date has invalid length')

    return dt

def bin_to_c_hex(data):
    c_hex = ""
    for i in range(0, len(data)):
        if i != 0:
            c_hex += ','
        if i % 16 == 0:
            if i != 0:
                c_hex += '\n'
            c_hex += '    '
        elif i % 8 == 0:
            c_hex += '  '
        else:
            c_hex += ' '
        c_hex += '0x%02x' % data[i]

    return c_hex

def cert_sn_offset_length(cert):
    sn_der = bytearray(encoder.encode(cert['tbsCertificate']['serialNumber']))
    int_info = der_value_offset_length(sn_der)
    msb_idx = int_info['offset']
    if sn_der[msb_idx] & 0x80:
        if sn_der[msb_idx] == 0x81:
            sn_der[msb_idx] = 0x82
        else:
            sn_der[msb_idx] = 0x81
    else:
        if sn_der[msb_idx] == 0x01:
            sn_der[msb_idx] = 0x02
        else:
            sn_der[msb_idx] = 0x01
    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    cert_mod['tbsCertificate']['serialNumber'] = decoder.decode(bytes(sn_der))[0]
    
    return {'offset':diff_offset(cert_der, encoder.encode(cert_mod)), 'length':int_info['length']}

def cert_signer_id_offset_length(cert, name):
    name_der = bytearray(encoder.encode(cert['tbsCertificate'][name]))
    name_der = name_der.replace(b'FFFF', b'0000')

    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    cert_mod['tbsCertificate'][name] = decoder.decode(bytes(name_der))[0]

    return {'offset':diff_offset(cert_der, encoder.encode(cert_mod)), 'length':4}

def cert_time_offset_length(cert, name):
    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    time_str = str(cert_mod['tbsCertificate']['validity'][name].getComponent())
    time_str = chr(ord(time_str[0]) + 1) + time_str[1:]
    cert_mod['tbsCertificate']['validity'][name] = cert_mod['tbsCertificate']['validity'][name].getComponent().clone(
        value=time_str)

    return {'offset': diff_offset(cert_der, encoder.encode(cert_mod)), 'length': len(time_str)}

def cert_public_key_offset_length(cert):
    pk_der = bytearray(encoder.encode(cert['tbsCertificate']['subjectPublicKeyInfo']['subjectPublicKey']))
    pk_info = der_value_offset_length(pk_der)
    # Skip the unused bits field and key compression byte
    pk_der[pk_info['offset']+2] ^= 0xFF

    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    cert_mod['tbsCertificate']['subjectPublicKeyInfo']['subjectPublicKey'] = decoder.decode(bytes(pk_der))[0]
    
    return {'offset':diff_offset(cert_der, encoder.encode(cert_mod)), 'length':pk_info['length']-2}

def csr_public_key_offset_length(csr):
    pk_der = bytearray(encoder.encode(csr['certificationRequestInfo']['subjectPublicKeyInfo']['subjectPublicKey']))
    pk_info = der_value_offset_length(pk_der)
    # Skip the unused bits field and key compression byte
    pk_der[pk_info['offset']+2] ^= 0xFF

    csr_der = encoder.encode(csr)
    csr_mod = decoder.decode(csr_der, asn1Spec=rfc2314.CertificationRequest())[0]
    csr_mod['certificationRequestInfo']['subjectPublicKeyInfo']['subjectPublicKey'] = decoder.decode(bytes(pk_der))[0]
    
    return {'offset':diff_offset(csr_der, encoder.encode(csr_mod)), 'length':pk_info['length']-2}

def cert_subj_key_id_offset_length(cert):
    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    for ext in cert_mod['tbsCertificate']['extensions']:
        if ext['extnID'] == rfc2459.id_ce_subjectKeyIdentifier:
            extn_value = decoder.decode(ext['extnValue'])[0]
            key_id = bytearray(decoder.decode(extn_value, asn1Spec=rfc2459.SubjectKeyIdentifier())[0])
            key_id[0] ^= 0xFF # Change first byte
            
            extn_value = rfc2459.SubjectKeyIdentifier(key_id)
            ext['extnValue'] = univ.OctetString(encoder.encode(extn_value))
            
            return {'offset':diff_offset(cert_der, encoder.encode(cert_mod)), 'length':len(key_id)}
    return None

def cert_auth_key_id_offset_length(cert):
    cert_der = encoder.encode(cert)
    cert_mod = decoder.decode(cert_der, asn1Spec=rfc2459.Certificate())[0]
    for ext in cert_mod['tbsCertificate']['extensions']:
        if ext['extnID'] == rfc2459.id_ce_authorityKeyIdentifier:
            extn_value = decoder.decode(ext['extnValue'])[0]
            auth_key_id = decoder.decode(extn_value, asn1Spec=rfc2459.AuthorityKeyIdentifier())[0]
            key_id = bytearray(auth_key_id['keyIdentifier'])
            key_id[0] ^= 0xFF # Change first byte
            
            auth_key_id['keyIdentifier'] = auth_key_id['keyIdentifier'].clone(value=key_id)
            ext['extnValue'] = univ.OctetString(encoder.encode(auth_key_id))
            
            return {'offset':diff_offset(cert_der, encoder.encode(cert_mod)), 'length':len(key_id)}
    return None

def cert_tbs_offset_length(cert):
    cert_der = encoder.encode(cert)
    cert_info = der_value_offset_length(cert_der)
    tbs_info = der_value_offset_length(cert_der[cert_info['offset']:])

    return {'offset':cert_info['offset'], 'length':(tbs_info['offset'] + tbs_info['length'])}

def cert_sig_offset_length(cert):
    cert_der = encoder.encode(cert)
    
    cert_info = der_value_offset_length(cert_der)
    offset = cert_info['offset']

    tbs_info = der_value_offset_length(cert_der[offset:])
    offset += tbs_info['offset'] + tbs_info['length']

    alg_info = der_value_offset_length(cert_der[offset:])
    offset += alg_info['offset'] + alg_info['length']

    sig_info = der_value_offset_length(cert_der[offset:])

    return {'offset':offset, 'length':(sig_info['offset'] + sig_info['length'])}

def der_value_offset_length(der):
    """Returns the offset and length of the value part of the DER tag-length-value object."""
    
    tag_len = 1 # Assume 1 byte tag

    if der[tag_len] < 0x80:
        # Length is short-form, only 1 byte
        len_len = 1
        len = int(der[tag_len])
    else:
        # Length is long-form, lower 7 bits indicates how many additional bytes are required
        len_len = (der[tag_len] & 0x7F) + 1
        len = int().from_bytes(der[tag_len+1:tag_len+len_len], byteorder='big', signed=False)
    return {'offset':tag_len+len_len, 'length':len}

def diff_offset(base, diff):
    """Return the index where the two parameters differ."""
    if len(base) != len(diff):
        raise ValueError('len(base)=%d != len(diff)=%d' % (len(base), len(diff)))
    for i in range(0, len(base)):
        if base[i] != diff[i]:
            return i
    return None

cert_def_1_signer_c = """
#include "atcacert/atcacert_def.h"

uint8_t g_signer_1_ca_public_key[64] = {
${ca_public_key}
};

const atcacert_cert_element_t g_cert_elements_1_signer[] = {
    {
        .id         = "IssueDate",
        .device_loc = {
            .zone      = DEVZONE_DATA,
            .slot      = 14,
            .is_genkey = 0,
            .offset    = 35-${issue_date_cert_loc_count},
            .count     = ${issue_date_cert_loc_count}
        },
        .cert_loc   = {
            .offset = ${issue_date_cert_loc_offset},
            .count  = ${issue_date_cert_loc_count}
        }
    },
    {
        .id         = "ExpireDate",
        .device_loc = {
            .zone      = DEVZONE_DATA,
            .slot      = 14,
            .is_genkey = 0,
            .offset    = 50-${expire_date_cert_loc_count},
            .count     = ${expire_date_cert_loc_count}
        },
        .cert_loc   = {
            .offset = ${expire_date_cert_loc_offset},
            .count  = ${expire_date_cert_loc_count}
        }
    }
};

const uint8_t g_cert_template_1_signer[] = {
${cert_template}
};

const atcacert_def_t g_cert_def_1_signer = {
    .type                   = CERTTYPE_X509,
    .template_id            = 1,
    .chain_id               = 0,
    .private_key_slot       = 0,
    .sn_source              = SNSRC_STORED,
    .cert_sn_dev_loc        = { 
        .zone      = DEVZONE_DATA,
        .slot      = 14,
        .is_genkey = 0,
        .offset    = 20-${cert_sn_cert_loc_count},
        .count     = ${cert_sn_cert_loc_count}
    },
    .issue_date_format      = ${issue_date_format},
    .expire_date_format     = ${expire_date_format},
    .tbs_cert_loc           = {
        .offset = ${tbs_cert_loc_offset},
        .count  = ${tbs_cert_loc_count}
    },
    .expire_years           = ${expire_years},
    .public_key_dev_loc     = {
        .zone      = DEVZONE_DATA,
        .slot      = 11,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 72
    },
    .comp_cert_dev_loc      = {
        .zone      = DEVZONE_DATA,
        .slot      = 12,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 72
    },
    .std_cert_elements      = {
        { // STDCERT_PUBLIC_KEY
            .offset = ${public_key_cert_loc_offset},
            .count  = ${public_key_cert_loc_count}
        },
        { // STDCERT_SIGNATURE
            .offset = ${signature_cert_loc_offset},
            .count  = ${signature_cert_loc_count}
        },
        { // STDCERT_ISSUE_DATE
            .offset = ${issue_date_cert_loc_offset},
            .count  = ${issue_date_cert_loc_count}
        },
        { // STDCERT_EXPIRE_DATE
            .offset = ${expire_date_cert_loc_offset},
            .count  = ${expire_date_cert_loc_count}
        },
        { // STDCERT_SIGNER_ID
            .offset = ${signer_id_cert_loc_offset},
            .count  = ${signer_id_cert_loc_count}
        },
        { // STDCERT_CERT_SN
            .offset = ${cert_sn_cert_loc_offset},
            .count  = ${cert_sn_cert_loc_count}
        },
        { // STDCERT_AUTH_KEY_ID
            .offset = ${auth_key_id_cert_loc_offset},
            .count  = ${auth_key_id_cert_loc_count}
        },
        { // STDCERT_SUBJ_KEY_ID
            .offset = ${subj_key_id_cert_loc_offset},
            .count  = ${subj_key_id_cert_loc_count}
        }
    },
    .cert_elements          = g_cert_elements_1_signer,
    .cert_elements_count    = sizeof(g_cert_elements_1_signer) / sizeof(g_cert_elements_1_signer[0]),
    .cert_template          = g_cert_template_1_signer,
    .cert_template_size     = sizeof(g_cert_template_1_signer)
};
"""

cert_def_2_device_c = """
#include "atcacert/atcacert_def.h"

const uint8_t g_cert_template_2_device[] = {
${cert_template}
};

const atcacert_def_t g_cert_def_2_device = {
    .type                   = CERTTYPE_X509,
    .template_id            = 2,
    .chain_id               = 0,
    .private_key_slot       = 0,
    .sn_source              = SNSRC_PUB_KEY_HASH,
    .cert_sn_dev_loc        = { 
        .zone      = DEVZONE_NONE,
        .slot      = 0,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 0
    },
    .issue_date_format      = ${issue_date_format},
    .expire_date_format     = ${expire_date_format},
    .tbs_cert_loc           = {
        .offset = ${tbs_cert_loc_offset},
        .count  = ${tbs_cert_loc_count}
    },
    .expire_years           = ${expire_years},
    .public_key_dev_loc     = {
        .zone      = DEVZONE_DATA,
        .slot      = 0,
        .is_genkey = 1,
        .offset    = 0,
        .count     = 64
    },
    .comp_cert_dev_loc      = {
        .zone      = DEVZONE_DATA,
        .slot      = 10,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 72
    },
    .std_cert_elements      = {
        { // STDCERT_PUBLIC_KEY
            .offset = ${public_key_cert_loc_offset},
            .count  = ${public_key_cert_loc_count}
        },
        { // STDCERT_SIGNATURE
            .offset = ${signature_cert_loc_offset},
            .count  = ${signature_cert_loc_count}
        },
        { // STDCERT_ISSUE_DATE
            .offset = ${issue_date_cert_loc_offset},
            .count  = ${issue_date_cert_loc_count}
        },
        { // STDCERT_EXPIRE_DATE
            .offset = ${expire_date_cert_loc_offset},
            .count  = ${expire_date_cert_loc_count}
        },
        { // STDCERT_SIGNER_ID
            .offset = ${signer_id_cert_loc_offset},
            .count  = ${signer_id_cert_loc_count}
        },
        { // STDCERT_CERT_SN
            .offset = ${cert_sn_cert_loc_offset},
            .count  = ${cert_sn_cert_loc_count}
        },
        { // STDCERT_AUTH_KEY_ID
            .offset = ${auth_key_id_cert_loc_offset},
            .count  = ${auth_key_id_cert_loc_count}
        },
        { // STDCERT_SUBJ_KEY_ID
            .offset = ${subj_key_id_cert_loc_offset},
            .count  = ${subj_key_id_cert_loc_count}
        }
    },
    .cert_elements          = NULL,
    .cert_elements_count    = 0,
    .cert_template          = g_cert_template_2_device,
    .cert_template_size     = sizeof(g_cert_template_2_device)
};
"""

cert_def_3_device_csr_c = """
#include "atcacert/atcacert_def.h"

const uint8_t g_csr_template_3_device[] = {
${cert_template}
};

const atcacert_def_t g_csr_def_3_device = {
    .type                   = CERTTYPE_X509,
    .template_id            = 3,
    .chain_id               = 0,
    .private_key_slot       = 0,
    .sn_source              = SNSRC_PUB_KEY_HASH,
    .cert_sn_dev_loc        = { 
        .zone      = DEVZONE_NONE,
        .slot      = 0,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 0
    },
    .issue_date_format      = DATEFMT_RFC5280_UTC,
    .expire_date_format     = DATEFMT_RFC5280_UTC,
    .tbs_cert_loc           = {
        .offset = ${tbs_cert_loc_offset},
        .count  = ${tbs_cert_loc_count}
    },
    .expire_years           = 0,
    .public_key_dev_loc     = {
        .zone      = DEVZONE_NONE,
        .slot      = 0,
        .is_genkey = 1,
        .offset    = 0,
        .count     = 64
    },
    .comp_cert_dev_loc      = {
        .zone      = DEVZONE_NONE,
        .slot      = 0,
        .is_genkey = 0,
        .offset    = 0,
        .count     = 0
    },
    .std_cert_elements      = {
        { // STDCERT_PUBLIC_KEY
            .offset = ${public_key_cert_loc_offset},
            .count  = ${public_key_cert_loc_count}
        },
        { // STDCERT_SIGNATURE
            .offset = ${signature_cert_loc_offset},
            .count  = ${signature_cert_loc_count}
        },
        { // STDCERT_ISSUE_DATE
            .offset = 0,
            .count  = 0
        },
        { // STDCERT_EXPIRE_DATE
            .offset = 0,
            .count  = 0
        },
        { // STDCERT_SIGNER_ID
            .offset = 0,
            .count  = 0
        },
        { // STDCERT_CERT_SN
            .offset = 0,
            .count  = 0
        },
        { // STDCERT_AUTH_KEY_ID
            .offset = 0,
            .count  = 0
        },
        { // STDCERT_SUBJ_KEY_ID
            .offset = 0,
            .count  = 0
        }
    },
    .cert_elements          = NULL,
    .cert_elements_count    = 0,
    .cert_template          = g_csr_template_3_device,
    .cert_template_size     = sizeof(g_csr_template_3_device)
};
"""

if __name__ == "__main__":
    main()
