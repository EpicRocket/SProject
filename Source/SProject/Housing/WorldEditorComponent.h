#pragma once

#include "Components/PrimitiveComponent.h"
#include "WorldEditorComponent.generated.h"

using FWorldEditorOctree = TOctree2<struct FWorldEditorOctreeElement, struct FWorldEditorOctreeSematics>;


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class SPROJECT_API UWorldEditorComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	

public:

	UWorldEditorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	// UPrimitiveComponent interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
#endif
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool UpdateOverlapsImpl(const TOverlapArrayView* NewPendingOverlaps = nullptr, bool bDoNotifies = true, const TOverlapArrayView* OverlapsAtEndLocation = nullptr) override;
	virtual bool OverlapComponent(const FVector& Pos, const FQuat& Rot, const FCollisionShape& CollisionShape) const override;
	// End of UPrimitiveComponent interface

	UFUNCTION(BlueprintCallable, Category = Debug, meta = (DevelopmentOnly))
	void DrawOctree();

	UFUNCTION(BlueprintCallable)
	void AddElement(AActor* InActor);

	UFUNCTION(BlueprintGetter)
	FVector GetExtent() const;

	UFUNCTION(BlueprintPure)
	FVector Origin() const;

	UFUNCTION(BlueprintGetter)
	float GetGridSize() const
	{
		return GridSize;
	}

	UFUNCTION(BlueprintPure)
	bool IsInWorld(FVector WorldPosition) const;

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw)
	FColor BorderColor{ 0xFF, 0xFF, 0xFF, 0xFF };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw)
	FColor GridColor{ 0xFF, 0xFF, 0xFF, 0x80 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw)
	float Thickness = 2.0F;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw)
	float DashSize = 10;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw, BlueprintGetter = GetGridSize)
	float GridSize = 100.0F;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Draw, BlueprintGetter = GetExtent)
	FVector Extent{ 1000.0F, 1000.0F, 1000.0F };

	FBox WorldBox;

	TObjectPtr<FWorldEditorOctree> Octree;
};
