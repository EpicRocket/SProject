

#pragma once

#include "CoreMinimal.h"
#include "Rect.generated.h"

USTRUCT(BlueprintType)
struct FRect
{
	GENERATED_BODY()

public:
	FRect() = default;
	FRect(float InX, float InY, float InWidth, float InHeight)
		: X(InX)
		, Y(InY)
		, Width(InWidth)
		, Height(InHeight)
	{
	}
	FRect(FVector2D InPosition, FVector2D InSize)
		: X(InPosition.X)
		, Y(InPosition.Y)
		, Width(InSize.X)
		, Height(InSize.Y)
	{
	}

	bool operator==(const FRect& Other) const
	{
		return FMath::IsNearlyEqual(X, Other.X) && FMath::IsNearlyEqual(Y, Other.Y) && FMath::IsNearlyEqual(Width, Other.Width) && FMath::IsNearlyEqual(Height, Other.Height);
	}

	bool operator!=(const FRect& Other) const
	{
		return !(*this == Other);
	}

	void operator+(FVector2D InVector)
	{
		X += InVector.X;
		Y += InVector.Y;
	}

	FRect& operator+=(FVector2D InVector)
	{
		X += InVector.X;
		Y += InVector.Y;
		return *this;
	}

	FVector2D GetCenter() const
	{
		return FVector2D(X, Y);
	}

	void SetMinX(float Value)
	{
		float Temp = GetMaxX();
		X = Value;
		Width = Temp - X;
	}

	void SetMaxX(float Value)
	{
		Width = Value - X;
	}

	float GetMinX() const
	{
		return X;
	}

	float GetMaxX() const
	{
		return X + Width;
	}

	void SetMinY(float Value)
	{
		float Temp = GetMaxY();
		Y = Value;
		Height = Temp - Y;
	}

	void SetMaxY(float Value)
	{
		Height = Value - Y;
	}

	float GetMinY() const
	{
		return Y;
	}

	float GetMaxY() const
	{
		return Y + Height;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("X=%3.3f Y=%3.3f Width=%3.3f Height=%3.3f"), X, Y, Width, Height);
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 0.0F;
};
