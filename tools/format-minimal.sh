#!/bin/bash
# Runs astyle with parameters which should be checked in a pre-commit hook
astyle \
	--style=otbs \
	--indent=spaces=4 \
	--convert-tabs \
	--keep-one-line-statements \
	--pad-header \
	"$@"
