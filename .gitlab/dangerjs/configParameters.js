let outputStatuses = [];

/**
 * Logs the status of a rule with padded formatting and stores it in the `outputStatuses` array.
 * If the rule already exists in the array, its status is updated.
 * @param message The name of the rule
 * @param status The output (exit) status of the rule
 */
function recordRuleExitStatus(message, status) {
    // Check if the rule already exists in the array
    const existingRecord = outputStatuses.find(
        (rule) => rule.message === message
    );

    if (existingRecord) {
        // Update the status of the existing rule
        existingRecord.status = status;
    } else {
        // If the rule doesn't exist, add it to the array
        outputStatuses.push({ message, status });
    }
}

/**
 * Displays all the rule output statuses stored in the `outputStatuses` array.
 * Filters out any empty lines, sorts them alphabetically, and prints the statuses
 * with a header and separator.
 * These statuses are later displayed in CI job tracelog.
 */
function displayAllOutputStatuses() {
    const lineLength = 100;
    const sortedStatuses = outputStatuses.sort((a, b) =>
        a.message.localeCompare(b.message)
    );

    const formattedLines = sortedStatuses.map((statusObj) => {
        const paddingLength =
            lineLength - statusObj.message.length - statusObj.status.length;
        const paddedMessage = statusObj.message.padEnd(
            statusObj.message.length + paddingLength,
            "."
        );
        return `${paddedMessage} ${statusObj.status}`;
    });

    console.log(
        "DangerJS checks (rules) output states:\n" + "=".repeat(lineLength + 2)
    );
    console.log(formattedLines.join("\n"));
    console.log("=".repeat(lineLength + 2));
}

module.exports = {
    displayAllOutputStatuses,
    recordRuleExitStatus,
};
