// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDGameInstance.generated.h"

class UFFDSaveGame;

/**
 * Persistent game instance that survives level transitions.
 * Manages save/load, settings, and unlocked content.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UFFDGameInstance();

    virtual void Init() override;

    // --- Save System ---

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool SaveGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool DeleteSave();

    UFUNCTION(BlueprintPure, Category = "Save")
    bool DoesSaveExist() const;

    UFUNCTION(BlueprintPure, Category = "Save")
    UFFDSaveGame* GetSaveData() const { return CurrentSaveData; }

    // --- Settings ---

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMouseSensitivity(float Sensitivity);

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMasterVolume() const { return MasterVolume; }

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMusicVolume() const { return MusicVolume; }

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetSFXVolume() const { return SFXVolume; }

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMouseSensitivity() const { return MouseSensitivity; }

    // --- Game Progress ---

    UFUNCTION(BlueprintPure, Category = "Progress")
    int32 GetHighestNightCompleted() const;

    UFUNCTION(BlueprintPure, Category = "Progress")
    bool IsExtrasUnlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Progress")
    void UnlockNight(int32 NightNumber);

    // --- Difficulty ---

    UPROPERTY(BlueprintReadWrite, Category = "Game")
    EDifficulty SelectedDifficulty = EDifficulty::Normal;

    UPROPERTY(BlueprintReadWrite, Category = "Game")
    int32 SelectedNight = 1;

protected:
    static const FString SaveSlotName;
    static const int32 SaveUserIndex;

    UPROPERTY()
    UFFDSaveGame* CurrentSaveData = nullptr;

    // Audio settings
    float MasterVolume = 1.0f;
    float MusicVolume = 0.8f;
    float SFXVolume = 1.0f;
    float MouseSensitivity = 1.0f;
};
