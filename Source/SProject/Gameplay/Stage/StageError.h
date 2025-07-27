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

		// 마지막 웨이브라서 다음 웨이브를 시작 할 수 없습니다.
		static const FString LAST_WAVE = TEXT("LAST_WAVE");

		// 다음 웨이브 정보가 없어 시작을 할 수 없습니다.
		static const FString NO_NEXT_WAVE = TEXT("NO_NEXT_WAVE");
	}
}
