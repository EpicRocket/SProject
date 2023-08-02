

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/Core/CinemachineScreenComposerSettings.h"
#include "CinemachineModifierInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineModifierValueSourceInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineModifierValueSourceInterface
{
	GENERATED_BODY()

public:
	virtual float NormalizedModifierValue() = 0;
};

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineModifiableLocationDampingInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineModifiableLocationDampingInterface
{
	GENERATED_BODY()

public:
	virtual FVector GetLocationDamping() const = 0;
	virtual void SetLocationDamping(const FVector& InLocationDamping) = 0;
};

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineModifiableCompositionInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineModifiableCompositionInterface
{
	GENERATED_BODY()

public:
	virtual FCinemachineScreenComposerSettings GetComposition() const = 0;
	virtual void SetComposition(const FCinemachineScreenComposerSettings& InComposition) = 0;
};


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineModifiableDistanceInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineModifiableDistanceInterface
{
	GENERATED_BODY()

public:
	virtual float GetDistance() const = 0;
	virtual void SetDistance(float InDistance) = 0;
};

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineModifiableNoiseInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineModifiableNoiseInterface
{
	GENERATED_BODY()

public:
	virtual FVector2D GetNoiseAmplitudeFrequency() const = 0;
	virtual void SetNoiseAmplitudeFrequency(const FVector2D& InNoiseAmplitudeFrequency) = 0;
};
