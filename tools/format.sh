#!/bin/bash
# Runs astyle with the full set of formatting options
astyle \
	--style=otbs \
	--attach-namespaces \
	--attach-classes \
	--indent=spaces=4 \
	--convert-tabs \
	--align-pointer=name \
	--align-reference=name \
	--keep-one-line-statements \
	--pad-header \
	--pad-oper \
	"$@"
