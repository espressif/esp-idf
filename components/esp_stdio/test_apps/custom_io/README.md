| Supported Targets | Linux |
| ----------------- | ----- |

# esp_stdio custom I/O (Linux host test)

Linux-only host test for the runtime console-mux registration API in `esp_stdio`:
`esp_stdio_register_custom_io()` / `esp_stdio_unregister_custom_io()` for both
`ESP_STDIO_PRIMARY` and `ESP_STDIO_AUXILIARY` roles. The mux logic is pure C
(dispatch through `esp_vfs_fs_ops_t` function pointers), so a small in-memory
mock VFS backend is enough — no MCU peripheral required.

## Run

```
idf.py --preview set-target linux
idf.py build
build/test_esp_stdio_custom_io.elf
```

Or via pytest:

```
pytest pytest_esp_stdio_custom_io.py --target linux
```
