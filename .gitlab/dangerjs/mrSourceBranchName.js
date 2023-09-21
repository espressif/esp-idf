/**
 * Throw Danger WARN if branch name contains more than one slash or uppercase letters
 *
 * @dangerjs INFO
 */
module.exports = function () {
    const sourceBranch = danger.gitlab.mr.source_branch;

    // Check if the source branch name contains more than one slash
    const slashCount = (sourceBranch.match(/\//g) || []).length;
    if (slashCount > 1) {
        return message(
            `The source branch name \`${sourceBranch}\` contains more than one slash. This can cause troubles with git sync. Please rename the branch.`
        );
    }

    // Check if the source branch name contains any uppercase letters
    if (sourceBranch !== sourceBranch.toLowerCase()) {
        return message(
            `The source branch name \`${sourceBranch}\` contains uppercase letters. This can cause troubles on case-insensitive file systems (macOS). Please use only lowercase letters.`,
        );
    }
};
