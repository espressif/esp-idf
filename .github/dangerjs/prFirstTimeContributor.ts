/**
 * Check if author of pull request is first time contributor
 *
 * @dangerjs INFO
 */

declare const danger: any;
declare const message: any;

interface Contributor {
	login: string;
}

export default async function (): Promise<void> {
	const authorLogin = danger.github.pr.user.login;

	const contributors = await danger.github.api.repos.listContributors({
		owner: 'espressif', // TODO: for test
		repo: 'esp-idf', // TODO: for test
		// owner: danger.github.thisPR.owner,
		// repo: danger.github.thisPR.repo,
	});

	const contributorsData: Contributor[] = contributors.data;
	const logins = contributorsData.map((contributor: Contributor) => contributor.login);

	if (!logins.includes(authorLogin)) {
		message(`👋 Welcome *${authorLogin}*! Thank you for your first contribution to \`espressif/esp-idf\` project!`);
	}
}
