#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Classify MR doc changes for ESP-IDF auto-translation CI and drive auto-translate
translate_files.py / check_line_num.py (no changes to auto-translate repo).

- incremental: normal EN<->CN docs (translate_files.py without -a)
- entire: zh_CN stubs that only include English (translate_files.py -a on the EN path)

All classified files are translated with local_flag=True (no per-file commit), then pushed
in a single GitLab commit via pak_commit_file_any_project.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from collections.abc import Sequence
from contextlib import AbstractContextManager
from dataclasses import dataclass
from dataclasses import field
from pathlib import Path
from types import TracebackType
from typing import TYPE_CHECKING
from typing import Literal

import yaml

if TYPE_CHECKING:
    import gitlab.v4.objects

EN_PREFIX = 'docs/en/'
ZH_PREFIX = 'docs/zh_CN/'
DOC_SUFFIXES = ('.rst', '.md', '.mdx')
INCLUDE_LINE_RE = re.compile(r'^\s*\.\.\s+include::\s+', re.IGNORECASE)
LINK_TO_TRANSLATION_RE = re.compile(r'^\s*:link_to_translation:`', re.IGNORECASE)
CJK_RE = re.compile(r'[\u4e00-\u9fff\u3400-\u4dbf\uf900-\ufaff]')
ZH_SEGMENT_RE = re.compile(r'(^|/)zh(?:[-_](?:hans|cn))(?=/|$)', re.IGNORECASE)


@dataclass
class TranslationPlan:
    incremental: list[str] = field(default_factory=list)
    entire: list[str] = field(default_factory=list)
    skipped: list[str] = field(default_factory=list)

    def to_dict(self) -> dict:
        return {
            'incremental': sorted(self.incremental),
            'entire': sorted(self.entire),
            'skipped': sorted(self.skipped),
        }


@dataclass
class PlanContext:
    compare_branch: str
    merge_base: str
    diff_label: str
    changed_raw: list[str] = field(default_factory=list)


def _load_tcf(config_path: Path) -> dict:
    with config_path.open(encoding='utf-8') as f:
        return yaml.safe_load(f) or {}


def _expand_paths(entries: Sequence[str], repo_root: Path) -> set[str]:
    paths: set[str] = set()
    for entry in entries:
        p = (repo_root / entry).resolve()
        if p.is_file():
            paths.add(p.relative_to(repo_root).as_posix())
        elif p.is_dir():
            for root, _, files in os.walk(p):
                for name in files:
                    paths.add((Path(root) / name).relative_to(repo_root).as_posix())
    return paths


def _in_scope(path: str, scope_files: set[str], scope_roots: Sequence[str]) -> bool:
    if path in scope_files:
        return True
    for root in scope_roots:
        root = root.rstrip('/') + '/'
        if path.startswith(root):
            return True
    return False


def _resolve_compare_branch(tcf_value: str, cli_override: str | None = None) -> str:
    if cli_override:
        return cli_override
    ci_target = os.environ.get('CI_MERGE_REQUEST_TARGET_BRANCH_NAME', '').strip()
    if ci_target:
        return ci_target
    return tcf_value or 'master'


def _merge_base(compare_branch: str) -> str:
    ci_base = os.environ.get('CI_MERGE_REQUEST_DIFF_BASE_SHA', '').strip()
    if ci_base:
        return ci_base
    subprocess.run(
        ['git', 'fetch', '--depth=200', 'origin', compare_branch],
        check=False,
    )
    p = subprocess.run(
        ['git', 'merge-base', f'origin/{compare_branch}', 'HEAD'],
        capture_output=True,
        text=True,
        check=True,
    )
    return p.stdout.strip()


def _changed_doc_paths(merge_base: str, *, use_working_tree: bool = False) -> list[str]:
    cmd = ['git', 'diff', '--name-only', '--diff-filter=ACMR', merge_base]
    if use_working_tree:
        cmd.extend(['--', 'docs/en', 'docs/zh_CN'])
    else:
        cmd.extend(['HEAD', '--', 'docs/en', 'docs/zh_CN'])
    p = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=True,
    )
    out = []
    for line in p.stdout.splitlines():
        path = line.strip()
        if path.endswith(DOC_SUFFIXES):
            out.append(path)
    return out


def _zh_for_en(en_path: str) -> str:
    if en_path.startswith(EN_PREFIX):
        return ZH_PREFIX + en_path[len(EN_PREFIX) :]
    return ''


def _en_for_zh(zh_path: str) -> str:
    if zh_path.startswith(ZH_PREFIX):
        return EN_PREFIX + zh_path[len(ZH_PREFIX) :]
    return ''


def _path_exists_at_rev(path: str, rev: str) -> bool:
    p = subprocess.run(
        ['git', 'cat-file', '-e', f'{rev}:{path}'],
        capture_output=True,
    )
    return p.returncode == 0


def _read_text_at_rev(path: str, rev: str) -> str | None:
    p = subprocess.run(
        ['git', 'show', f'{rev}:{path}'],
        capture_output=True,
        text=True,
    )
    if p.returncode != 0:
        return None
    return p.stdout


def _normalize_zh_cn_rel_path(path: str, repo_root: Path | None) -> str:
    rel = path.replace('\\', '/')
    if repo_root is not None:
        try:
            rel = Path(path).resolve().relative_to(repo_root.resolve()).as_posix()
        except ValueError:
            pass
    return ZH_SEGMENT_RE.sub(r'\1zh_CN', rel)


def _is_include_only_zh_cn_text(text: str, rel: str) -> bool:
    if not rel.startswith(ZH_PREFIX) or not rel.endswith(DOC_SUFFIXES):
        return False
    if CJK_RE.search(text):
        return False
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if INCLUDE_LINE_RE.match(line) or LINK_TO_TRANSLATION_RE.match(line):
            continue
        # Any other non-empty line (toctree, titles, comments with ::, etc.)
        return False
    return True


def is_include_only_zh_cn(path: str, repo_root: Path | None = None) -> bool:
    """
    True when docs/zh_CN file has no Chinese body text — only include/link directives.
    Matches ESP-IDF stubs like a single ``.. include:: ../../../en/...`` line.
    """
    rel = _normalize_zh_cn_rel_path(path, repo_root)
    if not rel.startswith(ZH_PREFIX) or not rel.endswith(DOC_SUFFIXES):
        return False
    full = Path(path)
    if repo_root is not None and not full.is_file():
        full = repo_root / rel
    try:
        text = full.read_text(encoding='utf-8')
    except OSError:
        return False
    return _is_include_only_zh_cn_text(text, rel)


def is_include_only_zh_cn_at_rev(path: str, rev: str) -> bool:
    """True when docs/zh_CN at ``rev`` is an include-only stub (e.g. on merge-base / master)."""
    rel = _normalize_zh_cn_rel_path(path, None)
    text = _read_text_at_rev(rel, rev)
    if text is None:
        return False
    return _is_include_only_zh_cn_text(text, rel)


def build_plan(
    repo_root: Path,
    config_path: Path | None = None,
    compare_branch: str | None = None,
    *,
    use_working_tree: bool = False,
) -> tuple[TranslationPlan, PlanContext]:
    config_path = config_path or repo_root / 'translate_config.yml'
    tcf = _load_tcf(config_path)
    compare_branch = _resolve_compare_branch(
        tcf.get('compare_branch', 'master'),
        compare_branch,
    )
    scope_roots = list(tcf.get('specified_files') or [])
    force_entire = list(tcf.get('force_full_translate') or [])

    scope_files = _expand_paths(scope_roots, repo_root) if scope_roots else set()
    merge_base = _merge_base(compare_branch)
    diff_label = 'working-tree' if use_working_tree else 'HEAD'
    changed_raw = _changed_doc_paths(merge_base, use_working_tree=use_working_tree)
    ctx = PlanContext(
        compare_branch=compare_branch,
        merge_base=merge_base,
        diff_label=diff_label,
        changed_raw=changed_raw,
    )
    plan = TranslationPlan()

    for changed in changed_raw:
        if scope_roots and not _in_scope(changed, scope_files, scope_roots):
            plan.skipped.append(changed)
            continue

        if force_entire and _in_scope(
            changed,
            _expand_paths(force_entire, repo_root),
            force_entire,
        ):
            _add_entire(plan, changed)
            continue

        if changed.startswith(EN_PREFIX):
            zh = _zh_for_en(changed)
            zh_path = repo_root / zh
            zh_was_include_stub = is_include_only_zh_cn_at_rev(zh, merge_base)
            if (
                (zh_path.is_file() and is_include_only_zh_cn(zh, repo_root))
                or zh_was_include_stub
                or not _path_exists_at_rev(zh, merge_base)
            ):
                _add_entire(plan, changed)
            else:
                _add_incremental(plan, changed)
        elif changed.startswith(ZH_PREFIX):
            if is_include_only_zh_cn(changed, repo_root):
                en = _en_for_zh(changed)
                if en:
                    _add_entire(plan, en)
                else:
                    plan.skipped.append(changed)
            elif is_include_only_zh_cn_at_rev(changed, merge_base):
                en = _en_for_zh(changed)
                if en:
                    _add_entire(plan, en)
                else:
                    plan.skipped.append(changed)
            else:
                _add_incremental(plan, changed)
        else:
            plan.skipped.append(changed)

    return plan, ctx


def _print_plan_diagnostics(ctx: PlanContext, plan: TranslationPlan) -> None:
    print(f'# compare_branch: {ctx.compare_branch}', file=sys.stderr)
    print(f'# merge_base: {ctx.merge_base}', file=sys.stderr)
    print(f'# diff: {ctx.merge_base[:12]}..{ctx.diff_label}', file=sys.stderr)
    print(f'# changed docs (before TCF filter): {len(ctx.changed_raw)}', file=sys.stderr)
    for path in ctx.changed_raw[:20]:
        print(f'#   {path}', file=sys.stderr)
    if len(ctx.changed_raw) > 20:
        print(f'#   ... and {len(ctx.changed_raw) - 20} more', file=sys.stderr)
    if not ctx.changed_raw:
        head = subprocess.run(
            ['git', 'rev-parse', '--short', 'HEAD'],
            capture_output=True,
            text=True,
            check=False,
        ).stdout.strip()
        mb = subprocess.run(
            ['git', 'rev-parse', '--short', ctx.merge_base],
            capture_output=True,
            text=True,
            check=False,
        ).stdout.strip()
        print(f'# HEAD: {head}  merge_base: {mb}', file=sys.stderr)
        if head == mb:
            print(
                '# Branch has no commits ahead of merge-base (no committed doc diff).',
                file=sys.stderr,
            )
            print(
                '# Local edits only? Re-run: python3 tools/ci/doc_auto_translate.py plan --working-tree',
                file=sys.stderr,
            )
    elif not plan.incremental and not plan.entire and not plan.skipped:
        print('# Doc paths changed but none matched classification (unexpected).', file=sys.stderr)
    elif plan.skipped and not plan.incremental and not plan.entire:
        print('# All changed docs are outside translate_config.yml specified_files.', file=sys.stderr)


def _add_entire(plan: TranslationPlan, path: str) -> None:
    if path.startswith(EN_PREFIX):
        if path not in plan.entire:
            plan.entire.append(path)
        plan.incremental[:] = [p for p in plan.incremental if p != path]
    elif path.startswith(ZH_PREFIX):
        en = _en_for_zh(path)
        if en:
            _add_entire(plan, en)
    else:
        plan.skipped.append(path)


def _add_incremental(plan: TranslationPlan, path: str) -> None:
    if path in plan.entire:
        return
    if path not in plan.incremental:
        plan.incremental.append(path)


def _line_check_targets(plan: TranslationPlan) -> list[str]:
    """Files that need check_line_num (incremental set only)."""
    return list(plan.incremental)


def _run_check_line_num(
    auto_translate_dir: Path,
    check_files: list[str],
    config_path: Path,
) -> None:
    sys.path.insert(0, str(auto_translate_dir))
    from check_line_num import check_line_num  # type: ignore

    if not check_files:
        print('doc_auto_translate: no incremental files for line check; skipping check_line_num.')
        return
    print(f'doc_auto_translate: check_line_num for {len(check_files)} file(s)')
    check_line_num(check_files=check_files, config_path=str(config_path))


def _setup_auto_translate_import(auto_translate_dir: Path) -> None:
    path_str = str(auto_translate_dir)
    if path_str not in sys.path:
        sys.path.insert(0, path_str)


def _translate_files_local(
    auto_translate_dir: Path,
    file_paths: Sequence[str],
    *,
    entire: bool,
    openai_key: str,
    url: str,
    bot: str,
    project_id: str,
    branch: str,
    diff_base_commit: str | None = None,
) -> list[str]:
    """Translate files without committing; return repo-relative output paths."""
    if not file_paths:
        return []
    _setup_auto_translate_import(auto_translate_dir)
    import openai
    from translate_files import get_out_file_path  # type: ignore[import-untyped]
    from translate_files import main_trans  # type: ignore[import-untyped]

    openai.api_key = openai_key
    output_files: list[str] = []
    mode_label = 'entire' if entire else 'incremental'
    for file_path in file_paths:
        print(f'doc_auto_translate: run {mode_label} on {file_path}')
        main_trans(
            url,
            bot,
            project_id,
            branch,
            trans_all_flag=entire,
            filename=file_path,
            local_flag=True,
            diff_base_commit=diff_base_commit,
        )
        output_files.append(get_out_file_path(file_path))
    return output_files


def _commit_translated_files(
    auto_translate_dir: Path,
    output_files: Sequence[str],
    url: str,
    bot: str,
    project_id: str,
    branch: str,
) -> None:
    unique_outputs = list(dict.fromkeys(output_files))
    if not unique_outputs:
        return
    _setup_auto_translate_import(auto_translate_dir)
    from translate_files import pak_commit_file_any_project  # type: ignore[import-untyped]

    print(
        f'doc_auto_translate: committing {len(unique_outputs)} translated file(s) in one commit',
    )
    pak_commit_file_any_project(unique_outputs, url, bot, project_id, branch)


def run_translation(
    plan: TranslationPlan,
    auto_translate_dir: Path,
    openai_key: str,
    url: str,
    bot: str,
    project_id: str,
    branch: str,
    mode: str,
    diff_base_commit: str | None = None,
) -> None:
    entire_files = list(plan.entire)
    incremental_files = list(plan.incremental)

    if mode == 'entire-only':
        incremental_files = []
    elif mode == 'incremental-only':
        entire_files = []

    all_outputs: list[str] = []
    if entire_files:
        all_outputs.extend(
            _translate_files_local(
                auto_translate_dir,
                entire_files,
                entire=True,
                openai_key=openai_key,
                url=url,
                bot=bot,
                project_id=project_id,
                branch=branch,
            ),
        )
    if incremental_files:
        all_outputs.extend(
            _translate_files_local(
                auto_translate_dir,
                incremental_files,
                entire=False,
                openai_key=openai_key,
                url=url,
                bot=bot,
                project_id=project_id,
                branch=branch,
                diff_base_commit=diff_base_commit,
            ),
        )
    if all_outputs:
        _commit_translated_files(
            auto_translate_dir,
            all_outputs,
            url,
            bot,
            project_id,
            branch,
        )


def _parse_trans_file(raw: str) -> list[str]:
    cleaned = raw.strip()
    for ch in '[]"':
        cleaned = cleaned.replace(ch, '')
    paths = [p.strip() for p in cleaned.replace(',', ' ').split() if p.strip()]
    return [_normalize_zh_cn_rel_path(p, None) for p in paths]


def _parse_label_list(raw: str) -> list[str]:
    return [p.strip() for p in raw.replace(',', ' ').split() if p.strip()]


def _mr_label_guard_enabled() -> bool:
    return bool(os.environ.get('CI_MERGE_REQUEST_IID', '').strip())


def _get_gitlab_mr() -> gitlab.v4.objects.ProjectMergeRequest | None:
    url = os.environ.get('CI_SERVER_URL', '').strip()
    token = os.environ.get('AUTO_TRANSLATE_BOT', '').strip()
    project_id = os.environ.get('CI_PROJECT_ID', '').strip()
    mr_iid = os.environ.get('CI_MERGE_REQUEST_IID', '').strip()
    if not all([url, token, project_id, mr_iid]):
        return None
    import gitlab

    gl = gitlab.Gitlab(url, private_token=token, api_version=4)
    project = gl.projects.get(project_id)
    return project.mergerequests.get(mr_iid)


def _remove_mr_labels(labels: Sequence[str]) -> list[str]:
    mr = _get_gitlab_mr()
    if mr is None:
        print(
            'doc_auto_translate: skip MR label remove (missing MR context or GitLab credentials)',
            file=sys.stderr,
        )
        return []
    current = set(mr.labels)
    to_remove = [label for label in labels if label in current]
    if not to_remove:
        return []
    mr.labels = sorted(current - set(to_remove))
    mr.save()
    print(f'doc_auto_translate: removed MR labels: {", ".join(to_remove)}', file=sys.stderr)
    return to_remove


def _restore_mr_labels(labels: Sequence[str]) -> None:
    if not labels:
        return
    mr = _get_gitlab_mr()
    if mr is None:
        print(
            'doc_auto_translate: cannot restore MR labels (missing MR context or GitLab credentials)',
            file=sys.stderr,
        )
        return
    current = set(mr.labels)
    to_add = [label for label in labels if label not in current]
    if not to_add:
        return
    mr.labels = sorted(current | set(to_add))
    mr.save()
    print(f'doc_auto_translate: restored MR labels: {", ".join(to_add)}', file=sys.stderr)


class MrLabelGuard(AbstractContextManager['MrLabelGuard']):
    """
    Remove trigger labels from the MR before translate_files.py commits.
    Restores removed labels when the guarded block raises.
    """

    def __init__(self, labels: Sequence[str]) -> None:
        self._requested = list(labels)
        self._removed: list[str] = []

    def __enter__(self) -> MrLabelGuard:
        if self._requested and _mr_label_guard_enabled():
            self._removed = _remove_mr_labels(self._requested)
        return self

    def __exit__(
        self,
        exc_type: type[BaseException] | None,
        exc_val: BaseException | None,
        exc_tb: TracebackType | None,
    ) -> Literal[False]:
        if exc_type is not None and self._removed:
            _restore_mr_labels(self._removed)
        return False


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description='ESP-IDF doc auto-translation CI orchestrator')
    parser.add_argument(
        'command',
        choices=['plan', 'check-line-num', 'run'],
        help='plan: print JSON; check-line-num: filtered line check; run: invoke translate_files.py',
    )
    parser.add_argument('--repo-root', default='.', help='ESP-IDF repository root')
    parser.add_argument('--config', default='translate_config.yml', help='TCF path relative to repo root')
    parser.add_argument('--auto-translate-dir', default='./auto-translate', help='Cloned auto-translate path')
    parser.add_argument(
        '--mode',
        choices=['all', 'incremental-only', 'entire-only'],
        default='all',
        help='For run: which translation buckets to execute',
    )
    parser.add_argument('--trans-file', default='', help='Optional override file list (space/comma separated)')
    parser.add_argument(
        '--compare-branch',
        default='',
        help='Override translate_config.yml compare_branch (default: TCF or CI MR target)',
    )
    parser.add_argument(
        '--working-tree',
        action='store_true',
        help='Diff merge_base against working tree (staged/unstaged), not only committed HEAD',
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Print diff diagnostics to stderr (always on when plan result is empty)',
    )
    parser.add_argument(
        '--guard-labels',
        default='',
        help=(
            'For run: comma/space-separated MR labels to remove before translating '
            '(restored automatically if translation fails; CI MR only)'
        ),
    )
    args = parser.parse_args(argv)

    repo_root = Path(args.repo_root).resolve()
    config_path = repo_root / args.config
    auto_translate_dir = Path(args.auto_translate_dir).resolve()
    compare_branch = args.compare_branch or None
    plan_kwargs = {
        'use_working_tree': args.working_tree,
    }

    if args.command == 'plan':
        plan, ctx = build_plan(
            repo_root,
            config_path,
            compare_branch,
            **plan_kwargs,
        )
        if args.verbose or (not plan.incremental and not plan.entire and not plan.skipped):
            _print_plan_diagnostics(ctx, plan)
        print(json.dumps(plan.to_dict(), indent=2))
        return 0

    if args.command == 'check-line-num':
        plan, _ctx = build_plan(
            repo_root,
            config_path,
            compare_branch,
            **plan_kwargs,
        )
        _run_check_line_num(auto_translate_dir, _line_check_targets(plan), config_path)
        return 0

    # run
    openai_key = os.environ.get('AUTO_TRANSLATE_OPENAI_API_KEY', '')
    url = os.environ.get('CI_SERVER_URL', '')
    bot = os.environ.get('AUTO_TRANSLATE_BOT', '')
    project_id = os.environ.get('CI_PROJECT_ID', '')
    branch = os.environ.get('CI_COMMIT_REF_NAME', '')
    for name, val in [
        ('AUTO_TRANSLATE_OPENAI_API_KEY', openai_key),
        ('CI_SERVER_URL', url),
        ('AUTO_TRANSLATE_BOT', bot),
        ('CI_PROJECT_ID', project_id),
        ('CI_COMMIT_REF_NAME', branch),
    ]:
        if not val:
            print(f'ERROR: missing {name}', file=sys.stderr)
            return 1

    diff_base_commit: str | None = None
    if args.trans_file.strip():
        paths = _parse_trans_file(args.trans_file)
        if args.mode == 'entire-only':
            plan = TranslationPlan()
            for path in paths:
                _add_entire(plan, path)
        else:
            plan = TranslationPlan(incremental=paths)
    else:
        plan, ctx = build_plan(
            repo_root,
            config_path,
            compare_branch,
            **plan_kwargs,
        )
        diff_base_commit = ctx.merge_base
        if args.verbose:
            _print_plan_diagnostics(ctx, plan)

    print(json.dumps(plan.to_dict(), indent=2))
    if not plan.incremental and not plan.entire:
        print('WARNING: nothing to translate for this MR and TCF scope.')
        return 0

    guard_labels = _parse_label_list(args.guard_labels)
    if guard_labels:
        with MrLabelGuard(guard_labels):
            run_translation(
                plan,
                auto_translate_dir,
                openai_key,
                url,
                bot,
                project_id,
                branch,
                args.mode,
                diff_base_commit=diff_base_commit,
            )
    else:
        run_translation(
            plan,
            auto_translate_dir,
            openai_key,
            url,
            bot,
            project_id,
            branch,
            args.mode,
            diff_base_commit=diff_base_commit,
        )
    return 0


if __name__ == '__main__':
    sys.exit(main())
