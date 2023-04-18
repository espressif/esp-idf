/**
 * Check if pull request has not an excessive numbers of commits (if squashed)
 *
 * @dangerjs INFO
 */

declare const danger: any;
declare const message: any;

export default function (): void {
	const tooManyCommitThreshold: number = 2; // above this number of commits, squash commits is suggested
	const prCommits: number = danger.github.commits.length;

	if (prCommits > tooManyCommitThreshold) {
		return message(`You might consider squashing your ${prCommits} commits (simplifying branch history).`);
	}
}
