// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimatronicBase.h"
#include "GoldenFoxy.generated.h"

/**
 * Golden Foxy — a custom animatronic unique to this game.
 * 
 * Behavior: The fastest animatronic. Stays in a "broken" state in
 * the storage room until triggered. Once activated, performs rush
 * attacks down hallways. The player must close doors or hide to
 * survive a rush. Between rushes, Golden Foxy returns to a random
 * hiding spot. Gets more aggressive as power drops.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AGoldenFoxy : public AAnimatronicBase
{
    GENERATED_BODY()

public:
    AGoldenFoxy();

    virtual void ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber) override;
    virtual void Tick(float DeltaTime) override;

    /** Begin a rush attack toward the player */
    UFUNCTION(BlueprintCallable, Category = "GoldenFoxy")
    void InitiateRush();

    UFUNCTION(BlueprintPure, Category = "GoldenFoxy")
    bool IsRushing() const { return bIsRushing; }

    UFUNCTION(BlueprintPure, Category = "GoldenFoxy")
    float GetRushCooldown() const { return RushCooldownTimer; }

protected:
    virtual void TickIdle(float DeltaTime) override;
    virtual void TickHunting(float DeltaTime) override;
    virtual void TickAggressive(float DeltaTime) override;

    void TickRush(float DeltaTime);
    void EndRush();
    FVector FindHidingSpot() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    float RushSpeed = 800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    float RushDuration = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    float RushCooldown = 30.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    float ActivationDelay = 120.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    float PowerThresholdForExtraAggression = 50.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GoldenFoxy|State")
    bool bIsRushing = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GoldenFoxy|State")
    float RushTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GoldenFoxy|State")
    float RushCooldownTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GoldenFoxy|State")
    float ActivationTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GoldenFoxy|State")
    bool bHasBeenTriggered = false;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    TObjectPtr<AActor> StorageRoomPosition;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GoldenFoxy|Config")
    TArray<AActor*> HidingSpots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoldenFoxy|Audio")
    TObjectPtr<USoundBase> RushSound;
};
