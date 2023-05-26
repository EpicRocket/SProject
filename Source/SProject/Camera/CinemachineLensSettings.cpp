
#include "CinemachineLensSettings.h"
#include "Math/UnrealMathUtility.h"
#include "Camera/CameraComponent.h"

FCinemachineLensSettings FCinemachineLensSettings::FromCamera(UCameraComponent* CameraComponent)
{
	FCinemachineLensSettings Lens;
	if (!IsValid(CameraComponent))
	{
		return Lens;
	}

	Lens.FieldOfView = CameraComponent->FieldOfView;
	Lens.AspectRatio = CameraComponent->AspectRatio;

    return Lens;
}

FCinemachineLensSettings FCinemachineLensSettings::Lerp(const FCinemachineLensSettings& A, const FCinemachineLensSettings& B, float Alpha)
{
    Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
    FCinemachineLensSettings Lens = Alpha < 0.5F ? A : B;
    Lens.FieldOfView = FMath::Lerp(A.FieldOfView, B.FieldOfView, Alpha);
    Lens.Dutch = FMath::Lerp(A.Dutch, B.Dutch, Alpha);
    return Lens;
}
