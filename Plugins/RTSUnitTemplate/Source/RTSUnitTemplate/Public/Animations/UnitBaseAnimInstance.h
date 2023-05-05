// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Core/UnitData.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Sound\SoundCue.h"
#include "UnitBaseAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FUnitAnimData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	TEnumAsByte<UnitData::EState> AnimState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float BlendPoint_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float BlendPoint_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float TransitionRate_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float TransitionRate_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float Resolution_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	float Resolution_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTSUnitTemplate)
	USoundBase* Sound;
};

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class RTSUNITTEMPLATE_API UUnitBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UUnitBaseAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		TEnumAsByte<UnitData::EState> CharAnimState;

	TEnumAsByte<UnitData::EState> LastAnimState = UnitData::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float BlendPoint_1 = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float BlendPoint_2 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float CurrentBlendPoint_1 = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float CurrentBlendPoint_2 = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float TransitionRate_1 = 0.5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float TransitionRate_2 = 0.5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float Resolution_1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float Resolution_2 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	float SoundTimer = 0.f;
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float Deltaseconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TopDownRTSTemplate)
	class UDataTable* AnimDataTable;

	void SetBlendPoints(AUnitBase* Unit, float Deltaseconds);

	FUnitAnimData* UnitAnimData;
	
};