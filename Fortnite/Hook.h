#pragma once

void Rehook(ULocalPlayer* LocalPlayer)
{
	static ULocalPlayer* OldLocalPlayer = nullptr;

	if (LocalPlayer != OldLocalPlayer)
	{
		bool LocalPlayerIsA = UGameplayStatics::ObjectIsA(LocalPlayer, Classes::LocalPlayer);
		if (LocalPlayerIsA)
		{
			static Hook::NewHook LocalPlayerHook;
			if (LocalPlayerHook.Initialize(LocalPlayer))
			{
				LocalPlayerHook.Insert(&GetViewPoint, Offsets::GetViewPoint, &GetViewPointOriginal);

				LocalPlayerHook.SwapContext();
			}

			OldLocalPlayer = LocalPlayer;
		}
	}

	if (APlayerController* PlayerController = LocalPlayer->PlayerController())
	{
		static APlayerController* OldPlayerController = nullptr;

		if (PlayerController != OldPlayerController)
		{
			bool PlayerCameraManagerIsA = UGameplayStatics::ObjectIsA(PlayerController, Classes::PlayerController);
			if (PlayerCameraManagerIsA)
			{
				static Hook::NewHook PlayerControllerHook;
				if (PlayerControllerHook.Initialize(PlayerController))
				{
					PlayerControllerHook.Insert(&GetPlayerViewPoint, Offsets::GetPlayerViewPoint, &GetPlayerViewPointOriginal);

					PlayerControllerHook.SwapContext();
				}

				OldPlayerController = PlayerController;
			}
		}

		if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager())
		{
			static APlayerCameraManager* OldPlayerCameraManager = nullptr;

			if (PlayerCameraManager != OldPlayerCameraManager)
			{
				bool PlayerCameraManagerIsA = UGameplayStatics::ObjectIsA(PlayerCameraManager, Classes::PlayerCameraManager);
				if (PlayerCameraManagerIsA)
				{
					static Hook::NewHook PlayerCameraManagerHook;
					if (PlayerCameraManagerHook.Initialize(PlayerCameraManager))
					{
						PlayerCameraManagerHook.Insert(&GetCameraViewPoint, Offsets::GetCameraViewPoint, &GetCameraViewPointOriginal);

						PlayerCameraManagerHook.SwapContext();
					}

					OldPlayerCameraManager = PlayerCameraManager;
				}
			}
		}

		if (APlayerPawn_Athena_C* AcknowledgedPawn = PlayerController->AcknowledgedPawn())
		{
			if (AFortWeapon* CurrentWeapon = AcknowledgedPawn->CurrentWeapon())
			{
				static AFortWeapon* OldCurrentWeapon = nullptr;

				if (CurrentWeapon != OldCurrentWeapon)
				{
					bool CurrentWeaponIsARanged = UGameplayStatics::ObjectIsA(CurrentWeapon, Classes::RangedWeaponClass);
					if (CurrentWeaponIsARanged)
					{
						static Hook::NewHook CurrentWeaponHook;
						if (CurrentWeaponHook.Initialize(CurrentWeapon))
						{
							CurrentWeaponHook.Insert(&FireSingle, Offsets::FireSingle, &FireSingleOriginal);
							//CurrentWeaponHook.Insert(&TryToFire, Offsets::TryToFire, &TryToFireOriginal);
							CurrentWeaponHook.Insert(&GetDamageStartLocation, Offsets::GetDamageStartLocation, &GetDamageStartLocationOriginal);
							CurrentWeaponHook.Insert(&GetWeaponTargetingTransform, Offsets::GetWeaponTargetingTransform, &GetWeaponTargetingTransformOriginal);
							CurrentWeaponHook.Insert(&GetReloadTime, Offsets::GetReloadTime, &GetReloadTimeOriginal);

							CurrentWeaponHook.SwapContext();
						}

						OldCurrentWeapon = CurrentWeapon;
					}
				}
			}
		}
	}
}