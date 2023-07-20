$(document).ready(function() {

    let version = DOCUMENTATION_OPTIONS.VERSION;
    let language = DOCUMENTATION_OPTIONS.LANGUAGE;
    let idf_target = DOCUMENTATION_OPTIONS.IDF_TARGET;
    let pagename = DOCUMENTATION_OPTIONS.PAGENAME + ".html";

    let defaults = DOCUMENTATION_VERSIONS.DEFAULTS;
    let versions = DOCUMENTATION_VERSIONS.VERSIONS;

    /* Apply the defaults to each version, where needed */
    for (let i = 0; i < versions.length; i++) {
        let v = versions[i];
        for (let d in defaults) {
            if (!(d in v)) {
                v[d] = defaults[d];
            }
        }
    }

    /* Find the (relative) base URL for this project, finding a sibling URL will be built as follows:
    <project_base_url>/<language>/<version>/<idf_target>

    (Where 'idf_target' path element are optional depending on if the project has multiple target support)
    */
    let project_base_url = DOCUMENTATION_OPTIONS.URL_ROOT + "../../..";
    if (DOCUMENTATION_OPTIONS.IDF_TARGETS) {
        project_base_url += "/..";
    }

    /* Given a version from the list, return the URL to link to it */
    function getVersionUrl(v) {
        let result = project_base_url + "/" + language + "/" + v.name;
        if (v.has_targets) {
            result += "/" + (idf_target || "esp32");
        }

        return result + "/";
    };

    function getReleaseNotesUrl(v){
        if (v.name.indexOf("release") !== 0) {
            let releaseNotesUrl = "https://github.com/espressif/esp-idf/releases/tag/" + v.name;
            return releaseNotesUrl;
        }
    }

    old_ver_table = "";
    old_ver_table_row = "<tr>"
    let row_items = 0;
    const ITEMS_PER_ROW = 3;

    for (let i = 0; i < versions.length; i++) {
        let v = versions[i];
        let url = getVersionUrl(v);
        let releaseNotesUrl = getReleaseNotesUrl(v);

        if (v.old) {
            if (releaseNotesUrl){
                old_ver_table_row += "<td><a href=\"" + url + "\">" + v.name + "&nbsp;-&nbsp;<a href=\"" + releaseNotesUrl + "\">Release Notes</a></td>";
            } else {
                old_ver_table_row += "<td><a href=\"" + url + "\">" + v.name + "</td>";
            }

            row_items++;

            if (row_items === ITEMS_PER_ROW) {
                old_ver_table_row += "</tr>";
                old_ver_table += old_ver_table_row;

                /* Prepare a new row */
                old_ver_table_row = "<tr>"
                row_items = 0
            }
        }
    }
    old_ver_table_row += "</tr>";
    old_ver_table += old_ver_table_row;

    $( "#version_table" ).append(old_ver_table);
});
