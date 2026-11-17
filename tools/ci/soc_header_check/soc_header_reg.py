# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Check that address macros in a register header expand cleanly.

Generated headers are almost entirely ``#define`` lines. A definition such as::

    # define AHB_DMA_IN_INT_RAW_CH0_REG (DR_REG_AHB_DMA_BASE + 0x0)

is never expanded by merely including the file, so a mistyped ``DR_REG_*_BASE``
is silent. This checker forces every address macro to be used in a host-gcc
syntax-only probe. Both ``FOO_REG`` style names (regtool output) and older
hand-written names whose body is ``(DR_REG_* + offset)`` are probed.

Include roots follow ``components/soc/<target>`` the same way the soc component
does, so ``soc/soc.h`` / ``soc/reg_base.h`` resolve without an IDF build.
"""

import functools
import re
import subprocess
from pathlib import Path

_DEFINE = re.compile(
    r'^[ \t]*#[ \t]*define[ \t]+'
    r'(?P<name>[A-Za-z_]\w*)'
    r'(?:\((?P<params>[^)]*)\))?'
    r'(?:[ \t]+(?P<body>\S.*))?$'
)
_BLOCK_COMMENT = re.compile(r'/\*.*?\*/', re.S)
_LINE_COMMENT = re.compile(r'//[^\n]*')
# Older chips (esp32, ...) use address macros that do not end in _REG, e.g.
# BBPD_CTRL or PERIPHS_IO_MUX_*. Match them by their "(BASE + offset)" body.
_ADDRESS_BODY = re.compile(r'^\(\s*(?:DR_REG_\w+|REG_\w+)\s*(?:\([^)]*\))?\s*[+-]')
# Object-like macros surface as "undeclared"; missing function-like macros
# (FOO_REG(i) when FOO_BASE(i) is not defined) surface as implicit declarations.
_UNDECLARED = re.compile(r'[\u2018\u2019\'"]([A-Za-z_]\w*)[\u2018\u2019\'"]\s+(?:undeclared|not declared)')
_IMPLICIT = re.compile(r"implicit declaration of function [\u2018\u2019'\"]([A-Za-z_]\w*)")


def _include_dirs(repo_root: Path, target: str) -> list[Path]:
    soc = repo_root / 'components' / 'soc' / target
    roots = [
        repo_root / 'components' / 'soc' / 'include',
        repo_root / 'components' / 'esp_common' / 'include',
        soc / 'include',
        soc,
    ]
    for directory in soc.rglob('*'):
        if directory.is_dir() and ((directory / 'soc').is_dir() or (directory / 'modem').is_dir()):
            roots.append(directory)
    return list(dict.fromkeys(root for root in roots if root.is_dir()))


@functools.cache
def _flags_for_target(repo_root: Path, target: str) -> tuple[str, ...]:
    return tuple(f'-I{root}' for root in _include_dirs(repo_root, target))


def _target_from_path(path: Path) -> str | None:
    match = re.search(r'(?:^|/)components/soc/([^/]+)/', path.as_posix())
    return match.group(1) if match else None


def probe_command(repo_root: Path, path: Path, compiler: str = 'gcc') -> list[str]:
    """Full host-gcc probe command. Put the header's own include root first so variant ``reg_base.h`` wins."""
    flags = list(_flags_for_target(repo_root, _target_from_path(path) or ''))
    for parent in path.parents:
        if (parent / 'soc').is_dir() or (parent / 'modem').is_dir():
            flag = f'-I{parent}'
            flags = [flag, *(f for f in flags if f != flag)]
            break
    # -Werror=implicit-function-declaration keeps the "missing function-like
    # base macro" diagnostic an error on gcc < 14, where it is only a warning.
    # Do not use -w: it would swallow that warning and the check would pass.
    return [compiler, '--std=c99', '-fsyntax-only', '-Werror=implicit-function-declaration', *flags, '-xc', '-']


def logical_lines(text: str) -> list[str]:
    """Strip comments and join backslash-continued lines."""
    text = _BLOCK_COMMENT.sub('', text)
    text = _LINE_COMMENT.sub('', text)
    out: list[str] = []
    buf = ''
    for line in text.splitlines():
        stripped = line.rstrip()
        if stripped.endswith('\\'):
            buf += stripped[:-1] + ' '
        else:
            out.append(buf + line)
            buf = ''
    if buf:
        out.append(buf)
    return out


def _probe_expr(name: str, params: str | None) -> str:
    if params is None:
        return name
    n_args = len([p for p in params.split(',') if p.strip()])
    if n_args == 0:
        return f'{name}()'
    return f'{name}({", ".join("0" for _ in range(n_args))})'


def address_macros(source: str) -> list[str]:
    """Return C expressions that force each address macro to expand."""
    exprs: list[str] = []
    for line in logical_lines(source):
        match = _DEFINE.match(line)
        if not match or not match.group('body'):
            continue
        name = match.group('name')
        if name.endswith('_REG') or _ADDRESS_BODY.match(match.group('body')):
            exprs.append(_probe_expr(name, match.group('params')))
    return exprs


def _undefined_macros(stderr: str) -> list[str]:
    """Return SCREAMING_SNAKE identifiers gcc could not resolve.

    Lowercase names (``rv_utils_get_core_id``, leftover macro parameters) are
    not register-base macros; other compile failures are reported as SKIP.
    """
    found: set[str] = set()
    for line in stderr.splitlines():
        match = _UNDECLARED.search(line) or _IMPLICIT.search(line)
        if match and match.group(1).isupper():
            found.add(match.group(1))
    return sorted(found)


def check_reg_content(source: str, path: Path, command: list[str]) -> list[tuple[str, int | None, str]]:
    macros = address_macros(source)
    if not macros:
        return []

    uses = ''.join(f'    (void)({expr});\n' for expr in macros)
    # Older hand-written headers (e.g. bb_reg.h) are not self-contained: their
    # address macros use DR_REG_*_BASE defined in soc/reg_base.h, which real
    # callers include first. Mirror that context so we report a genuinely
    # missing definition, not a missing include.
    probe = f'#include "soc/reg_base.h"\n#include "{path.resolve()}"\nvoid __soc_reg_macro_probe(void) {{\n{uses}}}\n'
    result = subprocess.run(command, input=probe, capture_output=True, text=True, check=False)
    if result.returncode == 0:
        return []

    undefined = _undefined_macros(result.stderr)
    if undefined:
        return [('ERROR', None, f'macro body refers to undefined identifier(s): {", ".join(undefined)}')]

    # The probe could not be compiled for an unrelated reason (missing include,
    # syntax error, ...). Never report this as a pass: the file was not checked.
    first_error = next(
        (line.strip() for line in result.stderr.splitlines() if 'error:' in line or 'fatal error:' in line),
        'unknown compiler failure',
    )
    return [('SKIP', None, f'probe could not be compiled ({first_error}); file NOT checked')]


def check_file(path: Path, command: list[str]) -> list[tuple[str, int | None, str]]:
    return check_reg_content(path.read_text(encoding='utf-8', errors='replace'), path, command)
