# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import contextlib
import io
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_soc_headers  # noqa: E402
import soc_header_reg  # noqa: E402
import soc_header_struct  # noqa: E402

REPO_ROOT = Path(__file__).resolve().parents[4]
HAS_GCC = shutil.which('gcc') is not None
# Keep the exact flags the production probe uses, minus the repo include roots.
# One -I is needed so the probe's "soc/reg_base.h" context resolves.
PROBE_COMMAND = [
    'gcc',
    '--std=c99',
    '-fsyntax-only',
    '-Werror=implicit-function-declaration',
    f'-I{REPO_ROOT / "components/soc/esp32c6/register"}',
    '-xc',
    '-',
]


def _write(tmp: str, name: str, source: str) -> Path:
    path = Path(tmp) / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(source, encoding='utf-8')
    return path


class TestLogicalLines(unittest.TestCase):
    def test_strips_block_comments_preserving_line_numbers(self) -> None:
        source = 'line one\n/** multi\n * line\n */\n#define FOO 1\n'
        lines = soc_header_struct.logical_lines(source)
        self.assertEqual([line.number for line in lines], [1, 5])
        self.assertEqual(lines[1].text.strip(), '#define FOO 1')

    def test_joins_continuations(self) -> None:
        source = '#define FOO \\\n    (BASE + 1)\n'
        lines = soc_header_reg.logical_lines(source)
        self.assertEqual(len(lines), 1)
        self.assertIn('BASE + 1', lines[0])


class TestAddressMacros(unittest.TestCase):
    def test_object_and_function_macros(self) -> None:
        source = (
            '#define GOOD_REG (BASE + 0)\n'
            '#define IDX_REG(i) (BASE(i) + 1)\n'
            '#define TWO_REG(i, j) (BASE(i) + (j))\n'
            '#define BBPD_CTRL (DR_REG_BB_BASE + 0x0)\n'
            '#define EMPTY_REG\n'
            '#define NOT_AN_ADDR 1\n'
            '#define FN(x) ((x) + 1)\n'
            '/* #define IN_COMMENT_REG 1 */\n'
        )
        self.assertEqual(
            soc_header_reg.address_macros(source),
            ['GOOD_REG', 'IDX_REG(0)', 'TWO_REG(0, 0)', 'BBPD_CTRL'],
        )

    def test_multiline_define(self) -> None:
        source = '#define WIDE_REG \\\n    (BASE + 0x4)\n'
        self.assertEqual(soc_header_reg.address_macros(source), ['WIDE_REG'])


class TestUndeclaredParsing(unittest.TestCase):
    def test_extracts_identifiers_and_skips_single_letters(self) -> None:
        err = (
            "p.c:5: error: 'DR_REG_AHB_BASE' undeclared (first use in this function)\n"
            'p.c:6: error: \u2018SOME_FIELD\u2019 undeclared here\n'
            "p.c:7: error: 'x' undeclared (first use in this function)\n"
        )
        self.assertEqual(soc_header_reg._undefined_macros(err), ['DR_REG_AHB_BASE', 'SOME_FIELD'])

    def test_implicit_declaration_of_base_macro(self) -> None:
        err = "p.c:5: error: implicit declaration of function 'REG_SPI_MEM_BASE'; did you mean 'DR_REG_SPIMEM1_BASE'?\n"
        self.assertEqual(soc_header_reg._undefined_macros(err), ['REG_SPI_MEM_BASE'])

    def test_unrelated_errors_are_ignored(self) -> None:
        self.assertEqual(soc_header_reg._undefined_macros('p.c:1: error: syntax error\n'), [])
        self.assertEqual(
            soc_header_reg._undefined_macros("p.c:1: error: implicit declaration of function 'rv_utils_get_core_id'\n"),
            [],
        )


class TestStructChecker(unittest.TestCase):
    def _check(self, source: str) -> list[tuple[str, int | None, str]]:
        result: list[tuple[str, int | None, str]] = soc_header_struct.check_struct_content(source)
        return result

    def test_valid_register_union_passes(self) -> None:
        source = """typedef union {
    struct {
        uint32_t a:1;
        uint32_t reserved:31;
    };
    uint32_t val;
} foo_reg_t;
"""
        self.assertEqual(self._check(source), [])

    def test_bitfield_sum_not_32_is_error(self) -> None:
        source = """typedef union {
    struct {
        uint32_t a:1;
    };
    uint32_t val;
} foo_reg_t;
"""
        self.assertTrue(any('bit-field sum is 1' in msg for _, _, msg in self._check(source)))

    def test_single_line_struct_bitfield_sum_is_error(self) -> None:
        source = """typedef union {
    struct { uint32_t a:1; };
    uint32_t val;
} foo_reg_t;
"""
        self.assertTrue(any('bit-field sum is 1' in msg for _, _, msg in self._check(source)))

    def test_overlay_union_is_ignored(self) -> None:
        source = """typedef union {
    struct {
        uint32_t a:1;
        uint32_t reserved:31;
    };
    struct {
        uint32_t b:1;
        uint32_t reserved:31;
    };
    uint32_t val;
} foo_reg_t;
"""
        self.assertEqual(self._check(source), [])

    def test_device_struct_without_bitfields_passes(self) -> None:
        source = """typedef struct {
    volatile foo_reg_t *pad;
    uint32_t fifo[16][0x400];
} dev_t;
"""
        self.assertEqual(self._check(source), [])

    def test_state_does_not_leak_between_files(self) -> None:
        first = """typedef union {
    struct {
        uint32_t a:32;
    };
    uint32_t val;
} first_reg_t;
"""
        second = """typedef struct {
    uint32_t x;
} second_dev_t;
"""
        self.assertEqual(self._check(first), [])
        self.assertEqual(self._check(second), [])


@unittest.skipUnless(HAS_GCC, 'host gcc required')
class TestRegChecker(unittest.TestCase):
    """End-to-end: no mocked compiler, these really invoke gcc."""

    def _check(self, source: str) -> list[tuple[str, int | None, str]]:
        with tempfile.TemporaryDirectory() as tmp:
            header = _write(tmp, 'probe_reg.h', source)
            result: list[tuple[str, int | None, str]] = soc_header_reg.check_file(header, PROBE_COMMAND)
            return result

    def test_bad_header_reports_undefined_identifier(self) -> None:
        findings = self._check('#define BAD_REG (DR_REG_MISSING + 0)\n')
        self.assertEqual(len(findings), 1)
        self.assertEqual(findings[0][0], 'ERROR')
        self.assertIn('DR_REG_MISSING', findings[0][2])

    def test_missing_function_like_base_is_reported(self) -> None:
        findings = self._check('#define IDX_REG(i) (MISSING_BASE(i) + 0)\n')
        self.assertTrue(any('MISSING_BASE' in msg for _, _, msg in findings))

    def test_legacy_address_macro_is_checked(self) -> None:
        findings = self._check('#define BBPD_CTRL (DR_REG_MISSING + 0x0)\n')
        self.assertTrue(any('DR_REG_MISSING' in msg for _, _, msg in findings))

    def test_good_header_passes(self) -> None:
        self.assertEqual(self._check('#define GOOD_REG (0x60000000 + 0)\n'), [])

    def test_uncompilable_header_is_reported_as_skip(self) -> None:
        source = '#include "no_such_soc_header.h"\n#define BAD_REG (DR_REG_MISSING + 0)\n'
        findings = self._check(source)
        self.assertEqual(len(findings), 1)
        self.assertEqual(findings[0][0], 'SKIP')
        self.assertIn('NOT checked', findings[0][2])


class TestReport(unittest.TestCase):
    def test_skipped_files_are_not_counted_as_passed(self) -> None:
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(out):
            rc = check_soc_headers.report([(Path('x_reg.h'), [('SKIP', None, 'not checked')])], kind='reg')
        self.assertEqual(rc, 0)
        self.assertIn('0/1', out.getvalue())
        self.assertIn('1 skipped', out.getvalue())

    def test_error_fails_the_run(self) -> None:
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(out):
            rc = check_soc_headers.report([(Path('x_reg.h'), [('ERROR', None, 'undefined')])], kind='reg')
        self.assertEqual(rc, 1)
        self.assertIn('1 failed', out.getvalue())

    def test_skips_at_limit_still_pass(self) -> None:
        results = [
            (Path(f'x{i}_reg.h'), [('SKIP', None, 'not checked')]) for i in range(check_soc_headers.MAX_SKIPPED_FILES)
        ]
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(out):
            rc = check_soc_headers.report(results, kind='reg')
        self.assertEqual(rc, 0)
        self.assertIn(f'{check_soc_headers.MAX_SKIPPED_FILES} skipped', out.getvalue())

    def test_skips_over_limit_fail(self) -> None:
        results = [
            (Path(f'x{i}_reg.h'), [('SKIP', None, 'not checked')])
            for i in range(check_soc_headers.MAX_SKIPPED_FILES + 1)
        ]
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(out):
            rc = check_soc_headers.report(results, kind='reg')
        self.assertEqual(rc, 1)
        self.assertIn('exceeding', out.getvalue())
        self.assertIn(f'{check_soc_headers.MAX_SKIPPED_FILES + 1} skipped', out.getvalue())


class TestCLI(unittest.TestCase):
    @unittest.skipUnless(HAS_GCC, 'host gcc required')
    def test_explicit_files_run_with_host_gcc(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            header = _write(tmp, 'probe_reg.h', '#define PROBE_REG (1)\n')
            rc = check_soc_headers.main(['--root', str(REPO_ROOT), 'reg', str(header)])
        self.assertEqual(rc, 0)

    def test_discover_finds_headers(self) -> None:
        structs = check_soc_headers.discover(REPO_ROOT, check_soc_headers.STRUCT_GLOBS)
        regs = check_soc_headers.discover(REPO_ROOT, check_soc_headers.REG_GLOBS)
        self.assertTrue(structs)
        self.assertTrue(regs)
        self.assertTrue(all(p.name.endswith('_struct.h') for p in structs))
        self.assertTrue(all(p.name.endswith('_reg.h') for p in regs))
        self.assertTrue(all('components/soc/' in p.as_posix() for p in structs + regs))

    def test_probe_command_enables_implicit_declaration_error(self) -> None:
        header = REPO_ROOT / 'components/soc/esp32c6/register/soc/uhci_reg.h'
        command = soc_header_reg.probe_command(REPO_ROOT, header)
        self.assertIn('-Werror=implicit-function-declaration', command)
        self.assertNotIn('-w', command)
        self.assertTrue(any(flag.startswith('-I') for flag in command))

    def test_probe_command_prioritises_variant_root(self) -> None:
        header = REPO_ROOT / 'components/soc/esp32p4/register/hw_ver3/soc/gpio_ext_reg.h'
        command = soc_header_reg.probe_command(REPO_ROOT, header)
        self.assertTrue(command[4].endswith('register/hw_ver3'))

    def test_modified_files_only_target_headers_stays_incremental(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            reg = _write(tmp, 'components/soc/esp32c6/register/soc/uhci_reg.h', '#define UHCI_REG 1\n')
            struct = root / 'components/soc/esp32c6/register/soc/uhci_struct.h'
            struct.write_text('typedef struct { uint32_t x; } uhci_dev_t;\n', encoding='utf-8')
            raw = f'{reg.relative_to(root)};{struct.relative_to(root)}'
            with mock.patch('check_soc_headers._run', return_value=0) as run:
                rc = check_soc_headers.main(['reg', '--modified-files', raw, '--root', str(root)])
        self.assertEqual(rc, 0)
        self.assertEqual(run.call_args[0][0], [reg.resolve()])

    def test_modified_files_with_dependency_falls_back_to_full_scan(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            reg = _write(tmp, 'components/soc/esp32c6/register/soc/uhci_reg.h', '#define UHCI_REG 1\n')
            base = root / 'components/soc/esp32c6/register/soc/reg_base.h'
            base.write_text('#define DR_REG_UHCI_BASE 1\n', encoding='utf-8')
            raw = str(base.relative_to(root))
            with mock.patch('check_soc_headers._run', return_value=0) as run:
                rc = check_soc_headers.main(['reg', '--modified-files', raw, '--root', str(root)])
        self.assertEqual(rc, 0)
        self.assertEqual(run.call_args[0][0], [reg.resolve()])

    def test_modified_files_empty_falls_back_to_full_scan(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            reg = _write(tmp, 'components/soc/esp32c6/register/soc/uhci_reg.h', '#define UHCI_REG 1\n')
            with mock.patch('check_soc_headers._run', return_value=0) as run:
                rc = check_soc_headers.main(['reg', '--modified-files', '', '--root', str(root)])
        self.assertEqual(rc, 0)
        self.assertEqual(run.call_args[0][0], [reg.resolve()])


if __name__ == '__main__':
    unittest.main()
