const { OpenAI } = require("langchain/llms/openai");
const {
    ChatPromptTemplate,
    SystemMessagePromptTemplate,
} = require("langchain/prompts");
const { LLMChain } = require("langchain/chains");
const { ChatOpenAI } = require("langchain/chat_models/openai");
const openAiTokenCount = require("openai-gpt-token-counter");

const mrModifiedFiles = danger.git.modified_files;
const mrCommits = danger.gitlab.commits;

module.exports = async function () {
    let mrDiff = await getMrGitDiff(mrModifiedFiles);
    const mrCommitMessages = getCommitMessages(mrCommits);

    // Init output message
    let outputDangerMessage = `\n\nPerhaps you could use an AI-generated suggestion for your commit message. Here is one `;

    // Setup LLM prompt
    const inputPrompt = `You are a helpful assistant that creates suggestions for single git commit message, that user can use to describe all the changes in their merge request.
        Use git diff: {mrDiff} and users current commit messages: {mrCommitMessages} to get the changes made in the commit.

        Output should be git commit message following the conventional commit format.

        Output only git commit message in desired format, without comments and other text.

        Do not include lines with JIRA tickets mentions (e.g. "Closes JIRA-123") to the output.

        Avoid including temporary commit messages (e.g. "Cleanup", "Merged" or "wip: Test") to the output.

        Avoid using vague terms (e.g. "some checks", "add new ones", "few changes" ) in the output.


        [EXAMPLE OUTPUT]
            feat(scope): add support for component XXX

            - adds support for wifi6
            - adds validations for logging script


        [EXAMPLE OUTPUT]
            feat(scope): add support for component XXX

            - adds support for wifi6
            - adds validations for logging script
            - Closes https://github.com/espressif/esp-idf/issues/1234

    `;

    // Count input tokens for LLM
    const mrCommitMessagesTokens = openAiTokenCount(mrCommitMessages.join(" "));
    const gitDiffTokens = openAiTokenCount(mrDiff);
    const promptTokens = openAiTokenCount(inputPrompt);

    const inputLlmTokens =
        mrCommitMessagesTokens + gitDiffTokens + promptTokens;

    console.log(`Input tokens for LLM: ${inputLlmTokens}`);

    if (inputLlmTokens < 4096) {
        outputDangerMessage += `(based on your MR git-diff and your current commit messages):\n\n`;
    } else {
        outputDangerMessage += `(based only on your current commit messages, git-diff of this MR is too big (${inputLlmTokens} tokens) for the AI model):\n\n`;
        mrDiff = "";
    }

    // Call LLM
    const generatedCommitMessage = await createAiGitMessage(
        inputPrompt,
        mrDiff,
        mrCommitMessages
    );

    outputDangerMessage += "```\n" + generatedCommitMessage + "\n```\n"; // Add the generated git message, format to the markdown code block
    outputDangerMessage += "\n**NOTE: AI-generated suggestions may not always be correct, please review the suggestion before using it.**"  // Add disclaimer
    return outputDangerMessage;
};

async function getMrGitDiff(mrModifiedFiles) {
    let mrDiff = "";
    for (const file of mrModifiedFiles) {
        const fileDiff = await danger.git.diffForFile(file);
        mrDiff += fileDiff.diff.trim();
    }

    return mrDiff;
}

function getCommitMessages(mrCommits) {
    let mrCommitMessages = [];
    for (const commit of mrCommits) {
        mrCommitMessages.push(commit.message);
    }

    return mrCommitMessages;
}

async function createAiGitMessage(inputPrompt, mrDiff, mrCommitMessages) {
    const chat = new ChatOpenAI({
        engine: "gpt-3.5-turbo",
        temperature: 0,
    });

    const chatPrompt = ChatPromptTemplate.fromPromptMessages([
        SystemMessagePromptTemplate.fromTemplate(inputPrompt),
    ]);

    const chain = new LLMChain({
        prompt: chatPrompt,
        llm: chat,
    });

    const response = await chain.call({
        mrDiff: mrDiff,
        mrCommitMessages: mrCommitMessages,
    });

    return response.text;
}
