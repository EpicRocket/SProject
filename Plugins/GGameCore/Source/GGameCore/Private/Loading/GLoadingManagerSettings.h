
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/SoftObjectPath.h"

#include "GLoadingManagerSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class GGAMECORE_API UGLoadingManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGLoadingManagerSettings();

public:
	UPROPERTY(Config, EditDefaultsOnly, Category = "로딩", meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;

	UPROPERTY(Config, EditDefaultsOnly, Category = "로딩")
	int32 LoadingScreenZOrder = 10000;

	UPROPERTY(Config, EditDefaultsOnly, Category = "환경설정", meta = (ForceUnits = s, ConsoleVariable = "GLoading.HoldLoadingScreenAdditionalSecs", Tooltip = "다른 로딩이 완료된 후 로딩 화면을 얼마나 오래 유지할지 (초 단위)  텍스처 스트리밍에 흐릿함을 피할 기회를 줍니다."))
	float HoldLoadingScreenAdditionalSecs = 2.0F;

	UPROPERTY(Config, EditDefaultsOnly, Category = "환경설정", meta = (ForceUnits = s, Tooltip = "로딩 화면이 영구적으로 중단된 것으로 간주되는 간격(초)입니다(0이 아닌 경우)."))
	float LoadingScreenHeartbeatHangDuration = 0.0F;

	UPROPERTY(Config, EditDefaultsOnly, Category = "환경설정", meta = (ForceUnits = s, Tooltip = "로딩 화면을 계속 표시하는 각 로그 사이의 간격(초)입니다(0이 아닌 경우)."))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	UPROPERTY(config, EditDefaultsOnly, Category = "환경설정", meta = (Tooltip = "에디터에서도 추가 HoldLoadingScreenAdditionalSecs 지연을 적용할 것인가?"))
	bool ForceTickLoadingScreenEvenInEditor = true;

	UPROPERTY(Transient, EditDefaultsOnly, Category = "디버그", meta = (ConsoleVariable = "GLoading.LogLoadingScreenReasonEveryFrame", Tooltip = "참이면 로딩 화면이 표시되거나 숨겨지는 이유가 매 프레임마다 로그에 출력됩니다."))
	bool LogLoadingScreenReasonEveryFrame = 0;

	UPROPERTY(Transient, EditDefaultsOnly, Category = "디버그", meta = (ConsoleVariable = "GLoading.AlwaysShow", Tooltip = "로딩 화면 강제 표시"))
	bool ForceLoadingScreenVisible = false;

	UPROPERTY(Transient, EditDefaultsOnly, Category = "디버그", meta = (Tooltip = "에디터에서도 추가 HoldLoadingScreenAdditionalSecs 지연을 적용할 것인가?"))
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;
};
