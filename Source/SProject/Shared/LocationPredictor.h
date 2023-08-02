

#pragma once

#include "CoreMinimal.h"
#include "Shared/Damper.h"
#include "LocationPredictor.generated.h"

USTRUCT(BlueprintType)
struct FLocationPredictor
{
    GENERATED_BODY()

public:
    bool IsEmpty() const
    {
        return !HaveLocation;
    }

    void ApplyTransformDelta(FVector LocationDelta)
    {
        Location += LocationDelta;
    }

    void Reset()
    {
        HaveLocation = false;
		SmoothDampVelocity = FVector::ZeroVector;
        Velocity = FVector::ZeroVector;
    }

    void AddLocation(FVector NextLocation, float DeltaTime)
    {
        if(DeltaTime < 0.0F)
        {
            Reset();
        }
        if(HaveLocation && DeltaTime > UE_KINDA_SMALL_NUMBER)
        {
            FVector LocationDelta = (NextLocation - Location / DeltaTime);
            bool Slowing = LocationDelta.SizeSquared() < Velocity.SizeSquared();
            Velocity = UDamper::SmoothDamp(Velocity, LocationDelta, SmoothDampVelocity, Smoothing / (Slowing ? 30.0F : 10.0F), TNumericLimits<float>::Max(), DeltaTime);
        }
        Location = NextLocation;
        HaveLocation = true;
    }

    FVector PredictLocationDelta(float LookaheadTime)
    {
        return Velocity * LookaheadTime;
    }

    FVector PredictLocation(float LookaheadTime)
    {
        return Location + PredictLocationDelta(LookaheadTime);
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector SmoothDampVelocity = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool HaveLocation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Smoothing = 0.0F;
};
