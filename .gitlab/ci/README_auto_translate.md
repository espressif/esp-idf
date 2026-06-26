# GitLab CI Auto Translation

Automatically translates English documentation to Chinese (EN → CN) on merge requests.

Orchestration lives in [`tools/ci/doc_auto_translate.py`](../../tools/ci/doc_auto_translate.py). The cloned **[auto-translate](https://gitlab.espressif.cn:6688/documentation/auto-translate)** repo runs **one file at a time** via `translate_files.py`.

Configure scope in [`translate_config.yml`](../../translate_config.yml). CI job definitions are in [`auto_trans.yml`](./auto_trans.yml).

---

## Quick start

1. Edit files under `docs/en/` on your MR branch.
2. Add **one** MR label ([choose a label](#choose-a-label)).
3. Open **Pipelines → Run pipeline** (a label alone does not re-run an existing pipeline unless you push a new commit or start a pipeline manually).

- When translation succeeds, CI pushes one `auto translation ...` bot commit to your MR branch.
- To run translation again: keep at most one such bot commit on the branch — [`check_auto_translate_commit_history`](#commit-history-check) runs on every MR pipeline (no label required) and fails otherwise. Squash or rebase if needed, re-add the MR label, then run the pipeline again.

### Choose a label

| Your MR changes | MR label | Translation |
|-----------------|----------|-------------|
| Existing paired EN/CN docs only — **most MRs** | **`auto-translate::incremental`** | [Incremental translation](#incremental-translation) only; skips files that need full-file translation |
| New English files and/or [full-file translation](#full-file-translation) | **`auto-translate::full`** | Incremental + full-file translation |

- Use **one** label per MR — do not add both.

> **Terminology**
>
> - <a id="incremental-translation"></a>**Incremental translation** — translate only the modified parts of the specified documents, not the full document.
> - <a id="full-file-translation"></a>**Full-file translation** — translate the entire English document to Chinese. Applies to new English files (no paired `docs/zh_CN` on the target branch) and to docs whose paired Chinese file is an **include-only placeholder** (a `docs/zh_CN` file whose only non-blank content is `.. include:: ...` and/or `:link_to_translation:` directives, with no translated Chinese body text).

> **Scope:** With current `specified_files: [docs/en]`, changing **only** `docs/zh_CN/...` (without a corresponding `docs/en/...` change) is out of scope and appears in `skipped`.

---

## MR labels and jobs

Two labels control which translation job runs. `check_line_num` runs automatically whenever either label is present. `check_auto_translate_commit_history` runs on **every** MR pipeline (no label required).

| MR label | Jobs that run | What gets translated |
|----------|---------------|----------------------|
| **`auto-translate::incremental`** | `check_auto_translate_commit_history` → `check_line_num` → `translate_incremental` | [`incremental`](#how-classification-works) bucket only |
| **`auto-translate::full`** | `check_auto_translate_commit_history` → `check_line_num` → `translate` | [`incremental`](#how-classification-works) + [`entire`](#how-classification-works) buckets |
| *(either label above)* | `check_line_num` runs automatically | Line-number consistency for **incremental** files only |

Do **not** add both labels on the same MR: `translate` and `translate_incremental` would both run and could translate the same incremental files twice.

| Job | Trigger | Purpose |
|-----|---------|---------|
| `check_auto_translate_commit_history` | Every MR pipeline | Fails if the MR branch has more than one bot commit (`auto translation ...`) |
| `check_line_num` | Either label (automatic) | Validates line-number consistency for [`incremental`](#how-classification-works) bucket only  |
| `translate_incremental` | `auto-translate::incremental` | Runs translation for [`incremental`](#how-classification-works) bucket only |
| `translate` | `auto-translate::full` | Runs translation for all classified buckets (see [How classification works](#how-classification-works)) |

**Line-number consistency** (checked by `check_line_num`): total line counts match between Chinese and English, and each line pair satisfies:

- if the Chinese line is text then the English line is text;
- if the Chinese line is blank then the English line is blank;
- if the Chinese line is `---` then the English line is `---`.

For example, if line 23 of the Chinese doc is `你好` and line 23 of the English doc is `Hello`, both are text, so that line is consistent.

**After adding a label**, use **Pipelines → Run pipeline** if there is no new commit — labels alone do not re-trigger an existing pipeline.

---

## How classification works

[`doc_auto_translate.py`](../../tools/ci/doc_auto_translate.py) builds a **TranslationPlan** for each MR: three buckets of repo-relative paths.

| Bucket | Meaning | `translate_files.py` flag |
|--------|---------|----------------------------|
| `incremental` | Paired EN/CN docs with normal Chinese content | No `-a` (incremental translation) |
| `entire` | Full-file translation (new EN file or include-only placeholder) | `-a` (full-file translation) on the paired `docs/en/...` path |
| `skipped` | Out of scope or not processed | — |

Example plan output:

```json
{
  "incremental": ["docs/en/foo.rst"],
  "entire": ["docs/en/new.rst"],
  "skipped": ["docs/zh_CN/baz.rst"]
}
```

**Labels do not affect classification** — they only choose which buckets a job translates (`incremental + entire` vs `incremental` only). Classification is always the same for a given MR diff.

### Decision flow

Applies to changed `.rst` / `.md` / `.mdx` files from `git diff merge_base..HEAD`.

With current config (`specified_files: [docs/en]`, no `force_full_translate`):

```text
Each changed doc path
│
├─ docs/zh_CN/...  ──────────────────────────► skipped
│                 (not under specified_files)
│
└─ docs/en/<path>
      │
      ├─ Paired docs/zh_CN/<path> meets ANY entire condition?
      │     • Working-tree CN is include-only placeholder
      │     • merge-base CN is include-only placeholder
      │     • merge-base has no docs/zh_CN/<path>
      │   └─ yes ───────────────────────────► entire  (stores docs/en/<path>)
      │
      └─ no ──────────────────────────────────► incremental (docs/en/<path>)
```

The `entire` bucket always stores **English** paths (`docs/en/...`).

### Examples (current `translate_config.yml`)

| MR change | `incremental` | `entire` | `skipped` |
|-----------|---------------|----------|-----------|
| `docs/en/foo.rst` (normal Chinese pair on target branch) | `docs/en/foo.rst` | — | — |
| `docs/en/new.rst` (no `docs/zh_CN/new.rst` on merge-base) | — | `docs/en/new.rst` | — |
| `docs/en/bar.rst` + CN is include-only placeholder | — | `docs/en/bar.rst` | — |
| `docs/zh_CN/baz.rst` only | — | — | `docs/zh_CN/baz.rst` |
| Both `docs/en` and `docs/zh_CN` for same doc | EN per rules above | *(same EN if entire)* | `docs/zh_CN/...` |

---

## `translate_config.yml` fields

| Field | Affects classification? | Current value | Effect |
|-------|-------------------------|---------------|--------|
| `specified_files` | Yes | `docs/en` | Only paths under this tree are classified; others → `skipped` |
| `compare_branch` | Yes (fallback) | `master` | Diff base when not in MR CI |
| `force_full_translate` | Yes | *(not set)* | If set, matching changed paths → `entire` |
| `target_language`, `trans_dict`, `target_language_folder` | No | — | Used by auto-translate scripts only |

In **GitLab MR pipelines**, `compare_branch` is overridden by `CI_MERGE_REQUEST_TARGET_BRANCH_NAME` (e.g. a release branch). Merge-base uses `CI_MERGE_REQUEST_DIFF_BASE_SHA` when present. The `compare_branch: master` setting is mainly for local runs without MR env vars.

Minimal config:

```yaml
specified_files:
  - docs/en

compare_branch: master

# force_full_translate:
#   - docs/en/path/to/doc.rst
```

---

## Loop prevention

Translation jobs push bot commits back to the MR branch. Two mechanisms prevent infinite re-trigger loops:

1. **Label removal (primary):** Before `translate` / `translate_incremental` invoke `translate_files.py`, the job removes the trigger label (`auto-translate::full` or `auto-translate::incremental`) via GitLab API. After a successful push, the label stays off so bot commits do not re-trigger translation (including merged-results pipelines where `CI_COMMIT_TITLE` is a merge message). If translation or push fails, the removed label is restored automatically. Re-add the label manually to run translation again.

2. **Bot commit skip (secondary):** Jobs are skipped when `CI_COMMIT_TITLE` starts with `auto translation`.

---

## Commit history check

`check_auto_translate_commit_history` runs on **every MR pipeline** (before `check_line_num` and translation). It does not require auto-translate labels and is not skipped on bot commits, so the bot-push pipeline fails if translation left more than one `auto translation ...` commit on the branch. Squash or rebase to at most one bot commit before re-running translation.

---

## Current limitations

- EN → CN translation only (based on current `translate_config.yml` and docs folder mapping).
- `check_line_num` validates **incremental** files only; `entire` paths (full-file translation) are excluded by design.
- Classification is limited by `specified_files`; out-of-scope doc changes are skipped.

---

## CI/CD variables

| Variable | Purpose |
|----------|---------|
| `AUTO_TRANSLATE_REPO` | Clone URL for auto-translate |
| `AUTO_TRANSLATE_OPENAI_API_KEY` | API key |
| `AUTO_TRANSLATE_BOT` | Token to push commits to MR branch |

---

## Local debugging

`plan` compares **`merge-base(compare_branch, HEAD)..HEAD`** (committed changes on the branch only).

```bash
python3 tools/ci/doc_auto_translate.py plan --repo-root .
```

If the result is empty but you have **uncommitted** doc edits (common before pushing):

```bash
python3 tools/ci/doc_auto_translate.py plan --repo-root . --working-tree
```

Diagnostics print to **stderr** when the plan is empty (merge-base, whether HEAD equals merge-base, etc.). Use `--verbose` to always show diagnostics. JSON on stdout: `incremental`, `entire`, `skipped`.
