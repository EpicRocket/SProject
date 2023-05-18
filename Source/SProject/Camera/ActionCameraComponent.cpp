
#include "ActionCameraComponent.h"


UActionCameraComponent::UActionCameraComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

void UActionCameraComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

void UActionCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UActionCameraComponent::Recall()
{
    // implementation goes here
}

void UActionCameraComponent::SetTarget(AActor* InTarget)
{
    // implementation goes here
}
