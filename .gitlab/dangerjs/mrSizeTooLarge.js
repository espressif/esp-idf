const { recordRuleExitStatus } = require("./configParameters.js");

/**
 * Check if MR is too large (more than 1000 lines of changes)
 *
 * @dangerjs INFO
 */
module.exports = async function () {
    const ruleName = "Merge request size (number of changed lines)";
    const bigMrLinesOfCodeThreshold = 1000;
    const totalLines = await danger.git.linesOfCode();

    if (totalLines > bigMrLinesOfCodeThreshold) {
        recordRuleExitStatus(ruleName, "Passed (with suggestions)");
        return message(
            `This MR seems to be quite large (total lines of code: ${totalLines}), you might consider splitting it into smaller MRs`
        );
    }

    // At this point, the rule has passed
    recordRuleExitStatus(ruleName, "Passed");
};
