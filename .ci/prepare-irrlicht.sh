#!/bin/bash
set -x
set -o errexit

source .ci/prepare-repo
prepare_repo "https://github.com/jwyxym/irrlicht.git" "irrlicht"
