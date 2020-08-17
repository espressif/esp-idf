import subprocess


# Get revision used for constructing github URLs
def get_github_rev():
    path = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip().decode('utf-8')
    try:
        tag = subprocess.check_output(['git', 'describe', '--exact-match']).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        tag = None
    print('Git commit ID: ', path)
    if tag:
        print('Git tag: ', tag)
        return tag
    return path
