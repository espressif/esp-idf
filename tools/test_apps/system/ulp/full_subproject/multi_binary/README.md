| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- |

# ULP LP-Core Multi-Binary Build System Example

This example demonstrates a CMake v2 ULP full subproject that builds multiple
ULP binaries from one ULP child project. The ULP project uses the lower-level
ULP build APIs instead of `ulp_project_default()`: it calls
`ulp_project_init()`, creates two ULP executables with
`ulp_build_executable()`, and generates the binary artifacts for each
executable with `ulp_build_binary()`.

The main application embeds both generated ULP binaries, runs the first one to
calculate an addition result, then loads and runs the second one to calculate a
multiplication result.

Both ULP binaries define a global variable named `result`. Because no explicit
`PREFIX` is passed to `ulp_add_project()`, the build system gives each binary a
binary-scoped default symbol prefix, so the parent app can read
`ulp_add_result` and `ulp_multiply_result` without the generated linker exports
colliding.

## Example Output

```
Running ULP add binary
ULP add result: 22
Running ULP multiply binary
ULP multiply result: 42
Example finished
```
