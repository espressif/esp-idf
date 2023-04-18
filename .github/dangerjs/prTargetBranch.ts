/**
 * Check if the target branch is `master`
 *
 * @dangerjs FAIL
 */

declare const danger: any;
declare const fail: any;

export default function (): void {
	const prTargetBranch: string = danger.github?.pr?.base?.ref;

	if (prTargetBranch !== 'master') {
		return fail('The target branch for this pull request should be `master`.');
	}
}
