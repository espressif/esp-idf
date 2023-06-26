import { DangerDSLType, DangerResults } from "danger";
declare const danger: DangerDSLType;
declare const warn: (message: string, results?: DangerResults) => void;

interface Commit {
    message: string;
}

/**
 * Check if commit messages are sufficiently descriptive (not too short).
 *
 * Search for commit messages that appear to be automatically generated or temporary messages and report them.
 *
 * @dangerjs WARN
 */
export default function (): void {
    const prCommits: Commit[] = danger.git.commits;

    const detectRegexes: RegExp[] = [
        /^Merge pull request #\d+ from .*/i, // Automatically generated message by GitHub
        /^Merged .+:.+ into .+/i, // Automatically generated message by GitHub
        /^Automatic merge by GitHub Action/i, // Automatically generated message by GitHub
        /^Merge branch '.*' of .+ into .+/i, // Automatically generated message by GitHub
        /^Create\s[a-zA-Z0-9_.-]+(\.[a-zA-Z0-9]{1,4})?(?=\s|$)/, // Automatically generated message by GitHub using UI
        /^Delete\s[a-zA-Z0-9_.-]+(\.[a-zA-Z0-9]{1,4})?(?=\s|$)/, // Automatically generated message by GitHub using UI
        /^Update\s[a-zA-Z0-9_.-]+(\.[a-zA-Z0-9]{1,4})?(?=\s|$)/, // Automatically generated message by GitHub using UI
        /^Initial commit/i, // Automatically generated message by GitHub
        /^WIP.*/i, // Message starts with prefix "WIP"
        /^Cleaned.*/i, // Message starts "Cleaned", , probably temporary
        /^Test:.*/i, // Message starts with "test" prefix, probably temporary
        /clean ?up/i, // Message contains "clean up", probably temporary
        /^[^A-Za-z0-9\s].*/, // Message starts with special characters
    ];

    let partMessages: string[] = [];

    for (const commit of prCommits) {
        const commitMessage: string = commit.message;
        const commitMessageTitle: string = commit.message.split("\n")[0];

        // Check if the commit message matches any regex from "detectRegexes"
        if (detectRegexes.some((regex) => commitMessage.match(regex))) {
            partMessages.push(
                `- the commit message \`${commitMessageTitle}\` appears to be a temporary or automatically generated message`
            );
            continue;
        }

        // Check if the commit message is not too short
        const shortCommitMessageThreshold: number = 20; // commit message is considered too short below this number of characters
        if (commitMessage.length < shortCommitMessageThreshold) {
            partMessages.push(
                `- the commit message \`${commitMessageTitle}\` may not be sufficiently descriptive`
            );
        }
    }

    // Create report
    if (partMessages.length) {
        partMessages.sort();
        let dangerMessage = `\nSome issues found for the commit messages in this MR:\n${partMessages.join(
            "\n"
        )}
		\nPlease consider updating these commit messages.`;
        warn(dangerMessage);
    }
}
