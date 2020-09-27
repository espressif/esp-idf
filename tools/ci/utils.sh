# Modified from https://gitlab.com/gitlab-org/gitlab/-/blob/master/scripts/utils.sh

# before each job, we need to check if this job is filtered by bot stage/job filter
function apply_bot_filter() {
  python "${IDF_PATH}"/tools/ci/apply_bot_filter.py || exit 0
}

function add_ssh_keys() {
  local key_string="${1}"
  mkdir -p ~/.ssh
  chmod 700 ~/.ssh
  echo -n "${key_string}" >~/.ssh/id_rsa_base64
  base64 --decode --ignore-garbage ~/.ssh/id_rsa_base64 >~/.ssh/id_rsa
  chmod 600 ~/.ssh/id_rsa
}

function add_gitlab_ssh_keys() {
  add_ssh_keys "${GITLAB_KEY}"
  echo -e "Host gitlab.espressif.cn\n\tStrictHostKeyChecking no\n" >>~/.ssh/config

  # For gitlab geo nodes
  if [ "${LOCAL_GITLAB_SSH_SERVER:-}" ]; then
    SRV=${LOCAL_GITLAB_SSH_SERVER##*@} # remove the chars before @, which is the account
    SRV=${SRV%%:*}                     # remove the chars after :, which is the port
    printf "Host %s\n\tStrictHostKeyChecking no\n" "${SRV}" >>~/.ssh/config
  fi
}

function add_github_ssh_keys() {
  add_ssh_keys "${GH_PUSH_KEY}"
  echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >>~/.ssh/config
}

function add_doc_server_ssh_keys() {
  local key_string="${1}"
  local server_url="${2}"
  local server_user="${3}"
  add_ssh_keys "${key_string}"
  echo -e "Host ${server_url}\n\tStrictHostKeyChecking no\n\tUser ${server_user}\n" >>~/.ssh/config
}

function fetch_submodules() {
  python "${SUBMODULE_FETCH_TOOL}" -s "${SUBMODULES_TO_FETCH}"
}

function get_all_submodules() {
  echo "$(git config --file .gitmodules --get-regexp path | awk '{ print $2 }' | sed -e 's|$|/**|' | xargs | sed -e 's/ /,/g')"
}

function set_component_ut_vars() {
  local exclude_list_fp="${IDF_PATH}/tools/ci/component_ut_excludes.txt"
  export COMPONENT_UT_DIRS=$(find components/ -name test_apps -type d)
  export COMPONENT_UT_EXCLUDES=$([ -r $exclude_list_fp ] && cat $exclude_list_fp | xargs)
  echo "COMPONENT_UT_DIRS, COMPONENT_UT_EXCLUDES written into export"
}
