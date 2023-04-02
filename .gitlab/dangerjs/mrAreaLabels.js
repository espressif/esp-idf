/**
 * Check if MR has area labels (light blue labels)
 *
 * @dangerjs WARN
 */
module.exports = async function () {
    const projectId = 103; // ESP-IDF
    const areaLabelColor = /^#d2ebfa$/i; // match color code (case-insensitive)
    const projectLabels = await danger.gitlab.api.Labels.all(projectId); // Get all project labels
    const areaLabels = projectLabels
        .filter((label) => areaLabelColor.test(label.color))
        .map((label) => label.name); // Filter only area labels
    const mrLabels = danger.gitlab.mr.labels; // Get MR labels

    if (!mrLabels.some((label) => areaLabels.includes(label))) {
        warn(
            `Please add some [area labels](${process.env.DANGER_GITLAB_HOST}/espressif/esp-idf/-/labels) to this MR.`
        );
    }
};
