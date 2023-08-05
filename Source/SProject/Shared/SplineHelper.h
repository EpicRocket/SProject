
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SplineHelper.generated.h"

UCLASS(meta = (BlueprintThreadSafe))
class USplineHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "SplineHelper")
	static float Bezier1(float Alpha, float P0, float P1, float P2, float P3)
	{
		Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
		float D = 1.0F - Alpha;
		return D * D * D * P0 + 3.0F * D * D * Alpha * P1 + 3.0F * D * Alpha * Alpha * P2 + Alpha * Alpha * Alpha * P3;
	}

	UFUNCTION(BlueprintPure, Category = "SplineHelper")
    static FVector Bezier3(float Alpha, FVector P0, FVector P1, FVector P2, FVector P3)
    {
        Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
        double D = 1.0F - Alpha;
        return D * D * D * P0 + 3.0F * D * D * Alpha * P1 + 3.0F * D * Alpha * Alpha * P2 + Alpha * Alpha * Alpha * P3;
    }
};
