<!-- This Template states the absolute minimum for an MR.
    If you want to have a more elaborate template or know why we have this structure,
    please use the "Mixed Template" or consult the Wiki. -->
<!-- Add the CI labels to trigger the appropriate tests (e.g. "unit_test_esp32") --><!-- Mandatory -->
<!-- Make sure the commit message follows the Wiki about "Commit Messages" --><!-- Mandatory -->

<!-- Add description of the change here --><!-- Mandatory -->

## Related <!-- Optional -->
<!-- Related Jira issues and Github issues or write "No related issues"-->

## Release notes <!-- Mandatory -->
<!-- Either state release notes or write "No release notes" -->

<!-- ## Breaking change notes --><!-- Optional -->

<!-- ## Dynamic Pipeline Configuration
```yaml
Test Case Filters:
    # Only run tests that match the given substring expression (modified files/components will be ignored):
    # Please use a list of strings. 
    # This will run the test cases filtered like `pytest -k "(<list_item_1>) or (<list_item_2>) or ...`
    # The fast pipeline will fail at the final stage.
    # For example:
      - test_sdm and not sdmmc
      - test_hello_world
    # This example will include all tests containing 'test_hello_world' in the name,
    # and include all tests containing 'test_sdm' but not 'sdmmc' in the name.
``` --><!-- Optional -->
