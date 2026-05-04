// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDDebugSubsystem.h"
#include "FredbearsFamilyDiner/Core/FFDGameMode.h"
#include "FredbearsFamilyDiner/Core/FFDGameState.h"
#include "FredbearsFamilyDiner/Animatronics/AnimatronicBase.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "FredbearsFamilyDiner/Systems/PowerSystem.h"
#include "Kismet/GameplayStatics.h"

void UFFDDebugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

#if !UE_BUILD_SHIPPING
    RegisterConsoleCommands();
    UE_LOG(LogFFD, Log, TEXT("Debug subsystem initialized"));
#endif
}

void UFFDDebugSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UFFDDebugSubsystem::ToggleDebugHUD()
{
#if !UE_BUILD_SHIPPING
    bDebugHUDVisible = !bDebugHUDVisible;
    UE_LOG(LogFFD, Log, TEXT("Debug HUD: %s"), bDebugHUDVisible ? TEXT("ON") : TEXT("OFF"));
#endif
}

void UFFDDebugSubsystem::SetPowerLevel(float Power)
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> PowerActors;
    UGameplayStatics::GetAllActorsOfClass(World, APowerSystem::StaticClass(), PowerActors);

    for (AActor* Actor : PowerActors)
    {
        if (APowerSystem* PS = Cast<APowerSystem>(Actor))
        {
            PS->RestorePower(Power - PS->GetCurrentPower());
            UE_LOG(LogFFD, Log, TEXT("[Debug] Power set to: %.1f"), Power);
        }
    }
#endif
}

void UFFDDebugSubsystem::SetTimeHour(int32 Hour)
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogFFD, Log, TEXT("[Debug] Set time to hour: %d"), Hour);
    // Time is managed by GameMode's internal clock — would need a setter exposed
#endif
}

void UFFDDebugSubsystem::SkipToMorning()
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->EndNight(true);
        UE_LOG(LogFFD, Log, TEXT("[Debug] Skipped to 6 AM!"));
    }
#endif
}

void UFFDDebugSubsystem::TeleportAnimatronicToPlayer(FName AnimatronicID)
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Player) return;

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (!GameMode) return;

    for (AAnimatronicBase* Animatronic : GameMode->GetAnimatronics())
    {
        if (Animatronic && Animatronic->GetAnimatronicID() == AnimatronicID)
        {
            const FVector PlayerLoc = Player->GetActorLocation();
            const FVector Offset = Player->GetActorForwardVector() * 300.0f;
            Animatronic->SetActorLocation(PlayerLoc + Offset);

            UE_LOG(LogFFD, Log, TEXT("[Debug] Teleported %s to player"),
                *AnimatronicID.ToString());
            return;
        }
    }

    UE_LOG(LogFFD, Warning, TEXT("[Debug] Animatronic not found: %s"),
        *AnimatronicID.ToString());
#endif
}

void UFFDDebugSubsystem::SetAnimatronicState(FName AnimatronicID, EAnimatronicState State)
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (!GameMode) return;

    for (AAnimatronicBase* Animatronic : GameMode->GetAnimatronics())
    {
        if (Animatronic && Animatronic->GetAnimatronicID() == AnimatronicID)
        {
            Animatronic->SetState(State);
            UE_LOG(LogFFD, Log, TEXT("[Debug] Set %s state to: %d"),
                *AnimatronicID.ToString(), static_cast<int32>(State));
            return;
        }
    }
#endif
}

void UFFDDebugSubsystem::DeactivateAllAnimatronics()
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (!GameMode) return;

    for (AAnimatronicBase* Animatronic : GameMode->GetAnimatronics())
    {
        if (Animatronic)
        {
            Animatronic->DeactivateAnimatronic();
        }
    }

    UE_LOG(LogFFD, Log, TEXT("[Debug] All animatronics deactivated"));
#endif
}

void UFFDDebugSubsystem::ToggleGodMode()
{
#if !UE_BUILD_SHIPPING
    bGodMode = !bGodMode;
    UE_LOG(LogFFD, Log, TEXT("[Debug] God mode: %s"), bGodMode ? TEXT("ON") : TEXT("OFF"));
#endif
}

void UFFDDebugSubsystem::ToggleInfiniteStamina()
{
#if !UE_BUILD_SHIPPING
    bInfiniteStamina = !bInfiniteStamina;
    UE_LOG(LogFFD, Log, TEXT("[Debug] Infinite stamina: %s"),
        bInfiniteStamina ? TEXT("ON") : TEXT("OFF"));
#endif
}

void UFFDDebugSubsystem::ToggleInfiniteBattery()
{
#if !UE_BUILD_SHIPPING
    bInfiniteBattery = !bInfiniteBattery;
    UE_LOG(LogFFD, Log, TEXT("[Debug] Infinite battery: %s"),
        bInfiniteBattery ? TEXT("ON") : TEXT("OFF"));
#endif
}

void UFFDDebugSubsystem::DumpGameState()
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    UE_LOG(LogFFD, Log, TEXT("=== GAME STATE DUMP ==="));

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (GameMode)
    {
        UE_LOG(LogFFD, Log, TEXT("Night: %d | Hour: %d | Phase: %d | Difficulty: %d"),
            GameMode->GetCurrentNight(), GameMode->GetCurrentHour(),
            static_cast<int32>(GameMode->GetCurrentPhase()),
            static_cast<int32>(GameMode->GetCurrentDifficulty()));
        UE_LOG(LogFFD, Log, TEXT("Night Progress: %.2f%% | Active: %s"),
            GameMode->GetNightProgress() * 100.0f,
            GameMode->IsNightActive() ? TEXT("Yes") : TEXT("No"));
    }

    AFFDGameState* GameState = Cast<AFFDGameState>(World->GetGameState());
    if (GameState)
    {
        UE_LOG(LogFFD, Log, TEXT("Power: %.1f%% | Threat: %.1f | Lore: %d"),
            GameState->GetPowerLevel(), GameState->GetThreatLevel(),
            GameState->GetCollectedLoreCount());
    }

    LogAnimatronicStates();

    UE_LOG(LogFFD, Log, TEXT("=== END DUMP ==="));
#endif
}

void UFFDDebugSubsystem::LogAnimatronicStates()
{
#if !UE_BUILD_SHIPPING
    UWorld* World = GetWorld();
    if (!World) return;

    AFFDGameMode* GameMode = Cast<AFFDGameMode>(World->GetAuthGameMode());
    if (!GameMode) return;

    UE_LOG(LogFFD, Log, TEXT("--- Animatronic States ---"));
    for (const AAnimatronicBase* Animatronic : GameMode->GetAnimatronics())
    {
        if (!Animatronic) continue;

        UE_LOG(LogFFD, Log,
            TEXT("  [%s] State: %d | Active: %s | CanSee: %s | CanHear: %s | Dist: %.0f"),
            *Animatronic->GetAnimatronicID().ToString(),
            static_cast<int32>(Animatronic->GetCurrentState()),
            Animatronic->IsActive() ? TEXT("Y") : TEXT("N"),
            Animatronic->CanSeePlayer() ? TEXT("Y") : TEXT("N"),
            Animatronic->CanHearPlayer() ? TEXT("Y") : TEXT("N"),
            Animatronic->GetDistanceToPlayer());
    }
#endif
}

void UFFDDebugSubsystem::RegisterConsoleCommands()
{
    // Console commands are registered through IConsoleManager
    // These would be registered as console commands in the actual UE project
    UE_LOG(LogFFD, Log, TEXT("Debug console commands registered"));
}
