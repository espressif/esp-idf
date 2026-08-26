# How the `generate_rules.py` works

## Functionalities

This script can do only two things:

1. Auto-generate some labels/rules we need and update them in `rules.yml`
2. Generate a dependency tree graph

## Schema

This file only used basic YAML grammar and has nothing to do with the GitLab version YAML file.

It has five custom keywords:

- `matrix`: An array of sub-arrays, used to replicate rules by formatting strings. You can use the format string everywhere, it will be formatted recursively
- `labels`: An array of `labels`.
- `patterns`: An array of `patterns`. Patterns that not included
- `included_in`: An array of other `rule` names. It indicates the `labels` and `patterns` will be included in all specified `rules` as well
- `deploy`: An array of strings, used to replicate rules by adding postfix `-<item in deploy array>`. It indicates the extra `label` used in `rules`, which will explain later.

## How to use this file to generate `rules.yml`

Let's take a complicated example to help understand the process

```yaml
"test-{0}-{1}":
    matrix:
        - [a, b]
        - [c, d]
    labels:
        - "{0}-{1}"
    patterns:
        - "{0}"
        - pattern-not-exist
    included_in:
        - build-{0}
```

1. expand the mapping dicts defined by `matrix`

   After this step, it will turn into 4 dicts:

   | key      | labels | patterns | included_in |
   | -------- | ------ | -------- | ----------- |
   | test-a-c | a-c    | a        | build-a     |
   | test-a-d | a-d    | a        | build-a     |
   | test-b-c | b-c    | b        | build-b     |
   | test-b-d | b-d    | b        | build-b     |

   **Advanced Usage: You can overwrite a mapping by declaring it again later**, For example:

   If we concatenate this part to the previous example,

    ```yaml
    # ... The same as the previous example
    
    test-a-c:
        labels:
            - overwrite
    ```

   `rule` `test-a-c` will be turned into:

   | key      | labels    |
   | -------- | --------- |
   | test-a-c | overwrite |

   **Mappings with the keyword `deploy` will also replicate by adding a postfix `-<item in deploy array>` to the mapping key**

2. create rules by `included_in`

   After this step, it will turn into 6 mapping dicts:

   | key      | labels   | patterns |
   | -------- | -------- | -------- |
   | test-a-c | a-c      | a        |
   | test-a-d | a-d      | a        |
   | test-b-c | b-c      | b        |
   | test-b-d | b-d      | b        |
   | build-a  | a-c, a-d | a        |
   | build-b  | b-c, b-d | b        |

3. replace the auto-generated region in `rules.yml` with `labels`, and `rules`. Each mapping will generate a `rule` and all the required labels. `patterns` are pre-defined in `rules.yml` and could not be generated automatically. If a mapping is using a `pattern` undefined, the `pattern` will be ignored.

    - If a mapping key has postfix `-preview`, no `if-protected-xxx` clause will be added
    - else if a mapping key has postfix `-production`, an `if-protected-no_label` clause will be added
    - else, an `if-protected` clause will be added

## Graph

All `label` nodes are in green, `pattern` nodes are in cyan, `rule` nodes are in blue

### Requirements

There are a few extra dependencies while generating the dependency tree graph, please refer to [pygraphviz](https://github.com/pygraphviz/pygraphviz/blob/master/INSTALL.txt) documentation to install both `graphviz` and `pygraphviz`

### CLI usage

`python $IDF_PATH/tools/ci/generate_rules.py --graph OUTPUT_PATH`
