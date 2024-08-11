#!/bin/sh
# Copyright Epic Games, Inc. All Rights Reserved.
#
# Simple wrapper around GenerateProjectFiles.sh using the
# .command extension enables it to be run from the OSX Finder.

sh "$(dirname "$0")"/GenerateProjectFiles.sh "$@"

sh /Users/heoga-eun/Desktop/EpicRocket/UnrealEngine/Engine/Build/BatchFiles/Mac/Build.sh -projectfiles -project="/Users/heoga-eun/Desktop/EpicRocket/SProject/SProject.uproject" -game -engine -progress -Mac -Development -waitmutex
