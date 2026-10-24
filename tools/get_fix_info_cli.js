function get_redirect_uri() {
    var currentUrl = window.location.href;
    return currentUrl.substring(0, currentUrl.lastIndexOf("/")) + "/login.html";
}

function on_login() {
    GitlabLogin.login_jump('get_fix_info.html', get_redirect_uri());
}

function loginFailureCallback(response) {
    console.error(response);
    document.getElementById("message").innerHTML = "Error logging in, please retry. Server response: " + JSON.stringify(response);
    return Promise.reject("quit");
}

async function gitlab_login() {
    const redirect_uri = get_redirect_uri();

    const  gitlab = await Gitlab.login(redirect_uri);

    const avatar_url = await gitlab.get_avatar();
    document.getElementById("avatar").src = avatar_url;

    const user_name = await gitlab.get_name();
    document.getElementById("message").innerHTML = "Welcome back " + user_name;

    return gitlab;
}

function failureCallback(response) {
    console.error(response);
    if (response == "quit") {
        return Promise.reject("quit");
    }
    document.getElementById("message").innerHTML = "Error: " + JSON.stringify(response);
}

function getQueryVariable(variable) {
    var query = window.location.search.substring(1);
    var vars = query.split("&");
    for (var i=0;i<vars.length;i++) {
            var pair = vars[i].split("=");
            if(pair[0] == variable){return pair[1];}
    }
    return(false);
}

async function retrieve_all_mr_info(gitlab, main_mr_iid) {
    const main_mr = await gitlab.get_mr(main_mr_iid);

    const tags = await gitlab.get_tags("v");
    const discussions = await gitlab.get_mr_discussions(main_mr_iid);
    const mentioned = get_mr_mentioned(discussions);

    const promise_array = mentioned.map((main_mr_iid) => gitlab.get_mr(main_mr_iid));
    const mr_array = await Promise.all(promise_array);
    mr_array.push(main_mr);

    const all_mr_info = mr_array.map((mr) => get_mr_info(mr, tags, main_mr_iid));
    update_result(main_mr, all_mr_info);
}

function get_mr_mentioned(discussions) {
    function is_mentioned_notes(discussion) {
        var notes = discussion.notes;
        return (notes.length == 1 && notes[0]["system"] && notes[0]["body"].match(/mentioned in merge request !/));
    }

    var valid_discussions = discussions.filter(is_mentioned_notes);
    return valid_discussions.map((discussion) => {
        return discussion.notes[0]["body"].replace("mentioned in merge request !", "")
    });
}

/*--- Extract information ---*/
function get_available_ver(mr, tags) {
    var target_ver = mr["target_branch"].replace("release/", "");

    var earliest_ver_str = null;
    var na_ver_str = null;

    if (mr["state"] == "merged") {
        var mr_date = Gitlab.toDate(mr["merged_at"]);
        var earliest_ver = null;
        var na_ver = null;

        if (target_ver == "master") {
            for (let i = 0; i < tags.length; i++) {
                if (tags[i]["name"].search("-dev") <= 0) {
                    continue;
                }
                var tag_date = Gitlab.toDate(tags[i]["commit"]["committed_date"]);
                if (tag_date > mr_date) {
                    earliest_ver = tags[i];
                } else {
                    na_ver = tags[i];
                    break;
                }
            }
            //For master MRs, a MR merged between v4.3-dev and v4.4-dev should be treated as successfully merged into v4.3
            if (na_ver) {
                earliest_ver_str = na_ver["name"].replace("-dev", "");
            }
            //Can't easily get previous version, leaving it null
        } else {
            for (let i = 0; i < tags.length; i++) {
                if (tags[i]["name"].search(target_ver) != 0) {
                    continue;
                }
                var tag_date = Gitlab.toDate(tags[i]["commit"]["committed_date"]);
                if (tag_date > mr_date) {
                    earliest_ver = tags[i];
                } else {
                    na_ver = tags[i];
                    break;
                }
            }
            if (earliest_ver) {
                earliest_ver_str = earliest_ver["name"];
            }
            if (na_ver) {
                na_ver_str = na_ver["name"];
            }
        }
    } else {
        for (let i = 0; i < tags.length; i++) {
            if (tags[i]["name"].search(target_ver) != 0) {
                continue;
            }
            na_ver_str = tags[i]["name"];
            break;
        }
    }

    return {
        "earliest_ver": earliest_ver_str,
        "na_ver": na_ver_str
    }
}

function get_github_link(sha) {
    var ret = null;
    if (sha) {
        ret = '<a href="https://github.com/espressif/esp-idf/commit/' + sha + '">' +
                sha.substr(0, 7) + "</a>"
    }
    return ret
}

function find_backport_mrs(mr_desc) {
    const lines = mr_desc.split('\n');
    // search lines starting from `backport`
    const backportRegex = /^\s*backport/i;
    // matching `!12345` or `merge_requests/12345`
    const mrRegex = /(![0-9]+|merge_requests\/([0-9]+))/gi;

    const results = [];
    for (let line of lines) {
        const backportIndex = line.toLowerCase().indexOf('backport');
        if (backportIndex !== -1) {
            // search after the backport string
            line = line.slice(backportIndex + 'backport'.length);

            let match;
            while ((match = mrRegex.exec(line)) !== null) {
                let mr_iid = null;
                if (match[1].startsWith('!')) {
                    results.push(`${match[1].slice(1)}`);
                } else if (match[1].startsWith('merge_requests/')) {
                    results.push(`${match[2]}`);
                }
            }
        }
    }
    return results;
}

function get_mr_info(mr, tags, main_mr_iid) {
    let pattern_backport = new RegExp(`backport.*(?<!\\w)${main_mr_iid}(?!\\w)`, 'i');
    let is_backport = mr["description"].search(pattern_backport) >= 0;

    let is_release_branch = mr["target_branch"].search(/release\/v\d+\.\d+/) == 0;

    let match = mr["iid"] == main_mr_iid || (is_release_branch && is_backport);

    versions = get_available_ver(mr, tags)
    return {
        "target_branch": mr["target_branch"],
        "gh_sha_link": get_github_link(mr["merge_commit_sha"]),
        "available": versions["earliest_ver"],
        "na": versions["na_ver"],
        "mr_url": '<a href="' + mr["web_url"] + '">' + "(!" + mr["iid"] + ")</a>",
        "mr_title": mr["title"],
        "is_backport": match,
    };
}

/*--- Display ---*/
function get_table_title(public_only) {
    var str = "<tr><td>Target Branch</td>";
    str += "<td>Merged</td>";
    str += "<td>Released</td>";
    str += "<td>Last N.A.</td>";
    if (!public_only) {
        str += "<td>MR URL</td>";
        str += "<td>MR title</td></tr>";
    }
    return str;
}

function get_table_line(mr_info, public_only) {
    var ret = "<tr><td>" + mr_info["target_branch"] + "</td>";
    ret += "<td>" + mr_info["gh_sha_link"] + "</td>";
    ret += "<td>" + mr_info["available"] + "</td>";
    ret += "<td>" + mr_info["na"] + "</td>";
    if (!public_only) {
        ret += "<td>" + mr_info["mr_url"] + "</td>";
        ret += "<td>" + mr_info["mr_title"] + "</td></tr>";
    }
    return ret;
}

function get_table(mr_list, public_only) {
    var result = "<table border=1>";
    result += get_table_title(public_only);

    mr_list.forEach((mr) => {
        result += get_table_line(mr, public_only);
    });
    result += "</table>";
    return result;
}

function get_backport_summary(main_mr_obj) {
    var result = "<hr>";
    result += `<p>(!${main_mr_obj.iid}) ${main_mr_obj.title}</p>`;

    result += "<p>Needs backport: ";
    main_mr_obj.labels.forEach((label) => {
        const found = label.search("needs backport");
        if (found >= 0) {
            result += label.replace("needs backport ", "v");
            result += ", ";
        }
    });
    result += "</p>";

    if (main_mr_obj["target_branch"] != "master") {
        const backport_mrs = find_backport_mrs(main_mr_obj["description"]);
        const url_prefix = "/scripts/get_fix_info.html?mr=";
        result += "<p>Backports (possibly): " + backport_mrs.map((iid) => `<a href="${url_prefix}${iid}">!${iid}</a>`).join(', ') + "</p>";
    }
    return result;
}

function update_result(main_mr_obj, all_mr_info) {
    const output = document.getElementById("output")

    var result = get_backport_summary(main_mr_obj);

    const correct_mrs = all_mr_info.filter((mr) => mr["is_backport"]);
    const incorrect_mrs = all_mr_info.filter((mr) => !mr["is_backport"]);

    result += "<h2>Public version</h2>"
    result += get_table(correct_mrs, true);

    result += "<h2>Internal version</h2>"
    result += get_table(correct_mrs, false);

    result += "<h2>Other MRs mentioned in the original MR</h2>"
    result += "<h3>public</h2>"
    result += get_table(incorrect_mrs, true);
    result += "<h3>internal</h2>"
    result += get_table(incorrect_mrs, false);


    output.innerHTML = result;
}

async function main() {
    const gitlab = await gitlab_login()
                            .catch(loginFailureCallback);

    const mr_iid = getQueryVariable("mr");
    if (mr_iid) {
        document.getElementById("mr_iid").value = mr_iid;
        retrieve_all_mr_info(gitlab, mr_iid);
    }
}

function load() {
    const node = document.createElement("div");
    node.id = "message";
    node.className = "banner_item message"
    node.innerHTML = "You are not logged in";

    document.getElementById("banner").appendChild(node);

    /* Give a default avatar if not logged in */
    document.getElementById("avatar").src = "/scripts/imgs/default_avatar.svg";

    main()
        .catch(failureCallback);
}
window.addEventListener("load", load, false);
