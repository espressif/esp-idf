import { DangerResults } from "danger";
declare const results: DangerResults;
declare const message: (message: string, results?: DangerResults) => void;
declare const markdown: (message: string, results?: DangerResults) => void;

// Import modules with danger rules
// (Modules with checks are stored in ".github/dangerjs/<module_name>.ts". To import them, use path relative to "dangerfile.ts")
import prCommitsTooManyCommits from "./prCommitsTooManyCommits";
import prDescription from "./prDescription";
import prTargetBranch from "./prTargetBranch";
import prInfoContributor from "./prInfoContributor";
import prCommitMessage from "./prCommitMessage";

async function runDangerRules(): Promise<void> {
    // Message to contributor about review and merge process
    const prInfoContributorMessage: string = await prInfoContributor();
    markdown(prInfoContributorMessage);

    // Run danger checks
    prCommitsTooManyCommits();
    prDescription();
    prTargetBranch();
    prCommitMessage();

    // Add success log if no issues
    const dangerFails: number = results.fails.length;
    const dangerWarns: number = results.warnings.length;
    const dangerInfos: number = results.messages.length;
    if (!dangerFails && !dangerWarns && !dangerInfos) {
        return message("Good Job! All checks are passing!");
    }

    // Add retry link
    addRetryLink();
}

runDangerRules();

function addRetryLink(): void {
    const serverUrl: string | undefined = process.env.GITHUB_SERVER_URL;
    const repoName: string | undefined = process.env.GITHUB_REPOSITORY;
    const runId: string | undefined = process.env.GITHUB_RUN_ID;

    const retryLinkUrl: string = `${serverUrl}/${repoName}/actions/runs/${runId}`;
    const retryLink: string = `<sub>:repeat: You can re-run automatic PR checks by retrying the <a href="${retryLinkUrl}">DangerJS action</a></sub>`;

    markdown(retryLink);
}
