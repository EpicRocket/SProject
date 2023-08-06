
#pragma once

#include "Framework/SuperPlayerController.h"
#include "MainPlayerController.generated.h"

UCLASS(Blueprintable)
class AMainPlayerController : public ASuperPlayerController
{
    GENERATED_BODY()


public:
	virtual void BeginPlay() override;

protected:
	virtual void SetupInputComponent() override;

private:
    UFUNCTION()
    void OnTouchStart() {/*TODO*/}

    UFUNCTION()
    void OnTouchMove() {/*TODO*/}

    UFUNCTION()
    void OnTouchEnd() {/*TODO*/}

    UFUNCTION()
    void OnMouseClickStart() {/*TODO*/}

    UFUNCTION()
    void OnMouseClickMove() {/*TODO*/}

    UFUNCTION()
    void OnMouseClickEnd() {/*TODO*/}

private:
    UPROPERTY()
    class UMainInputComponent* MainInputComponent = nullptr;

	// Test용
public:
    /* 스폰 관련해서 테스트용 유닛 생성 가능토록 해봐야함 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACharacter> TestCharacterPtr;
};
