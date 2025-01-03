﻿#!/bin/bash

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

"$ENGINE_PATH"/Engine/Binaries/Mac/UnrealEditor "$PROJECT"
