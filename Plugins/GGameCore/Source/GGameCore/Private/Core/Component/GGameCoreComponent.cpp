
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

void UGGameCoreComponent::WaitForInitialize(FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		GameCore::Throw(GameErr::WORLD_INVALID);
		return;
	}

	auto OnComplete = []()
		{
		};

	auto OnError = [](FGErrorInfo Err)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize component: %s"), *Err.Description.ToString());
		};

	auto NewAction = new FGGameComponentLoadAction(LatentInfo, GetWorld(), this, OnComplete, OnError);
	World->GetLatentActionManager().AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
}
