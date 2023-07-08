

#include "CinemachineLockonFreeLookComponent.h"
#include "Camera/CinemachineTargetGroupComponent.h"

void UCinemachineLockonFreeLookComponent::OnInitailize_Implementation()
{
	Super::OnInitailize_Implementation();

	EObjectFlags Flags = RF_Public;
	Flags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
	if (HasAllFlags(RF_Transient))
	{
		Flags |= RF_Transient;
	}

	LockonExtension = NewObject<UCinemachineLockonExtension>(this, UCinemachineLockonExtension::StaticClass(), FName("LockonExtension(CV)"), Flags);
	LockonExtension->LockonData = LockonExtensionData;
	LockonExtension->Init(this);

	TargetGroupComponent = NewObject<UCinemachineTargetGroupComponent>(this, UCinemachineTargetGroupComponent::StaticClass(), FName("TargetGroupComponent(CV)"), Flags);
	TargetGroupComponent->SetupAttachment(this);
	TargetGroupComponent->SetAbsolute(true, true, true);
	TargetGroupComponent->RegisterComponent();
	TargetGroupComponent->LocationMode = ECVTargetGroupLocationMode::GroupAverage;
	TargetGroupComponent->RotationMode = ECVTargetGroupRotationMode::GroupAverage;

	SetLookAt(TargetGroupComponent);
}

void UCinemachineLockonFreeLookComponent::OnEnable()
{
	Super::OnEnable();
	TargetGroupComponent->SetComponentTickEnabled(true);
}

void UCinemachineLockonFreeLookComponent::OnDisable()
{
	Super::OnDisable();
	TargetGroupComponent->SetComponentTickEnabled(false);
}

void UCinemachineLockonFreeLookComponent::SetFollow(USceneComponent* FollowSceneComponent)
{
	Super::SetFollow(FollowSceneComponent);
	USceneComponent* Seeker = IsValid(FollowTarget) ? FollowTarget : GetFollow();
	if (IsValid(Seeker))
	{
		TargetGroupComponent->RemoveMember(Seeker);
		TargetGroupComponent->InsertMember(Seeker, 0, 1.0F, 1.0F);
		//UpdateTargetGroupWeight();
	}
}

void UCinemachineLockonFreeLookComponent::LockonActor(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}
	if (IsValid(TargetGroupComponent))
	{
		TargetGroupComponent->RemoveMember(Target->GetRootComponent());
		TargetGroupComponent->AddMember(Target->GetRootComponent(), 1.0F, 1.0F);
		//UpdateTargetGroupWeight();
	}
}

void UCinemachineLockonFreeLookComponent::UnlockonActor(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}
	if (IsValid(TargetGroupComponent))
	{
		TargetGroupComponent->RemoveMember(Target->GetRootComponent());
		//UpdateTargetGroupWeight();
	}
}

void UCinemachineLockonFreeLookComponent::LockonLoction(USceneComponent* Target)
{
	if (!IsValid(Target))
	{
		return;
	}
	if (IsValid(TargetGroupComponent))
	{
		TargetGroupComponent->RemoveMember(Target);
		TargetGroupComponent->AddMember(Target, 1.0F, 1.0F);
		//UpdateTargetGroupWeight();
	}
}

void UCinemachineLockonFreeLookComponent::UnlockonLoction(USceneComponent* Target)
{
	if (!IsValid(Target))
	{
		return;
	}
	if (IsValid(TargetGroupComponent))
	{
		TargetGroupComponent->RemoveMember(Target);
		//UpdateTargetGroupWeight();
	}
}

void UCinemachineLockonFreeLookComponent::Unlock()
{
	if (!IsValid(TargetGroupComponent))
	{
		return;
	}
	TargetGroupComponent->ClearMembers();

	USceneComponent* Seeker = IsValid(FollowTarget) ? FollowTarget : GetFollow();
	if (IsValid(Seeker))
	{
		TargetGroupComponent->InsertMember(Seeker, 0, 1.0F, 1.0F);
	}
}

void UCinemachineLockonFreeLookComponent::UpdateTargetGroupWeight()
{
	if (IsValid(TargetGroupComponent))
	{
		const int32 MemberCount = TargetGroupComponent->Members.Num();
		if (MemberCount > 1)
		{
			const float AverageWeight = 1.0F / MemberCount;
			for (FCVTargetGroupMember& Memeber : TargetGroupComponent->Members)
			{
				Memeber.Weight = AverageWeight;
			}
		}
	}
}

bool UCinemachineLockonFreeLookComponent::IsPossibleToLockonActor(AActor* Target) const
{
	if (!IsValid(Target))
	{
		return false;
	}

	if (IsValid(TargetGroupComponent))
	{
		for (const FCVTargetGroupMember& Memeber : TargetGroupComponent->Members)
		{
			if (Memeber.Target == Target->GetRootComponent())
			{
				return true;
			}
		}
	}
	return false;
}

bool UCinemachineLockonFreeLookComponent::IsPossibleToLockonLoction(USceneComponent* Target) const
{
	if (IsValid(TargetGroupComponent))
	{
		for (const FCVTargetGroupMember& Memeber : TargetGroupComponent->Members)
		{
			if (Memeber.Target == Target)
			{
				return true;
			}
		}
	}
	 return false;
}

bool UCinemachineLockonFreeLookComponent::IsPossibleLockon() const
{
	return IsValid(TargetGroupComponent) && TargetGroupComponent->Members.Num() > 1;
}

USceneComponent* UCinemachineLockonFreeLookComponent::GetSeeker() const
{
	if (!IsValid(TargetGroupComponent))
	{
		return nullptr;
	}
	return TargetGroupComponent->Members.IsEmpty() ? nullptr : TargetGroupComponent->Members[0].Target;
}
