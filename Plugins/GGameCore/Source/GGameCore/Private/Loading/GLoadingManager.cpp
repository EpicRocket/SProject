
#include "Loading/GLoadingManager.h"
// Include Engine
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/GameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Images/SThrobber.h"
#include "HAL/ThreadHeartBeat.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "PreLoadScreen.h"
#include "PreLoadScreenManager.h"
#include "ShaderPipelineCache.h"
#include "Animation/WidgetAnimationEvents.h"
// include GGameCore
#include "GLoadingManagerSettings.h"
#include "Loading/Interface/IGLoadingProcess.h"
#include "Loading/UI/GLoadingWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GLoadingManager)

DEFINE_LOG_CATEGORY(LogLoading);
CSV_DEFINE_CATEGORY(Loading, true);

namespace GLoadingCVars
{
	static float HoldLoadingScreenAdditionalSecs = 2.0F;
	static FAutoConsoleVariableRef CVarHoldLoadingScreenUpAtLeastThisLongInSecs(
		TEXT("GLoading.HoldLoadingScreenUpAtLeastThisLongInSecs"),
		HoldLoadingScreenAdditionalSecs,
		TEXT("텍스처 스트리밍이 흐릿해지지 않도록 하기 위해 다른 로딩이 완료된 후 로딩 화면을 얼마나 오래 유지할지(초)"),
		ECVF_Default | ECVF_Preview);

	static bool LogLoadingScreenReasonEveryFrame = false;
	static FAutoConsoleVariableRef CVarLogLoadingScreenReasonEveryFrame(
		TEXT("GLoading.LogLoadingScreenReasonEveryFrame"),
		LogLoadingScreenReasonEveryFrame,
		TEXT("참이면 로딩 화면이 표시되거나 숨겨지는 이유가 매 프레임마다 로그에 출력됩니다."),
		ECVF_Default);

	static bool ForceLoadingScreenVisible = false;
	static FAutoConsoleVariableRef CVarForceLoadingScreenVisible(
		TEXT("GLoading.AlwaysShow"),
		ForceLoadingScreenVisible,
		TEXT("로딩 화면이 강제로 표시되도록 합니다."),
		ECVF_Default);
}

void UGLoadingManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UGLoadingManager::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGLoadingManager::HandlePostLoadMap);
}

void UGLoadingManager::Deinitialize()
{
	// 위젯 삭제
	RemoveLoadingWidgetFromViewport();
	RemoveTransitionWidgetFromViewport();

	// 바인딩 제거
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UGLoadingManager::Tick(float DeltaTime)
{
	UpdateLoadingScreen();

	TimeUntilNextLogHeartbeatSeconds = FMath::Max(TimeUntilNextLogHeartbeatSeconds - DeltaTime, 0.0);
}

bool UGLoadingManager::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId UGLoadingManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGLoadingManager, STATGROUP_Tickables);
}

UWorld* UGLoadingManager::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}

FString UGLoadingManager::GetDebugReasonForShowingOrHidingLoadingScreen() const
{
	return DebugReasonForShowingOrHidingLoadingScreen;
}

bool UGLoadingManager::GetLoadingScreenDisplayStatus() const
{
	return bCurrentlyShowingLoadingScreen;
}

void UGLoadingManager::RegisterLoadingProcessor(TScriptInterface<IGLoadingProcess> Interface)
{
	if (Interface.GetObject() != nullptr)
	{
		ExternalLoadingProcessors.Emplace(Interface.GetObject());
	}
}

void UGLoadingManager::UnregisterLoadingProcessor(TScriptInterface<IGLoadingProcess> Interface)
{
	if (Interface.GetObject() != nullptr)
	{
		ExternalLoadingProcessors.Remove(Interface.GetObject());
	}

}

void UGLoadingManager::BeginTransition(FOnDynamicLoadingWidgetAnimationDelegate BeginEvent, FOnDynamicLoadingWidgetAnimationDelegate EndEvent)
{
	if (bCurrentlyShowingTransitionScreen)
	{
		return;
	}

	auto Settings = GetDefault<UGLoadingManagerSettings>();

	bCurrentlyShowingTransitionScreen = true;

	auto GameInstance = GetGameInstance();

	TSubclassOf<UUserWidget> TransitionScreenWidgetClass = Settings->TransitionScreenWidget.TryLoadClass<UUserWidget>();
	if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*GameInstance, TransitionScreenWidgetClass, NAME_None))
	{
		TransitionScreenWidgetPtr = UserWidget->TakeWidget();
		TransitionScreenWidget = Cast<UGLoadingWidget>(UserWidget);
		if (TransitionScreenWidget.IsValid())
		{
			if (TransitionScreenWidget->IsExistBeginAnimation())
			{
				TransitionScreenWidget->BindToBeginAnimationFinished(
					FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
						this,
						[BeginEvent]() {
							BeginEvent.ExecuteIfBound();
						}
					)
				);
			}

			if (TransitionScreenWidget->IsExistEndAnimation())
			{
				TransitionScreenWidget->BindToEndAnimationFinished(
					FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
						this,
						[CreatedWidget = TWeakPtr<SWidget>(UserWidget->TakeWidget()), GameInstance = GetGameInstance()]() {
							if (CreatedWidget.IsValid() && IsValid(GameInstance))
							{
								if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
								{
									GameViewportClient->RemoveViewportWidgetContent(CreatedWidget.Pin().ToSharedRef());
								}
							}
						}
					)
				);

				TransitionScreenWidget->BindToEndAnimationFinished(
					FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
						this,
						[EndEvent]() {
							EndEvent.ExecuteIfBound();
						}
					)
				);
			}
			TransitionScreenWidget->ActivateWidget();
		}
	}
	else
	{
		UE_LOG(LogLoading, Error, TEXT("전환 화면 위젯 %s을(를) 로드하지 못하여 자리 표시자로 되돌아갔습니다."), *Settings->TransitionScreenWidget.ToString());
		TransitionScreenWidgetPtr = SNew(SThrobber);
	}

	UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
	GameViewportClient->AddViewportWidgetContent(TransitionScreenWidgetPtr.ToSharedRef(), Settings->LoadingScreenZOrder - 1000);

	if (!GIsEditor || Settings->ForceTickLoadingScreenEvenInEditor)
	{
		// 전환 화면이 즉시 표시되도록 슬레이트를 체크합니다.
		FSlateApplication::Get().Tick();
	}
}

void UGLoadingManager::EndTransition()
{
	if (!bCurrentlyShowingTransitionScreen)
	{
		return;
	}
	
	GEngine->ForceGarbageCollection(true);
	
	if (TransitionScreenWidget.IsValid())
	{
		TransitionScreenWidget->DeactivateWidget();
	}
	else
	{
		RemoveTransitionWidgetFromViewport();
	}

	bCurrentlyShowingTransitionScreen = false;
}

void UGLoadingManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance == GetGameInstance())
	{
		bCurrentlyInLoadMap = true;

		if (GEngine->IsInitialized())
		{
			UpdateLoadingScreen();
		}
	}
}

void UGLoadingManager::HandlePostLoadMap(UWorld* World)
{
	if ((World != nullptr) && (World->GetGameInstance() == GetGameInstance()))
	{
		bCurrentlyInLoadMap = false;
	}
}

bool UGLoadingManager::CheckForAnyNeedToShowLoadingScreen()
{
	DebugReasonForShowingOrHidingLoadingScreen = TEXT("로딩 화면이 표시/숨겨지는 이유를 알 수 없습니다!");

	auto GameInstance = GetGameInstance();

	if (GLoadingCVars::ForceLoadingScreenVisible)
	{
		DebugReasonForShowingOrHidingLoadingScreen = TEXT("GLoading.AlwaysShow가 참입니다.");
		return true;
	}

	auto Context = GameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("게임 인스턴스의 Context가 null 입니다."));
		return true;
	}

	auto World = Context->World();
	if (World == nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("월드가 없습니다. (FWorldContext의 World()가 null 입니다)"));
		return true;
	}

	auto GameState = World->GetGameState();
	if (GameState == nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("GameState가 아직 리플리케이트되지 않았습니다. (null입니다)"));
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		// 로드맵에 있는 경우 로딩 화면 표시
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("맵 로드 중입니다."));
		return true;
	}

	if (!Context->TravelURL.IsEmpty())
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("보류 중인 작업이 있습니다(TravelURL이 비어 있지 않음)."));
		return true;
	}

	if (Context->PendingNetGame != nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("다른 서버에 연결 중입니다. (PendingNetGame != nullptr)"));
		return true;
	}

	if (!World->HasBegunPlay())
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("월드 플레이가 시작되지 않았습니다."));
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("원활한 작업 중입니다."));
		return true;
	}

	if (IGLoadingProcess::ShouldShowLoadingScreen(GameState, DebugReasonForShowingOrHidingLoadingScreen))
	{
		return true;
	}

	for (UActorComponent* TestComponent : GameState->GetComponents())
	{
		if (IGLoadingProcess::ShouldShowLoadingScreen(TestComponent, DebugReasonForShowingOrHidingLoadingScreen))
		{
			return true;
		}
	}

	for (const TWeakInterfacePtr<IGLoadingProcess>& Processor : ExternalLoadingProcessors)
	{
		if (IGLoadingProcess::ShouldShowLoadingScreen(Processor.GetObject(), DebugReasonForShowingOrHidingLoadingScreen))
		{
			return true;
		}
	}

	bool bFoundAnyLocalPC = false;
	bool bMissingAnyLocalPC = false;

	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (LocalPlayer != nullptr)
		{
			if (APlayerController* PC = LocalPlayer->PlayerController)
			{
				bFoundAnyLocalPC = true;

				if (IGLoadingProcess::ShouldShowLoadingScreen(PC, DebugReasonForShowingOrHidingLoadingScreen))
				{
					return true;
				}

				for (UActorComponent* TestComponent : PC->GetComponents())
				{
					if (IGLoadingProcess::ShouldShowLoadingScreen(TestComponent, DebugReasonForShowingOrHidingLoadingScreen))
					{
						return true;
					}
				}
			}
			else
			{
				bMissingAnyLocalPC = true;
			}
		}
	}

	auto GameViewportClient = GameInstance->GetGameViewportClient();
	const bool bIsInSplitscreen = GameViewportClient->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None;

	if (bIsInSplitscreen && bMissingAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("분할 화면에서 하나 이상의 로컬 플레이어 컨트롤러가 누락되었습니다."));
		return true;
	}

	if (!bIsInSplitscreen && !bFoundAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("하나 이상의 로컬 플레이어 컨트롤러가 필요합니다."));
		return true;
	}

	DebugReasonForShowingOrHidingLoadingScreen = TEXT("로딩스크린을 더이상 보여주지 않습니다.");

	return false;
}

void UGLoadingManager::UpdateLoadingScreen()
{
	bool bLogLoadingScreenStatus = GLoadingCVars::LogLoadingScreenReasonEveryFrame;

	if (ShouldShowLoadingScreen())
	{
		auto Settings = GetDefault<UGLoadingManagerSettings>();

		FThreadHeartBeat::Get().MonitorCheckpointStart(GetFName(), 0.0F);
		ShowLoadingScreen();

		if ((Settings->LogLoadingScreenHeartbeatInterval > 0.0f) && (TimeUntilNextLogHeartbeatSeconds <= 0.0))
		{
			bLogLoadingScreenStatus = true;
			TimeUntilNextLogHeartbeatSeconds = Settings->LogLoadingScreenHeartbeatInterval;
		}
	}
	else
	{
		HideLoadingScreen();

		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}

	if (bLogLoadingScreenStatus)
	{
		UE_LOG(LogLoading, Log, TEXT("로딩 화면이 표시되고 있습니다[%s]: %s"), (bCurrentlyShowingLoadingScreen ? TEXT("TRUE") : TEXT("FALSE")), *DebugReasonForShowingOrHidingLoadingScreen);
	}
}

void UGLoadingManager::ShowLoadingScreen()
{
	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	// 엔진이 로딩 화면으로 로딩 중인 경우 로딩 화면을 표시할 수 없습니다.
	if (FPreLoadScreenManager::Get() && FPreLoadScreenManager::Get()->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
	{
		return;
	}

	auto Settings = GetDefault<UGLoadingManagerSettings>();

	TimeLoadingScreenShown = FPlatformTime::Seconds();

	bCurrentlyShowingLoadingScreen = true;

	CSV_EVENT(Loading, TEXT("Show"));

	if (IsShowingInitialLoadingScreen())
	{
		UE_LOG(LogLoading, Log, TEXT("'IsShowingInitialLoadingScreen()'이 참이면 로딩 화면을 표시합니다."));
		UE_LOG(LogLoading, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);
	}
	else
	{
		UE_LOG(LogLoading, Log, TEXT("'IsShowingInitialLoadingScreen()'이 거짓일 때 로딩 화면 표시."));
		UE_LOG(LogLoading, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);

		auto GameInstance = GetGameInstance();

		TSubclassOf<UUserWidget> LoadingScreenWidgetClass = Settings->LoadingScreenWidget.TryLoadClass<UUserWidget>();
		if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*GameInstance, LoadingScreenWidgetClass, NAME_None))
		{
			LoadingScreenWidgetPtr = UserWidget->TakeWidget();
			LoadingScreenWidget = Cast<UGLoadingWidget>(UserWidget);
			if (LoadingScreenWidget.IsValid())
			{
				if (LoadingScreenWidget->IsExistBeginAnimation())
				{
					LoadingScreenWidget->BindToBeginAnimationFinished(
						FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
							this,
							[this, ThisPtr = TWeakObjectPtr<UGLoadingManager>(this)]() {
								if (!ThisPtr.IsValid())
								{
									return;
								}
								ChangePerformanceSettings(true);
							}
						)
					);
				}

				if (LoadingScreenWidget->IsExistEndAnimation())
				{
					LoadingScreenWidget->BindToEndAnimationStarted(
						FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
							this,
							[this, ThisPtr = TWeakObjectPtr<UGLoadingManager>(this)]() {
								if (!ThisPtr.IsValid())
								{
									return;
								}
								ChangePerformanceSettings(false);
							}
						)
					);

					LoadingScreenWidget->BindToEndAnimationFinished(
						FOnLoadingWidgetAnimationDelegate::CreateWeakLambda(
							this,
							[this, ThisPtr = TWeakObjectPtr<UGLoadingManager>(this), CreatedWidget = TWeakPtr<SWidget>(UserWidget->TakeWidget()), GameInstance = GetGameInstance()]() {
								if (!ThisPtr.IsValid())
								{
									return;
								}

								if (CreatedWidget.IsValid() && IsValid(GameInstance))
								{
									if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
									{
										GameViewportClient->RemoveViewportWidgetContent(CreatedWidget.Pin().ToSharedRef());
									}
								}
							}
						)
					);
				}
				
				LoadingScreenWidget->ActivateWidget();

				UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
				GameViewportClient->AddViewportWidgetContent(LoadingScreenWidgetPtr.ToSharedRef(), Settings->LoadingScreenZOrder);
			}
		}
		else
		{
			UE_LOG(LogLoading, Error, TEXT("로딩 화면 위젯 %s을(를) 로드하지 못하여 자리 표시자로 되돌아갔습니다."), *Settings->LoadingScreenWidget.ToString());
			LoadingScreenWidgetPtr = SNew(SThrobber);

			UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
			GameViewportClient->AddViewportWidgetContent(LoadingScreenWidgetPtr.ToSharedRef(), Settings->LoadingScreenZOrder);

			ChangePerformanceSettings(true);
		}

		if (!GIsEditor || Settings->ForceTickLoadingScreenEvenInEditor)
		{
			// 로딩 화면이 즉시 표시되도록 슬레이트를 체크합니다.
			FSlateApplication::Get().Tick();
		}
	}
}

void UGLoadingManager::HideLoadingScreen()
{
	if (!bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	if (IsShowingInitialLoadingScreen())
	{
		UE_LOG(LogLoading, Log, TEXT("'IsShowingInitialLoadingScreen()'이 참이면 로딩 화면을 숨깁니다."));
		UE_LOG(LogLoading, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);
	}
	else
	{
		UE_LOG(LogLoading, Log, TEXT("'IsShowingInitialLoadingScreen()'이 거짓일 때 로딩 화면 숨기기."));
		UE_LOG(LogLoading, Log, TEXT("%s"), *DebugReasonForShowingOrHidingLoadingScreen);

		UE_LOG(LogLoading, Log, TEXT("로드 화면 삭제 전 가비지 수집"));
		GEngine->ForceGarbageCollection(true);

		if (LoadingScreenWidget.IsValid())
		{
			LoadingScreenWidget->DeactivateWidget();
		}
		else
		{
			RemoveLoadingWidgetFromViewport();
			ChangePerformanceSettings(false);
		}
	}

	CSV_EVENT(Loading, TEXT("Hide"));

	const double LoadingScreenDuration = FPlatformTime::Seconds() - TimeLoadingScreenShown;
	UE_LOG(LogLoading, Log, TEXT("%.2fs에 대해 로드 화면이 표시되었습니다."), LoadingScreenDuration);

	bCurrentlyShowingLoadingScreen = false;
}

bool UGLoadingManager::ShouldShowLoadingScreen()
{
	auto Settings = GetDefault<UGLoadingManagerSettings>();

#if !UE_BUILD_SHIPPING
	static bool bCmdLineNoLoadingScreen = FParse::Param(FCommandLine::Get(), TEXT("NoLoadingScreen"));
	if (bCmdLineNoLoadingScreen)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("명령줄에 'NoLoadingScreen'이 있습니다."));
		return false;
	}
#endif

	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();
	bool bWantToForceShowLoadingScreen = false;

	if (bNeedToShowLoadingScreen)
	{
		TimeLoadingScreenLastDismissed = -1.0;
	}
	else
	{
		const double CurrentTime = FPlatformTime::Seconds();
		const bool bCanHoldLoadingScreen = (!GIsEditor || Settings->HoldLoadingScreenAdditionalSecsEvenInEditor);
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ? GLoadingCVars::HoldLoadingScreenAdditionalSecs : 0.0;

		if (TimeLoadingScreenLastDismissed < 0.0)
		{
			TimeLoadingScreenLastDismissed = CurrentTime;
		}
		const double TimeSinceScreenDismissed = CurrentTime - TimeLoadingScreenLastDismissed;

		if ((HoldLoadingScreenAdditionalSecs > 0.0) && (TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs))
		{
			UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient();
			GameViewportClient->bDisableWorldRendering = false;

			DebugReasonForShowingOrHidingLoadingScreen = FString::Printf(TEXT("텍스처 스트리밍을 허용하기 위해 로딩 화면을 추가로 %.2f초 동안 유지합니다."), HoldLoadingScreenAdditionalSecs);
			bWantToForceShowLoadingScreen = true;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool UGLoadingManager::IsShowingInitialLoadingScreen()
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}
	return false;
}

void UGLoadingManager::RemoveLoadingWidgetFromViewport()
{
	if (LoadingScreenWidgetPtr.IsValid())
	{
		auto GameInstance = GetGameInstance();
		if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidgetPtr.ToSharedRef());
		}
		LoadingScreenWidgetPtr.Reset();
	}

	LoadingScreenWidget.Reset();
}

void UGLoadingManager::RemoveTransitionWidgetFromViewport()
{
	if (TransitionScreenWidgetPtr.IsValid())
	{
		auto GameInstance = GetGameInstance();
		if (UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(TransitionScreenWidgetPtr.ToSharedRef());
		}
		TransitionScreenWidgetPtr.Reset();
	}

	TransitionScreenWidget.Reset();
}

void UGLoadingManager::ChangePerformanceSettings(bool bEnabingLoadingScreen)
{
	auto GameInstance = GetGameInstance();
	auto GameViewportClient = GameInstance->GetGameViewportClient();

	FShaderPipelineCache::SetBatchMode(bEnabingLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);
	GameViewportClient->bDisableWorldRendering = bEnabingLoadingScreen;

	if (UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			WorldSettings->bHighPriorityLoadingLocal = bEnabingLoadingScreen;
		}
	}

	if (bEnabingLoadingScreen)
	{
		double HangDurationMultiplier;
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), /*out*/ HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}
		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}
