#pragma once

void (*GetCameraViewPointOriginal)(APlayerCameraManager*, FVector*, FRotator*);
void GetCameraViewPoint(APlayerCameraManager* PlayerCameraManager, FVector* OutLocation, FRotator* OutRotation)
{
	reinterpret_cast<void(__cdecl*)(APlayerCameraManager*, FVector*, FRotator*, uintptr_t, void*)>(DoSpoofCall)(PlayerCameraManager, OutLocation, OutRotation, SpoofCode, GetCameraViewPointOriginal);

	static bool Old_bShouldStartAtLocationTarget = false;
	static FVector StaticStickyLocation = FVector();

	if (Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget != Old_bShouldStartAtLocationTarget)
	{
		StaticStickyLocation = *OutLocation;

		Old_bShouldStartAtLocationTarget = Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget;
	}

	if (Settings::Aimbot::StickySilentAim && Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget)
	{
		if (Settings::Aimbot::StickySilent::StickyCameraSpin && StaticStickyLocation)
		{
			*OutLocation = StaticStickyLocation;
		}
		else
		{
			*OutLocation = Variables::GetCameraViewPoint::StickyLocation;
		}

		*OutRotation = UKismetMathLibrary::FindLookAtRotation(*OutLocation, Variables::GetPlayerViewPoint::SilentLocationTarget);

		Variables::GetCameraViewPoint::LastStickyRotation = *OutRotation;
	}
}