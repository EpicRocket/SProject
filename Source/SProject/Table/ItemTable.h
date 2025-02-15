// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemTable.generated.h"

enum class EItemType : uint8;
enum class EUseType : uint8;
class UTexture2D;

USTRUCT(BlueprintType)
struct SPROJECT_API FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType = static_cast<EItemType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUseType UseType = static_cast<EUseType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Value;

};

