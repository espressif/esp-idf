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
    let project_base_url = DOCUMENTATION_OPTIONS.URL_ROOT + "../..";
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

    old_ver_list = "";

    for (let i = 0; i < versions.length; i++) {
        let v = versions[i];
        let url = getVersionUrl(v);

        if (v.old) {
            old_ver_list += "<li><a href=\"" + url + "\">" + v.name + "</li>";
        }

    }

    $( "#version_list" ).append(old_ver_list);
});
