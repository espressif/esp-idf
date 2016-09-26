# The test folder in SDK

## File Structure

```
test --- CIConfigs --- sanity_test1.yml (Runner config files)
     |             |-- stress_test1.yml
     |-- TestCaseAll.yml (TestCaseFiles)
     |-- TestEnvAll.yml (TestCaseFiles)
     |-- InitialConditionAll.yml (TestCaseFiles)
     |-- TestCaseScript --- ... (Test case scripts)
```

1. CIConfigs folder
    * config for CI config files are put in this folder
    * CI config files configs the cases and some other options for the CI job with same name
1. Test case files
    * TestCaseAll.yml (test cases)
    * InitialConditionAll.yml (initial conditions)
    * TestEnvAll.yml (test environments)
    * [how to modify test cases](https://gitlab.espressif.cn:6688/yinling/auto_test_script/blob/master/public/Design/TestCaseFiles.DesignNote.md)
1. Test case scripts
    * some cases are implemented by specified script. those scripts are put in this folder.


## Modify test cases

1. check if the "SDK" of the test case only contain the current SDK
    * if Yes, then just modify the test case behavior
    * if No: 
        1. then remove current SDK name from "SDK" of the test case
        2. Add a new test case, and set "SDK" only support current SDK name
2. use [auto_test_script](https://gitlab.espressif.cn:6688/yinling/auto_test_script) to load the modified case and verify the modification
3. create a merge request and assign to HYL (or add comment @yinling for merging test). 
After review it will be merged to SDK and will be The cases will be synced to database in auto_test_script.
