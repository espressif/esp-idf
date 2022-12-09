# Build

```bash
make -j 6
```

# Run
* Run particular test case:
```bash
./test_nvs "<particular test case>"

```
* Run all quick tests:
```bash
./test_nvs -d yes exclude:[long]
```

* Run all tests (takes several hours)
```bash
./test_nvs -d yes
```

