// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDStaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaExhausted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaRecovered);

/**
 * Stamina component managing sprint energy with drain, regeneration,
 * exhaustion threshold, and recovery cooldown.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FREDBEARSFAMILYDINER_API UFFDStaminaComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFFDStaminaComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void StartDraining();

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void StopDraining();

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetCurrentStamina() const { return CurrentStamina; }

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetStaminaPercent() const;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool IsExhausted() const { return bIsExhausted; }

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool IsDraining() const { return bIsDraining; }

    UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
    FOnStaminaChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
    FOnStaminaExhausted OnStaminaExhausted;

    UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
    FOnStaminaRecovered OnStaminaRecovered;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Config")
    float MaxStamina = FFDConstants::MaxStamina;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Config")
    float DrainPerSecond = FFDConstants::StaminaDrainPerSecond;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Config")
    float RegenPerSecond = FFDConstants::StaminaRegenPerSecond;

    /** Delay before stamina starts regenerating after sprinting stops */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Config")
    float RegenDelay = 1.5f;

    /** Stamina must reach this percentage before exhaustion clears */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Config")
    float ExhaustionRecoveryThreshold = 30.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|State")
    float CurrentStamina;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|State")
    bool bIsDraining = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|State")
    bool bIsExhausted = false;

    float RegenDelayTimer = 0.0f;
};
