
# Note: The test cases in this folder are for Espressif internal use.

# Goto internal project wiki Testing page for detail about this folder.

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


## Run test case locally

1. clone auto_test_script (ssh://git@gitlab.espressif.cn:27227/yinling/auto_test_script.git) from gitlab
2. create test environment:
    1. search test case (search test case ID in components/test/TestCaseAll.yml, get the "test environment" value
    2. goto [test environment list](https://gitlab.espressif.cn:6688/yinling/auto_test_script/blob/master/public/Documents/TestEnvList.md), find the link and goto the environment detail page 
    3. create test environment according to figure and description
    4. [config test environment](https://gitlab.espressif.cn:6688/yinling/auto_test_script/blob/master/public/Design/TestEnvConfig.DesignNote.md). All parameters in table "Parameters require config before use" MUST be configured.
3. run test cases with [CIRunner.py](https://gitlab.espressif.cn:6688/yinling/auto_test_script/blob/master/public/Design/RunnerConfigs.DesignNote.md)



## exclude known issues for CI
the test cases listed in file "KnownIssues" will be excluded by CI when calculating results

Editing KnownIssues file is very simple, one single line for the ID for each case.
```
TCPIP_TCP_0101
TCPIP_TCP_0201
...
```