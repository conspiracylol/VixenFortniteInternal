#pragma once

float (*GetCalculatedReticleSpreadOriginal)(AFortWeapon*);
float GetCalculatedReticleSpread(AFortWeapon* CurrentWeapon)
{
	float ReturnValue = reinterpret_cast<float(__cdecl*)(AFortWeapon*, uintptr_t, void*)>(DoSpoofCall)(CurrentWeapon, SpoofCode, GetCalculatedReticleSpreadOriginal);

	if (Settings::Exploits::NoSpread)
	{
		return 0.f;
	}

	return ReturnValue;
}