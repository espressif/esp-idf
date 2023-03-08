/**
 * Check if documentation needs translation labels
 *
 * #TODO: this simple logic will be improved in future MRs - Jira IDF-6855.
 *
 * @dangerjs WARN
 */
module.exports = function () {
    const mrLabels = danger.gitlab.mr.labels;
    const changesInDocsEN = /docs\/en/.test(
        danger.git.modified_files ? danger.git.modified_files[0] : ""
    ); // Test if changes in directory "docs/EN"
    const changesInDocsCH = /docs\/zh_CN/.test(
        danger.git.modified_files ? danger.git.modified_files[0] : ""
    ); // Test if changes in directory "docs/CH"

    // Only English docs has been changed
    if (changesInDocsEN && !changesInDocsCH) {
        if (!mrLabels.includes("needs translation: CN")) {
            return warn(
                "The updated documentation will need to be translated into Chinese, please add the MR label `needs translation: CN`"
            );
        }
    }
    // Only Chineese docs has been changed
    if (!changesInDocsEN && changesInDocsCH) {
        if (!mrLabels.includes("needs translation: EN")) {
            return warn(
                "The updated documentation will need to be translated into English, please add the MR label `needs translation: EN`"
            );
        }
    }
};
