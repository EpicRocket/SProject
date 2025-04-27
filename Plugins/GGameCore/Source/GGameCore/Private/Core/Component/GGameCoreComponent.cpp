
#include "Core/Component/GGameCoreComponent.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
// include GameCore
#include "Error/GError.h"
#include "Core/Action/GGameComponentLoadAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameCoreComponent)

UGGameCoreComponent::UGGameCoreComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	bAllowAnyoneToDestroyMe = true;
	PrimaryComponentTick.bCanEverTick = false;
}

FGErrorInfo UGGameCoreComponent::WaitForInitialize(FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID);
	}

	FGErrorInfo ErrorInfo;
	auto OnComplete = [this]()
		{
			OnInitialize();
		};

	auto NewAction = new FGGameComponentLoadAction(LatentInfo, GetWorld(), this, OnComplete, [&ErrorInfo](FGErrorInfo Err) {ErrorInfo = Err; });
	World->GetLatentActionManager().AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return ErrorInfo;
}
