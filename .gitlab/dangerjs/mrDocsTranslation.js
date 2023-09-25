const { recordRuleExitStatus } = require("./configParameters.js");

/**
 * Check the documentation files in this MR.
 *
 * Generate an object with all docs/ files found in this MR with paths to their EN/CN versions.
 *
 * For common files (both language versions exist in this MR), compare the lines of both files.
 * Ignore if the CN file is only a single line file with an "include" reference to the EN version.
 *
 * For files that only have a CN version in this MR, add a message to the message that an EN file also needs to be created.
 *
 * For a file that only has an EN version in this MR, try loading its CN version from the target Gitlab branch.
 * If its CN version doesn't exist in the repository or it does exist,
 * but its contents are larger than just an "include" link to the EN version (it's a full-size file),
 * add a message to the report
 *
 * Create a compiled report with the docs/ files issues found and set its severity (WARN/INFO).
 * Severity is based on the presence of "needs translation: ??" labels in this MR
 *
 * @dangerjs WARN (if docs translation issues in the MR)
 * @dangerjs INFO (if docs translation issues in the MR and the user has already added translation labels).
 * Adding translation labels "needs translation: XX" automatically notifies the Documentation team
 *
 * @dangerjs WARN (if there are no docs issues in MR, but translation labels have been added anyway)
 *
 */
module.exports = async function () {
    const ruleName = 'Documentation translation';
    let partMessages = []; // Create a blank field for future records of individual issues
    const pathProject = "espressif/esp-idf";
    const regexIncludeLink = /\.\.\sinclude::\s((\.\.\/)+)en\//;
    const allMrFiles = [
        ...danger.git.modified_files,
        ...danger.git.created_files,
        ...danger.git.deleted_files,
    ];

    const docsFilesMR = parseMrDocsFiles(allMrFiles); // Create single object of all doc files in MR with names, paths and groups

    // Both versions (EN and CN) of document found changed in this MR
    for (const file of docsFilesMR.bothFilesInMr) {
        file.contentEn = await getContentFileInMR(file.fileEnPath); // Get content of English file
        file.linesEn = file.contentEn.split("\n").length; // Get number of lines of English file

        file.contentCn = await getContentFileInMR(file.fileCnPath); // Get content of Chinese file
        file.linesCn = file.contentCn.split("\n").length; // Get number of lines of English file

        // Compare number of lines in both versions
        if (file.linesEn !== file.linesCn) {
            // Check if CN file is only link to EN file
            if (!regexIncludeLink.test(file.contentCn)) {
                // if not just a link ...
                partMessages.push(
                    `- please synchronize the EN and CN version of \`${file.fileName}\`. [\`${file.fileEnPath}\`](${file.fileUrlRepoEN}) has ${file.linesEn} lines; [\`${file.fileCnPath}\`](${file.fileUrlRepoCN}) has ${file.linesCn} lines.`
                );
            }
        }
    }

    // Only Chinese version of document found changed in this MR
    for (const file of docsFilesMR.onlyCnFilesInMr) {
        partMessages.push(
            `- file \`${file.fileEnPath}\` doesn't exist in this MR or in the GitLab repo. Please add \`${file.fileEnPath}\` into this MR.`
        );
    }

    // Only English version of document found in this MR
    for (const file of docsFilesMR.onlyEnFilesInMr) {
        const targetBranch = danger.gitlab.mr.target_branch;
        file.contentCn = await getContentFileInGitlab(
            file.fileCnPath,
            targetBranch
        ); // Try to fetch CN file from target branch of Gitlab repository and store content

        if (file.contentCn) {
            // File found on target branch in Gitlab repository
            if (!regexIncludeLink.test(file.contentCn)) {
                // File on Gitlab master is NOT just an ..include:: link to ENG version
                file.fileUrlRepoMasterCN = `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${targetBranch}/${file.fileCnPath}`;
                partMessages.push(
                    `- file \`${file.fileCnPath}\` was not updated in this MR, but found unchanged full document (not just link to EN) in target branch of Gitlab repository [\`${file.fileCnPath}\`](${file.fileUrlRepoMasterCN}). Please update \`${file.fileCnPath}\` into this MR.`
                );
            }
        } else {
            // File failed to fetch, probably does not exist in the target branch
            partMessages.push(
                `- file \`${file.fileCnPath}\` probably doesn't exist in this MR or in the GitLab repo. Please add \`${file.fileCnPath}\` into this MR.`
            );
        }
    }

    // Create a report with found issues with documents in MR
    createReport();

    // At this point, the rule has passed
	recordRuleExitStatus(ruleName, 'Passed');

    /**
     * Generates an object that represents the relationships between files in two different languages found in this MR.
     *
     * @param {string[]} docsFilesEN - An array of file paths for documents in English.
     * @param {string[]} docsFilesCN - An array of file paths for documents in Chinese.
     * @returns {Object} An object with the following properties:
     * - bothFilesInMr: An array of objects that represent files that found in MR in both languages. Each object has the following properties:
     *   - fileName: The name of the file.
     *   - fileEnPath: The path to the file in English.
     *   - fileCnPath: The path to the file in Chinese.
     *   - fileUrlRepoEN: The URL link to MR branch path to the file in English.
     *   - fileUrlRepoCN: The URL link to MR branch path to the file in Chinese.
     * - onlyCnFilesInMr: An array of objects that represent files that only found in MR in English. Each object has the following properties:
     *   - fileName: The name of the file.
     *   - fileEnPath: The path to the file in English.
     * 	 - fileCnPath: The FUTURE path to the file in Chinese.
     *   - fileUrlRepoEN: The URL link to MR branch path to the file in English.
     *   - fileUrlRepoCN: The URL link to MR branch path to the file in Chinese.
     * - onlyEnFilesInMr: An array of objects that represent files that only found in MR in Chinese. Each object has the following properties:
     *   - fileName: The name of the file.
     *   - fileEnPath: The FUTURE path to the file in English.
     *   - fileCnPath: The path to the file in Chinese.
     *   - fileUrlRepoEN: The URL link to MR branch path to the file in English.
     *   - fileUrlRepoCN: The URL link to MR branch path to the file in Chinese.
     */
    function parseMrDocsFiles(allMrFiles) {
        const path = require("path");
        const mrBranch = danger.gitlab.mr.source_branch;

        const docsEnFilesMrPath = allMrFiles.filter((file) =>
            file.startsWith("docs/en")
        ); // Filter all English doc files in MR
        const docsCnFilesMrPath = allMrFiles.filter((file) =>
            file.startsWith("docs/zh_CN")
        ); // Filter all Chinese doc files in MR

        const docsEnFileNames = docsEnFilesMrPath.map((filePath) =>
            path.basename(filePath)
        ); // Get (base) file names for English docs
        const docsCnFileNames = docsCnFilesMrPath.map((filePath) =>
            path.basename(filePath)
        ); // Get (base) file names for Chinese docs

        const bothFileNames = docsEnFileNames.filter((fileName) =>
            docsCnFileNames.includes(fileName)
        ); // Get file names that are common to both English and Chinese docs
        const onlyEnFileNames = docsEnFileNames.filter(
            (fileName) => !docsCnFileNames.includes(fileName)
        ); // Get file names that are only present in English version
        const onlyCnFileNames = docsCnFileNames.filter(
            (fileName) => !docsEnFileNames.includes(fileName)
        ); // Get file names that are only present in Chinese version

        return {
            bothFilesInMr: bothFileNames.map((fileName) => {
                const fileEnPath =
                    docsEnFilesMrPath[docsEnFileNames.indexOf(fileName)];
                const fileCnPath =
                    docsCnFilesMrPath[docsCnFileNames.indexOf(fileName)];

                return {
                    fileName,
                    fileEnPath,
                    fileCnPath,
                    fileUrlRepoEN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileEnPath}`,
                    fileUrlRepoCN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileCnPath}`,
                };
            }),
            onlyEnFilesInMr: onlyEnFileNames.map((fileName) => {
                const fileEnPath =
                    docsEnFilesMrPath[docsEnFileNames.indexOf(fileName)];
                const fileCnPath = fileEnPath.replace("en", "zh_CN"); // Generate future CN file path

                return {
                    fileName,
                    fileEnPath,
                    fileCnPath,
                    fileUrlRepoEN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileEnPath}`,
                    fileUrlRepoCN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileCnPath}`,
                };
            }),
            onlyCnFilesInMr: onlyCnFileNames.map((fileName) => {
                const fileCnPath =
                    docsCnFilesMrPath[docsCnFileNames.indexOf(fileName)];
                const fileEnPath = fileCnPath.replace("zh_CN", "en"); // Generate future EN file path

                return {
                    fileName,
                    fileEnPath,
                    fileCnPath,
                    fileUrlRepoEN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileEnPath}`,
                    fileUrlRepoCN: `${process.env.DANGER_GITLAB_HOST}/${pathProject}/-/blob/${mrBranch}/${fileCnPath}`,
                };
            }),
        };
    }

    /**
     * Retrieves the contents of a file from GitLab using the GitLab API.
     *
     * @param {string} filePath - The path of the file to retrieve.
     * @param {string} branch - The branch where the file is located.
     * @returns {string|null} - The contents of the file, with any trailing new lines trimmed, or null if the file cannot be retrieved.
     */
    async function getContentFileInGitlab(filePath, branch) {
        const axios = require("axios");

        const encFilePath = encodeURIComponent(filePath);
        const encBranch = encodeURIComponent(branch);
        const urlApi = `${process.env.DANGER_GITLAB_API_BASE_URL}/projects/${danger.gitlab.mr.project_id}/repository/files/${encFilePath}/raw?ref=${encBranch}`;

        try {
            const response = await axios.get(urlApi, {
                headers: {
                    "Private-Token": process.env.DANGER_GITLAB_API_TOKEN,
                },
            });
            return response.data.trim(); // Trim trailing  new line
        } catch (error) {
            return null;
        }
    }

    /**
     * Retrieves the contents of a file in a DangerJS merge request object.
     *
     * @param {string} filePath - The path of the file to retrieve.
     * @returns {string|null} - The contents of the file, with any trailing new lines trimmed, or null if the file cannot be retrieved.
     */
    async function getContentFileInMR(filePath) {
        try {
            const content = await danger.git.diffForFile(filePath);
            const fileContentAfter = content.after.trim(); // Trim trailing new lines
            return fileContentAfter;
        } catch (error) {
            console.error(`Error while getting file content MR: ${error}`);
            return null;
        }
    }

    /**
     * Creates a compiled report for found documentation issues in the current MR and alerts the Documentation team if there are any "needs translation" labels present.
     *
     * Report if documentation labels have been added by mistake.
     */
    function createReport() {
        const mrLabels = danger.gitlab.mr.labels; // Get MR labels
        const regexTranslationLabel = /needs translation:/i;

        const translationLabelsPresent = mrLabels.some((label) =>
            regexTranslationLabel.test(label)
        ); // Check if any of MR labels are "needs translation: XX"

        // No docs issues found in MR, but translation labels have been added anyway
        if (!partMessages.length && translationLabelsPresent) {
            recordRuleExitStatus(ruleName, "Failed");
            return warn(
                `Please remove the \`needs translation: XX\` labels. For documents that need to translate from scratch, Doc team will translate them in the future. For the current stage, we only focus on updating exiting EN and CN translation to make them in sync.`
            );
        }

        // Docs issues found in this MR
        partMessages.sort();
        let dangerMessage = `Some of the documentation files in this MR seem to have translations issues:\n${partMessages.join(
            "\n"
        )}\n`;

        if (partMessages.length) {
            if (!translationLabelsPresent) {
                dangerMessage += `
				\nWhen synchronizing the EN and CN versions, please follow the [Documentation Code](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/contribute/documenting-code.html#standardize-document-format). The total number of lines of EN and CN should be same.\n
				\nIf you have difficulty in providing translation, you can contact Documentation team by adding <kbd>needs translation: CN</kbd> or <kbd>needs translation: EN</kbd> labels into this MR and retrying Danger CI job. The documentation team will be automatically notified and will help you with the translations before the merge.\n`;
                recordRuleExitStatus(ruleName, "Failed");
                return warn(dangerMessage); // no "needs translation: XX" labels in MR; report issues as warn
            } else {
				dangerMessage += `\nTranslation labels <kbd>needs translation: CN</kbd> or <kbd>needs translation: EN</kbd> were added - this will automatically notify the Documentation team to help you with translation issues.`;
                recordRuleExitStatus(ruleName, 'Passed (with suggestions)');
                return message(dangerMessage); // "needs translation: XX" labels were found in MR and Docs team was notified; report issues as info
            }
        }
    }
};
