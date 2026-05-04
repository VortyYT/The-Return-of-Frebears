// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDGameMode.generated.h"

class AAnimatronicBase;
class AFFDPlayerCharacter;
class UFFDSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPowerDepleted);

/**
 * Core game mode controlling night progression, difficulty, and win/loss conditions.
 * Manages the overall game loop: night start -> survive -> 6 AM or death.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AFFDGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AFFDGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Night Management ---

    /** Start a new night with the given difficulty */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartNight(EDifficulty InDifficulty, int32 NightNumber = 1);

    /** End the current night (victory or death) */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void EndNight(bool bPlayerSurvived);

    /** Get the current in-game hour (12 = midnight, 1-6 = AM) */
    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetCurrentHour() const { return CurrentHour; }

    /** Get normalized night progress (0.0 = midnight, 1.0 = 6 AM) */
    UFUNCTION(BlueprintPure, Category = "Game")
    float GetNightProgress() const;

    /** Get difficulty multiplier for current difficulty setting */
    UFUNCTION(BlueprintPure, Category = "Game")
    float GetDifficultyMultiplier() const;

    /** Get the random seed for this playthrough */
    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetRandomSeed() const { return RandomSeed; }

    /** Trigger player death sequence */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void TriggerPlayerDeath(AAnimatronicBase* Killer);

    /** Pause/unpause the game */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetGamePaused(bool bPaused);

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category = "Game|Events")
    FOnNightStarted OnNightStarted;

    UPROPERTY(BlueprintAssignable, Category = "Game|Events")
    FOnNightCompleted OnNightCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Game|Events")
    FOnHourChanged OnHourChanged;

    UPROPERTY(BlueprintAssignable, Category = "Game|Events")
    FOnPlayerDeath OnPlayerDeath;

    UPROPERTY(BlueprintAssignable, Category = "Game|Events")
    FOnPowerDepleted OnPowerDepleted;

    // --- Accessors ---

    UFUNCTION(BlueprintPure, Category = "Game")
    EDifficulty GetCurrentDifficulty() const { return CurrentDifficulty; }

    UFUNCTION(BlueprintPure, Category = "Game")
    EGamePhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetCurrentNight() const { return CurrentNight; }

    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsNightActive() const { return bNightActive; }

    /** Get all animatronics registered in this game */
    UFUNCTION(BlueprintPure, Category = "Game")
    const TArray<AAnimatronicBase*>& GetAnimatronics() const { return Animatronics; }

    /** Register an animatronic with the game mode */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void RegisterAnimatronic(AAnimatronicBase* Animatronic);

protected:
    /** Advance the in-game clock */
    void UpdateNightTime(float DeltaTime);

    /** Check if the night is over (reached 6 AM) */
    void CheckNightCompletion();

    /** Initialize random seed for this playthrough */
    void InitializeRandomSeed();

    /** Activate all animatronics based on difficulty and night number */
    void ActivateAnimatronics();

    /** Deactivate all animatronics */
    void DeactivateAnimatronics();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game|Config")
    float NightDuration = FFDConstants::NightDurationSeconds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    EDifficulty CurrentDifficulty = EDifficulty::Normal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    EGamePhase CurrentPhase = EGamePhase::MainMenu;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    int32 CurrentNight = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    int32 CurrentHour = FFDConstants::NightStartHour;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    float NightElapsedTime = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    bool bNightActive = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|State")
    int32 RandomSeed = 0;

    UPROPERTY()
    TArray<AAnimatronicBase*> Animatronics;

    UPROPERTY()
    AFFDPlayerCharacter* PlayerCharacterRef = nullptr;
};
