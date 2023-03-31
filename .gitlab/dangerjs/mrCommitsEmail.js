/**
 * Check if the author is accidentally making a commit using a personal email
 *
 * @dangerjs INFO
 */
module.exports = function () {
    const mrCommitAuthorEmails = danger.gitlab.commits.map(commit => commit.author_email);
    const mrCommitCommitterEmails = danger.gitlab.commits.map(commit => commit.committer_email);
	const emailPattern = /.*@espressif\.com/;
	const filteredEmails = [...mrCommitAuthorEmails, ...mrCommitCommitterEmails].filter((email) => !emailPattern.test(email));
	if (filteredEmails.length) {
		return message(
			`Some of the commits were authored or committed by developers outside Espressif: ${filteredEmails.join(', ')}. Please check if this is expected.`
		);
	}
};
