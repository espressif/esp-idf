# 'cmake -E' doesn't have a way to fail outright, so run this script
# with 'cmake -P' to fail a build.
message(FATAL_ERROR "$ENV{FAIL_MESSAGE}")
