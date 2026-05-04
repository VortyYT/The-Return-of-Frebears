// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "PowerSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerChanged, float, NewPower);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPowerDepleted_System);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerUsageChanged, int32, UsageLevel);

/**
 * Manages the diner's power supply. Power drains based on active systems
 * (flashlight, cameras, doors, lights). When power hits zero, lights go
 * out and animatronics become more aggressive.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API APowerSystem : public AActor
{
    GENERATED_BODY()

public:
    APowerSystem();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Power Management ---

    UFUNCTION(BlueprintCallable, Category = "Power")
    void AddPowerConsumer(FName ConsumerID, float DrainRate);

    UFUNCTION(BlueprintCallable, Category = "Power")
    void RemovePowerConsumer(FName ConsumerID);

    UFUNCTION(BlueprintCallable, Category = "Power")
    void SetConsumerActive(FName ConsumerID, bool bActive);

    UFUNCTION(BlueprintPure, Category = "Power")
    float GetCurrentPower() const { return CurrentPower; }

    UFUNCTION(BlueprintPure, Category = "Power")
    float GetPowerPercent() const;

    UFUNCTION(BlueprintPure, Category = "Power")
    bool IsPowerDepleted() const { return CurrentPower <= 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Power")
    float GetTotalDrainRate() const;

    UFUNCTION(BlueprintPure, Category = "Power")
    int32 GetUsageLevel() const;

    UFUNCTION(BlueprintCallable, Category = "Power")
    void RestorePower(float Amount);

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category = "Power|Events")
    FOnPowerChanged OnPowerChanged;

    UPROPERTY(BlueprintAssignable, Category = "Power|Events")
    FOnPowerDepleted_System OnPowerDepleted;

    UPROPERTY(BlueprintAssignable, Category = "Power|Events")
    FOnPowerUsageChanged OnPowerUsageChanged;

protected:
    struct FPowerConsumer
    {
        FName ID;
        float DrainRate;
        bool bIsActive;
    };

    void UpdatePowerDrain(float DeltaTime);
    void HandlePowerDepletion();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power|Config")
    float MaxPower = FFDConstants::MaxPower;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power|Config")
    float BaseDrainRate = FFDConstants::BasePowerDrainPerSecond;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power|State")
    float CurrentPower;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power|State")
    bool bPowerDepleted = false;

    TMap<FName, FPowerConsumer> PowerConsumers;
    int32 LastUsageLevel = 0;
};
