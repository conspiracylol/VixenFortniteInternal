#pragma once

void (*GetViewPointOriginal)(ULocalPlayer*, FMinimalViewInfo*);
void GetViewPoint(ULocalPlayer* LocalPlayer, FMinimalViewInfo* OutViewInfo)
{
	reinterpret_cast<void(__cdecl*)(ULocalPlayer*, FMinimalViewInfo*, uintptr_t, void*)>(DoSpoofCall)(LocalPlayer, OutViewInfo, SpoofCode, GetViewPointOriginal);
}