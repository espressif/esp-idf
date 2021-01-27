#!/usr/bin/env python

import os
import sys
import unittest

try:
    import gen_crt_bundle
except ImportError:
    sys.path.append('..')
    import gen_crt_bundle


idf_path            = os.environ['IDF_PATH']
ca_crts_path        = idf_path + '/components/mbedtls/esp_crt_bundle/'
test_crts_path      = idf_path + '/components/mbedtls/esp_crt_bundle/test_gen_crt_bundle/'

ca_bundle_bin_file  = 'x509_crt_bundle'

der_test_file       = 'baltimore.der'
pem_test_file       = 'entrust.pem'
verified_der_bundle = 'baltimore_crt_bundle'
verified_pem_bundle = 'entrust_crt_bundle'
invalid_test_file   = 'invalid_crt.pem'
non_ascii_file      = 'non_ascii_crt.pem'
ca_crts_all_file    = 'cacrt_all.pem'


class Py23TestCase(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(Py23TestCase, self).__init__(*args, **kwargs)
        try:
            self.assertRaisesRegex
        except AttributeError:
            # assertRaisesRegexp is deprecated in Python3 but assertRaisesRegex doesn't exist in Python2
            # This fix is used in order to avoid using the alias from the six library
            self.assertRaisesRegex = self.assertRaisesRegexp


class GenCrtBundleTests(Py23TestCase):

    # Verify generation from der vs known certificate
    def test_gen_from_der(self):
        bundle = gen_crt_bundle.CertificateBundle()
        bundle.add_from_file(test_crts_path + der_test_file)

        crt_bundle = bundle.create_bundle()

        with open(test_crts_path + verified_der_bundle, 'rb') as f:
            verified_bundle = f.read()

        self.assertEqual(crt_bundle, verified_bundle)

    # Verify generation from pem vs known certificate
    def test_gen_from_pem(self):
        bundle = gen_crt_bundle.CertificateBundle()
        bundle.add_from_file(test_crts_path + pem_test_file)

        crt_bundle = bundle.create_bundle()

        with open(test_crts_path + verified_pem_bundle, 'rb') as f:
            verified_bundle = f.read()

        self.assertEqual(crt_bundle, verified_bundle)

    def test_invalid_crt_input(self):
        bundle = gen_crt_bundle.CertificateBundle()

        with self.assertRaisesRegex(gen_crt_bundle.InputError, 'Invalid certificate'):
            bundle.add_from_file(test_crts_path  + invalid_test_file)

        with self.assertRaisesRegex(gen_crt_bundle.InputError, 'No certificate found'):
            bundle.add_from_pem('')

    def test_non_ascii_crt_input(self):
        bundle = gen_crt_bundle.CertificateBundle()

        bundle.add_from_file(test_crts_path  + non_ascii_file)
        self.assertTrue(len(bundle.certificates))


if __name__ == '__main__':
    unittest.main()
