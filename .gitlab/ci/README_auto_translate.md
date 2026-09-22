# GitLab CI Auto Translation

Automatically translates English documentation to Chinese (EN → CN) on merge requests.

Orchestration lives in [`tools/ci/doc_auto_translate.py`](../../tools/ci/doc_auto_translate.py). The cloned **auto-translate** repo runs **one file at a time** via `translate_files.py`.

Configure scope and terminology in [`translate_config.yml`](../../translate_config.yml). CI job definitions are in [`auto_trans.yml`](./auto_trans.yml). Terminology comes from the ESP-Term glossary — see [Terminology enforcement](#terminology-enforcement-esp-term-glossary).

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
| `glossary` | No | enabled | ESP-Term terminology enforcement — see [Terminology enforcement](#terminology-enforcement-esp-term-glossary) |
| `rst_safeguards` | No | both `true` | Keeps `.. toctree::` untranslated; spaces Chinese text away from inline markup so Sphinx builds |
| `ai_rst_postprocess` | No | `enabled: false` | Optional LLM pass to repair RST formatting; off to avoid extra cost and non-determinism in CI |
| `target_language`, `trans_dict`, `target_language_folder`, `debug` | No | — | Used by auto-translate scripts only |

In **GitLab MR pipelines**, `compare_branch` is overridden by `CI_MERGE_REQUEST_TARGET_BRANCH_NAME` (e.g. a release branch). Merge-base uses `CI_MERGE_REQUEST_DIFF_BASE_SHA` when present. The `compare_branch: master` setting is mainly for local runs without MR env vars.

Minimal config for classification (the checked-in file also configures terminology and RST handling):

```yaml
specified_files:
  - docs/en

compare_branch: master

# force_full_translate:
#   - docs/en/path/to/doc.rst
```

---

## Terminology enforcement (ESP-Term glossary)

Translations follow the ESP-Term glossary (`Consolidated_Terms.yaml`, ~300 bilingual terms), so the same English term maps to the same Chinese term across docs. Only the terms that actually occur in the text being translated are injected into each prompt, so prompt size stays small.

**How CI wires it up**

1. `ESP_TERM_REPO` is set as a CI/CD variable (clone URL including token).
2. Every auto-translate job clones it to `./esp-term` (see `.auto_translate_setup` in [`auto_trans.yml`](./auto_trans.yml)).
3. `glossary.file` in `translate_config.yml` points at `./esp-term/Consolidated_Terms.yaml`.

The clone is **best-effort**: if `ESP_TERM_REPO` is unset or the clone fails, the job logs a warning and translation continues without terminology enforcement. A missing or malformed glossary file behaves the same way — it never fails the job.

**Current settings** in [`translate_config.yml`](../../translate_config.yml)

| Setting | Value | Effect |
|---------|-------|--------|
| `enabled` | `true` | Glossary is consulted for every translated file |
| `file` | `./esp-term/Consolidated_Terms.yaml` | Runtime clone path created by the job |
| `max_terms` | `200` | Cap on terms injected per request; truncation is reported in the log |
| `min_en_word_len` | `5` | Bare English words shorter than this are not enforced |
| `include_acronyms` / `include_synonyms` | `true` | Also match `Acronym` / `Full_Form` and `Synonym` surface forms |
| `match_variants` | `true` | Also match plural and inflected forms (`register` → `registers`) |
| `post_process` | `true` | Hard-replace source-language terms the model left untranslated (safe subset only, see below) |
| `log` | `true` | Per-file term summary plus a compliance scan of the translated output |

**`trans_dict` wins over the glossary.** ESP-IDF house-style overrides go there (currently `pin: 管脚`, `datasheet: 技术规格书`); on conflict the glossary entry is ignored.

**Reviewing terminology in a job log.** Each translated file prints a `[glossary]` block that groups the matched terms by how they were applied:

```text
[glossary] docs/en/api-reference/peripherals/gpio.rst (EN source)
  source hits: glossary 3, trans_dict 1
  unique terms in source: 4 = 2 fixed + 1 keep English + 1 model chooses
  fixed (prompt; leftover English may be hard-replaced): 2
    - interrupt -> 中断
    - pin -> 管脚  [trans_dict]
  keep English (prompt only, never hard-replaced): 1
    - eFuse -> eFuse
  model chooses (prompt only, never hard-replaced): 1
    - flash -> flash [EN when meaning 闪存] / 烧录
  hard replacements in the output (among the 2 fixed terms; xN = occurrences): 1
    - interrupt -> 中断 x1
  compliance: OK (no leftover English among the 2 fixed terms)
```

Only the `fixed` group is enforced in the output and covered by the compliance scan. A `compliance:` warning listing leftover English terms, or a `max_terms` truncation warning, is worth checking before approving the bot commit.

**Matching is deliberately conservative**, so not every glossary term in a doc is forced:

- Bare English words shorter than `min_en_word_len` are skipped, so common words such as `code`, `data`, and `bit` keep a context-appropriate translation. Multi-word terms and acronyms are always matched.
- Terms with several Chinese equivalents (for example `闪存；烧录`) are all offered to the model so it can pick by context, and are never force-replaced.
- Terms that must stay in English — flagged by a `Note` such as `文档中保留英文: eFuse`, or auto-detected when `Chinese_Equivalent` itself contains Latin letters — are kept in English and excluded from replacement.
- Case matters where it should: ALL-UPPERCASE glossary terms (`CAN`, `LED`) only match exactly, so prose `can`/`led` is never a hit; and an ALL-UPPERCASE occurrence of a lowercase term is never force-replaced — the `pin` override does not rewrite `PIN` in "PIN code".
- Longest match wins, so `chip series group` is not also counted as `chip series`.
- `post_process` never rewrites protected spans: inline code, `:role:` targets, URLs, and directive or decoration lines.
- Whenever the glossary is enabled, stray ASCII spaces between two Chinese characters are collapsed in the output (`例如 调试` → `例如调试`); spaces around Latin runs such as `eFuse 参数` are preserved.


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
- [Terminology enforcement](#terminology-enforcement-esp-term-glossary) is best-effort: a failed ESP-Term clone does not fail the job, so translations may land without glossary terms applied. Check the `[glossary]` log block if terminology looks off.

---

## CI/CD variables

| Variable | Purpose |
|----------|---------|
| `AUTO_TRANSLATE_REPO` | Clone URL for auto-translate |
| `AUTO_TRANSLATE_OPENAI_API_KEY` | API key |
| `AUTO_TRANSLATE_BOT` | Token to push commits to MR branch |
| `ESP_TERM_REPO` | Clone URL for ESP-Term; enables [terminology enforcement](#terminology-enforcement-esp-term-glossary). Optional — translation runs without it |

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
