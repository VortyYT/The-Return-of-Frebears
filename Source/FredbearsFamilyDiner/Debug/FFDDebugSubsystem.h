// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDDebugSubsystem.generated.h"

/**
 * Debug subsystem for development and testing.
 * Provides console commands, debug visualization, and
 * diagnostic information for all game systems.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDDebugSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // --- Debug Commands ---

    /** Toggle debug HUD overlay showing AI states and system info */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleDebugHUD();

    /** Set power level directly */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void SetPowerLevel(float Power);

    /** Set the time directly */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void SetTimeHour(int32 Hour);

    /** Skip to 6 AM (win the night) */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void SkipToMorning();

    /** Teleport an animatronic to the player */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void TeleportAnimatronicToPlayer(FName AnimatronicID);

    /** Force an animatronic into a specific state */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void SetAnimatronicState(FName AnimatronicID, EAnimatronicState State);

    /** Deactivate all animatronics */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DeactivateAllAnimatronics();

    /** Toggle god mode (invulnerable) */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleGodMode();

    /** Toggle infinite stamina */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleInfiniteStamina();

    /** Toggle infinite flashlight battery */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleInfiniteBattery();

    /** Dump system state to log */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DumpGameState();

    /** Log all animatronic states */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void LogAnimatronicStates();

    // --- State ---

    UFUNCTION(BlueprintPure, Category = "Debug")
    bool IsDebugHUDVisible() const { return bDebugHUDVisible; }

    UFUNCTION(BlueprintPure, Category = "Debug")
    bool IsGodModeActive() const { return bGodMode; }

protected:
    bool bDebugHUDVisible = false;
    bool bGodMode = false;
    bool bInfiniteStamina = false;
    bool bInfiniteBattery = false;

    void RegisterConsoleCommands();
};
