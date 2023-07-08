

#include "Camera/VirtualCamera/CinemachineFreeLookComponent.h"
#include "Camera/Extension/CinemachineLockonExtension.h"
#include "CinemachineLockonFreelookcomponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineLockonFreeLookComponent : public UCinemachineFreeLookComponent
{
	GENERATED_BODY()

public:
	virtual void SetFollow(USceneComponent* FollowSceneComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void LockonActor(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void UnlockonActor(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void LockonLoction(USceneComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void UnlockonLoction(USceneComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void Unlock();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|FreeLock")
	void UpdateTargetGroupWeight();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	bool IsPossibleToLockonActor(AActor* Target) const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	bool IsPossibleToLockonLoction(USceneComponent* Target) const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	bool IsPossibleLockon() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|FreeLock")
	USceneComponent* GetSeeker() const;

protected:
    //~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual void OnInitailize_Implementation() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

private:
	UPROPERTY(Transient)
	class UCinemachineTargetGroupComponent* TargetGroupComponent = nullptr;

	UPROPERTY(Transient)
	UCinemachineLockonExtension* LockonExtension = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true"))
	FCVLockonExtensionData LockonExtensionData;
};
