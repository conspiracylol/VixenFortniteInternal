#include <includes.h>

std::future <void> main_future;

void DllStart()
{
    GObjects = GetObjects();

    KismetMathLibrary = UObject::FindObject(L"Engine.Default__KismetMathLibrary");
    GameplayStatics = UObject::FindObject(L"Engine.Default__GameplayStatics");
    KismetMaterialLibrary = UObject::FindObject(L"Engine.Default__KismetMaterialLibrary");
    FortKismetLibrary = UObject::FindObject(L"FortniteGame.Default__FortKismetLibrary");
    KismetSystemLibrary = UObject::FindObject(L"Engine.Default__KismetSystemLibrary");
    KismetStringLibrary = UObject::FindObject(L"Engine.Default__KismetStringLibrary");

    Keys::LeftMouseButton = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"LeftMouseButton") }, 0 };
    Keys::RightMouseButton = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"RightMouseButton") }, 0 };
    Keys::Insert = FKey{ FName{UKismetStringLibrary::Conv_StringToName(L"Insert") }, 0 };
    Keys::F2 = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"F2") }, 0 };
    Keys::F8 = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"F8") }, 0 };
    Keys::W = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"W") }, 0 };
    Keys::A = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"A") }, 0 };
    Keys::S = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"S") }, 0 };
    Keys::D = FKey{ FName{UKismetStringLibrary::Conv_StringToName(L"D") }, 0 };
    Keys::SpaceBar = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"SpaceBar") }, 0 };
    Keys::LeftShift = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"LeftShift") }, 0 };
    Keys::Capslock = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"Capslock") }, 0 };
    Keys::GamepadLeftTrigger = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"GamepadLeftTrigger") }, 0 };
    Keys::ThumbMouseButton = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"ThumbMouseButton") }, 0 };
    Keys::ThumbMouseButton2 = FKey{ FName{ UKismetStringLibrary::Conv_StringToName(L"ThumbMouseButton2") }, 0 };

    Classes::LocalPlayer = UObject::FindObject(L"Engine.LocalPlayer");
    Classes::PlayerController = UObject::FindObject(L"Engine.PlayerController");
    Classes::PlayerCameraManager = UObject::FindObject(L"Engine.PlayerCameraManager");
    Classes::PlayerClass = UObject::FindObject(L"FortniteGame.FortPlayerPawnAthena");
    Classes::PickupClass = UObject::FindObject(L"FortniteGame.FortPickup");
    Classes::ContainerClass = UObject::FindObject(L"FortniteGame.BuildingContainer");
    Classes::ProjectileClass = UObject::FindObject(L"FortniteGame.FortProjectileBase");
    Classes::WeakspotClass = UObject::FindObject(L"FortniteGame.BuildingWeakSpot");
    Classes::BuildingTrapClass = UObject::FindObject(L"FortniteGame.BuildingTrap");
    Classes::VehicleClass = UObject::FindObject(L"FortniteGame.FortAthenaVehicle");
    Classes::MarkerClass = UObject::FindObject(L"FortniteGame.FortPlayerMarkerBase");
    Classes::SupplyDropClass = UObject::FindObject(L"FortniteGame.FortAthenaSupplyDrop");
    Classes::RangedWeaponClass = UObject::FindObject(L"FortniteGame.FortWeaponRanged");
    Classes::FortWeaponClass = UObject::FindObject(L"FortniteGame.FortWeapon");
    Classes::PickaxeClass = UObject::FindObject(L"FortniteGame.FortWeaponPickaxeAthena");

    Classes::ActorPlayerNamePrivate = UObject::FindObject(L"FortKismetLibrary.GetPlayerNameSafe");

    Variables::MaterialParameter = UKismetStringLibrary::Conv_StringToName(L"HairTone_Highlights");
    Variables::MaterialParameter2 = UKismetStringLibrary::Conv_StringToName(L"Skin Boost Color And Exponent");
    Variables::MaterialParameter4 = UKismetStringLibrary::Conv_StringToName(L"HitGlow");
    Variables::MaterialParameter5 = UKismetStringLibrary::Conv_StringToName(L"Emissive_Color");
    //printf("PE and FindObj Have been called!\n");

    UWorld* GWorld = UWorld::GetWorld();
    UGameInstance* OwningGameInstance = GWorld->OwningGameInstance();
    ULocalPlayer* LocalPlayer = OwningGameInstance->LocalPlayers()[0];
    UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient();

    // CanvasObject -> temporary canvas if there isn't already one
    //Variables::Canvas = reinterpret_cast<UCanvas*>(UObject::FindObject(L"DebugCanvasObject", reinterpret_cast<UObject*>(-1)));
    UEngine* GEngine = reinterpret_cast<UEngine*>(OwningGameInstance->GetOuterObject());
    MediumFont = GEngine->MediumFont();

    //printf("initializing viewport hook type (class stuff)..\r\n");

    static Hook::NewHook ViewportHook;

    //printf("initializing viewport hook..\r\n");

    if (ViewportHook.Initialize(ViewportClient))
    {
       // printf("inserting viewport hook..\r\n");

        ViewportHook.Insert(&DrawTitleSafeArea, Offsets::DrawTitleSafeArea, &DrawTitleSafeAreaOriginal);
        
        //printf("swapping viewport hook..\r\n");

        ViewportHook.SwapContext();

        //printf("done with viewport hook..\r\n");

    }
}

BOOL DllMain(void* hModule, DWORD ul_reason_for_call, void* lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        Variables::BuildTime = GetBuildTime();

        auto EncryptedModule_FortniteClientWin64Shipping = Encrypt(L"FortniteClient-Win64-Shipping.exe");
        GameBase = FindModuleByName(EncryptedModule_FortniteClientWin64Shipping.decrypt()); EncryptedModule_FortniteClientWin64Shipping.clear();
        if (!GameBase) return false;

        SpoofCall::Initialize(reinterpret_cast<unsigned char*>(GameBase));

        reinterpret_cast<void(__cdecl*)(uintptr_t, void*)>(DoSpoofCall)(SpoofCode, &DllStart);
    }

    return true;
}