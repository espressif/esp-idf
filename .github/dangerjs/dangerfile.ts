declare const results: any;
declare const message: any;
declare const markdown: any;

import prCommitsTooManyCommits from './prCommitsTooManyCommits';
import prDescription from './prDescription';
import prTargetBranch from './prTargetBranch';
import prFirstTimeContributor from './prFirstTimeContributor';
import prCommitMessage from './prCommitMessage';

async function runDangerRules() {
	// await prFirstTimeContributor();
	// prCommitsTooManyCommits();
	// prDescription();
	// prTargetBranch();
	// prCommitMessage();

	// Add success log if no issues
	const dangerFails: number = results.fails.length;
	const dangerWarns: number = results.warnings.length;
	const dangerInfos: number = results.messages.length;
	if (!dangerFails && !dangerWarns && !dangerInfos) {
		return message('Good Job! All checks are passing!');
	}
}

runDangerRules();

// Add merge and review process description
const mergeProcessDescription: string = `
#### Pull request review and merge process you can expect
The main development of \`ESP-IDF\` project is done on Espressif Gitlab (internal). The GitHub repository is only a public mirror of Gitlab.

After you open the Pull Request, there will probably be some discussion in the comments field of the request itself.

The GitHub sync. bot will automatically creates a Jira ticket with your Pull request in Espressif's internal project management system and assign it to the responsible Espressif engineer.

Once the issue is assigned, the assignee evaluates the contribution, tests the changes to the Pull request in internal system, and reviews it with other Espressif engineers.

Once the Pull Request is ready to merge, it will first be merged into our internal git system (Gitlab) for in-house automated testing.

If this process passes, it will be merged onto the public github repository.

You can read more about this process in [Contributions Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/index.html#contributions-guide)
`;
markdown(mergeProcessDescription);

// Add retry link
const serverUrl: string | undefined = process.env.GITHUB_SERVER_URL;
const repoName: string | undefined = process.env.GITHUB_REPOSITORY;
const runId: string | undefined = process.env.GITHUB_RUN_ID;

const retryLinkUrl: string = `${serverUrl}/${repoName}/actions/runs/${runId}`;
const retryLink: string = `***\n#### :repeat: You can enforce automatic PR checks by retrying the [DangerJS action](${retryLinkUrl})\n***`;

markdown(retryLink);
