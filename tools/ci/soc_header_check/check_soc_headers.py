#!/usr/bin/env python
# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Check SOC register headers for layout issues and undefined address macros.

See README.md in this directory for local usage.
"""

import argparse
import os
import sys
from collections.abc import Callable
from collections.abc import Iterable
from collections.abc import Sequence
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import soc_header_reg
import soc_header_struct

STRUCT_GLOBS = ('components/soc/**/*_struct.h',)
REG_GLOBS = ('components/soc/**/*_reg.h',)
TARGET_SUFFIXES = ('_struct.h', '_reg.h')

# Known baseline of headers that cannot be host-compiled: esp32/dport_reg.h
# (needs sdkconfig.h) and the esp32p4 / esp32s31 interrupt_reg.h files
# (call rv_utils_get_core_id). Allow exactly these; any further SKIP means the
# probe silently stopped covering a file and must be investigated.
MAX_SKIPPED_FILES = 3

# Both checkers return (level, line, message). level is ERROR (fails the job)
# or SKIP (the probe could not run; the file is not counted as passed).
Finding = tuple[str, int | None, str]


def repo_root(path: Path | None = None) -> Path:
    if path is not None:
        return path.resolve()
    if os.environ.get('IDF_PATH'):
        return Path(os.environ['IDF_PATH']).resolve()
    here = Path(__file__).resolve()
    for parent in here.parents:
        if (parent / 'components' / 'soc').is_dir():
            return parent
    return here.parents[3]


def discover(root: Path, patterns: Iterable[str]) -> list[Path]:
    found: set[Path] = set()
    for pattern in patterns:
        found.update(p for p in root.glob(pattern) if p.is_file())
    return sorted(found)


def parse_modified_files(raw: str, root: Path) -> list[Path]:
    """Parse a semicolon-separated list (CI ``MR_MODIFIED_FILES``). Newlines also work."""
    raw = raw.strip().strip('"').strip("'")
    if not raw:
        return []
    found: set[Path] = set()
    for part in raw.replace('\r', '\n').replace('\n', ';').split(';'):
        part = part.strip().strip('"').strip("'")
        if not part:
            continue
        path = Path(part)
        if not path.is_absolute():
            path = root / part
        if path.is_file():
            found.add(path.resolve())
    return sorted(found)


def is_target_header(path: Path) -> bool:
    return path.name.endswith(TARGET_SUFFIXES)


def select_paths(root: Path, files: list[Path], modified_files: str | None) -> tuple[list[Path], list[Path]]:
    """Return (struct_paths, reg_paths).

    On MR pipelines only the changed ``*_struct.h`` / ``*_reg.h`` are checked.
    An empty list, or a change to any other file (``reg_base.h``, ``soc.h``,
    ...), falls back to a full scan: those files are included by the register
    headers, so an incremental scan would silently miss their effect.
    """
    if files:
        return (
            [p for p in files if p.name.endswith('_struct.h')],
            [p for p in files if p.name.endswith('_reg.h')],
        )
    if modified_files is not None:
        changed = parse_modified_files(modified_files, root)
        if changed and all(is_target_header(p) for p in changed):
            return (
                [p for p in changed if p.name.endswith('_struct.h')],
                [p for p in changed if p.name.endswith('_reg.h')],
            )
    return discover(root, STRUCT_GLOBS), discover(root, REG_GLOBS)


def report(results: list[tuple[Path, list[Finding]]], *, verbose: bool = False, kind: str = '') -> int:
    """Print results and return the exit code (0 = no ERROR and skips within limit)."""
    passed = skipped = failed = 0
    for path, findings in results:
        if not findings:
            passed += 1
            if verbose:
                print(f'{path}: OK')
            continue
        for level, line, message in findings:
            location = f'{path}:{line}' if line is not None else str(path)
            print(f'{location}: {level}: {message}', file=sys.stderr if level == 'ERROR' else sys.stdout)
        if any(level == 'ERROR' for level, _, _ in findings):
            failed += 1
        else:
            skipped += 1
    over_limit = skipped > MAX_SKIPPED_FILES
    label = f'{kind} file(s)' if kind else 'file(s)'
    summary = f'\n{passed}/{len(results)} {label} passed'
    if skipped:
        summary += f', {skipped} skipped'
    if failed:
        summary += f', {failed} failed'
    # stdout is block-buffered when redirected; flush so the diagnostics above
    # are written before anything we send to stderr.
    sys.stdout.flush()
    print(summary, file=sys.stderr if failed or over_limit else sys.stdout)
    if over_limit:
        print(
            f'ERROR: {skipped} file(s) could not be checked, exceeding the allowed maximum of '
            f'{MAX_SKIPPED_FILES}. Investigate the SKIP lines above.',
            file=sys.stderr,
        )
    return 1 if failed or over_limit else 0


def _run(
    paths: list[Path],
    check: Callable[[Path], list[Finding]],
    jobs: int,
    verbose: bool,
    kind: str,
) -> int:
    if not paths:
        print(f'No {kind} headers to check.')
        return 0
    with ThreadPoolExecutor(max_workers=max(jobs, 1)) as pool:
        results = list(pool.map(lambda p: (p, check(p)), paths))
    return report(results, verbose=verbose, kind=kind)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog='check_soc_headers.py',
        description='Check SOC register headers (struct layout and/or undefined macros).',
    )
    parser.add_argument(
        'check',
        choices=('struct', 'reg', 'all'),
        help='struct: *_struct.h layout; reg: *_reg.h macros; all: both',
    )
    parser.add_argument('-v', '--verbose', action='store_true', help='also print an OK line per passing file')
    parser.add_argument('-j', '--jobs', type=int, default=1, help='parallel workers (default: 1)')
    parser.add_argument('--root', type=Path, default=None, help='IDF root (default: $IDF_PATH or repo root)')
    parser.add_argument('--compiler', default='gcc', help='host C compiler for the reg check (default: gcc)')
    parser.add_argument(
        '--modified-files',
        metavar='LIST',
        help=(
            'semicolon-separated paths (CI: $MR_MODIFIED_FILES); falls back to a full scan '
            'when empty or when the list contains non-header files; omit for a full scan'
        ),
    )
    parser.add_argument('files', nargs='*', help='explicit headers (default: discover under --root)')
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_intermixed_args(argv)
    root = repo_root(args.root)
    struct_paths, reg_paths = select_paths(root, [Path(p) for p in args.files], args.modified_files)

    def run_struct(paths: list[Path]) -> int:
        return _run(paths, soc_header_struct.check_file, args.jobs, args.verbose, 'struct')

    def run_reg(paths: list[Path]) -> int:
        def check(path: Path) -> list[Finding]:
            result: list[Finding] = soc_header_reg.check_file(
                path, soc_header_reg.probe_command(root, path, args.compiler)
            )
            return result

        return _run(paths, check, args.jobs, args.verbose, 'reg')

    if args.check == 'struct':
        return run_struct(struct_paths)
    if args.check == 'reg':
        return run_reg(reg_paths)
    struct_rc = run_struct(struct_paths)
    reg_rc = run_reg(reg_paths)
    return struct_rc or reg_rc


if __name__ == '__main__':
    sys.exit(main())
