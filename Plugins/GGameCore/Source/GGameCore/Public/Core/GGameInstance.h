// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Engine/GameInstance.h"

#include "GGameInstance.generated.h"

class ULocalPlayer;
class UGLocalPlayer;

UCLASS(Abstract)
class GGAMECORE_API UGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;

	UFUNCTION(BlueprintCallable)
	UGLocalPlayer* GetLocalPlayer(uint8 UniqueId) const;

private:
	TSortedMap<uint8, TWeakObjectPtr<UGLocalPlayer>> LocalPlayers;

};
