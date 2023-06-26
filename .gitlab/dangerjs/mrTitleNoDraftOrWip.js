/**
 * Check if MR Title contains prefix "WIP: ...".
 *
 * @dangerjs WARN
 */
module.exports = function () {
	const mrTitle = danger.gitlab.mr.title;
    const regexes = [
        { prefix: 'WIP', regex: /^WIP:/i },
        { prefix: 'W.I.P', regex: /^W\.I\.P/i },
        { prefix: '[WIP]', regex: /^\[WIP/i },
        { prefix: '[W.I.P]', regex: /^\[W\.I\.P/i },
        { prefix: '(WIP)', regex: /^\(WIP/i },
        { prefix: '(W.I.P)', regex: /^\(W\.I\.P/i },
    ];

	for (const item of regexes) {
		if (item.regex.test(mrTitle)) {
			return warn(`Please remove the \`${item.prefix}\` prefix from the MR name before merging this MR.`);
		}
	}
};
