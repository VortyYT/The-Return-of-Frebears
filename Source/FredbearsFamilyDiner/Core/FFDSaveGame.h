// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDSaveGame.generated.h"

/**
 * Serializable save game data. Stores player progress, collected items,
 * unlocked nights, statistics, and settings.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UFFDSaveGame();

    // --- Progress ---

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Progress")
    int32 HighestNightCompleted = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Progress")
    TSet<FName> CollectedLoreItems;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Progress")
    TSet<FName> UnlockedSecrets;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Progress")
    bool bCompletedAllNights = false;

    // --- Statistics ---

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 TotalDeaths = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    float TotalPlayTime = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 TotalJumpscares = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 TotalNightsAttempted = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 TotalNightsCompleted = 0;

    // --- Per-animatronic death counts ---

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    TMap<FName, int32> DeathsByAnimatronic;

    // --- Settings (persisted) ---

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    float MasterVolume = 1.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    float MusicVolume = 0.8f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    float SFXVolume = 1.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    float MouseSensitivity = 1.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    EDifficulty LastDifficulty = EDifficulty::Normal;

    // --- Save metadata ---

    UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Meta")
    FDateTime SaveTimestamp;

    UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Meta")
    int32 SaveVersion = 1;

    /** Update the save timestamp before writing to disk */
    void PreSave();
};
