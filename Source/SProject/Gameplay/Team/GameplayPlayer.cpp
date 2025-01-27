
#include "GameplayPlayer.h"
// include GameCore
#include "Error/GErrorTypes.h"
// include Project
#include "Table/ConstTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayPlayer)

int64 UGameplayPlayer::GetItemCount(int32 Key) const
{
    return 0;
}

FGErrorInfo UGameplayPlayer::ConsumeItem(int32 Key, int32 Count)
{
    // FIXME: 구현되어 있지 않는 상태에서는 Erorr를 반환해줘야함...
	return FGErrorInfo();
}

int64 UGameplayPlayer::GetUsePoint() const
{
    return GetItemCount(GetDefault<UConstSettings>()->UsePointKey);
}

FGErrorInfo UGameplayPlayer::ConsumeUsePoint(int32 Count)
{
	return ConsumeItem(GetDefault<UConstSettings>()->UsePointKey, Count);
}

int64 UGameplayPlayer::GetGold() const
{
    return GetItemCount(GetDefault<UConstSettings>()->GoldKey);
}

FGErrorInfo UGameplayPlayer::ConsumedGold(int32 Count)
{
	return ConsumeItem(GetDefault<UConstSettings>()->GoldKey, Count);
}
