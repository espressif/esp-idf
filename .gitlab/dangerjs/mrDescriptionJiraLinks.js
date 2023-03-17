/** Check that there are valid JIRA links in MR description.
 *
 * This check extracts the "Related" section from the MR description and
 * searches for JIRA ticket references in the format "Closes [JIRA ticket key]".
 *
 * It then extracts the closing GitHub links from the corresponding JIRA tickets and
 * checks if the linked GitHub issues are still in open state.
 *
 * Finally, it checks if the required GitHub closing links are present in the MR's commit messages.
 *
 */
module.exports = async function () {
    const axios = require("axios");
    const mrDescription = danger.gitlab.mr.description;
    const mrCommitMessages = danger.gitlab.commits.map(
        (commit) => commit.message
    );
    const jiraTicketRegex = /[A-Z0-9]+-[0-9]+/;

    let partMessages = []; // Create a blank field for future records of individual issues

    // Parse section "Related" from MR Description
    const sectionRelated = extractSectionRelated(mrDescription);

    if (
        !sectionRelated.header || // No section Related in MR description or ...
        !jiraTicketRegex.test(sectionRelated.content) // no Jira links in section Related
    ) {
        return message(
            "Please consider adding references to JIRA issues in the `Related` section of the MR description."
        );
    }

    // Get closing (only) JIRA tickets
    const jiraTickets = findClosingJiraTickets(sectionRelated.content);

    for (const ticket of jiraTickets) {
        ticket.jiraUIUrl = `https://jira.espressif.com:8443/browse/${ticket.ticketName}`;

        if (!ticket.correctFormat) {
            partMessages.push(
                `- closing ticket \`${ticket.record}\` seems to be in the wrong format (or inaccessible to Jira DangerBot).. The correct format is for example \`- Closes JIRA-123\`.`
            );
        }

        // Get closing GitHub issue links from JIRA tickets
        const closingGithubLink = await getGitHubClosingLink(ticket.ticketName);
        if (closingGithubLink) {
            ticket.closingGithubLink = closingGithubLink;
        } else if (closingGithubLink === null) {
            partMessages.push(
                `- the Jira issue number [\`${ticket.ticketName}\`](${ticket.jiraUIUrl}) seems to be invalid (please check if the ticket number is correct)`
            );
            continue; // Handle unreachable JIRA tickets; skip the following checks
        } else {
            continue; // Jira ticket have no GitHub closing link; skip the following checks
        }

        // Get still open GitHub issues
        const githubIssueStatusOpen = await isGithubIssueOpen(
            ticket.closingGithubLink
        );
        ticket.isOpen = githubIssueStatusOpen;
        if (githubIssueStatusOpen === null) {
            // Handle unreachable GitHub issues
            partMessages.push(
                `- the GitHub issue [\`${ticket.closingGithubLink}\`](${ticket.closingGithubLink}) does not seem to exist on GitHub (referenced from JIRA ticket [\`${ticket.ticketName}\`](${ticket.jiraUIUrl}) )`
            );
            continue; // skip the following checks
        }

        // Search in commit message if there are all GitHub closing links (from Related section) for still open GH issues
        if (ticket.isOpen) {
            if (
                !mrCommitMessages.some((item) =>
                    item.includes(`Closes ${ticket.closingGithubLink}`)
                )
            ) {
                partMessages.push(
                    `- please add \`Closes ${ticket.closingGithubLink}\` to the commit message`
                );
            }
        }
    }

    // Create report / DangerJS check feedback if issues with Jira links found
    if (partMessages.length) {
        createReport();
    }

    // ---------------------------------------------------------------

    /**
     * This function takes in a string mrDescription which contains a Markdown-formatted text
     * related to a Merge Request (MR) in a GitLab repository. It searches for a section titled "Related"
     * and extracts the content of that section. If the section is not found, it returns an object
     * indicating that the header and content are null. If the section is found but empty, it returns
     * an object indicating that the header is present but the content is null. If the section is found
     * with content, it returns an object indicating that the header is present and the content of the
     * "Related" section.
     *
     * @param {string} mrDescription - The Markdown-formatted text related to the Merge Request.
     * @returns {{
     *  header: string | boolean | null,
     * 	content: string | null
     * 	}} - An object containing the header and content of the "Related" section, if present.
     */

    function extractSectionRelated(mrDescription) {
        const regexSectionRelated = /## Related([\s\S]*?)(?=## |$)/;
        const sectionRelated = mrDescription.match(regexSectionRelated);
        if (!sectionRelated) {
            return { header: null, content: null }; // Section "Related" is missing
        }

        const content = sectionRelated[1].replace(/(\r\n|\n|\r)/gm, ""); // Remove empty lines
        if (!content.length) {
            return { header: true, content: null }; // Section "Related" is present, but empty
        }

        return { header: true, content: sectionRelated[1] }; // Found section "Related" with content
    }

    /**
     * Finds all JIRA tickets that are being closed in the given sectionRelatedcontent.
     * The function searches for lines that start with - Closes and have the format Closes [uppercase letters]-[numbers].
     * @param {string} sectionRelatedcontent - A string that contains lines with mentions of JIRA tickets
     * @returns {Array} An array of objects with ticketName property that has the correct format
     */

    function findClosingJiraTickets(sectionRelatedcontent) {
        let closingTickets = [];
        const lines = sectionRelatedcontent.split("\n");
        for (const line of lines) {
            if (!line.startsWith("- Closes")) {
                continue; // Not closing-type ticket, skip
            }

            const correctJiraClosingLinkFormat = new RegExp(
                `^- Closes ${jiraTicketRegex.source}$`
            );
            const matchedJiraTicket = line.match(jiraTicketRegex);
            if (matchedJiraTicket) {
                if (!correctJiraClosingLinkFormat.test(line)) {
                    closingTickets.push({
                        record: line,
                        ticketName: matchedJiraTicket[0],
                        correctFormat: false,
                    });
                } else {
                    closingTickets.push({
                        record: line,
                        ticketName: matchedJiraTicket[0],
                        correctFormat: true,
                    });
                }
            }
        }
        return closingTickets;
    }

    /**
     * This function takes a JIRA issue key and retrieves the description from JIRA's API.
     * It then searches the description for a GitHub closing link in the format "Closes https://github.com/owner/repo/issues/123".
     * If a GitHub closing link is found, it is returned. If no GitHub closing link is found, it returns null.
     * @param {string} jiraIssueKey - The key of the JIRA issue to search for the GitHub closing link.
     * @returns {Promise<string|null>} - A promise that resolves to a string containing the GitHub closing link if found,
     * or null if not found.
     */
    async function getGitHubClosingLink(jiraIssueKey) {
        let jiraDescription = "";

        // Get JIRA ticket description content
        try {
            const response = await axios({
                url: `https://jira.espressif.com:8443/rest/api/latest/issue/${jiraIssueKey}`,
                auth: {
                    username: process.env.DANGER_JIRA_USER,
                    password: process.env.DANGER_JIRA_PASSWORD,
                },
            });
            jiraDescription = response.data.fields.description
                ? response.data.fields.description
                : ""; // if the Jira ticket has an unfilled Description, the ".description" property is missing in API response - in that case set "jiraDescription" to an empty string
        } catch (error) {
            return null;
        }

        // Find GitHub closing link in description
        const regexClosingGhLink =
            /Closes\s+(https:\/\/github.com\/\S+\/\S+\/issues\/\d+)/;
        const closingGithubLink = jiraDescription.match(regexClosingGhLink);

        if (closingGithubLink) {
            return closingGithubLink[1];
        } else {
            return false; // Jira issue has no GitHub closing link in description
        }
    }

    /**
     * Check if a GitHub issue linked in a merge request is still open.
     *
     * @param {string} link - The link to the GitHub issue.
     * @returns {Promise<boolean>} A promise that resolves to a boolean indicating if the issue is open.
     * @throws {Error} If the link is invalid or if there was an error fetching the issue.
     */
    async function isGithubIssueOpen(link) {
        const parsedUrl = new URL(link);
        const [owner, repo] = parsedUrl.pathname.split("/").slice(1, 3);
        const issueNumber = parsedUrl.pathname.split("/").slice(-1)[0];

        try {
            const response = await axios.get(
                `https://api.github.com/repos/${owner}/${repo}/issues/${issueNumber}`
            );
            return response.data.state === "open"; // return True if GitHub issue is open
        } catch (error) {
            return null; // GET request to issue fails
        }
    }

    function createReport() {
        partMessages.sort();
        let dangerMessage = `Some issues found for the related JIRA tickets in this MR:\n${partMessages.join(
            "\n"
        )}`;
        warn(dangerMessage);
    }
};
