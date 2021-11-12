#!/usr/bin/env bash

# Now git submodules are redirected to mirror automatically according to relative URLs in .gitmodules

echo "WARNING: This script is deprecated. Use standard ways to obtain git submodules"
git submodule update --init
