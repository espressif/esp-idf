const {
    minimumSummaryChars,
    maximumSummaryChars,
    maximumBodyLineChars,
    allowedTypes,
} = require("./mrCommitsConstants.js");
const { gptStandardModelTokens } = require("./mrCommitsConstants.js");

const { ChatPromptTemplate } = require("langchain/prompts");
const { SystemMessagePromptTemplate } = require("langchain/prompts");
const { LLMChain } = require("langchain/chains");
const { ChatOpenAI } = require("langchain/chat_models/openai");
const openAiTokenCount = require("openai-gpt-token-counter");

module.exports = async function () {
    let outputDangerMessage = `\n\nPerhaps you could use an AI-generated suggestion for your commit message. Here is one `;

    let mrDiff = await getMrGitDiff(danger.git.modified_files);
    const mrCommitMessages = getCommitMessages(danger.gitlab.commits);
    const inputPrompt = getInputPrompt();
    const inputLlmTokens = getInputLlmTokens(
        inputPrompt,
        mrDiff,
        mrCommitMessages
    );
    console.log(`Input tokens for LLM: ${inputLlmTokens}`);

    if (inputLlmTokens >= gptStandardModelTokens) {
        mrDiff = ""; // If the input mrDiff is larger than 16k model, don't use mrDiff, use only current commit messages
        outputDangerMessage += `(based only on your current commit messages, git-diff of this MR is too big (${inputLlmTokens} tokens) for the AI models):\n\n`;
    } else {
        outputDangerMessage += `(based on your MR git-diff and your current commit messages):\n\n`;
    }

    // Generate AI commit message
    let generatedCommitMessage = "";
    try {
        const rawCommitMessage = await createAiGitMessage(
            inputPrompt,
            mrDiff,
            mrCommitMessages
        );
        generatedCommitMessage = postProcessCommitMessage(rawCommitMessage);
    } catch (error) {
        console.error("Error in generating AI commit message: ", error);
        outputDangerMessage +=
            "\nCould not generate commit message due to an error.\n";
    }

    // Append closing statements ("Closes https://github.com/espressif/esp-idf/issues/XXX") to the generated commit message
    let closingStatements = extractClosingStatements(mrCommitMessages);
    if (closingStatements.length > 0) {
        generatedCommitMessage += "\n\n" + closingStatements;
    }

    // Add the generated git message, format to the markdown code block
    outputDangerMessage += `\n\`\`\`\n${generatedCommitMessage}\n\`\`\`\n`;
    outputDangerMessage +=
        "\n**NOTE: AI-generated suggestions may not always be correct, please review the suggestion before using it.**"; // Add disclaimer
    return outputDangerMessage;
};

async function getMrGitDiff(mrModifiedFiles) {
    const fileDiffs = await Promise.all(
        mrModifiedFiles.map((file) => danger.git.diffForFile(file))
    );
    return fileDiffs.map((fileDiff) => fileDiff.diff.trim()).join(" ");
}

function getCommitMessages(mrCommits) {
    return mrCommits.map((commit) => commit.message);
}

function getInputPrompt() {
    return `You are a helpful assistant that creates suggestions for single git commit message, that user can use to describe all the changes in their merge request.
    Use git diff: {mrDiff} and users current commit messages: {mrCommitMessages} to get the changes made in the commit.

    Output should be git commit message following the conventional commit format.

    Output only git commit message in desired format, without comments and other text.

    Do not include the closing statements ("Closes https://....") in the output.

    Here are the strict rules you must follow:

    - Avoid mentioning any JIRA tickets (e.g., "Closes JIRA-123").
    - Be specific. Don't use vague terms (e.g., "some checks", "add new ones", "few changes").
    - The commit message structure should be:  <type><(scope/component)>: <summary>
    - Types allowed: ${allowedTypes.join(", ")}
    - If 'scope/component' is used, it must start with a lowercase letter.
    - The 'summary' must NOT end with a period.
    - The 'summary' must be between ${minimumSummaryChars} and ${maximumSummaryChars} characters long.

    If a 'body' of commit message is used:

    - Each line must be no longer than ${maximumBodyLineChars} characters.
    - It must be separated from the 'summary' by a blank line.

    Examples of correct commit messages:

        - With scope and body:
            fix(freertos): Fix startup timeout issue

            This is a text of commit message body...
            - adds support for wifi6
            - adds validations for logging script

        - Without scope and body:
            ci: added target test job for ESP32-Wifi6`;
}

function getInputLlmTokens(inputPrompt, mrDiff, mrCommitMessages) {
    const mrCommitMessagesTokens = openAiTokenCount(mrCommitMessages.join(" "));
    const gitDiffTokens = openAiTokenCount(mrDiff);
    const promptTokens = openAiTokenCount(inputPrompt);
    return mrCommitMessagesTokens + gitDiffTokens + promptTokens;
}

async function createAiGitMessage(inputPrompt, mrDiff, mrCommitMessages) {
    const chat = new ChatOpenAI({ engine: "gpt-3.5-turbo", temperature: 0 });
    const chatPrompt = ChatPromptTemplate.fromPromptMessages([
        SystemMessagePromptTemplate.fromTemplate(inputPrompt),
    ]);
    const chain = new LLMChain({ prompt: chatPrompt, llm: chat });

    const response = await chain.call({
        mrDiff: mrDiff,
        mrCommitMessages: mrCommitMessages,
    });
    return response.text;
}

function postProcessCommitMessage(rawCommitMessage) {
    // Split the result into lines
    let lines = rawCommitMessage.split("\n");

    // Format each line
    for (let i = 0; i < lines.length; i++) {
        let line = lines[i].trim();

        // If the line is longer than maximumBodyLineChars, split it into multiple lines
        if (line.length > maximumBodyLineChars) {
            let newLines = [];
            while (line.length > maximumBodyLineChars) {
                let lastSpaceIndex = line.lastIndexOf(
                    " ",
                    maximumBodyLineChars
                );
                newLines.push(line.substring(0, lastSpaceIndex));
                line = line.substring(lastSpaceIndex + 1);
            }
            newLines.push(line);
            lines[i] = newLines.join("\n");
        }
    }

    // Join the lines back into a single string with a newline between each one
    return lines.join("\n");
}

function extractClosingStatements(mrCommitMessages) {
    let closingStatements = [];
    mrCommitMessages.forEach((message) => {
        const lines = message.split("\n");
        lines.forEach((line) => {
            if (line.startsWith("Closes")) {
                closingStatements.push(line);
            }
        });
    });
    return closingStatements.join("\n");
}
