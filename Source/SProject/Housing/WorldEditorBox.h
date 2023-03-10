#pragma once

#include "GameFramework/Actor.h"
#include "WorldEditorBox.generated.h"

UCLASS()
class SPROJECT_API AWorldEditorBox : public AActor
{
	GENERATED_BODY()
	
public:

	AWorldEditorBox();

	// AActor interface
	virtual void PreInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
#if WITH_EDITORONLY_DATA
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif
	// End of AActor interface

	UFUNCTION(BlueprintPure, Category = Utility)
	FVector WorldToBoxPosition(FVector LocalPosition);

	UFUNCTION(BlueprintPure, Category = Utility)
	FVector WorldAndAxisToBoxPosition(FVector WorldPosition, FIntVector Axis);

	/** 해당 좌표가 WorldEditorBox내에 포함 되는지 확인합니다. */
	UFUNCTION(BlueprintPure, Category = Utility)
	bool IsInWorld(FVector WorldPosition) const;

protected:
	
	// AActor interface
	virtual void BeginPlay() override;
	// End of AActor interface

private:

#if WITH_EDITORONLY_DATA
	void OnActorMoving(AActor* SelcetedActor);
	void OnActorMoved(AActor* SelcetedActor);
#endif
	
public:

	static FName RootName;
	static FName WorldEditorComponentName;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	TObjectPtr<class UWorldEditorComponent> WorldEditorComponent;

private:

#if WITH_EDITORONLY_DATA
	TObjectPtr<class AEnvironmentProp> DebugSelectedProp;
#endif

};
