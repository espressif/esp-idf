/**
 * Check if MR Description contains mandatory section "Release notes"
 *
 * Extracts the content of the "Release notes" section from the GitLab merge request description.
 *
 * @dangerjs WARN (if section missing, is empty or wrong markdown format)
 */
module.exports = function () {
    const mrDescription = danger.gitlab.mr.description;
    const regexSectionReleaseNotes = /## Release notes([\s\S]*?)(?=## |$)/;
    const sectionReleaseNotes = mrDescription.match(regexSectionReleaseNotes);

    if (!sectionReleaseNotes) {
        warn(
            'The `Release Notes` section seems to be missing. Please check if the section header in MR description is present and in the correct markdown format ("## Release Notes")\n'
        );
        return null;
    }

    let content = sectionReleaseNotes[1].replace(/(\r\n|\n|\r)/gm, "").trim(); // Remove empty lines and whitespace

    if (!content.length) {
        warn(
            "The `Release Notes` section seems to be empty (no section content)\n"
        );
        return null;
    }
};
