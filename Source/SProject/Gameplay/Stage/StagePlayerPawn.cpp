
#include "StagePlayerPawn.h"
// include Engine
#include "GameFramework/Controller.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameplayMessageTypes2.h"
// include Project
#include "StageLogging.h"
#include "StageGameplayTags.h"
#include "StageLevel.h"
#include "ETC/StageBuildZone.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerPawn)


void AStagePlayerPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!NewController->IsLocalPlayerController())
    {
        return;
    }

    UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
    
    FGameplayMessageListenerParams<FStageRequestBuildMessage> RequestBuildMessage{
        .MatchType = EGameplayMessageMatch::ExactMatch,
        .OnMessageReceivedCallback = [this, ThisPtr = TWeakObjectPtr<ThisClass>(this)](auto Tag, auto Message) -> void {
            if (!ThisPtr.IsValid())
            {
                UE_LOG(LogStage, Warning, TEXT("Stage Player Pawn이 제거되었습니다."));
                return;
            }

           /* if (!CurrentLevel.IsValid())
            {
                UE_LOG(LogStage, Warning, TEXT("Current Level이 유효하지 않습니다."));
                return;
            }*/

            UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);

            // TODO: 상태 조건 추가
            GameplayMessageSubsystem.BroadcastMessage(Stage::Tag_Gameplay_Stage_Request_Build_Select, Message);
        }
    };
    GameplayMessageListenerHandles.Emplace(GameplayMessageSubsystem.RegisterListener(Stage::Tag_Gameplay_Stage_Request_Build, RequestBuildMessage));

    OnUpdateState();
}

void AStagePlayerPawn::UnPossessed()
{
    Super::UnPossessed();

    for (auto& Handle : GameplayMessageListenerHandles)
	{
        Handle.Unregister();
	}
    GameplayMessageListenerHandles.Empty();

    OnUpdateState();
}

void AStagePlayerPawn::OnUpdateState()
{
}
