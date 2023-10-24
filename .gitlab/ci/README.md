# IDF CI

- [IDF CI](#idf-ci)
  - [General Workflow](#general-workflow)
  - [What if Expected Jobs ARE NOT Created?](#what-if-expected-jobs-are-not-created)
  - [MR labels for additional jobs](#mr-labels-for-additional-jobs)
    - [Supported MR Labels](#supported-mr-labels)
    - [How to trigger a `detached` pipeline without pushing new commits?](#how-to-trigger-a-detached-pipeline-without-pushing-new-commits)
  - [How to Develop With `rules.yml`?](#how-to-develop-with-rulesyml)
    - [General Concepts](#general-concepts)
    - [How to Add a New `Job`?](#how-to-add-a-new-job)
    - [How to Add a New `Rules` Template?](#how-to-add-a-new-rules-template)
    - [How to Add a New `if` Anchor?](#how-to-add-a-new-if-anchor)
    - [Naming Rules](#naming-rules)
      - [Common Naming Rules](#common-naming-rules)
      - [`if` Anchors Naming Rules](#if-anchors-naming-rules)
      - [`rules` Template Naming Rules](#rules-template-naming-rules)
  - [Reusable Shell Script `tools/ci/utils.sh`](#reusable-shell-script-toolsciutilssh)
    - [Functions](#functions)
      - [CI Job Related](#ci-job-related)
      - [Shell Script Related](#shell-script-related)
  - [Manifest File to Control the Build/Test apps](#manifest-file-to-control-the-buildtest-apps)
    - [Grammar](#grammar)

## General Workflow

1. Push to a remote branch
2. Create an MR, choose related labels (not required)
3. A `detached` pipeline will be created.
4. if you push a new commit, a new pipeline will be created automatically.

## What if Expected Jobs ARE NOT Created?

1. check the file patterns

   If you found a job that is not running as expected with some file changes, a git commit to improve the `pattern` will be appreciated.

2. please add MR labels to run additional tests, currently we have to do this only for `target-test` jobs, please use it as few as possible. Our final goal is to remove all the labels and let the file changes decide everything!

## MR labels for additional jobs

### Supported MR Labels

- `build`
- `build_docs`
- `component_ut[_esp32/esp32s2/...]`
- `custom_test[_esp32/esp32s2/...]`
- `docker`
- `docs`
- `docs_full`, triggers a full docs build, regardless of files changed
- `example_test[_esp32/esp32s2/...]`
- `fuzzer_test`
- `host_test`
- `integration_test`
- `iperf_stress_test`
- `macos`
- `macos_test`
- `nvs_coverage`
- `submodule`
- `unit_test[_esp32/esp32s2/...]`
- `weekend_test`
- `windows`

There are two general labels (not recommended since these two labels will trigger a lot of jobs)

- `target_test`: includes all target for `example_test`, `custom_test`, `component_ut`, `unit_test`, `integration_test`
- `all_test`: includes all test labels

### How to trigger a `detached` pipeline without pushing new commits?

Go to MR web page -> `Pipelines` tab -> click `Run pipeline` button.

In very rare case, this tab will not show up because no merge_request pipeline is created before. Please use web API then.

```shell
curl -X POST --header "PRIVATE-TOKEN: [YOUR PERSONAL ACCESS TOKEN]" [GITLAB_SERVER]/api/v4/projects/103/merge_requests/[MERGE_REQUEST_IID]/pipelines
```

## How to Develop With `rules.yml`?

### General Concepts

- `pattern`: Defined in an array. A GitLab job will be created if the changed files in this MR matched one of the patterns. For example:

    ```yaml
    .patterns-python-files: &patterns-python-files
      - "**/*.py"
    ```

- `label`: Defined in an if clause, similar as the previous bot command. A GitLab job will be created if the pipeline variables contains variables in `BOT_LABEL_xxx` format (DEPRECATED) or included in the MR labels. For example:

    ```yaml
    .if-label-build_docs: &if-label-build_docs
      if: '$BOT_LABEL_BUILD_DOCS || $CI_MERGE_REQUEST_LABELS =~ /^(?:[^,\n\r]+,)*build_docs(?:,[^,\n\r]+)*$/i'
    ```

- `rule`: A combination of various patterns, and labels. It will be used by GitLab YAML `extends` keyword to tell GitLab in what conditions will this job be created. For example:

    ```yaml
    .rules:build:docs:
      rules:
        - <<: *if-protected
        - <<: *if-label-build_docs
        - <<: *if-label-docs
        - <<: *if-dev-push
          changes: *patterns-docs
    ```

  An example for GitLab job on how to use extends:

    ```yaml
    check_docs_lang_sync:
      extends:
        - .pre_check_template
        - .rules:build:docs
      script:
        - cd docs
        - ./check_lang_folder_sync.sh
    ```

### How to Add a New `Job`?

check if there's a suitable `.rules:<rules-you-need>` template

1. if there is, put this in the job `extends`. All done, now you can close this window. (`extends` could be array or string)
2. if there isn't
    1. check [How to Add a New `Rules` Template?](#how-to-add-a-new-rules-template), create a suitable one
    2. follow step 1

### How to Add a New `Rules` Template?

check if this rule is related to `labels`, `patterns`

1. if it is, please refer to [dependencies/README.md](./dependencies/README.md) and add new rules by auto-generating
2. if it isn't, please continue reading

check if there's a suitable `.if-<if-anchor-you-need>` anchor

1. if there is, create a rule following [`rules` Template Naming Rules](#rules-template-naming-rules).For detail information, please refer to [GitLab Documentation `rules-if`](https://docs.gitlab.com/ee/ci/yaml/README.html#rulesif). Here's an example.

    ```yaml
    .rules:patterns:python-files:
      rules:
        - <<: *if-protected
        - <<: *if-dev-push
          changes: *patterns-python-files
    ```

2. if there isn't

    1. check [How to Add a New `if` Anchor?](#how-to-add-a-new-if-anchor), create a suitable one
    2. follow step 1

### How to Add a New `if` Anchor?

Create an `if` anchor following [`if` Anchors Naming Rules](#if-anchors-naming-rules). For detailed information about how to write the condition clause, please refer to [GitLab Documentation `only/except (advanced)](https://docs.gitlab.com/ee/ci/yaml/README.html#onlyexcept-advanced). Here's an example.

```yaml
.if-schedule: &if-schedule:
  if: '$CI_PIPELINE_SOURCE == "schedule"'
```

### Naming Rules

#### Common Naming Rules

if a phrase has multi words, use `_` to concatenate them.

> e.g. `regular_test`

if a name has multi phrases, use `-` to concatenate them.

> e.g. `regular_test-example_test`

#### `if` Anchors Naming Rules

- if it's a label: `.if-label-<label_name>`
- if it's a ref: `.if-ref-<ref_name>`
- if it's a branch: `.if-branch-<branch_name>`
- if it's a tag: `.if-tag-<tag_name>`
- if it's multi-type combination: `.if-ref-<release_name>-branch-<branch_name>`

  **Common Phrases/Abbreviations**

    - `no_label`

      `$BOT_TRIGGER_WITH_LABEL == null`

    - `protected`

      `($CI_COMMIT_REF_NAME == "master" || $CI_COMMIT_BRANCH =~ /^release\/v/ || $CI_COMMIT_TAG =~ /^v\d+\.\d+(\.\d+)?($|-)/)`

    - `target_test`

      a combination of `example_test`, `custom_test`, `unit_test`, `component_ut`, `integration_test` and all targets

#### `rules` Template Naming Rules

- if it's tag related: `.rules:tag:<tag_1>-<tag_2>`
- if it's label related: `.rules:labels:<label_1>-<label_2>`
- if it's test related: `.rules:test:<test_type>`
- if it's build related: `.rules:build:<build_type>`
- if it's pattern related: `.rules:patterns:<patterns>`

## Reusable Shell Script `tools/ci/utils.sh`

It is used to put all the reusable shell scripts as small functions. If you want to set `before_script: []` for you job, now you can set `extends: .before_script_slim` instead. it will only run `source tools/ci/utils.sh`

If you're developing CI shell scripts, you can use these functions without `source` them. They're already included in all `before_script`

To run these commands in shell script locally, place `source tools/ci/utils.sh` at the very beginning.

### Functions

#### CI Job Related

- `add_gitlab_ssh_keys`
- `add_github_ssh_keys`
- `add_doc_server_ssh_keys`
- `fetch_submodules`
- `get_all_submodules`

#### Shell Script Related

- `error`: log in red color
- `warning`: log in orange color
- `info`: log in green color
- `run_cmd`: run the command with duration seconds info
- `retry_failed`: run the command with duration seconds info, retry when failed

## Manifest File to Control the Build/Test apps

`.build-test-rules.yml` file is a manifest file to control if the CI is running the build and test job or not. The Supported Targets table in `README.md` for apps would be auto-generated by `pre-commit` from the app's `.build-test-rules.yml`.

### Grammar

We're using the latest version of [idf-build-apps][idf-build-apps]. Please refer to their [documentation][manifest-doc]

[idf-build-apps]: https://github.com/espressif/idf-build-apps
[manifest-doc]: https://docs.espressif.com/projects/idf-build-apps/en/latest/manifest.html
