const { recordRuleExitStatus } = require("./configParameters.js");

/**
 * Throw Danger WARN if branch name contains more than one slash or uppercase letters
 *
 * @dangerjs INFO
 */
module.exports = function () {
    const ruleName = "Source branch name";
    const sourceBranch = danger.gitlab.mr.source_branch;

    // Check if the source branch name contains more than one slash
    const slashCount = (sourceBranch.match(/\//g) || []).length;
    if (slashCount > 1) {
        recordRuleExitStatus(ruleName, "Failed");
        return warn(
            `The source branch name \`${sourceBranch}\` contains more than one slash. This can cause troubles with git sync. Please rename the branch.`
        );
    }

    // Check if the source branch name contains any uppercase letters
    if (sourceBranch !== sourceBranch.toLowerCase()) {
        recordRuleExitStatus(ruleName, "Failed");
        return warn(
            `The source branch name \`${sourceBranch}\` contains uppercase letters. This can cause troubles on case-insensitive file systems (macOS). Please use only lowercase letters.`
        );
    }

    // At this point, the rule has passed
    recordRuleExitStatus(ruleName, "Passed");
};
