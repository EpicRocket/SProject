
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakInterfacePtr.h"
#include "UObject/ScriptInterface.h"

#include "GLoadingManager.generated.h"

class IGLoadingProcess;
class ILoadingProcessInterface;
class UGLoadingWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogLoading, Log, All);

DECLARE_DYNAMIC_DELEGATE(FOnDynamicLoadingWidgetAnimationDelegate);

UCLASS()
class GGAMECORE_API UGLoadingManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//FTickableObjectBase
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	// ~FTickableObjectBase

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Loading")
	void SetUseLoadingScreen(bool bValue);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Loading")
	FString GetDebugReasonForShowingOrHidingLoadingScreen() const;

	bool GetLoadingScreenDisplayStatus() const;

	void RegisterLoadingProcessor(TScriptInterface<IGLoadingProcess> Interface);

	void UnregisterLoadingProcessor(TScriptInterface<IGLoadingProcess> Interface);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Transition")
	void BeginTransition(FOnDynamicLoadingWidgetAnimationDelegate BeginEvent, FOnDynamicLoadingWidgetAnimationDelegate EndEvent);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Transition")
	void EndTransition();

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Transition")
	bool IsTransition() const;

private:
	// 맵 로드 전
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);

	// 맵 로드 후
	void HandlePostLoadMap(UWorld* World);

	bool CheckForAnyNeedToShowLoadingScreen();
	
	void UpdateLoadingScreen();

	void ShowLoadingScreen();

	void HideLoadingScreen();

	bool ShouldShowLoadingScreen();

	bool IsShowingInitialLoadingScreen();

	void RemoveLoadingWidgetFromViewport();

	void RemoveTransitionWidgetFromViewport();

	void ChangePerformanceSettings(bool bEnabingLoadingScreen);

private:
	bool bUseLoadingScreen = false;

	// Loading Screen Widget Reference
	TSharedPtr<SWidget> LoadingScreenWidgetPtr;
	TWeakObjectPtr<UGLoadingWidget> LoadingScreenWidget;

	// 외부 로딩 프로세서, 컴포넌트는 로딩을 지연시키는 액터일 수 있습니다.
	TArray<TWeakInterfacePtr<IGLoadingProcess>> ExternalLoadingProcessors;

	// 로딩 화면이 뜨는 이유(또는 뜨지 않는 이유)
	FString DebugReasonForShowingOrHidingLoadingScreen;

	// 로딩 화면이 표시되기 시작한 시간
	double TimeLoadingScreenShown = 0.0;

	// 로딩 화면이 계속 표시되는 이유에 대한 다음 로그까지의 시간
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	// 로딩 화면이 가장 최근에 종료되기를 원했던 시간(최소 표시 시간 요구 사항으로 인해 아직 표시 중일 수 있음)
	double TimeLoadingScreenLastDismissed = -1.0F;

	// PreLoadMap과 PostLoadMap 사이에 있을 때 참입니다
	bool bCurrentlyInLoadMap = false;

	// 현재 로딩 화면이 표시되고 있는지?
	bool bCurrentlyShowingLoadingScreen = false;

	// Transition Screen Widget Reference
	TSharedPtr<SWidget> TransitionScreenWidgetPtr;
	TWeakObjectPtr<UGLoadingWidget> TransitionScreenWidget;

	// 현재 전환 화면이 표시되고 있는지?
	bool bCurrentlyShowingTransitionScreen = false;

};
