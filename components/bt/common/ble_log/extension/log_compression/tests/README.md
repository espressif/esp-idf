# BLE Log Compression Test Suite

## Overview

This test suite validates the BLE log compression pipeline — from C source input through tree-sitter AST parsing, log ID assignment, compressed macro generation, to final `log_index.h` header output.

All tests use Python standard library `unittest` only. No third-party test frameworks required.

## Directory Structure

```
tests/
├── README.md                       # This file
├── test_utils.py                   # Shared utilities: path setup, PipelineContext, helpers
├── test_format_parser.py           # FormatToken NamedTuple and parse_format_string
├── test_arg_size_types.py          # ARG_SIZE_TYPE determination (U32/U64/STR)
├── test_db_manager.py              # LogDBManager state persistence and incremental logic
├── test_log_extraction.py          # Tree-sitter AST log extraction
├── test_macro_generation.py        # Bluedroid and Mesh module macro generators
├── test_pipeline_e2e.py            # End-to-end .c -> .h with golden file comparison
├── test_incremental.py             # Incremental compression scenarios
├── update_golden.py                # Script to regenerate golden expected files
└── fixtures/
    ├── c_sources/                  # Test input C files
    │   ├── simple_logs.c           # Basic: no-arg, single-arg, multi-arg, string
    │   ├── format_specifiers.c     # All format -> size type mappings
    │   ├── special_tokens.c        # __func__, __LINE__, MAC2STR, bt_hex
    │   ├── concatenated_strings.c  # PRIu64, PRId64 macro concatenation
    │   ├── multi_level.c           # ERROR/WARNING/API/DEBUG/EVENT/VERBOSE
    │   ├── no_logs.c               # Valid C with no matching log tags
    │   └── mesh_logs.c             # BLE Mesh module log patterns
    └── expected/                   # Golden output headers for comparison
        ├── simple_logs_index.h
        ├── format_specifiers_index.h
        ├── special_tokens_index.h
        ├── multi_level_index.h
        └── multi_file_index.h
```

## Running Tests

From the `tests/` directory:

```bash
# Run all tests
python3 -m unittest discover -v

# Run a single test module
python3 -m unittest test_format_parser -v

# Run a single test class
python3 -m unittest test_arg_size_types.TestU64SizeType -v

# Run a single test method
python3 -m unittest test_db_manager.TestAddLog.test_sequential_ids -v
```

## Test Modules

### test_format_parser.py — Format String Parsing

Validates `FormatToken(NamedTuple)` and `parse_format_string()` in `c_format_parse.py`.

| Test Class | Coverage |
|---|---|
| `TestFormatTokenStructure` | NamedTuple field names, named vs index access, tuple subclass |
| `TestBasicSpecifiers` | 18 conversion characters (`%d`, `%s`, `%f`, `%p`, ...), escaped `%%`, empty string |
| `TestPositionTracking` | `FormatToken.start` / `.end` positions in source string |
| `TestLengthModifiers` | `h`, `hh`, `l`, `ll`, `z`, `j`, `t` |
| `TestWidthAndPrecision` | Fixed width/precision, dynamic `%*d` / `%.*s`, both dynamic |
| `TestFlags` | `-`, `+`, ` `, `#`, `0` |
| `TestMixedContent` | Interleaved text and specifiers, quoted format strings |

### test_arg_size_types.py — Size Type Determination

**Core regression test for the off-by-one bug fix.** Verifies that `FormatToken.conv_char` and `FormatToken.length` (not the old incorrect indices) are used to determine `ARG_SIZE_TYPE`.

| Format | Expected Size Type | Field Checked |
|---|---|---|
| `%d`, `%u`, `%x`, `%X`, `%o` | `U32` (0) | `token.conv_char` |
| `%s` | `STR` (1) | `token.conv_char` |
| `%f` | `U64` (2) | `token.conv_char` |
| `%lld`, `%llx` | `U64` (2) | `token.length` |
| `__func__` / `__LINE__` | skipped | `token.conv_char == '@func'` / `'@line'` |
| `MAC2STR(addr)` | buffer arg | `token.conv_char.startswith('@hex_func')` |

### test_db_manager.py — Database State Management

Tests `LogDBManager` in `LogDBManager.py`.

| Test Class | Coverage |
|---|---|
| `TestAddLog` | SUCCESS / LOG_EXISTS return codes, sequential IDs, unique key fields |
| `TestPersistence` | `save_all()` + reload preserves data |
| `TestFileProcessing` | `is_file_processed`: new file, unchanged, modified |
| `TestConfigUpdate` | `is_config_updated`: fresh DB, same config after reload, changed config |
| `TestSourceUpdateState` | `SOURCE_LOG_UPDATE_FULL` / `PARTIAL` / `NONE` transitions |

### test_log_extraction.py — AST Log Extraction

Tests `LogCompressor.extract_log_calls()` using tree-sitter.

| Test Class | Coverage |
|---|---|
| `TestExtractNoLogs` | No matching tags, no functions |
| `TestExtractBasicLogs` | Single log, no-arg log, multiple tags |
| `TestExtractFormatTokens` | Token population, hexify=True, dynamic width/precision filtered |
| `TestExtractSpecialTokens` | `__func__` -> `@func`, `__LINE__` -> `@line`, `MAC2STR` -> `@hex_func` |
| `TestExtractCallerInfo` | Correct enclosing function identification |
| `TestExtractArgMismatch` | Format/argument count mismatch raises `SyntaxError` |

### test_macro_generation.py — Module-Specific Macro Generators

Tests `make_bluedroid_log_macro.py` and `make_mesh_log_macro.py` directly.

| Test Class | Coverage |
|---|---|
| `TestBluedroidMacroGen` | `gen_header_head`, zero/single/multi-arg macros, level checks (`appl_trace_level >= BT_TRACE_LEVEL_*`), buffer args |
| `TestMeshMacroGen` | Header declarations, `BT_ERR`/`BT_DBG` level checks |

### test_pipeline_e2e.py — End-to-End Pipeline

Full `.c` -> `.h` pipeline tests with golden file comparison.

| Test Class | Coverage |
|---|---|
| `TestSimpleLogsE2E` | Header generation, tag replacement in source, header structure, sorted IDs, preserve tags |
| `TestFormatSpecifiersE2E` | All size types (U32/STR/U64) present in output |
| `TestSpecialTokensE2E` | `ble_log_compressed_hex_print_buf` generated, `__func__` excluded |
| `TestNoLogsE2E` | Zero macros for file without matching tags |
| `TestMultiLevelE2E` | Unique IDs per level, different level check prefixes |
| `TestMultiFileE2E` | Globally unique IDs across multiple files |
| `TestConcatenatedStringsE2E` | `PRIu64` produces U64 size type |
| `TestGoldenFileComparison` | Exact output comparison against `fixtures/expected/*.h` |

### test_incremental.py — Incremental Compression

Tests caching, re-runs, and config change behavior.

| Test Class | Coverage |
|---|---|
| `TestFirstRun` | All files compressed, header generated |
| `TestRerunUnchanged` | File hashes tracked, log IDs stable on re-add |
| `TestRerunModifiedFile` | Modified file reprocessed, existing IDs stable |
| `TestAddNewFile` | New file IDs continue from `max_id + 1` |
| `TestConfigChange` | Config change triggers `SOURCE_LOG_UPDATE_FULL` |

## Golden File Workflow

The `fixtures/expected/` directory contains known-good header outputs. End-to-end tests compare generated output against these files (with copyright year normalized).

When the compression logic changes intentionally:

```bash
# Regenerate all golden files
python3 update_golden.py

# Review the diff
git diff fixtures/expected/

# Commit after verification
```

## Fixture C Source Files

These files are parsed by tree-sitter only — they do not need to compile. Clang warnings about implicit declarations and unknown types are expected and irrelevant.

| File | Purpose |
|---|---|
| `simple_logs.c` | 4 functions: no-arg, single `%d`, triple `%d`, `%s` |
| `format_specifiers.c` | All `ARG_SIZE_TYPE` mappings: `%d/%u/%x/%X/%o` (U32), `%f` (U64), `%lld/%llx` (U64), `%s` (STR), mixed |
| `special_tokens.c` | `__func__`, `__FUNCTION__`, `__LINE__`, `MAC2STR`, mixed special + normal args |
| `concatenated_strings.c` | `PRIu64`, `PRId64`, `PRIx64` macro concatenation |
| `multi_level.c` | ERROR, WARNING, API, DEBUG, EVENT, VERBOSE in one function |
| `no_logs.c` | Only `printf`, no matching log tags |
| `mesh_logs.c` | BLE Mesh tags: `BT_ERR`, `BT_WARN`, `BT_INFO`, `BT_DBG` |

## Dependencies

- Python 3.8+
- `tree-sitter` + `tree-sitter-c` (same versions as in `scripts/requirements.txt`)
- `pyyaml`
- No additional test frameworks required (`unittest` is in the Python standard library)
