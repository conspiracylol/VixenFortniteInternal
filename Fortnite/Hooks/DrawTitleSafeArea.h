#pragma once
#include "../DrawMenu.h"

void(*DrawTitleSafeAreaOriginal)(UGameViewportClient* Viewport, UCanvas* Canvas) = nullptr;
void DrawTitleSafeArea(UGameViewportClient* Viewport, UCanvas* Canvas)
{
	if (!Viewport || !Canvas)
		return DrawTitleSafeAreaOriginal(Viewport, Canvas);
	Variables::Canvas = Canvas;
	Variables::ScreenSize = FVector2D(double(Variables::Canvas->ClipX()), double(Variables::Canvas->ClipY()));
	Variables::ScreenCenter = FVector2D(Variables::ScreenSize.X / 2.0, Variables::ScreenSize.Y / 2.0);

	bool Update__GetDamageStartLocation__bSilentAimActive = false;
	FVector Update__GetDamageStartLocation__SilentLocationTarget = FVector();

	bool GetPlayerViewPoint_bShouldStartAtLocationTarget = false;
	FVector GetPlayerViewPoint_SilentLocationTarget = FVector();

	bool Update_SpinningRightNow = false;
	static bool SpinningRightNow = false;
	bool IsCurrentlySickySilentAiming = false;
	bool IsCurrentlySilentAiming = false;

	bool Update_ShootOutOfAir = false;
	FVector Update_LocalHead = FVector();

	bool Update_FieldOfViewChanger = false;
	char UpdateMyTeam = char(1337);
	bool bInVehicle = false;

	UWorld* GWorld = UWorld::GetWorld();
	if (!GWorld) return DrawTitleSafeAreaOriginal(Viewport, Canvas);

	UGameInstance* Gameinstance = GWorld->OwningGameInstance();
	if (!Gameinstance) return DrawTitleSafeAreaOriginal(Viewport, Canvas);

	ULocalPlayer* LocalPlayer = Gameinstance->LocalPlayers()[0];
	if (!LocalPlayer) return DrawTitleSafeAreaOriginal(Viewport, Canvas);

	APlayerController* PlayerController = LocalPlayer->PlayerController();
	if (!PlayerController) return DrawTitleSafeAreaOriginal(Viewport, Canvas);
	Variables::PlayerController = PlayerController;

	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager();
	if (!PlayerCameraManager) return DrawTitleSafeAreaOriginal(Viewport, Canvas);

	APlayerState* PlayerState = PlayerController->PlayerState();
	if (!PlayerState) return DrawTitleSafeAreaOriginal(Viewport, Canvas);

	APlayerPawn_Athena_C* AcknowledgedPawn = PlayerController->AcknowledgedPawn();
	Variables::AcknowledgedPawn = AcknowledgedPawn;

	Variables::FieldOfView = PlayerCameraManager->GetFOVAngle();
	Variables::CameraLocation = PlayerCameraManager->GetCameraLocation();
	Variables::CameraRotation = PlayerCameraManager->GetCameraRotation();

	if (Settings::Exploits::Hook)
	{
		Rehook(LocalPlayer);
	}

	double SmallestDegrees = DBL_MAX;
	FVector SmallestDegreesOwnerWorldLocation = FVector();

	double ClosestDistanceToMyself = DBL_MAX;
	double ClosestDistanceToCenter = DBL_MAX;
	APlayerPawn_Athena_C* TargetPlayer = nullptr;

	double AimbotFOV = (Settings::Aimbot::FOV * Variables::ScreenSize.X / Variables::FieldOfView) / 2.0;

	if (AcknowledgedPawn)
	{
		UpdateMyTeam = AcknowledgedPawn->GetTeam();
		bInVehicle = AcknowledgedPawn->IsInVehicle();

		if (USkeletalMeshComponent* Mesh = AcknowledgedPawn->Mesh())
		{
			Update_LocalHead = Mesh->GetBoneLocation(Bones::head);
		}

		FVector StickyLocation = AcknowledgedPawn->K2_GetActorLocation(); StickyLocation.Z += 100.0;
		Variables::GetCameraViewPoint::StickyLocation = StickyLocation;



		double FarestDistance = DBL_MIN;
		AFortProjectileBase* FarestTPProjectile = nullptr;

		TArray<UObject*> ProjectileArray = UGameplayStatics::GetAllActorsOfClass(GWorld, Classes::ProjectileClass);
		for (int i = 0; i < ProjectileArray.Count; i++)
		{
			AFortProjectileBase* ProjectileActor = reinterpret_cast<AFortProjectileBase*>(ProjectileArray[i]);
			if (!ProjectileActor) continue;

			bool ProjectileIsA = UGameplayStatics::ObjectIsA(ProjectileActor, Classes::ProjectileClass);
			if (!ProjectileIsA) continue;

			FVector2D ScreenLocation = FVector2D();
			FVector WorldLocation = ProjectileActor->K2_GetActorLocation();
			if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, &ScreenLocation))
			{
				double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, WorldLocation) * 0.01;
				if (Distance < 1500.0)
				{
					if (ProjectileActor->GetOwnerWeapon() == AcknowledgedPawn->CurrentWeapon() && Distance > FarestDistance)
					{
						if (ProjectileActor->GetGameTimeSinceCreation() < 7.5f)
						{
							FarestTPProjectile = ProjectileActor;
							FarestDistance = Distance;
						}
					}
				}
			}
		}
	}

	TArray<UObject*> PlayerList = UGameplayStatics::GetAllActorsOfClass(GWorld, Classes::PlayerClass);
	for (int32_t i = 0; i < PlayerList.Count; i++)
	{
		APlayerPawn_Athena_C* Player = reinterpret_cast<APlayerPawn_Athena_C*>(PlayerList[i]);
		if (!Player)
			continue;

		bool PlayerCheck = UGameplayStatics::ObjectIsA(Player, Classes::PlayerClass);
		if (PlayerCheck)
		{
			if (Player == AcknowledgedPawn) continue;

			bool Wound = Player->IsDBNO();

			bool Dead = Player->IsDead();
			if (Dead) continue;

			char Team = Player->GetTeam();
			bool Teammate = UpdateMyTeam != char(1337) && UpdateMyTeam == Team;
			if (Teammate && !Settings::Player::ShowTeam) continue;

			bool Visable = UFortKismetLibrary::CheckLineOfSightToActorWithChannel(Variables::CameraLocation, Player, ECollisionChannel::ECC_Visibility, nullptr);

			USkeletalMeshComponent* Mesh = Player->Mesh();
			if (!Mesh) continue;

			if (Settings::Player::Wireframe)
			{
				int DepthStencil = 11;

				Mesh->SetCustomDepthStencilValue(DepthStencil);

				TArray<USkeletalMeshComponent*> SkeletalMeshes = Player->SkeletalMeshes();
				for (int si = 0; si < SkeletalMeshes.Count; si++)
				{
					USkeletalMeshComponent* SkeletalMesh = SkeletalMeshes[si];
					if (!SkeletalMesh) continue;

					SkeletalMesh->SetRenderCustomDepth(true);
					SkeletalMesh->SetCustomDepthStencilValue(DepthStencil);
				}

				//UMaterial* MaterialObj = (UMaterial*)UObject::FindObject(L"Material M_Elimination_DigitizeCubes.M_Elimination_DigitizeCubes");
				//*(byte*)(MaterialObj + 0x1a8) |= 1 << 0; // bDisableDepthTest
				//printf("bDisableDepthTest: %i\n", MaterialObj + 0x1a8);

				//TArray<UMaterialInterface*> Materials = Mesh->GetMaterials();
				//for (int j = 0; j < Materials.Count; j++) {
				//	Mesh->SetMaterial(j, (UMaterialInterface*)MaterialObj);
				//}

				TArray<UMaterialInstanceDynamic*> MaterialInstances = Player->PawnMaterials_ALL();
				for (int mi = 0; mi < MaterialInstances.Count; mi++)
				{
					UMaterialInstanceDynamic* MaterialInstance = MaterialInstances[mi];
					if (!MaterialInstance) continue;;

					MaterialInstance->SetVectorParameterValue(Variables::MaterialParameter4, Settings::Colors::SkinColor);

					UMaterial* Material = MaterialInstance->GetMaterial();
					if (!Material) continue;

					*(byte*)(Material + 0x1a8) |= 1 << 6; // Wireframe
					*(byte*)(Material + 0x1a8) |= 1 << 0; // bDisableDepthTest
					// old -> 0x1a8
				}

				Mesh->SetRenderCustomDepth(true);
			}

			FLinearColor BoxColor = Settings::Colors::BoxInVisible;
			FLinearColor SkeletonColor = Settings::Colors::SkeletonInVisible;

			if (Visable)
			{
				BoxColor = Settings::Colors::BoxVisible;
				SkeletonColor = Settings::Colors::SkeletonVisible;
			}

			if (Teammate)
			{
				BoxColor = Settings::Colors::TeammateColor;
				SkeletonColor = Settings::Colors::TeammateColor;
			}

			FVector Root = Mesh->GetBoneLocation(Bones::Root);
			double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, Root) * 0.01;

			FVector Head = Mesh->GetBoneLocation(Bones::head);
			FVector2D HeadScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(Head, &HeadScreen)) continue;

			if (HeadScreen.X < 5.0 || HeadScreen.X > Variables::ScreenSize.X - (5.0 * 2)) continue;
			if (HeadScreen.Y < 5.0 || HeadScreen.Y > Variables::ScreenSize.Y - (5.0 * 2)) continue;

			//if (UCharacterMovementComponent* MovementComponent = Player->CharacterMovement())
			//{
			//	Update_ShootOutOfAir = MovementComponent->MovementMode() == EMovementMode::MOVE_Falling;
			//}

			bool AimbotVisable = (Settings::Aimbot::VisableOnly && Visable) || !Settings::Aimbot::VisableOnly;
			bool AimbotKnocked = (Settings::Aimbot::SkipKnocked && !Wound) || !Settings::Aimbot::SkipKnocked;
			bool AimbotValid = (AimbotVisable && AimbotKnocked && !Teammate) && Distance < Settings::Aimbot::MaxDistance;
			if (AimbotValid) {
				if (Distance < ClosestDistanceToMyself) {
					double DistanceToCenter = UKismetMathLibrary::Vector_Distance2D(Variables::ScreenCenter, HeadScreen);
					if (DistanceToCenter < ClosestDistanceToCenter) {
						ClosestDistanceToMyself = Distance;
						ClosestDistanceToCenter = DistanceToCenter;
						TargetPlayer = Player;
					}
				}
			}

			if (Distance > Settings::Player::MaxDistance) continue;

			FVector2D RootScreen;
			if (!PlayerController->ProjectWorldLocationToScreen(Root, &RootScreen)) continue;

			FVector Root2 = FVector(Root.X, Root.Y, Root.Z + 6.0);
			FVector2D RootScreen2;
			if (!PlayerController->ProjectWorldLocationToScreen(Root2, &RootScreen2)) continue;

			FVector Head2 = FVector(Head.X, Head.Y, Head.Z + 18.0);
			FVector2D HeadScreen2;
			if (!PlayerController->ProjectWorldLocationToScreen(Head2, &HeadScreen2)) continue;

			FVector Neck = Mesh->GetBoneLocation(Bones::neck_02);
			FVector2D NeckScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(Neck, &NeckScreen)) continue;

			FVector Chest = Mesh->GetBoneLocation(Bones::spine_05);
			FVector2D ChestScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(Chest, &ChestScreen)) continue;

			FVector Pelvis = Mesh->GetBoneLocation(Bones::pelvis);
			FVector2D PelvisScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(Pelvis, &PelvisScreen)) continue;

			FVector RShoulder = Mesh->GetBoneLocation(Bones::upperarm_r);
			FVector2D RShoulderScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RShoulder, &RShoulderScreen)) continue;

			FVector RElbow = Mesh->GetBoneLocation(Bones::lowerarm_r);
			FVector2D RElbowScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RElbow, &RElbowScreen)) continue;

			FVector RElbow2 = FVector(RElbow.X + 8.0, RElbow.Y, RElbow.Z);
			FVector2D RElbowScreen2 = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RElbow2, &RElbowScreen2)) continue;

			FVector RHand = Mesh->GetBoneLocation(Bones::hand_r);
			FVector2D RHandScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RHand, &RHandScreen)) continue;

			FVector RThigh = Mesh->GetBoneLocation(Bones::thigh_r);
			FVector2D RThighScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RThigh, &RThighScreen)) continue;

			FVector RCalf = Mesh->GetBoneLocation(Bones::calf_r);
			FVector2D RCalfScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RCalf, &RCalfScreen)) continue;

			FVector RKnee = Mesh->GetBoneLocation(Bones::calf_twist_01_r);
			FVector2D RKneeScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RKnee, &RKneeScreen)) continue;

			FVector RFoot = Mesh->GetBoneLocation(Bones::foot_r);
			FVector2D RFootScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(RFoot, &RFootScreen)) continue;

			FVector LShoulder = Mesh->GetBoneLocation(Bones::upperarm_l);
			FVector2D LShoulderScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LShoulder, &LShoulderScreen)) continue;

			FVector LElbow = Mesh->GetBoneLocation(Bones::lowerarm_l);
			FVector2D LElbowScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LElbow, &LElbowScreen)) continue;

			FVector LElbow2 = FVector(LElbow.X + 8.0, LElbow.Y, LElbow.Z);
			FVector2D LElbowScreen2 = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LElbow2, &LElbowScreen2)) continue;

			FVector LHand = Mesh->GetBoneLocation(Bones::hand_l);
			FVector2D LHandScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LHand, &LHandScreen)) continue;

			FVector LThigh = Mesh->GetBoneLocation(Bones::thigh_l);
			FVector2D LThighScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LThigh, &LThighScreen)) continue;

			FVector LCalf = Mesh->GetBoneLocation(Bones::calf_l);
			FVector2D LCalfScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LCalf, &LCalfScreen)) continue;

			FVector LKnee = Mesh->GetBoneLocation(Bones::calf_twist_01_l);
			FVector2D LKneeScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LKnee, &LKneeScreen)) continue;

			FVector LFoot = Mesh->GetBoneLocation(Bones::foot_l);
			FVector2D LFootScreen = FVector2D();
			if (!PlayerController->ProjectWorldLocationToScreen(LFoot, &LFootScreen)) continue;

			FVector2D BonesToCheck[] = { RootScreen, RootScreen2, HeadScreen2, NeckScreen, ChestScreen, PelvisScreen, RShoulderScreen, RElbowScreen, RElbowScreen2, RHandScreen, RCalfScreen, RKneeScreen, RFootScreen, LShoulderScreen, LElbowScreen, LElbowScreen2, LHandScreen, LCalfScreen, LKneeScreen, LFootScreen };

			double MostLeft = DBL_MAX;
			double MostRight = DBL_MIN;
			double MostTop = DBL_MAX;
			double MostBottom = DBL_MIN;

			for (int ci = 0; ci < sizeof(BonesToCheck) / sizeof(FVector2D); ci++)
			{
				FVector2D CurrentBone = BonesToCheck[ci];

				if (CurrentBone.X < MostLeft)
					MostLeft = CurrentBone.X;

				if (CurrentBone.X > MostRight)
					MostRight = CurrentBone.X;

				if (CurrentBone.Y < MostTop)
					MostTop = CurrentBone.Y;

				if (CurrentBone.Y > MostBottom)
					MostBottom = CurrentBone.Y;
			}

			double ActorHeight = MostBottom - MostTop;
			double ActorWidth = MostRight - MostLeft;

			double DistanceDifference = 225.0 - Distance;
			double DistanceOffset = DistanceDifference * 0.03;
			double CornerWidth = ActorWidth / 4;
			double CornerHeight = ActorHeight / 3;
			double ThreeDimensionalWidth = ActorWidth / 3;

			double TopTextOffset = 52.0;
			double BottomTextOffset = 7.0;

			FVector2D BottomMiddle = FVector2D(MostLeft + (ActorWidth / 2.0), MostBottom);

			if (Settings::Player::Skeleton)
			{
				Wrapper::Line(HeadScreen, NeckScreen, SkeletonColor, 1.f);
				Wrapper::Line(NeckScreen, ChestScreen, SkeletonColor, 1.f);
				Wrapper::Line(ChestScreen, PelvisScreen, SkeletonColor, 1.f);

				Wrapper::Line(ChestScreen, RShoulderScreen, SkeletonColor, 1.f);
				Wrapper::Line(RShoulderScreen, RElbowScreen, SkeletonColor, 1.f);
				Wrapper::Line(RElbowScreen, RHandScreen, SkeletonColor, 1.f);

				Wrapper::Line(ChestScreen, LShoulderScreen, SkeletonColor, 1.f);
				Wrapper::Line(LShoulderScreen, LElbowScreen, SkeletonColor, 1.f);
				Wrapper::Line(LElbowScreen, LHandScreen, SkeletonColor, 1.f);

				Wrapper::Line(PelvisScreen, RThighScreen, SkeletonColor, 1.f);
				Wrapper::Line(RThighScreen, RCalfScreen, SkeletonColor, 1.f);
				Wrapper::Line(RCalfScreen, RKneeScreen, SkeletonColor, 1.f);
				Wrapper::Line(RKneeScreen, RFootScreen, SkeletonColor, 1.f);

				Wrapper::Line(PelvisScreen, LThighScreen, SkeletonColor, 1.f);
				Wrapper::Line(LThighScreen, LCalfScreen, SkeletonColor, 1.f);
				Wrapper::Line(LCalfScreen, LKneeScreen, SkeletonColor, 1.f);
				Wrapper::Line(LKneeScreen, LFootScreen, SkeletonColor, 1.f);
			}

			if (Settings::Player::Distance)
			{
				FVector PlayerHeadLocation = Mesh->GetBoneLocation(Bones::head);

				double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, PlayerHeadLocation) * 0.01;

				if (Distance)
				{
					FString DistanceText = UKismetStringLibrary::BuildString_Double(L"", L"", std::round(Distance), L"m");

					Wrapper::Text(DistanceText, FVector2D(HeadScreen.X, HeadScreen.Y + TopTextOffset * 2), Settings::Colors::PlayerTextColor);
					TopTextOffset += 30.0;
				}
			}



			if (Settings::Player::ShowBots && AcknowledgedPawn)
			{
				auto Platform = Player->PlayerState()->GetPlatformSDK();
				if (!Platform.c_str())
				{
					Wrapper::Text(Platform, FVector2D(HeadScreen.X, HeadScreen.Y + TopTextOffset * 2), Settings::Colors::PlayerTextColor);
					TopTextOffset += 30.0;
				}
				else if (Settings::Player::ShowPlatform)
				{
					Wrapper::Text(Platform, FVector2D(HeadScreen.X, HeadScreen.Y + TopTextOffset * 2), Settings::Colors::PlayerTextColor);
					TopTextOffset += 30.0;
				}
			}

			if (Settings::Player::ShowPlatform && !Settings::Player::ShowBots && AcknowledgedPawn)
			{
				auto Platform = Player->PlayerState()->GetPlatformSDK();
				if (Platform.c_str())
				{
					Wrapper::Text(Platform, FVector2D(HeadScreen.X, HeadScreen.Y + TopTextOffset * 2), Settings::Colors::PlayerTextColor);
					TopTextOffset += 30.0;
				}
			}

			if (Settings::Player::Username)
			{
				FString PlayerName = Player->GetPlayerNameSafe(TargetPlayer, Classes::ActorPlayerNamePrivate);

				if (PlayerName.c_str())
				{
					Wrapper::Text(PlayerName, FVector2D(HeadScreen.X, HeadScreen.Y + TopTextOffset * 2), Settings::Colors::PlayerTextColor);
					TopTextOffset += 30.0;
				}
			}

			if (Settings::Player::BoxType == 1)
			{
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), BoxColor, 1.5f);
			}
			else if (Settings::Player::BoxType == 2)
			{
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), FVector2D(MostLeft - DistanceOffset + CornerWidth, MostTop - DistanceOffset), BoxColor, 2.f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset + CornerHeight), BoxColor, 2.f);

				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset - CornerWidth, MostTop - DistanceOffset), BoxColor, 2.f);
				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset + CornerHeight), BoxColor, 2.f);

				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostLeft - DistanceOffset + CornerWidth, MostBottom + DistanceOffset), BoxColor, 2.f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset - CornerHeight), BoxColor, 2.f);

				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostRight + DistanceOffset - CornerWidth, MostBottom + DistanceOffset), BoxColor, 2.f);
				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset - CornerHeight), BoxColor, 2.f);
			}
			else if (Settings::Player::BoxType == 3)
			{
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostLeft - DistanceOffset, MostBottom + DistanceOffset), FVector2D(MostLeft - DistanceOffset, MostTop - DistanceOffset), BoxColor, 1.5f);
				Wrapper::Line(FVector2D(MostRight + DistanceOffset, MostTop - DistanceOffset), FVector2D(MostRight + DistanceOffset, MostBottom + DistanceOffset), BoxColor, 1.5f);

				// Define the top-left and bottom-right coordinates of the box to fill
				FVector2D BoxTopLeft(MostLeft - DistanceOffset, MostTop - DistanceOffset);
				FVector2D BoxBottomRight(MostRight + DistanceOffset, MostBottom + DistanceOffset);

				// Draw the filled rectangle inside the box
				Wrapper::RectFilled(BoxTopLeft, BoxBottomRight, FLinearColor(1.f, 1.f, 1.f, 0.4f), false);
			}
		}
	}

	if (AcknowledgedPawn)
	{
		if (!TargetPlayer)
		{
			IsCurrentlySickySilentAiming = false;
			GetPlayerViewPoint_bShouldStartAtLocationTarget = false;
			IsCurrentlySilentAiming = false;
			Update__GetDamageStartLocation__bSilentAimActive = false;
			Variables::GetDamageStartLocation::bSilentAimActive = false;
			Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
			Variables::GetWeaponTargetingTransform::bSilentAimActive = false;
			Variables::GetTargetingSourceLocation::bSilentAimActive = false;
			Variables::GetPlayerViewPoint::bSilentAimActive = false;
			Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
			Variables::GetWeaponTargetingTransform::bSilentAimActive = false;
		}

		if (AcknowledgedPawn && Settings::Exploits::Spinbot)
		{
			auto Mesh = AcknowledgedPawn->Mesh();
			if (Mesh)
			{
				if (Settings::Exploits::Spin >= 359)
				{
					Settings::Exploits::Spin = 0;
				}

				Settings::Exploits::Spin += Settings::Exploits::SpinbotSpeed;

				Settings::Exploits::bCachedHasUsedSpinbot = true;

				*(FRotator*)(Mesh + 0x138) = FRotator(0, Settings::Exploits::Spin, 0);

			}
		}
		else if (Settings::Exploits::bCachedHasUsedSpinbot)
		{
			Settings::Exploits::bCachedHasUsedSpinbot = false;

			auto Mesh = AcknowledgedPawn->Mesh();
			if (Mesh)
			{
				*(FRotator*)(Mesh + 0x138) = FRotator(0, -90, 0);
			}
		}

		if (AcknowledgedPawn && PlayerController && Settings::Exploits::FOVChanger) // prevent numerous calls every drwa frame
		{
			if (!Settings::Exploits::bHasCachedFOVChanger || (Settings::Exploits::FOVChangerValue != Settings::Exploits::CachedFOVChangerValue))
			{
				Settings::Exploits::CachedFOVChangerValue = Settings::Exploits::FOVChangerValue;
				PlayerController->FOV(static_cast<float>(Settings::Exploits::FOVChangerValue));
				Settings::Exploits::bHasCachedFOVChanger = true; // set cached to true so we dont constantly call if it hasnt been turned on or hasnt changed
			}
		}
		else if (Settings::Exploits::bHasCachedFOVChanger)
		{
			Settings::Exploits::bHasCachedFOVChanger = false; // reset cached to false

			Settings::Exploits::CachedFOVChangerValue = 80; // reset to non cached lol

			if (PlayerController)
			{
				PlayerController->FOV(80.f); // default
			}
		}

		if (Settings::Aimbot::AimType)
		{
			if (Settings::Aimbot::DrawFOV)
			{
				Wrapper::Circle(Variables::ScreenCenter, Settings::Colors::FieldOfView, AimbotFOV, 128.0);
			}

			FKey AimbotKey = Keys::RightMouseButton;

			if (Settings::Aimbot::KeyType == 1)
				AimbotKey = Keys::LeftMouseButton;
			else if (Settings::Aimbot::KeyType == 2)
				AimbotKey = Keys::LeftShift;
			else if (Settings::Aimbot::KeyType == 3)
				AimbotKey = Keys::ThumbMouseButton;
			else if (Settings::Aimbot::KeyType == 4)
				AimbotKey = Keys::ThumbMouseButton2;

			if (APlayerPawn_Athena_C* Target = TargetPlayer)
			{
				if (USkeletalMeshComponent* Mesh = Target->Mesh())
				{
					if (AFortWeapon* CurrentWeapon = AcknowledgedPawn->CurrentWeapon())
					{
						bool IsHoldingWeapon = UGameplayStatics::ObjectIsA(CurrentWeapon, Classes::RangedWeaponClass);

						if (IsHoldingWeapon)
						{
							bool IsHoldingPickaxe = UGameplayStatics::ObjectIsA(CurrentWeapon, Classes::PickaxeClass);
							if (!IsHoldingPickaxe)
							{
								static Bones TargetBoneIndex = Bones::head;

								if (Settings::Aimbot::BoneType == 1)
									TargetBoneIndex = Bones::neck_02;
								else if (Settings::Aimbot::BoneType == 2)
									TargetBoneIndex = Bones::spine_05;
								else if (Settings::Aimbot::BoneType == 3) {
									static double LastBoneChangeTime = 0.00;
									const Bones BoneArray[] = {
										Bones::neck_01,
										Bones::neck_02,
										Bones::spine_01,
										Bones::spine_02,
										Bones::spine_03,
										Bones::clavicle_r,
										Bones::clavicle_l
									};

									const int NumBones = sizeof(BoneArray) / sizeof(Bones);

									if (UKismetSystemLibrary::GetGameTimeInSeconds(GWorld) - LastBoneChangeTime >= 1.00)
									{
										int32_t RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, NumBones - 1);
										TargetBoneIndex = BoneArray[RandomIndex];

										LastBoneChangeTime = UKismetSystemLibrary::GetGameTimeInSeconds(GWorld);
									}
								}




								FVector2D TargetScreen = FVector2D();
								FVector TargetWorld = Mesh->GetBoneLocation(TargetBoneIndex);


								if (PlayerController->ProjectWorldLocationToScreen(TargetWorld, &TargetScreen))
								{

									if (Custom::InCircle(Variables::ScreenCenter.X, Variables::ScreenCenter.Y, Settings::Aimbot::TriggerBotFOVSize, TargetScreen.X, TargetScreen.Y))
									{
										if (Settings::Aimbot::Triggerbot)
										{

											FKey TriggerBotKey = Keys::RightMouseButton;
											if (Settings::Aimbot::TriggerbotKeyType == 2)
											{
												TriggerBotKey = Keys::LeftMouseButton;
											}
											else if (Settings::Aimbot::TriggerbotKeyType == 3)
											{
												TriggerBotKey = Keys::LeftShift;
											}

											if (Settings::Aimbot::TriggerBotAlwaysActive || PlayerController->IsInputKeyDown(TriggerBotKey))
											{
												static std::chrono::time_point<std::chrono::steady_clock> LastTimePoint = std::chrono::steady_clock::now();
												auto CurTimePoint = std::chrono::steady_clock::now();

												if (CurTimePoint - LastTimePoint >= std::chrono::milliseconds(static_cast<long long>(Settings::Aimbot::TriggerbotDelay))) // delay
												{
													const bool isAlreadyShooting = GetAsyncKeyState(VK_LBUTTON) < 0;

													if (!isAlreadyShooting)
													{
														mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
														mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
													}
												}
											}
										}
									}

									if (Custom::InCircle(Variables::ScreenCenter.X, Variables::ScreenCenter.Y, AimbotFOV, TargetScreen.X, TargetScreen.Y))
									{
										Variables::BulletTeleportHead = TargetWorld;

										if (Settings::Aimbot::TargetLine)
										{
											FVector2D MuzzleScreen = FVector2D();
											FVector Muzzle = CurrentWeapon->GetMuzzleLocation();
											if (PlayerController->ProjectWorldLocationToScreen(Muzzle, &MuzzleScreen))
											{
												Wrapper::Line(MuzzleScreen, TargetScreen, Settings::Colors::TargetLine, 1.f);
											}
										}


										/*if (Settings::Exploits::BulletTeleport && TargetPlayer && AcknowledgedPawn)
										{
											TArray<UObject*> ProjectileArray = UGameplayStatics::GetAllActorsOfClass(GWorld, Classes::ProjectileClass);
											for (int i = 0; i < ProjectileArray.Count; i++)
											{
												AFortProjectileBase* ProjectileActor = reinterpret_cast<AFortProjectileBase*>(ProjectileArray[i]);
												if (!ProjectileActor) continue;

												bool ProjectileIsA = UGameplayStatics::ObjectIsA(ProjectileActor, Classes::ProjectileClass);
												if (!ProjectileIsA) continue;

												FVector2D ScreenLocation = FVector2D();
												FVector WorldLocation = ProjectileActor->K2_GetActorLocation();
												if (TargetPlayer && AcknowledgedPawn)
												{
													static Bones TargetBoneIndex = Bones::head;

													if (Settings::Aimbot::BoneType == 1)
														TargetBoneIndex = Bones::neck_02;
													else if (Settings::Aimbot::BoneType == 2)
														TargetBoneIndex = Bones::spine_05;

													ProjectileActor->K2_TeleportTo(Variables::BulletTeleportHead, TargetPlayer->K2_GetActorRotation());
												}
											}
										}*/


										if (Settings::Aimbot::AimType == 1)
										{
											if (PlayerController->IsInputKeyDown(AimbotKey) || PlayerController->GetInputAxisKeyValue(Keys::GamepadLeftTrigger) >= 0.15f)
											{
												FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Variables::CameraLocation, TargetWorld);

												if (Settings::Aimbot::Shake)
												{
													double CurrentTime = UGameplayStatics::GetTimeSeconds(GWorld);

													double ShakeFactor = UKismetMathLibrary::sin(CurrentTime * Settings::Aimbot::ShakeSpeed);

													double MaxPitchShake = 5.0;

													double PitchShake = MaxPitchShake * ShakeFactor;

													TargetRotation.Pitch += PitchShake;
												}

												if (!Settings::Aimbot::Prediction)
												{
													Custom::AddInput(GWorld, PlayerController, Variables::CameraRotation, TargetRotation, Settings::Aimbot::Interpolate, Settings::Aimbot::Constant, 30.0 - Settings::Aimbot::Smoothness);
												}
												else
												{
													float projectileSpeed = 75000; // should work for abt everything
													float projectileGravityScale = 3.5f; // should work for abt everything

													FVector TargetBoneLocation = Mesh->GetBoneLocation(TargetBoneIndex);

													double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, TargetBoneLocation) * 0.01;

													auto Velocity = Target->GetVelocity();

													FVector PredictionLocation = Custom::Prediction(TargetBoneLocation, Velocity, static_cast<float>(Distance), projectileSpeed);


													FRotator PredictionTargetRotation = UKismetMathLibrary::FindLookAtRotation(PredictionLocation, TargetWorld);

													Custom::AddInput(GWorld, PlayerController, Variables::CameraRotation, PredictionTargetRotation, true, Settings::Aimbot::Constant, 30.0 - Settings::Aimbot::Smoothness);
												}
											}
										}

										else if (Settings::Aimbot::AimType == 2)
										{
											if (PlayerController->IsInputKeyDown(AimbotKey) || PlayerController->GetInputAxisKeyValue(Keys::GamepadLeftTrigger) >= 0.15f)
											{
												if (Settings::Aimbot::StickySilentAim)
												{
													if (TargetPlayer)
													{
														IsCurrentlySickySilentAiming = true;

														GetPlayerViewPoint_bShouldStartAtLocationTarget = true;
														GetPlayerViewPoint_SilentLocationTarget = TargetWorld;

														Variables::GetPlayerViewPoint::SilentLocationTarget = GetPlayerViewPoint_SilentLocationTarget;
														Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = true;
													}
												}
											}
											else
											{
												IsCurrentlySickySilentAiming = false;
												GetPlayerViewPoint_bShouldStartAtLocationTarget = false;
												Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
											}
										}
										else if (Settings::Aimbot::AimType == 3)
										{
											if (PlayerController->IsInputKeyDown(AimbotKey) || PlayerController->GetInputAxisKeyValue(Keys::GamepadLeftTrigger) >= 0.15f)
											{
												if (Settings::Aimbot::SilentAim)
												{
													if (TargetPlayer)
													{
														FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Variables::CameraLocation, TargetWorld);

														IsCurrentlySickySilentAiming = true;


														Variables::GetPlayerViewPoint::SilentLocationTarget = TargetWorld;
														Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = true;
														Variables::GetDamageStartLocation::bSilentAimActive = true;
														Variables::GetDamageStartLocation::SilentLocationTarget = TargetWorld;
														Variables::GetCameraViewPoint::StickyLocation = TargetWorld;
														Variables::GetCameraViewPoint::bFreecamActive = true;
														Variables::GetPlayerViewPoint::bSilentAimActive = true;
													}

												}
											}
											else
											{
												IsCurrentlySickySilentAiming = false;
												GetPlayerViewPoint_bShouldStartAtLocationTarget = false;
												Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
												Variables::GetWeaponTargetingTransform::bSilentAimActive = false;
												Variables::GetDamageStartLocation::bSilentAimActive = false;
												Variables::GetCameraViewPoint::bFreecamActive = false;
												Variables::GetPlayerViewPoint::bSilentAimActive = false;
												Variables::GetCameraViewPoint::bFreecamActive = false;
												Variables::GetDamageStartLocation::bSilentAimActive = false;
												Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
												Variables::GetPlayerViewPoint::bSilentAimActive = false;


											}
										}
									}

									if (Settings::Exploits::BulletTeleport && TargetPlayer && AcknowledgedPawn)
									{
										TArray<UObject*> projectile_array = UGameplayStatics::GetAllActorsOfClass((UObject*)GWorld, Classes::ProjectileClass); // GameplayStatics->
										for (int i = 0; i < projectile_array.Size(); i++) {
											if (!projectile_array.Valid(i)) continue;

											auto projectile = (AActor*)projectile_array[i];
											if (!projectile) continue;

											auto object_name = UKismetSystemLibrary::GetObjectName(projectile); //KismetSystemLibrary->

											FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Variables::CameraLocation, Variables::BulletTeleportHead);
											projectile->K2_SetActorLocation(Variables::BulletTeleportHead);
										}
									}
								}
							}
						}

						if (Settings::Exploits::AimWhileJumping && AcknowledgedPawn && PlayerController)
						{

							if ((uintptr_t)AcknowledgedPawn + Offsets::bAdsWhileNotOnGround)
							{
								*(bool*)(AcknowledgedPawn + Offsets::bAdsWhileNotOnGround) = true;
							}
						}

						if (Settings::Exploits::reloadWhileAiming && AcknowledgedPawn && PlayerController)
						{
							auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();

							if (CurrentWeapon)
							{
								*(bool*)(CurrentWeapon + Offsets::WeaponData + 0x41f) = false;
								*(bool*)(CurrentWeapon + Offsets::WeaponData + 0x41f) = false;
							}
						}

						if (Settings::Exploits::VehicleTpAura && TargetPlayer && AcknowledgedPawn)
						{
							AFortAthenaVehicle* CurrentVehicle = AcknowledgedPawn->CurrentVehicle();
							if (UKismetSystemLibrary::IsValid(CurrentVehicle))
							{
								FVector CurrentLocation = CurrentVehicle->K2_GetActorLocation();
								FVector TargetLocationHelper = CurrentLocation;

								TargetLocationHelper.Y += 15;

								FVector TargetLocation = TargetPlayer->K2_GetActorLocation();
								FRotator LocalRotation = AcknowledgedPawn->K2_GetActorRotation();

								FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetLocation);

								TargetLocation.Y += 25;
								CurrentVehicle->K2_TeleportTo(TargetLocation, LocalRotation);
								CurrentVehicle->RootComponent()->RelativeRotation();
							}
						}



						if (Settings::Exploits::BulletTeleportV3 && TargetPlayer && AcknowledgedPawn)
						{
							FVector TargetLocation = TargetPlayer->K2_GetActorLocation();
							FRotator TargetRotation = TargetPlayer->K2_GetActorRotation();
							FRotator LocalRotation = AcknowledgedPawn->K2_GetActorRotation();
							FVector LocalLocation = AcknowledgedPawn->K2_GetActorLocation();

							FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(Variables::CameraLocation, TargetLocation);
							double CleanDistance = 0;
							double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, TargetLocation) * 0.01;
							CleanDistance = std::round(Distance);

							if (15.0 >= CleanDistance)
							{
								if (PlayerController->IsInputKeyDown(Keys::Capslock))
								{
									FVector TeleportLocation = TargetLocation;
									TeleportLocation.X -= 2;
									TeleportLocation.X -= (CleanDistance / 2.0);


									AcknowledgedPawn->K2_SetActorLocationAndRotation(TeleportLocation, TargetLookAtRotation);
								}
								else
								{
									if (Settings::Aimbot::TargetLine)
									{
										FVector2D MuzzleScreen = FVector2D();
										FVector Muzzle = CurrentWeapon->GetMuzzleLocation();

										static Bones TargetBoneIndex = Bones::head;

										if (Settings::Aimbot::BoneType == 1)
											TargetBoneIndex = Bones::neck_02;
										else if (Settings::Aimbot::BoneType == 2)
											TargetBoneIndex = Bones::spine_05;
										else if (Settings::Aimbot::BoneType == 3) {

											FVector2D TargetScreen = FVector2D();
											FVector TargetWorld = Mesh->GetBoneLocation(TargetBoneIndex);

											if (PlayerController->ProjectWorldLocationToScreen(TargetWorld, &TargetScreen))
											{

												if (PlayerController->ProjectWorldLocationToScreen(Muzzle, &MuzzleScreen))
												{
													Wrapper::Line(MuzzleScreen, TargetScreen, FLinearColor(1.f, 0.35f, 0.1f, 1.f), 2.f);
												}
											}
										}
									}
									// note: maybe sticky silent player locaiton and use sticky silent aim so no aids lag
								}
							}
						}
						else
						{
							Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
							Variables::GetWeaponTargetingTransform::bSilentAimActive = false;
							Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
							Variables::GetPlayerViewPoint::bSilentAimActive = false;
						}
					}
				}
			}
			else
			{
				Variables::GetPlayerViewPoint::bShouldStartAtLocationTarget = false;
			}

			if (Settings::Exploits::NoReload && AcknowledgedPawn)
			{
				if (AcknowledgedPawn->CurrentWeapon())
				{
					if (AcknowledgedPawn->CurrentWeapon()->IsReloading())
					{
						uintptr_t MeshPtr = (uintptr_t)AcknowledgedPawn->Mesh();
						if (MeshPtr)
						{
							if (MeshPtr + Offsets::GlobalAnimRateScale)
							{
								*(float*)(MeshPtr + Offsets::GlobalAnimRateScale) = 999; // 0xA18 = GlobalAnimRateScale
							}
						}
					}
					else
					{
						uintptr_t MeshPtr = (uintptr_t)AcknowledgedPawn->Mesh();
						if (MeshPtr)
						{
							if (MeshPtr + Offsets::GlobalAnimRateScale)
							{
								*(float*)(MeshPtr + Offsets::GlobalAnimRateScale) = 1; // 0xA18 = GlobalAnimRateScale
							}
						}
					}
				}
			}

			if (Settings::Exploits::RapidFire && AcknowledgedPawn)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();

				if ((uintptr_t)CurrentWeapon)
				{
					uintptr_t LastFireTimeOffset = 0xe34;
					uintptr_t LastFireTimeVerifiedOffset = 0xe38;

					if ((uintptr_t)CurrentWeapon + LastFireTimeOffset)
					{
						*(float*)(CurrentWeapon + LastFireTimeOffset) = 0;
						*(float*)(CurrentWeapon + LastFireTimeVerifiedOffset) = 0;
					}
				}
			}

			if (Settings::Exploits::noWeaponCooldown && AcknowledgedPawn)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();

				if (CurrentWeapon)
				{
					if ((uintptr_t)CurrentWeapon + Offsets::CustomTimeDilation)
					{
						*(float*)(CurrentWeapon + Offsets::CustomTimeDilation) = FLT_MAX;
					}
				}
			}

			if (Settings::Exploits::noRecoil && AcknowledgedPawn && PlayerController)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();

				if (CurrentWeapon)
				{
					if ((uintptr_t)PlayerController + Offsets::CustomTimeDilation)
					{
						*(float*)(PlayerController + Offsets::CustomTimeDilation) = FLT_MAX;
					}
				}
			}

			if (Settings::Exploits::fullAutoWeapons && AcknowledgedPawn && PlayerController)
			{
				auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();

				if (CurrentWeapon)
				{
					auto WeaponData = AcknowledgedPawn->CurrentWeapon()->WeaponData();
					if (WeaponData)
					{
						if ((uintptr_t)WeaponData + Offsets::TriggerType)
						{
							*(uint8_t*)(WeaponData + Offsets::TriggerType) = 1;
						}
					}
				}
			}

			/*
			if (Settings::Exploits::noWeaponCooldown && AcknowledgedPawn)
			{
				if (AcknowledgedPawn->CurrentWeapon())
				{
					auto CurrentWeapon = AcknowledgedPawn->CurrentWeapon();
					uintptr_t CustomTimeDilationOffset = 0x64;
					if (CurrentWeapon + CustomTimeDilationOffset) // CustomTimeDilation
					{
						*(float*)(CurrentWeapon + CustomTimeDilationOffset) = FLT_MAX;
					}
				}
			}*/

			/*
			if (Settings::Exploits::DoubleJump && AcknowledgedPawn)
			{
				if (PlayerController->IsInputKeyDown(Keys::SpaceBar))
				{
					FRotator LocalRotation = AcknowledgedPawn->K2_GetActorRotation();
					FVector LocalLocation = AcknowledgedPawn->K2_GetActorLocation();
					LocalLocation.Y += 7;

					AcknowledgedPawn->K2_TeleportTo(LocalLocation, LocalRotation);
				}
			}*/

			if (Settings::Exploits::CarFly && Settings::Exploits::VehicleFlyType != 1)
			{
				AFortAthenaVehicle* CurrentVehicle = AcknowledgedPawn->CurrentVehicle();
				if (UKismetSystemLibrary::IsValid(CurrentVehicle))
				{
					FVector CurrentLocation = CurrentVehicle->K2_GetActorLocation();
					if (PlayerController->IsInputKeyDown(Keys::W)) {
						CurrentLocation = CurrentLocation + UKismetMathLibrary::GetForwardVector(Variables::CameraRotation) * Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}
					if (PlayerController->IsInputKeyDown(Keys::A)) {
						CurrentLocation = CurrentLocation - UKismetMathLibrary::GetRightVector(Variables::CameraRotation) * Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}
					if (PlayerController->IsInputKeyDown(Keys::S)) {
						CurrentLocation = CurrentLocation - UKismetMathLibrary::GetForwardVector(Variables::CameraRotation) * Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}
					if (PlayerController->IsInputKeyDown(Keys::D))
					{
						CurrentLocation = CurrentLocation + UKismetMathLibrary::GetRightVector(Variables::CameraRotation) * Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}

					if (PlayerController->IsInputKeyDown(Keys::SpaceBar))
					{
						CurrentLocation.Z = CurrentLocation.Z + Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}

					if (PlayerController->IsInputKeyDown(Keys::LeftShift))
					{
						CurrentLocation.Z = CurrentLocation.Z - Settings::Exploits::CarSpeed;
						CurrentVehicle->K2_TeleportTo(CurrentLocation, Variables::CameraRotation);
					}

				}
			}

			if (Settings::Exploits::VehicleFlyType == 1 && PlayerController->WasInputKeyJustPressed(Keys::F8))
			{
				AFortAthenaVehicle* CurrentVehicle = AcknowledgedPawn->CurrentVehicle();
				if (UKismetSystemLibrary::IsValid(CurrentVehicle))
				{
					double ClosestMarker = DBL_MAX;
					AFortPlayerMarkerBase* TargetMarker = nullptr;

					TArray<UObject*> MarkerArray = UGameplayStatics::GetAllActorsOfClass(GWorld, Classes::MarkerClass);
					for (int i = 0; i < MarkerArray.Count; i++)
					{
						AFortPlayerMarkerBase* MarkerActor = reinterpret_cast<AFortPlayerMarkerBase*>(MarkerArray[i]);
						if (!MarkerActor) continue;

						bool MarkerIsA = UGameplayStatics::ObjectIsA(MarkerActor, Classes::MarkerClass);
						if (!MarkerIsA) continue;

						FVector WorldLocation = MarkerActor->K2_GetActorLocation();
						double Distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, WorldLocation) * 0.01;
						if (Distance < ClosestMarker)
						{
							ClosestMarker = Distance;
							TargetMarker = MarkerActor;
						}
					}

					if (TargetMarker)
					{
						FVector WorldLocation = TargetMarker->K2_GetActorLocation();
						FVector PlayerWorldLocation = AcknowledgedPawn->K2_GetActorLocation();

						if (WorldLocation)
						{
							WorldLocation.Y = PlayerWorldLocation.Y; // fixed
							CurrentVehicle->K2_SetActorLocationAndRotation(WorldLocation, Variables::CameraRotation);
						}
					}
				}
			}


			if (Settings::Exploits::InfiniteFuel)
			{
				AFortAthenaVehicle* CurrentVehicle = AcknowledgedPawn->CurrentVehicle();

				if (UKismetSystemLibrary::IsValid(CurrentVehicle))
				{
					UFortVehicleFuelComponent* CachedFuelComponent = CurrentVehicle->CachedFuelComponent();

					//CachedFuelComponent->SetFuel(1337.f);

					*(float*)((uintptr_t)CurrentVehicle + Offsets::CachedFuelComponent + Offsets::ServerFuel) = 1337.f; // cached fuel -> server fuel
				}
			}

			if (Settings::Misc::BulletTraces && AcknowledgedPawn && PlayerController)
			{
				for (int ri = 0; ri < 5; ri++)
				{
					FVector BulletImpact = Variables::Last5BulletImpacts[ri];
					FVector BulletImpactMuzzle = Variables::Last5BulletImpactsMuzzle[ri];

					if (BulletImpact && BulletImpactMuzzle)
					{
						FVector2D P1;
						FVector2D P2;
						PlayerController->ProjectWorldLocationToScreen(BulletImpact, &P1);
						PlayerController->ProjectWorldLocationToScreen(BulletImpactMuzzle, &P2);

						Wrapper::Line(P1, P2, Settings::Colors::BulletColor, 1.f);
					}
				}
			}

		}
		if (Settings::Misc::CrosshairMode)
		{
			if (Settings::Misc::CrosshairMode == 1)
			{
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - 10, Variables::ScreenCenter.Y), FVector2D(Variables::ScreenCenter.X + 10, Variables::ScreenCenter.Y), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 1.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y - 10), FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y + 10), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 1.f);
			}
			else if (Settings::Misc::CrosshairMode == 2)
			{
				double crosshair_outer = double(14);
				double crosshair_inner = double(8);

				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - crosshair_outer, Variables::ScreenCenter.Y), FVector2D(Variables::ScreenCenter.X - crosshair_inner, Variables::ScreenCenter.Y), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X + crosshair_inner, Variables::ScreenCenter.Y), FVector2D(Variables::ScreenCenter.X + crosshair_outer, Variables::ScreenCenter.Y), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y - crosshair_outer), FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y - crosshair_inner), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y + crosshair_inner), FVector2D(Variables::ScreenCenter.X, Variables::ScreenCenter.Y + crosshair_outer), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 2.f);
			}
			else if (Settings::Misc::CrosshairMode == 3)
			{
				Wrapper::Circle(Variables::ScreenCenter, FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 5.0, 38.0, true);
			}
			else if (Settings::Misc::CrosshairMode == 4)
			{
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - 10, Variables::ScreenCenter.Y - 10), FVector2D(Variables::ScreenCenter.X + 10, Variables::ScreenCenter.Y + 10), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 1.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - 10, Variables::ScreenCenter.Y + 10), FVector2D(Variables::ScreenCenter.X + 10, Variables::ScreenCenter.Y - 10), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), 1.f);

				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - 10, Variables::ScreenCenter.Y - 10), FVector2D(Variables::ScreenCenter.X - 5, Variables::ScreenCenter.Y - 5), FLinearColor(0.f, 0.f, 0.f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X - 10, Variables::ScreenCenter.Y + 10), FVector2D(Variables::ScreenCenter.X - 5, Variables::ScreenCenter.Y + 5), FLinearColor(0.f, 0.f, 0.f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X + 10, Variables::ScreenCenter.Y + 10), FVector2D(Variables::ScreenCenter.X + 5, Variables::ScreenCenter.Y + 5), FLinearColor(0.f, 0.f, 0.f, 1.f), 2.f);
				Wrapper::Line(FVector2D(Variables::ScreenCenter.X + 10, Variables::ScreenCenter.Y - 10), FVector2D(Variables::ScreenCenter.X + 5, Variables::ScreenCenter.Y - 5), FLinearColor(0.f, 0.f, 0.f, 1.f), 2.f);
			}
		}
	}
	
	if (Variables::PlayerController->WasInputKeyJustPressed(Keys::Insert)) Variables::DrawMenu = !Variables::DrawMenu;
	if (Variables::DrawMenu) Framework::DrawMenu();
	Wrapper::Render();
	Wrapper::ClearTargets();
	
	return DrawTitleSafeAreaOriginal(Viewport, Canvas);
}
