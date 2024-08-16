#!/bin/bash

ENGINE_PATH="/Users/heoga-eun/Desktop/EpicRocket/UnrealEngine"
PROJECT="/Users/heoga-eun/Desktop/EpicRocket/SProject/SProject.uproject"

if [ ! -d "$ENGINE_PATH" ]; then
    echo "Engine path not found: $ENGINE_PATH"
    exit 1
fi

if [ ! -f "$PROJECT" ]; then
    echo "Project file not found: $PROJECT"
    exit 1
fi

# Run Build
"$ENGINE_PATH/Engine/Build/BatchFiles/Mac/Build.sh" SProjectEditor Mac Development $PROJECT -waitmutex
