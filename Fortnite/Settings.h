#pragma once

namespace Settings
{
	namespace Aimbot
	{
		bool SilentAim = true;
		bool BulletTP = false;
		bool MouseAim = false;
		bool StickySilentAim = false;
		namespace StickySilent
		{
			bool StickyCameraSpin = false;
		}
		bool Triggerbot = false;
		bool Interpolate = true;
		bool Constant = false;
		bool TargetLine = true;
		double Smoothness = 20.0;
		double FOV = 20.0;
		bool DrawFOV = true;
		double MaxDistance = 300.0;
		int AimType = 1;
		int BoneType = 0;
		int KeyType = 0;
		double TriggerbotDelay = 50;
		int TriggerbotKeyType = 0;
		bool TriggerBotAlwaysActive = false;
		double TriggerBotFOVSize = 30.0;
		bool Backtrack = false;
		bool SkipKnocked = false;
		bool VisableOnly = false;
		bool Shake = false;
		double ShakeSpeed = 2.0;
		bool Prediction = false;
	}

	namespace Player
	{
		bool Outlines = false;

		int BoxType = 2;
		bool Skeleton = false;
		bool Wireframe = false;

		// Player 2
		bool ShowBots = false;
		bool ShowTeam = true;
		bool ShowPlatform = false;
		bool Distance = false;
		bool Username = false;

		double MaxDistance = 350.0;
	}

	namespace Exploits
	{
		bool FOVChanger = false;
		double FOVChangerValue = 120;
		bool bHasCachedFOVChanger = false;
		float CachedFOVChangerValue;

		bool VehicleFlyTest = false;
		bool AimWhileJumping = false;
		bool RapidFire = false;
		bool fullAutoWeapons = false;
		bool noRecoil = false;
		bool noWeaponCooldown = false;
		bool reloadWhileAiming = false;
		bool NoReload = false;
		bool Spinbot = false;
		bool PlayerFly = false;
		bool DoubleJump = false;
		bool BulletTeleportV3 = false;
		bool BulletTeleportV2 = false;
		bool BulletTeleport = false;
		bool InfiniteFuel = false;
		double CarSpeed = 85;
		bool VehicleTpAura = false;
		bool Hook = true;
		int VehicleFlyType = 0;
		bool CarFly = false;
		bool bCachedHasUsedSpinbot = false;
		double SpinbotSpeed = 5;
		double Spin = 0;
	}

	namespace Misc
	{
		int CrosshairMode = 1;
		bool BulletTraces = false;
	}

	namespace Colors
	{
		FLinearColor TargetLine = FLinearColor(1.f, 0.f, 0.f, 1.f);
		FLinearColor FieldOfView = FLinearColor(1.f, 1.f, 1.f, 1.f);
		FLinearColor BulletColor = FLinearColor(1.f, 1.f, 0.f, 1.f);
		FLinearColor TeammateColor = FLinearColor(0.2f, 0.8f, 0.2f, 1.f);
		FLinearColor BoxVisible = FLinearColor(1.f, 0.f, 0.f, 1.f);
		FLinearColor BoxInVisible = FLinearColor(0.f, 1.f, 1.f, 1.f);
		FLinearColor SkeletonVisible = FLinearColor(1.f, 0.f, 0.f, 1.f);
		FLinearColor SkeletonInVisible = FLinearColor(0.f, 1.f, 1.f, 1.f);
		FLinearColor PlayerTextColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
		FLinearColor SkinColor = FLinearColor(1.f, 1.f, 0.f, 1.f);
	}
}