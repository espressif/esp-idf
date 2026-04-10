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

<!-- Don't remove the next line - assigns the MR author as the assignee -->
/assign me
