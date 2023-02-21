/**
 * Check if MR has not an excessive numbers of commits (if squashed)
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6856.
 *
 * @dangerjs INFO
 */
module.exports = function () {
    const tooManyCommitThreshold = 5; // above this number of commits, squash is recommended
    const mrCommits = danger.gitlab.commits;

    if (mrCommits.length > tooManyCommitThreshold) {
        return message(
            `You might consider squashing your ${mrCommits.length} commits (simplifying branch history).`
        );
    }
};
