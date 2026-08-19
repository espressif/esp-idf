#!/usr/bin/env python
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

try:
    import gen_crt_bundle
except ImportError:
    sys.path.append('..')
    import gen_crt_bundle


idf_path = os.environ['IDF_PATH']
ca_crts_path = idf_path + '/components/mbedtls/esp_crt_bundle/'
test_crts_path = idf_path + '/components/mbedtls/esp_crt_bundle/test_gen_crt_bundle/'

ca_bundle_bin_file = 'x509_crt_bundle'

der_test_file = 'baltimore.der'
pem_test_file = 'entrust.pem'
verified_der_bundle = 'baltimore_crt_bundle'
verified_pem_bundle = 'entrust_crt_bundle'
invalid_test_file = 'invalid_crt.pem'
non_ascii_file = 'non_ascii_crt.pem'
unsupported_file = 'unsupported_ext_crt.crt'
ca_crts_all_file = 'cacrt_all.pem'
cmn_filter_file = 'cmn_crt_authorities.csv'


class Py23TestCase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
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
            bundle.add_from_file(test_crts_path + invalid_test_file)

        bundle_prev_len = len(bundle.certificates)
        bundle.add_from_pem('')
        self.assertEqual(len(bundle.certificates), bundle_prev_len)

    # A file given directly is expected to be a certificate, so an unknown extension is an error
    def test_unsupported_extension_input(self):
        bundle = gen_crt_bundle.CertificateBundle()

        with self.assertRaisesRegex(gen_crt_bundle.InputError, 'Unsupported file extension'):
            bundle.add_from_file(test_crts_path + unsupported_file)

        self.assertEqual(len(bundle.certificates), 0)

    # While scanning a directory, files with an unknown extension are skipped instead
    def test_unsupported_extension_in_dir(self):
        bundle = gen_crt_bundle.CertificateBundle()

        self.assertFalse(bundle.add_from_file(test_crts_path + unsupported_file, strict=False))
        self.assertEqual(len(bundle.certificates), 0)

        # A directory holding both valid and unsupported files adds only the valid ones
        with tempfile.TemporaryDirectory() as crts_dir:
            shutil.copy(test_crts_path + unsupported_file, crts_dir)
            shutil.copy(test_crts_path + pem_test_file, crts_dir)

            bundle.add_from_path(crts_dir)
            self.assertEqual(len(bundle.certificates), 1)

        # A directory without any supported file is an error
        with tempfile.TemporaryDirectory() as crts_dir:
            shutil.copy(test_crts_path + unsupported_file, crts_dir)

            with self.assertRaisesRegex(gen_crt_bundle.InputError, 'No valid x509 certificates found'):
                bundle.add_from_path(crts_dir)

    def test_non_ascii_crt_input(self):
        bundle = gen_crt_bundle.CertificateBundle()

        bundle.add_from_file(test_crts_path + non_ascii_file)
        self.assertTrue(len(bundle.certificates))


class GenCrtBundleCliTests(unittest.TestCase):
    """Exercise the command line interface, including the flag shapes used by the build system"""

    def run_tool(self, args, cwd):
        return subprocess.run(
            [sys.executable, gen_crt_bundle.__file__] + args,
            cwd=cwd,
            capture_output=True,
            text=True,
        )

    def test_help(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(['-h'], workdir)

        self.assertEqual(result.returncode, 0)
        self.assertIn('certificate bundle utility', result.stdout)

    # The build system passes several paths to a single --input, then further options
    def test_multiple_input_paths(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(
                [
                    '--input',
                    test_crts_path + der_test_file,
                    test_crts_path + pem_test_file,
                    '-q',
                    '--max-certs',
                    '200',
                ],
                workdir,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            # --quiet keeps the status messages out of the build log
            self.assertEqual(result.stdout, '')
            self.assertTrue(os.path.isfile(os.path.join(workdir, ca_bundle_bin_file)))

    def test_status_messages_on_stdout(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(['--input', test_crts_path + pem_test_file], workdir)

        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Parsing certificates from', result.stdout)
        # Long paths are not wrapped, so they stay usable in the build log
        self.assertIn(test_crts_path + pem_test_file, result.stdout)

    def test_filter(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(
                [
                    '--input',
                    ca_crts_path + ca_crts_all_file,
                    '--filter',
                    ca_crts_path + cmn_filter_file,
                    '-q',
                ],
                workdir,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(os.path.getsize(os.path.join(workdir, ca_bundle_bin_file)) > 0)

    def test_missing_input(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool([], workdir)

        self.assertEqual(result.returncode, 2)
        self.assertIn('--input', result.stderr)

    # Input errors are reported as a single error line, not as a traceback
    def test_unsupported_extension(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(['--input', test_crts_path + unsupported_file], workdir)

        self.assertEqual(result.returncode, 2)
        self.assertIn('Unsupported file extension', result.stderr)
        self.assertNotIn('Traceback', result.stderr)

    def test_max_certs_exceeded(self):
        with tempfile.TemporaryDirectory() as workdir:
            result = self.run_tool(
                ['--input', test_crts_path + pem_test_file, '--max-certs', '0'],
                workdir,
            )

        self.assertEqual(result.returncode, 2)
        self.assertIn('exceeding the maximum', result.stderr)
        self.assertNotIn('Traceback', result.stderr)


if __name__ == '__main__':
    unittest.main()
