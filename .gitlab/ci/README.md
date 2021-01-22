# Rules for `rules.yml`

- [Rules for `rules.yml`](#rules-for-rulesyml)
  - [How to Develop With `rules.yml`?](#how-to-develop-with-rulesyml)
    - [How to Add a New `Job`?](#how-to-add-a-new-job)
    - [How to Add a New `Rules` Template?](#how-to-add-a-new-rules-template)
    - [How to Add a New `if` Anchor?](#how-to-add-a-new-if-anchor)
  - [Naming Rules](#naming-rules)
    - [Common Naming Rules](#common-naming-rules)
    - [`if` Anchors Naming Rules](#if-anchors-naming-rules)
      - [Common Phrases/Abbreviations](#common-phrasesabbreviations)
    - [`rules` Template Naming Rules](#rules-template-naming-rules)
  - [Reusable Shell Script `tools/ci/utils.sh`](#reusable-shell-script-toolsciutilssh)
    - [Functions](#functions)
      - [CI Job Related](#ci-job-related)
      - [Shell Script Related](#shell-script-related)

## How to Develop With `rules.yml`?

### How to Add a New `Job`?

check if there's a suitable `.rules:<rules-you-need>` template

1. if there is, put this in the job `extends`. All done, now you can close this window. (`extends` could be array or string)
2. if there isn't
   1. check [How to Add a New `Rules` Template?](#how-to-add-a-new-rules-template), create a suitable one
   2. follow step 1

### How to Add a New `Rules` Template?

check if there's a suitable `.if-<if-anchor-you-need>` anchor

 1. if there is, create a rule following [`rules` Template Naming Rules](#rules-template-naming-rules).For detail information, please refer to [GitLab Documentation `rules-if`](https://docs.gitlab.com/ee/ci/yaml/README.html#rulesif). Here's an example.

```yaml
.rules:dev:
rules:
  - <<: *if-trigger
  - <<: *if-dev-push
```

2. if there isn't

   1. check [How to Add a New `if` Anchor?](#how-to-add-a-new-if-anchor), create a suitable one
   2. follow step 1

### How to Add a New `if` Anchor?

Create an `if` anchor following [`if` Anchors Naming Rules](#if-anchors-naming-rules). For detail information about how to write the condition clause, please refer to [GitLab Documentation `only/except (advanced)](https://docs.gitlab.com/ee/ci/yaml/README.html#onlyexcept-advanced). Here's an example.

```yaml
.if-schedule: &if-schedule:
  if: '$CI_PIPELINE_SOURCE == "schedule"'
```

## Naming Rules

### Common Naming Rules

if a phrase has multi words, use `_` to concat them.

> e.g. `regular_test`

if a name have multi phrase, use `-` to concat them.

> e.g. `regular_test-example_test`

### `if` Anchors Naming Rules

- if it's a label: `.if-label-<label_name>`
- if it's a ref: `.if-ref-<ref_name>`
- if it's a branch: `.if-branch-<branch_name>`
- if it's a tag: `.if-tag-<tag_name>`
- if it's a operating system: `.if-os-mac`
- if it's multi-type combination: `.if-ref-<release_name>-branch-<branch_name>`

#### Common Phrases/Abbreviations

- `no_label`

   `$BOT_TRIGGER_WITH_LABEL == null`

- `protected`

  `($CI_COMMIT_REF_NAME == "master" || $CI_COMMIT_BRANCH =~ /^release\/v/ || $CI_COMMIT_TAG =~ /^v\d+\.\d+(\.\d+)?($|-)/)`

-  `target_test`

   `example_test` or `custom_test` or `unit_test-all_targets`

### `rules` Template Naming Rules

- if it's os related: `.rules:os:<os_name>`
- if it's tag related: `.rules:tag:<tag_1>-<tag_2>`
- if it's label related: `.rules:labels:<label_1>-<label_2>`

    By default, all `.rules:labels` should include both `if-label-regular_test` and `if-protected-no-label` implicitly, unless they have special postfixes:

    - slim: `regular_test` not included
    - only: only have the phrases listed

- if it's target test related: `.rules:tests:<test_type_1>-<test_type_2>`

    By default, all `.rules:tests` should include `if-protected-no_label` implicitly, unless they have special postfixes (same as above)

- if it needs to build at first, then do some target test: `.rules:build_tests:<test_type_1>-<test_type_2>`

    By default, all `.rules:build_tests` should include `if-protected-no-label`, `if-label-build`, `if-label-regular-test` implictly, unless they have special postfixes (same as above)

- if a job supports all targets, use `-all_targets` as postfix

## Reusable Shell Script `tools/ci/utils.sh`

It is used to put all the reusable shell script as small functions. If you want to set `before_script: []` for you job, now you can set `extends: .before_script_slim` instead. it will only run `source tools/ci/utils.sh`

If you're developing CI shell scripts, you can use these functions without source. They're already included in all `before_script`

To run these commands in shell script locally, place `source tools/ci/utils.sh` at the very beginning.

### Functions

#### CI Job Related
- `apply_bot_filter`
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
