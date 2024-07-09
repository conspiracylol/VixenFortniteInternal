#pragma once

double* (*GetTargetingSourceLocationOriginal)(AFortWeapon*, double*);
double* GetTargetingSourceLocation(AFortWeapon* CurrentWeapon, double* Location)
{
	double* ReturnValue = reinterpret_cast<double* (__cdecl*)(AFortWeapon*, double*, uintptr_t, void*)>(DoSpoofCall)(CurrentWeapon, Location, SpoofCode, GetTargetingSourceLocationOriginal);

    if (Variables::GetTargetingSourceLocation::bSilentAimActive && Variables::GetTargetingSourceLocation::SilentLocationTarget)
    {
        ReturnValue[0] = Variables::GetTargetingSourceLocation::SilentLocationTarget.X;
        ReturnValue[1] = Variables::GetTargetingSourceLocation::SilentLocationTarget.Y;
        ReturnValue[2] = Variables::GetTargetingSourceLocation::SilentLocationTarget.Z;
    }

    return ReturnValue;
}