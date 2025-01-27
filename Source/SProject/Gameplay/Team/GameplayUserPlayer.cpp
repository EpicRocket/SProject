
#include "GameplayUserPlayer.h"

// include GameCore
#include "Error/GErrorTypes.h"
// include Project
#include "Core/MyPlayerController.h"
#include "User/InventorySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayUserPlayer)

int64 UGameplayUserPlayer::GetItemCount(int32 Key) const
{
    /*
    배치 포인트의 경우에는 스테이지에 재화가 귀속이 되는데
    어떻게 해야 하지?
    */
    return int64();
}

FGErrorInfo UGameplayUserPlayer::ConsumeItem(int32 Key, int32 Count)
{
    return FGErrorInfo();
}
