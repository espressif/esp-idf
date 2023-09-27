#!/usr/bin/env bash
set -euo pipefail

if [ ! -x "$(command -v astyle_py)" ]; then
	echo "astyle_py not found, please install astyle_py:"
	echo "  pip install -U astyle_py==VERSION"
	echo "where VERSION is the same as in .pre-commit-config.yaml."
	exit 1
fi

# "--astyle-version" here has to be in sync with pre-commit-config.yaml.
# The rest of the options have to be in sync with the default rules in tools/ci/astyle-rules.yml.
astyle_py --astyle-version=3.4.7 \
	--style=otbs \
	--attach-namespaces \
	--attach-classes \
	--indent=spaces=4 \
	--convert-tabs \
	--align-reference=name \
	--keep-one-line-statements \
	--pad-header \
	--pad-oper \
	--unpad-paren \
	--max-continuation-indent=120 \
	"$@"
