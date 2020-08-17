# Documentation Test Folder

This folder contains the files needed for running tests the ESP-IDF documentation building system.

Tests are divided into two categories: unit tests and integration tests:

## Unit Tests

The Sphinx IDF extensions are unit-tested in [test_sphinx_idf_extensions.py](test_sphinx_idf_extensions.py)

## Integration Tests
Due to the thigh integration with Sphinx some functionality is difficult to test with simple unit tests.

 To check that the output from the Sphinx build process is as expected [test_docs.py](test_docs.py) builds a test subset of the documentation, found in the [en](en/) folder. The HTML output is then checked to see that it contains the expected content.

# Running Tests

Both [test_sphinx_idf_extensions.py](test_sphinx_idf_extensions.py) and [test_docs.py](test_docs.py) are run as part of the `host_test` stage of the CI pipeline.

It's also possible to run the tests locally by running the following commands from the test folder:

* `./test_sphinx_idf_extensions.py`
* `./test_docs.py`

Note that [test_docs.py](test_docs.py) tries to build a test subset of the documentation, and thus requires your environment to be set up for building documents. See [Documenting Code](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/documenting-code.html) for instructions on how to set up the `build_docs` environment.