
#include "StagePawn.h"
// include Engine
#include "GameFramework/Controller.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameplayMessageTypes2.h"
// include Project
#include "StageGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePawn)


void AStagePawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!NewController->IsLocalPlayerController())
    {
        return;
    }

    UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);

    FGameplayMessageListenerParams<FStageRequestBuildMessage> RequestBuildMessage{
        .MatchType = EGameplayMessageMatch::ExactMatch,
        .OnMessageReceivedCallback = [](auto Tag, auto Message) -> void {

        }
    };

    GameplayMessageListenerHandles.Emplace(GameplayMessageSubsystem.RegisterListener(Stage::Tag_Gameplay_Stage_Request_Build, RequestBuildMessage));
}

void AStagePawn::UnPossessed()
{
    Super::UnPossessed();

    for (auto& Handle : GameplayMessageListenerHandles)
	{
        Handle.Unregister();
	}
    GameplayMessageListenerHandles.Empty();
}
