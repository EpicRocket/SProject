
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
    ��ġ ����Ʈ�� ��쿡�� ���������� ��ȭ�� �ͼ��� �Ǵµ�
    ��� �ؾ� ����?
    */
    return int64();
}

FGErrorInfo UGameplayUserPlayer::ConsumeItem(int32 Key, int32 Count)
{
    return FGErrorInfo();
}
