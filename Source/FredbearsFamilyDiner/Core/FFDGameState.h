// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDGameState.generated.h"

/**
 * Tracks global game state visible to all systems: power level, time,
 * animatronic threat level, and collectible progress.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AFFDGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AFFDGameState();

    // --- Power ---

    UFUNCTION(BlueprintCallable, Category = "Power")
    void SetPowerLevel(float NewPower);

    UFUNCTION(BlueprintPure, Category = "Power")
    float GetPowerLevel() const { return CurrentPower; }

    UFUNCTION(BlueprintPure, Category = "Power")
    bool IsPowerDepleted() const { return CurrentPower <= 0.0f; }

    // --- Threat ---

    UFUNCTION(BlueprintCallable, Category = "Threat")
    void SetThreatLevel(float NewThreat);

    UFUNCTION(BlueprintPure, Category = "Threat")
    float GetThreatLevel() const { return CurrentThreatLevel; }

    // --- Lore Collectibles ---

    UFUNCTION(BlueprintCallable, Category = "Collectibles")
    void CollectLoreItem(FName ItemID);

    UFUNCTION(BlueprintPure, Category = "Collectibles")
    bool HasCollectedLoreItem(FName ItemID) const;

    UFUNCTION(BlueprintPure, Category = "Collectibles")
    int32 GetCollectedLoreCount() const { return CollectedLoreItems.Num(); }

    UFUNCTION(BlueprintPure, Category = "Collectibles")
    const TSet<FName>& GetCollectedLoreItems() const { return CollectedLoreItems; }

    // --- Statistics ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 JumpscareCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float TotalTimeSurvived = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 DoorsUsed = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 CameraChecks = 0;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float CurrentPower = FFDConstants::MaxPower;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Threat")
    float CurrentThreatLevel = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collectibles")
    TSet<FName> CollectedLoreItems;
};
