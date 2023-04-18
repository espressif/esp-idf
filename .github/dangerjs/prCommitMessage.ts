/**
 * Check if commit messages are sufficiently descriptive (not too short).
 *
 * Search for commit messages that appear to be automatically generated or temporary messages and report them.
 *
 * @dangerjs WARN
 */

declare const danger: any;
declare const warn: any;

export default function (): void {
	const commitMessages: string[] = danger.git.commits.map((commit: { message: string }) => commit.message);

	console.log(commitMessages);
	const detectRegexes: RegExp[] = [
		/^Merge pull request #\d+ from .*/i, // Automatically generated message by GitHub
		/^Merged .+:.+ into .+/i, // Automatically generated message by GitHub
		/^Automatic merge by GitHub Action/i, // Automatically generated message by GitHub
		/^Merge branch '.*' of .+ into .+/i, // Automatically generated message by GitHub
		/^Initial commit/i, // Automatically generated message by GitHub
		/^WIP.*/i, // Message starts with prefix "WIP"
		/^Cleaned.*/i, // Message starts "Cleaned"
		/clean ?up/i, // Message contains "clean up"
		/^[^A-Za-z0-9\s].*/, // Message starts with special characters
	];

	let partMessages: string[] = [];

	for (message of commitMessages) {
		if (detectRegexes.some((regex) => message.match(regex))) {
			warn(`The commit message \`${message}\` appears to be a temporary or automatically generated message`);
		}


	// // Search for the messages in each commit
	// let partMessages: string[] = [];
	// for (const commit of prCommits) {
	// 	const commitMessage: string = commit.message;
	// 	const commitMessageTitle: string = commit.title;

	// 	// Check if the commit message matches any regex from "detectRegexes"
	// 	if (detectRegexes.some((regex) => commitMessage.match(regex))) {
	// 		partMessages.push(`- the commit message \`${commitMessageTitle}\` appears to be a temporary or automatically generated message`);
	// 		continue;
	// 	}

	// 	// Check if the commit message is not too short
	// 	const shortCommitMessageThreshold: number = 20; // commit message is considered too short below this number of characters
	// 	if (commitMessage.length < shortCommitMessageThreshold) {
	// 		partMessages.push(`- the commit message \`${commitMessageTitle}\` may not be sufficiently descriptive`);
	// 	}
	// }

	// // Create report
	// if (partMessages.length) {
	// 	partMessages.sort();
	// 	warn(`\nSome issues found for the commit messages in this PR:\n${partMessages.join('\n')}\nPlease consider updating these commit messages. `);
	// }
}
