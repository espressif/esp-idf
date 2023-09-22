const {
    minimumSummaryChars,
    maximumSummaryChars,
    maximumBodyLineChars,
    allowedTypes,
} = require("./mrCommitsConstants.js");
const { recordRuleExitStatus } = require("./configParameters.js");

/**
 * Check that commit messages are based on the Espressif ESP-IDF project's rules for git commit messages.
 *
 * @dangerjs WARN
 */
module.exports = async function () {
    const ruleName = "Commit messages style";
    const mrCommits = danger.gitlab.commits;
    const lint = require("@commitlint/lint").default;

    const lintingRules = {
        // rule definition: [(0-1 = off/on), (always/never = must be/mustn't be), (value)]
        "body-max-line-length": [1, "always", maximumBodyLineChars], // Max length of the body line
        "footer-leading-blank": [1, "always"], // Always have a blank line before the footer section
        "footer-max-line-length": [1, "always", maximumBodyLineChars], // Max length of the footer line
        "subject-max-length": [1, "always", maximumSummaryChars], // Max length of the "Summary"
        "subject-min-length": [1, "always", minimumSummaryChars], // Min length of the "Summary"
        "scope-case": [1, "always", "lower-case"], // "scope/component" must start with lower-case
        "subject-full-stop": [1, "never", "."], // "Summary" must not end with a full stop (period)
        "subject-empty": [1, "never"], // "Summary" is mandatory
        "type-case": [1, "always", "lower-case"], // "type/action" must start with lower-case
        "type-empty": [1, "never"], // "type/action" is mandatory
        "type-enum": [1, "always", allowedTypes], // "type/action" must be one of the allowed types
        "body-leading-blank": [1, "always"], // Always have a blank line before the body section
    };

    // Switcher for AI suggestions (for poor messages)
    let generateAISuggestion = false;

    // Search for the messages in each commit
    let issuesAllCommitMessages = [];

    for (const commit of mrCommits) {
        const commitMessage = commit.message;
        const commitMessageTitle = commit.title;

        let issuesSingleCommitMessage = [];
        let reportSingleCommitMessage = "";

        // Check if the commit message contains any Jira ticket references
        const jiraTicketRegex = /[A-Z0-9]+-[0-9]+/g;
        const jiraTicketMatches = commitMessage.match(jiraTicketRegex);
        if (jiraTicketMatches) {
            const jiraTicketNames = jiraTicketMatches.join(", ");
            issuesSingleCommitMessage.push(
                `- probably contains Jira ticket reference (\`${jiraTicketNames}\`). Please remove Jira tickets from commit messages.`
            );
        }

        // Lint commit messages with @commitlint (Conventional Commits style)
        const result = await lint(commit.message, lintingRules);

        for (const warning of result.warnings) {
            // Custom messages for each rule with terminology used by Espressif conventional commits guide
            switch (warning.name) {
                case "subject-max-length":
                    issuesSingleCommitMessage.push(
                        `- *summary* appears to be too long`
                    );
                    break;
                case "type-empty":
                    issuesSingleCommitMessage.push(
                        `- *type/action* looks empty`
                    );
                    break;
                case "type-case":
                    issuesSingleCommitMessage.push(
                        `- *type/action* should start with a lowercase letter`
                    );

                    break;
                case "scope-empty":
                    issuesSingleCommitMessage.push(
                        `- *scope/component* looks empty`
                    );
                    break;
                case "scope-case":
                    issuesSingleCommitMessage.push(
                        `- *scope/component* should be lowercase without whitespace, allowed special characters are \`_\` \`/\` \`.\` \`,\` \`*\` \`-\` \`.\``
                    );
                    break;
                case "subject-empty":
                    issuesSingleCommitMessage.push(`- *summary* looks empty`);
                    generateAISuggestion = true;
                    break;
                case "subject-min-length":
                    issuesSingleCommitMessage.push(
                        `- *summary* looks too short`
                    );
                    generateAISuggestion = true;
                    break;
                case "subject-case":
                    issuesSingleCommitMessage.push(
                        `- *summary* should start with a capital letter`
                    );
                    break;
                case "subject-full-stop":
                    issuesSingleCommitMessage.push(
                        `- *summary* should not end with a period (full stop)`
                    );
                    break;
                case "type-enum":
                    issuesSingleCommitMessage.push(
                        `- *type/action* should be one of [${allowedTypes
                            .map((type) => `\`${type}\``)
                            .join(", ")}]`
                    );
                    break;

                default:
                    issuesSingleCommitMessage.push(`- ${warning.message}`);
            }
        }

        if (issuesSingleCommitMessage.length) {
            reportSingleCommitMessage = `- the commit message \`"${commitMessageTitle}"\`:\n${issuesSingleCommitMessage
                .map((message) => `  ${message}`) // Indent each issue by 2 spaces
                .join("\n")}`;
            issuesAllCommitMessages.push(reportSingleCommitMessage);
        }
    }

    // Create report
    if (issuesAllCommitMessages.length) {
        issuesAllCommitMessages.sort();
        const basicTips = [
            `- correct format of commit message should be: \`<type/action>(<scope/component>): <summary>\`, for example \`fix(esp32): Fixed startup timeout issue\``,
            `- allowed types are: \`${allowedTypes}\``,
            `- sufficiently descriptive message summary should be between ${minimumSummaryChars} to ${maximumSummaryChars} characters and start with upper case letter`,
            `- avoid Jira references in commit messages (unavailable/irrelevant for our customers)`,
            `- follow this [commit messages guide](${process.env.DANGER_GITLAB_HOST}/espressif/esp-idf/-/wikis/dev-proc/Commit-messages)`,
        ];
        let dangerMessage = `\n**Some issues found for the commit messages in this MR:**\n${issuesAllCommitMessages.join(
            "\n"
        )}
			\n***
			\n**Please consider updating these commit messages** - here are some basic tips:\n${basicTips.join(
                "\n"
            )}
            \n \`TIP:\` You can install commit-msg pre-commit hook (\`pre-commit install -t pre-commit -t commit-msg\`) to run this check when committing.
            \n***
            `;

        if (generateAISuggestion) {
            // Create AI generated suggestion for git commit message based of gitDiff and current commit messages
            const AImessageSuggestion =
                await require("./aiGenerateGitMessage.js")();
            dangerMessage += AImessageSuggestion;
        }

        recordRuleExitStatus(ruleName, "Failed");
        return warn(dangerMessage);
    }

    // At this point, the rule has passed
    recordRuleExitStatus(ruleName, "Passed");
};
