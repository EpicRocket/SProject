#include "GameHelper.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
// include Project
#include "Character/Unit.h"
#include "Game/MainGameStageComponent.h"

AUnit* UGameHelper::SummonUnitFromGameStage(const UObject* WorldContextObject, int32 GameStageId, int32 UnitId)
{
    if (nullptr == WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (nullptr == World)
    {
		return nullptr;
	}

    AGameStateBase* GameState = World->GetGameState();
    if (nullptr == GameState)
    {
        return nullptr;
    }

    UMainGameStageComponent* MainGameStageComponent = GameState->FindComponentByClass<UMainGameStageComponent>();

    if (nullptr == MainGameStageComponent)
    {
		return nullptr;
	}

    // TODO: Class가져 올 수 있도록 수정 필요함!
    return nullptr;
    //return MainGameStageComponent->SummonUnit(GameStageId, ;
}
