# Modified from https://gitlab.com/gitlab-org/gitlab/-/blob/master/scripts/utils.sh

function add_ssh_keys() {
  local key_string="${1}"
  mkdir -p ~/.ssh
  chmod 700 ~/.ssh
  echo -n "${key_string}" >~/.ssh/id_rsa_base64
  # Detect base64 implementation via --help output
  if base64 --help 2>&1 | grep -q -- '--ignore-garbage'; then
    # GNU coreutils base64
    base64 --decode --ignore-garbage ~/.ssh/id_rsa_base64 >~/.ssh/id_rsa
  else
    # macOS/BSD base64 - requires stdin or -i flag
    base64 --decode -i ~/.ssh/id_rsa_base64 -o ~/.ssh/id_rsa
  fi
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
  section_start "fetch_submodules" "Fetching submodules..."
  python "${SUBMODULE_FETCH_TOOL}" -s "${SUBMODULES_TO_FETCH}"
  section_end "fetch_submodules"
}

function get_all_submodules() {
  git config --file .gitmodules --get-regexp path | awk '{ print $2 }' | sed -e 's|$|/**|' | xargs | sed -e 's/ /,/g'
}

function set_component_ut_vars() {
  local exclude_list_fp="${IDF_PATH}/tools/ci/component_ut_excludes.txt"
  export COMPONENT_UT_DIRS=$(find components/ -name test_apps -type d | xargs)
  export COMPONENT_UT_EXCLUDES=$([ -r $exclude_list_fp ] && cat $exclude_list_fp | xargs)
  echo "exported variables COMPONENT_UT_DIRS, COMPONENT_UT_EXCLUDES"
}

# https://docs.gitlab.com/ee/ci/yaml/script.html#use-a-script-to-improve-display-of-collapsible-sections
function section_start() {
  local section_title="${1}"
  local section_description="${2:-$section_title}"

  echo -e "section_start:`date +%s`:${section_title}[collapsed=true]\r\e[0K${section_description}"
}
function section_end() {
  local section_title="${1}"

  echo -e "section_end:`date +%s`:${section_title}\r\e[0K"
}

function error() {
  printf "\033[0;31m%s\n\033[0m" "${1}" >&2
}

function info() {
  printf "\033[0;32m%s\n\033[0m" "${1}" >&2
}

function warning() {
  printf "\033[0;33m%s\n\033[0m" "${1}" >&2
}

function run_cmd() {
  local cmd="$*"
  local start=$(date +%s)

  info "\$ ${cmd}"
  eval "${cmd}"

  local ret=$?
  local end=$(date +%s)
  local runtime=$((end-start))

  if [[ $ret -eq 0 ]]; then
    info "==> '\$ ${cmd}' succeeded in ${runtime} seconds."
    return 0
  else
    error "==> '\$ ${cmd}' failed (${ret}) in ${runtime} seconds."
    return $ret
  fi
}

function pytest_for_ut() {
  pytest_args="-c NUL -p no:idf-ci -p no:pytest_embedded"

  if [ -n "${CI_JOB_ID-}" ]; then
    if [ -z "${KNOWN_FAILURE_CASES_FILE_NAME-}" ]; then
      echo "Error: KNOWN_FAILURE_CASES_FILE_NAME is not set."
      return 1
    fi

    pytest_args="${pytest_args} --junitxml XUNIT_RESULT_${CI_JOB_ID}.xml --ignore-result-files ${KNOWN_FAILURE_CASES_FILE_NAME}"

    if [ ! -e "${KNOWN_FAILURE_CASES_FILE_NAME}" ]; then
      run_cmd idf-ci gitlab download-known-failure-cases-file "${KNOWN_FAILURE_CASES_FILE_NAME}"
    fi
  fi

  run_cmd pytest "$@" "${pytest_args}"
}

# Retries a command RETRY_ATTEMPTS times in case of failure
# Inspired by https://stackoverflow.com/a/8351489
function retry_failed() {
  local max_attempts=${RETRY_ATTEMPTS-3}
  local timeout=${RETRY_TIMEWAIT-1}
  local attempt=1
  local exitCode=0

  whole_start=$(date +%s)
  while true; do
    if run_cmd "$@"; then
      exitCode=0
      break
    else
      exitCode=$?
    fi

    if ((attempt >= max_attempts)); then
      break
    fi

    error "Retrying in ${timeout} seconds..."
    sleep $timeout
    attempt=$((attempt + 1))
    timeout=$((timeout * 2))
  done

  local duration=$(($(date '+%s') - whole_start))
  if [[ $exitCode != 0 ]]; then
    error "Totally failed! Spent $duration sec in total"
  else
    info "Done! Spent $duration sec in total"
  fi
  return $exitCode
}

function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

function is_based_on_commits() {
  # This function would accept space-separated args as multiple commits.
  # The return value would be 0 if current HEAD is based on any of the specified commits.
  #
  # In our CI, we use environment variable $REQUIRED_ANCESTOR_COMMITS to declare the ancestor commits.
  # Please remember to set one commit for each release branch.

  commits=$*
  if [[ -z $commits ]]; then
    info "Not specifying commits that branches should be based on, skipping check..."
    return 0
  fi

  commits_str="$(join_by " or " $commits)" # no doublequotes here, passing array

  info "Checking if current branch is based on $commits_str..."
  for i in $commits; do
    if git merge-base --is-ancestor "$i" HEAD >/dev/null 2>&1; then
      info "Current branch is based on $i"
      return 0
    else
      info "Current branch is not based on $i"
    fi
  done

  error "The base commit of your branch is too old."
  error "The branch should be more recent than either of the following commits:"
  error "  $commits_str"
  error "To fix the issue:"
  error " - If your merge request is 'Draft', or has conflicts with the target branch, rebase it to the latest master or release branch"
  error " - Otherwise, simply run a new pipeline."

  return 1
}
