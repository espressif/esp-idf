/**
 * Check if MR is too large (more than 1000 lines of changes)
 *
 * @dangerjs INFO
 */
module.exports = async function () {
    const bigMrLinesOfCodeThreshold = 1000;
    const totalLines = await danger.git.linesOfCode();

    if (totalLines > bigMrLinesOfCodeThreshold) {
        return message(
            `This MR seems to be quite large (total lines of code: ${totalLines}), you might consider splitting it into smaller MRs`
        );
    }
};
