import { DangerDSLType, DangerResults } from "danger";
declare const danger: DangerDSLType;
declare const fail: (message: string, results?: DangerResults) => void;

/**
 * Check if the target branch is "master"
 *
 * @dangerjs FAIL
 */
export default function (): void {
    const prTargetBranch: string = danger.github?.pr?.base?.ref;

    if (prTargetBranch !== "master") {
        return fail(`
        The target branch for this pull request should be \`master\`.\n
        If you would like to add this feature to the release branch, please state this in the PR description and we will consider backporting it.
        `);
    }
}
