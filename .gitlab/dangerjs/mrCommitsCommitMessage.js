/**
 * Check commit message are descriptive enough (longer that 10 characters)
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6856.
 *
 * @dangerjs WARN
 */
module.exports = function () {
    const shortCommitMessageThreshold = 10; // commit message is considered too short below this number of characters
    const mrCommit = danger.gitlab.commits;

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
};
