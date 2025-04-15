
#include "StagePlayerPawn.h"
// include Engine
#include "GameFramework/Controller.h"
// include Project
#include "StageLogging.h"
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

    //UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
    //
    //FGameplayMessageListenerParams<FStageRequestBuildMessage> RequestBuildMessage{
    //    .MatchType = EGameplayMessageMatch::ExactMatch,
    //    .OnMessageReceivedCallback = [this, ThisPtr = TWeakObjectPtr<ThisClass>(this)](auto Tag, auto Message) -> void {
    //        if (!ThisPtr.IsValid())
    //        {
    //            UE_LOG(LogStage, Warning, TEXT("Stage Player Pawn???쒓굅?섏뿀?듬땲??"));
    //            return;
    //        }

    //       /* if (!CurrentLevel.IsValid())
    //        {
    //            UE_LOG(LogStage, Warning, TEXT("Current Level???좏슚?섏? ?딆뒿?덈떎."));
    //            return;
    //        }*/

    //        UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);

    //        // TODO: ?곹깭 議곌굔 異붽?
    //        GameplayMessageSubsystem.BroadcastMessage(Stage::Tag_Gameplay_Stage_Request_Build_Select, Message);
    //    }
    //};
    //GameplayMessageListenerHandles.Emplace(GameplayMessageSubsystem.RegisterListener(Stage::Tag_Gameplay_Stage_Request_Build, RequestBuildMessage));

    OnUpdateState();
}

void AStagePlayerPawn::UnPossessed()
{
    Super::UnPossessed();

    OnUpdateState();
}

void AStagePlayerPawn::OnUpdateState()
{
}
