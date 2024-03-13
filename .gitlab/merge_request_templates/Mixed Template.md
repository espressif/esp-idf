_All texts in italics are instructional and should be replaced by contents or removed._

## Checklist

_This entire section can be deleted if all items are checked._

* [ ] Enough information to help reviewers understand the issue, its root cause, impact, and the proposed solution
* [ ] Enough information to help reviewers understand the feature, its functional description, example, documentation, test cases, test results, feature TODO list
* [ ] The MR Title describes the change, including the component name, ie "lwip: Add support for IP over Pigeon"
* [ ] All related links, including JIRA, backport, submodule MR, are mentioned in the `Related` subsection.
* [ ] Any GitHub issues are linked inside the git commit message and corresponding release notes
* [ ] Add label for the area this MR is part of
* [ ] For documentation updates, check if label `needs translation:CN` or `needs translation:EN` have been added when the other language version still needs the update. Don't add such labels if the document updated has never been translated before and requires translation from scratch.
* [ ] Any necessary "needs backport" labels are added
* [ ] Check if this is a breaking change. If it is, add notes to the `Breaking change notes` subsection below
* [ ] Release note entry if this is a new public feature, or a fix for an issue introduced in the previous release.
* [ ] The commit log is clean and ready to merge.
* [ ] All relevant CI jobs have been run, i.e. jobs which cover the code changed by the MR.

---

_For issues, put enough information here to help reviewers understand the issue, its root cause, impact, and the proposed solution._

_If this issue is a regression, specify in the `Related` subsection below, in which commit or MR it was introduced. This helps reviewers to check if the backport labels are set correctly._

---

_For features, put enough information here to help reviewers understand the feature, its functional description, example, documentation, test cases, test results, feature TODO list._

---

_For other small/non-public changes, which are not expected to be in the release notes, can be mentioned here, include:_

 * changes that don't apply to customers, e.g. some CI fixes, test only MR and etc

## Related

* Mention or related JIRA tasks (e.g. IDF-0000), to make sure they get updated.
* Mention submodule MR, if there is
* Mention backport(ed) MR, if there is

_Don't touch the subsection titles below, they will be parsed by scripts._

## Release notes <!-- Mandatory -->

_Changes made in this MR that should go into the **Release Notes** should be listed here. Please use **past tense** and *specify the area (see maintainers page of IDF internal wiki)*. If there is a subscope, include it and separate with slash (`/`). Minor changes can go to the descriptions above without a release notes entry._

_Write all the changes in a **list** (Start at the beginning of the line with `-` or `*`). If multiple changes are made, each of them should take a single line. If there is only one change to list, it should still be the only line of a list. If this MR does not need any release notes, write "No release notes" here without the `-` or `*`. e.g._

* [WiFi] Changed/fixed/updated xxx
* [WiFi] Added support of xxx
* [Peripheral Drivers/I2S] Fixed xxx (https://github.com/espressif/esp-idf/issues/xxxx)

## Breaking change notes

_Remove this subsection if not used._

_If there are any breaking changes, please mention it here. Talking about (1) what is not accepted any more, (2) the alternative solution and (3) the benefits/reason. e.g._

_Please strictly follow the breaking change restriction, which means, if there is a breaking change but you are merging to non-major versions, you have to separate the breaking part out to another MR for a major version. The breaking change subsection is only accepted in MRs merging to major versions._

* [VFS/UART] Now vfs_uart_set_rts_cts accept one more instance argument, to support configuration to different ports.
