/*
 * Modules with checks are stored in ".gitlab/dangerjs/<module_name>". To import them, use path relative to "dangerfile.js"
 */

async function runChecks() {
    // Checks for merge request title
    require("./mrTitleNoDraftOrWip.js")();

    // Checks for merge request description
    require("./mrDescriptionLongEnough.js")();
    require("./mrDescriptionHasReleaseNotes.js")();
    await require("./mrDescriptionJiraLinks.js")();

    // Checks for documentation
    require("./mrDocsTranslation.js")();

    // Checks for MR commits
    require("./mrCommitsTooManyCommits.js")();
    require("./mrCommitsCommitMessage.js")();
    require("./mrCommitsEmail.js")();

    // Checks for MR code
    require("./mrSizeTooLarge.js")();

    // Checks for MR area labels
    await require("./mrAreaLabels.js")();

    // Add success log if no issues
    if (
        results.fails.length === 0 &&
        results.warnings.length === 0 &&
        results.messages.length === 0
    ) {
        return message("Good Job! All checks are passing!");
    }
}

runChecks();

// Add retry link
const retryLink = `${process.env.DANGER_GITLAB_HOST}/${process.env.CI_PROJECT_PATH}/-/jobs/${process.env.CI_JOB_ID}`;
markdown(
    `***\n#### :repeat: You can enforce automatic MR checks by retrying the [DangerJS job](${retryLink})\n***`
);
