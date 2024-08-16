#!/bin/sh
# Copyright Epic Games, Inc. All Rights Reserved.
#
# Simple wrapper around GenerateProjectFiles.sh using the
# .command extension enables it to be run from the OSX Finder.

set -e

sh "$(dirname "$0")"/GenerateProjectFiles.sh "$@"

sh "$(dirname "$0")"/Batch/Build.sh

sh "$(dirname "$0")"/Batch/Run.sh

exit