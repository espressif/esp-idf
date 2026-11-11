# SOC Header Check

Local checks for register headers under `components/soc`.

There are two independent checks:

- `struct` — walks `*_struct.h` and checks that each struct with bit-fields sums to exactly 32 bits. Pure Python; no compiler.
- `reg` — compiles a small host-gcc probe that *uses* every `*_REG` address macro in a `*_reg.h` file. Macro bodies are otherwise unexpanded, so a mistyped `DR_REG_*_BASE` (or a missing instance index on a multi-instance peripheral) shows up here instead of later in a full IDF build.

Both use the host toolchain only. You do not need a configured IDF project or a cross-compiler. The chip is inferred from the header path (`components/soc/<target>/...`).

## Prerequisites

- a C99 compiler named `gcc` on `PATH` (override with `--compiler`)

From the IDF root, `$IDF_PATH` is used when set; otherwise the script walks up from its own location.

## Usage

```bash
# Layout-check every *_struct.h
python tools/ci/soc_header_check/check_soc_headers.py struct

# Compile-check every *_reg.h (this is the full local sweep; use -j)
python tools/ci/soc_header_check/check_soc_headers.py reg -j 8

# Both
python tools/ci/soc_header_check/check_soc_headers.py all -j 8

# One file, or a few
python tools/ci/soc_header_check/check_soc_headers.py reg \
    components/soc/esp32c6/register/soc/uhci_reg.h

python tools/ci/soc_header_check/check_soc_headers.py struct \
    components/soc/esp32c6/register/soc/uhci_struct.h
```

By default only errors and a `N/M file(s) passed` summary are printed. `-v` also prints an `OK` line per passing file. `-j N` runs files in parallel.

A file whose `reg` probe cannot be compiled is reported as `SKIP` — it is not counted as passed and is listed in the summary. The exit code is 0 only when there are no `ERROR`s and no more than `MAX_SKIPPED_FILES` files are skipped.

## CI behaviour

On MR pipelines the job passes `--modified-files "$MR_MODIFIED_FILES"`. Only changed `*_struct.h` / `*_reg.h` are checked; if the list is empty or contains any other file (for example `reg_base.h` or `soc.h`, which the register headers include), the checker falls back to a full scan. Protected branches and local runs always scan everything.

The `reg` probe includes `soc/reg_base.h` before the header under test, mirroring how drivers include these headers — older hand-written headers such as `bb_reg.h` are not self-contained and reference `DR_REG_*_BASE` without including its definition. Besides `*_REG` names, address macros whose body looks like `(DR_REG_* + offset)` are probed too, covering the older chips' headers. The probe uses `-Werror=implicit-function-declaration` instead of `-w`, so a missing function-like base macro is caught on any host gcc version.


## Tests

```bash
python -m unittest discover -s tools/ci/soc_header_check/test -p 'test_check_soc_headers.py' -v
```
