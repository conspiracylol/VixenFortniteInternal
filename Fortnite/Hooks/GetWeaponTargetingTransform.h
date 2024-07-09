#pragma once

double* (*GetWeaponTargetingTransformOriginal)(AFortWeapon*, double*, double*);
double* GetWeaponTargetingTransform(AFortWeapon* CurrentWeapon, double* StartLocation, double* EndLocation)
{
	double* ReturnValue = reinterpret_cast<double* (__cdecl*)(AFortWeapon*, double*, double*, uintptr_t, void*)>(DoSpoofCall)(CurrentWeapon, StartLocation, EndLocation, SpoofCode, GetWeaponTargetingTransformOriginal);

    if (Variables::GetWeaponTargetingTransform::bSilentAimActive && Variables::GetWeaponTargetingTransform::SilentLocationTarget)
    {
        EndLocation[0] = Variables::GetWeaponTargetingTransform::SilentLocationTarget.X - StartLocation[0];
        EndLocation[1] = Variables::GetWeaponTargetingTransform::SilentLocationTarget.Y - StartLocation[1];
        EndLocation[2] = Variables::GetWeaponTargetingTransform::SilentLocationTarget.Z - StartLocation[2];
    }

	return ReturnValue;
}