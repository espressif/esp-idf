const { recordRuleExitStatus } = require("./configParameters.js");

/**
 * Check if MR has not an excessive numbers of commits (if squashed)
 *
 * @dangerjs INFO
 */
module.exports = function () {
	const ruleName = 'Number of commits in merge request';
    const tooManyCommitThreshold = 2; // above this number of commits, squash commits is suggested
    const mrCommits = danger.gitlab.commits;

    if (mrCommits.length > tooManyCommitThreshold) {
        recordRuleExitStatus(ruleName, "Passed (with suggestions)");
        return message(
            `You might consider squashing your ${mrCommits.length} commits (simplifying branch history).`
        );
    }

    // At this point, the rule has passed
	recordRuleExitStatus(ruleName, 'Passed');
};
