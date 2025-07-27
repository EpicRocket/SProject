// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"

namespace GameErr
{
	namespace Stage
	{
		// 건물 건설 불가
		static const FString CANNOT_BUILD_TOWER = TEXT("CANNOT_BUILD_TOWER");

		// 웨이브가 이미 진행 중
		static const FString WAVE_IS_PLAYING = TEXT("WAVE_IS_PLAYING");

		// 모든 웨이브를 클리어 함
		static const FString ALL_CLEAR_WAVE = TEXT("ALL_CLEAR_WAVE");
	}
}
