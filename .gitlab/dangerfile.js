import { danger, warn, message, results } from "danger"

/**
 * Check if MR Title contains prefix "Draft: ... or "WIP: ...".
 *
 * @dangerjs WARN
*/
function checkMrTitle() {
    const mrTitle = danger.gitlab.mr.title

    if (mrTitle.toUpperCase().includes("WIP") || mrTitle.toUpperCase().includes("DRAFT")) {
        return warn("Please remove the `WIP:`/`Draft:` prefix from the MR name before merging this MR.");
    }
}
checkMrTitle();


/**
 * Check if MR Description is longer than 50 characters".
 *
 * @dangerjs WARN
 */
function checkMrDescription() {
    const shortMrDescriptionThreshold = 50;// MR description is considered too short below this number of characters
    const mrDescription = danger.gitlab.mr.description

    if (mrDescription.length < shortMrDescriptionThreshold) {
        return warn("The MR description looks very brief, please check if more details can be added.");
    }
}
checkMrDescription();


/**
 * Check if MR Description contains mandatory section "Release notes"
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6852
 *
 * @dangerjs WARN
 */
function checkMrReleaseNotes() {
    const mrDescription = danger.gitlab.mr.description

    if (!mrDescription.toUpperCase().includes("## Release notes".toUpperCase())) {
        return warn("Please update the MR description, the mandatory section `Release Notes` seems to be missing.");
    }
}
checkMrReleaseNotes();


/**
 * Check if MR Description contains JIRA issues references
 *
 * Check if the associated GitHub Jira ticket has a GitHub closure reference in the commit message.
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6854
 *
 * @dangerjs WARN
 */
function checkMrJiraLinks() {
    const mrDescription = danger.gitlab.mr.description
    const mrCommitMessages = danger.gitlab.commits.map(commit => commit.message);

    const matchBlockRelated = mrDescription.match(/\#\# Related.*$/s); // Match MR description starting with line ## Related till the end of MR description
    const noRelatedIssues = /No related issues/.test(matchBlockRelated ? matchBlockRelated[0] : '');  // Check if there is "No related issues"
    const testJiraLabels = /[A-Z]+-[0-9]+/.test(matchBlockRelated ? matchBlockRelated[0] : ''); // Test if pattern of Jira label "JIRA-1234" or "RDT-311" is in section Related
    const ghIssueTicket = /IDFGH-[0-9]+/.test(matchBlockRelated ? matchBlockRelated[0] : ''); // Check if there is JIRA link starts with "IDFGH-*" in MR description, section "Related"
    const testGithubLink = /Closes https:\/\/github\.com\/espressif\/esp-idf\/issues\/[0-9]+/

    if (mrDescription.toUpperCase().includes("## RELATED") && noRelatedIssues) {
        return
    }

    if (!mrDescription.toUpperCase().includes("## RELATED") || !testJiraLabels) { // Missing section "Related" or missing links to JIRA tickets
        return warn("Please add links to JIRA issues to the MR description section `Related`.");

    } else if (ghIssueTicket) { // Found JIRA ticket linked GitHub issue
        if (!testGithubLink.test(mrCommitMessages)) {  // Commit message does not contain a link to close the issue on GitHub
            return warn("Please add GitHub issue closing link `Closes https://github.com/espressif/esp-idf/issues/<github-issue-number>` to the commit message.");
        }
    }
}
checkMrJiraLinks();


/**
 * Check if MR has not an excessive numbers of commits (if squashed)
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6856.
 *
 * @dangerjs INFO
 */
function checkMrTooManyCommits() {
    const tooManyCommitThreshold = 5  // above this number of commits, squash is recommended
    const mrCommits = danger.gitlab.commits

    if (mrCommits.length > tooManyCommitThreshold) {
        return message(`You might consider squashing your ${mrCommits.length} commits (simplifying branch history).`);
    }
}
checkMrTooManyCommits();


/**
 * Check commit message are descriptive enough (longer that 10 characters)
 * @dangerjs WARN
 */
function checkMrCommitMessagesLength() {
    const shortCommitMessageThreshold = 10;// commit message is considered too short below this number of characters
    const mrCommit = danger.gitlab.commits

    let shortCommitMessages = [];
    for (let i = 0; i < mrCommit.length; i++) {
        const commitMessage = mrCommit[i].message;

        if (commitMessage.length < shortCommitMessageThreshold) {
            shortCommitMessages.push(`- commit message: ${commitMessage}`);
        }
    }

    if (shortCommitMessages.length) {
        warn(`Some of your commit messages may not be sufficiently descriptive (are shorter than ${shortCommitMessageThreshold} characters):
        \n${shortCommitMessages.join("")}
        \nYou might consider squashing commits (simplifying branch history) or updating those short commit messages.`);
    }
}
checkMrCommitMessagesLength();


/**
 * Check if MR is too large (more than 1000 lines of changes)
 *
 * @dangerjs INFO
 */
function checkMrIsTooLarge() {
    const bigMrLinesOfCodeThreshold = 1000

    danger.git.linesOfCode()
        .then((totalLines) => {
            if (totalLines > bigMrLinesOfCodeThreshold) {
                return message(`This MR seems to be quiet large (total lines of code: ${totalLines}), you might consider splitting it into smaller MRs`);
            }
        });
}
checkMrIsTooLarge();


/**
 * Check if documentation needs translation labels
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6855.
 *
 * @dangerjs WARN
 */
function checkMrNeedsTranlation() {
    const mrLabels = danger.gitlab.mr.labels
    const changesInDocsEN = /docs\/en/.test(danger.git.modified_files ? danger.git.modified_files[0] : ''); // Test if changes in directory "docs/EN"
    const changesInDocsCH = /docs\/zh_CN/.test(danger.git.modified_files ? danger.git.modified_files[0] : ''); // Test if changes in directory "docs/CH"

    // Only English docs has been changed
    if (changesInDocsEN && !changesInDocsCH) {
        if (!mrLabels.includes("needs translation: CN")) {
            return warn("The updated documentation will need to be translated into Chinese, please add the MR label `needs translation: CN`");
        }
    }
    // Only Chineese docs has been changed
    if (!changesInDocsEN && changesInDocsCH) {
        if (!mrLabels.includes("needs translation: EN")) {
            return warn("The updated documentation will need to be translated into English, please add the MR label `needs translation: EN`");
        }
    }
}
checkMrNeedsTranlation();

/**
 * Add a link to manual retry a DangerJS job (without committing to the repository)
 *
 * @dangerjs MARKDOWN
 */
function addRetryLink() {
    const retryLink = `${process.env.DANGER_GITLAB_HOST}/${process.env.CI_PROJECT_PATH}/-/jobs/${process.env.CI_JOB_ID}`
    return markdown(`***\n#### :repeat: If you want to run these checks again, please retry this [DangerJS job](${retryLink})\n***`);
}
addRetryLink();

function printSuccessLog() {
    if (results.fails.length === 0 && results.warnings.length === 0 && results.messages.length === 0) {
      return message('Good Job! All checks are passing!')
    }
}
printSuccessLog();
