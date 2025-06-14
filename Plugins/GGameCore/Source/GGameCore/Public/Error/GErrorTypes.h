// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "GErrorTypes.generated.h"

UENUM(BlueprintType)
enum class EGErrType : uint8
{
	None,
	Verbose,
	Warning,
	Error
};

USTRUCT(BlueprintType)
struct FGErrorInfo : public FTableRowBase
{
	GENERATED_BODY()

	FGErrorInfo() {}

	FGErrorInfo(EGErrType InErrType, const FString& InErrCode, const FText& InDescription)
		: ErrType(InErrType)
		, ErrCode(InErrCode)
		, Description(InDescription)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGErrType ErrType = EGErrType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ErrCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};
