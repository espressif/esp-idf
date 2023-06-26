/**
 * Check if MR has not an excessive numbers of commits (if squashed)
 *
 * @dangerjs INFO
 */
module.exports = function () {
    const tooManyCommitThreshold = 2; // above this number of commits, squash commits is suggested
    const mrCommits = danger.gitlab.commits;

    if (mrCommits.length > tooManyCommitThreshold) {
        return message(
            `You might consider squashing your ${mrCommits.length} commits (simplifying branch history).`
        );
    }
};
